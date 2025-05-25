#!/usr/bin/env python3

try:
    import argparse
    import fnmatch
    import logging
    import http
    import json
    import netrc
    import os
    import subprocess
    import sys
    import traceback
    import urllib.error
    import urllib.parse
    import urllib.request
    import zipfile
except ImportError as err:
    raise ImportError(err) from err


class Schedule:
    repo_url = "https://github.com/ryftchen/foo.git"
    api_url = "https://api.github.com/repos/ryftchen/foo/actions/artifacts?per_page=5"
    artifact_name = "foo_artifact"
    target_dir = "/var/www/foo_doc"
    netrc_file = "~/.netrc"

    def __init__(self, logger):
        self.logger = logger
        self.forced_pull = False
        self.proxy_port = None

        env = os.getenv("FOO_ENV")
        if env is not None:
            if env != "foo_doc":
                self.abort("The environment variable FOO_ENV must be foo_doc.")
        else:
            self.abort("Please export the environment variable FOO_ENV.")
        script_path = os.path.split(os.path.realpath(__file__))[0]
        if not fnmatch.fnmatch(script_path, "*foo/script"):
            self.abort("Illegal path to current script.")
        self.project_path = os.path.dirname(script_path)

    def configure_from_cli(self):
        def check_port_range(value):
            value = int(value)
            if 0 <= value <= 65535:
                return value
            raise argparse.ArgumentTypeError("Must be in the range of 0-65535.")

        parser = argparse.ArgumentParser(description="pull artifact script")
        parser.add_argument("-f", "--force", action="store_true", default=False, help="forced pull")
        parser.add_argument(
            "-p",
            "--port",
            nargs="?",
            type=check_port_range,
            help="proxy port",
            metavar="[0-65535]",
        )

        args = parser.parse_args()
        if args.force:
            self.forced_pull = True
        if args.port is not None:
            self.proxy_port = args.port

    def pull_artifact(self):
        self.configure_from_cli()

        self.notice(">>>>>>>>>>>>>>>> PULL ARTIFACT >>>>>>>>>>>>>>>>")
        if not os.path.exists(self.target_dir):
            self.abort(f"Please create a {self.target_dir} folder for storing pages.")
        self.download_artifact()
        self.update_document()
        self.notice("<<<<<<<<<<<<<<<< PULL ARTIFACT <<<<<<<<<<<<<<<<")

    def download_artifact(self):
        self.notice("############## DOWNLOAD ARTIFACT ##############")
        local_commit_id, _, _ = self.executor(f"git -C {self.project_path} rev-parse HEAD")
        remote_commit_id, _, _ = self.executor(
            f"git -C {self.project_path} ls-remote {self.repo_url} refs/heads/master | cut -f 1"
        )
        if not remote_commit_id:
            self.abort("Could not get the latest commit id.")
        if local_commit_id != remote_commit_id:
            self.executor(f"git -C {self.project_path} pull origin master")
        elif (
            not self.forced_pull
            and os.path.exists(f"{self.target_dir}/doxygen")
            and os.path.exists(f"{self.target_dir}/browser")
        ):
            self.abort("No commit change.")

        try:
            if self.proxy_port is not None:
                proxy = {
                    "http": f"http://localhost:{self.proxy_port}",
                    "https": f"https://localhost:{self.proxy_port}",
                    "ftp": f"ftp://localhost:{self.proxy_port}",
                }
                handler = urllib.request.ProxyHandler(proxy)
                opener = urllib.request.build_opener(handler)
                urllib.request.install_opener(opener)

            headers = {}
            netrc_info = netrc.netrc(os.path.expanduser(self.netrc_file))
            _, _, access_token = netrc_info.authenticators(urllib.parse.urlparse(self.api_url).hostname)
            if access_token:
                headers = {"Authorization": f"token {access_token}", "User-Agent": "ryftchen/foo"}

            download_url = ""
            request = urllib.request.Request(self.api_url, headers=headers)
            with urllib.request.urlopen(request, timeout=60) as response:
                if response.status != http.HTTPStatus.OK:
                    raise urllib.error.HTTPError(
                        self.api_url, response.status, "HTTP request failed.", response.headers, None
                    )
                json_detail = json.loads(response.read().decode("utf-8"))
                for index in range(json_detail["total_count"]):
                    if json_detail["artifacts"][index]["name"] == self.artifact_name:
                        download_url = json_detail["artifacts"][index]["archive_download_url"]
                        break

            redirect_location = self.get_redirect_location(download_url, headers)
            request = urllib.request.Request(redirect_location)
            with urllib.request.urlopen(request, timeout=60) as response:
                if response.status != http.HTTPStatus.OK:
                    raise urllib.error.HTTPError(
                        redirect_location, response.status, "File download failed.", response.headers, None
                    )
                with open(f"{self.target_dir}/{self.artifact_name}.zip", "wb") as output_file:
                    output_file.write(response.read())

            with zipfile.ZipFile(f"{self.target_dir}/{self.artifact_name}.zip", "r") as zip_file:
                if zip_file.testzip() is not None:
                    raise zipfile.BadZipFile("Corrupted zip file.")
        except Exception as error:
            self.executor(f"rm -rf {self.target_dir}/{self.artifact_name}.zip")
            self.executor(f"git -C {self.project_path} reset --hard {local_commit_id}")
            raise type(error)(str(error)) from None

    def update_document(self):
        self.notice("############### UPDATE DOCUMENT ###############")
        command_list = [
            f"rm -rf {self.target_dir}/doxygen {self.target_dir}/browser",
            f"unzip {self.target_dir}/{self.artifact_name}.zip -d {self.target_dir}",
            f"tar -jxvf {self.target_dir}/foo_doxygen_*.tar.bz2 -C {self.target_dir} >/dev/null",
            f"tar -jxvf {self.target_dir}/foo_browser_*.tar.bz2 -C {self.target_dir} >/dev/null",
            f"rm -rf {self.target_dir}/*.zip {self.target_dir}/*.tar.bz2",
        ]
        for entry in command_list:
            _, stderr, return_code = self.executor(entry)
            if stderr or return_code:
                self.executor(f"rm -rf {self.target_dir}/doxygen {self.target_dir}/browser")
                self.abort(f"Interrupted due to a failure of the \"{entry}\" command.")

    def get_redirect_location(self, url, headers):
        class NoRedirectHandler(urllib.request.HTTPRedirectHandler):
            def redirect_request(self, *args, **kwargs):  # pylint: disable=unused-argument
                return None

        location = ""
        try:
            request = urllib.request.Request(url, headers=headers)
            opener = urllib.request.build_opener(NoRedirectHandler)
            with opener.open(request, timeout=60):
                pass
        except urllib.error.HTTPError as error:
            if error.status != http.HTTPStatus.FOUND:
                raise urllib.error.HTTPError(url, error.status, "URL redirect expected.", error.headers, None) from None
            location = error.headers["location"]

        return location

    def executor(self, command):
        self.logger.debug(command)
        try:
            process = subprocess.run(
                command,
                executable="/bin/bash",
                shell=True,
                universal_newlines=True,
                capture_output=True,
                check=True,
                encoding="utf-8",
                timeout=300,
            )
            return process.stdout.strip(), process.stderr.strip(), process.returncode
        except subprocess.CalledProcessError as error:
            return error.stdout.strip(), error.stderr.strip(), error.returncode
        except subprocess.TimeoutExpired as error:
            return "", str(error), 124

    def notice(self, message):
        self.logger.info(message)

    def abort(self, message):
        self.logger.warning(message)
        sys.exit(1)


def setup_logger(log_level, log_file):
    logger = logging.getLogger(__name__)
    logger.setLevel(log_level)

    file_handler = logging.FileHandler(log_file, mode="at", encoding="utf-8")
    formatter = logging.Formatter("%(asctime)s - %(levelname)-8s - %(message)s", datefmt="%Y-%m-%d %H:%M:%S")
    file_handler.setFormatter(formatter)
    logger.addHandler(file_handler)

    return logger


if __name__ == "__main__":
    LOGGER = setup_logger(logging.INFO, f"/tmp/foo_{os.path.splitext(os.path.basename(__file__))[0]}.log")
    try:
        Schedule(LOGGER).pull_artifact()
    except Exception:  # pylint: disable=broad-except
        LOGGER.error(traceback.format_exc().replace("\n", "\\n"))
