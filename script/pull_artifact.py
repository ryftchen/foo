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


class Schedule:  # pylint: disable=too-few-public-methods
    _repo_url = "https://github.com/ryftchen/foo.git"
    _api_url = "https://api.github.com/repos/ryftchen/foo/actions/artifacts?per_page=5"
    _artifact_name = "foo_artifact"
    _target_dir = "/var/www/foo_doc"
    _netrc_file = "~/.netrc"

    def __init__(self, logger: logging.Logger):
        self._logger = logger
        self._forced_pull = False
        self._proxy_port = None

        env = os.getenv("FOO_ENV")
        if env is None:
            self._abort("Please export the environment variable FOO_ENV.")
        if env != "foo_doc":
            self._abort("The environment variable FOO_ENV must be foo_doc.")
        self_path = os.path.split(os.path.realpath(__file__))[0]
        if not fnmatch.fnmatch(self_path, "*foo/script"):
            self._abort("Illegal path to current script.")
        self._project_path = os.path.dirname(self_path)

    def pull_artifact(self):
        self._configure_from_cli()

        self._notice(">>>>>>>>>>>>>>>> PULL ARTIFACT >>>>>>>>>>>>>>>>")
        if not os.path.exists(self._target_dir):
            self._abort(f"Please create a {self._target_dir} folder for storing pages.")
        self._download_artifact()
        self._update_document()
        self._notice("<<<<<<<<<<<<<<<< PULL ARTIFACT <<<<<<<<<<<<<<<<")

    def _configure_from_cli(self):
        def _check_port_range(value: int):
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
            type=_check_port_range,
            help="proxy port",
            metavar="[0-65535]",
        )

        args = parser.parse_args()
        if args.force:
            self._forced_pull = True
        if args.port is not None:
            self._proxy_port = args.port

    def _download_artifact(self):
        self._notice("############## DOWNLOAD ARTIFACT ##############")
        local_commit_id, _, _ = self._executor(f"git -C {self._project_path} rev-parse HEAD")
        remote_commit_id, _, _ = self._executor(
            f"git -C {self._project_path} ls-remote {self._repo_url} refs/heads/master | cut -f 1"
        )
        if not remote_commit_id:
            self._abort("Could not get the latest commit id.")
        if local_commit_id != remote_commit_id:
            self._executor(f"git -C {self._project_path} pull origin master")
        elif (
            not self._forced_pull
            and os.path.exists(f"{self._target_dir}/doxygen")
            and os.path.exists(f"{self._target_dir}/browser")
        ):
            self._abort("No commit change.")

        try:
            if self._proxy_port is not None:
                proxy = {
                    "http": f"http://localhost:{self._proxy_port}",
                    "https": f"https://localhost:{self._proxy_port}",
                    "ftp": f"ftp://localhost:{self._proxy_port}",
                }
                handler = urllib.request.ProxyHandler(proxy)
                opener = urllib.request.build_opener(handler)
                urllib.request.install_opener(opener)

            headers = {}
            netrc_info = netrc.netrc(os.path.expanduser(self._netrc_file))
            _, _, access_token = netrc_info.authenticators(urllib.parse.urlparse(self._api_url).hostname)
            if access_token:
                headers = {"Authorization": f"token {access_token}", "User-Agent": "ryftchen/foo"}

            download_url = ""
            request = urllib.request.Request(self._api_url, headers=headers)
            with urllib.request.urlopen(request, timeout=60) as response:
                if response.status != http.HTTPStatus.OK:
                    raise urllib.error.HTTPError(
                        self._api_url, response.status, "HTTP request failed.", response.headers, None
                    )
                json_detail = json.loads(response.read().decode("utf-8"))
                for index in range(json_detail["total_count"]):
                    if json_detail["artifacts"][index]["name"] == self._artifact_name:
                        download_url = json_detail["artifacts"][index]["archive_download_url"]
                        break

            redirect_location = self._get_redirect_location(download_url, headers)
            request = urllib.request.Request(redirect_location)
            with urllib.request.urlopen(request, timeout=60) as response:
                if response.status != http.HTTPStatus.OK:
                    raise urllib.error.HTTPError(
                        redirect_location, response.status, "File download failed.", response.headers, None
                    )
                with open(f"{self._target_dir}/{self._artifact_name}.zip", "wb") as output_file:
                    output_file.write(response.read())

            with zipfile.ZipFile(f"{self._target_dir}/{self._artifact_name}.zip", "r") as zip_file:
                if zip_file.testzip() is not None:
                    raise zipfile.BadZipFile("Corrupted zip file.")
        except Exception as error:
            self._executor(f"rm -rf {self._target_dir}/{self._artifact_name}.zip")
            self._executor(f"git -C {self._project_path} reset --hard {local_commit_id}")
            raise type(error)(str(error)) from None

    def _update_document(self):
        self._notice("############### UPDATE DOCUMENT ###############")
        command_list = [
            f"rm -rf {self._target_dir}/doxygen {self._target_dir}/browser",
            f"unzip {self._target_dir}/{self._artifact_name}.zip -d {self._target_dir}",
            f"tar -jxvf {self._target_dir}/foo_doxygen_*.tar.bz2 -C {self._target_dir} >/dev/null",
            f"tar -jxvf {self._target_dir}/foo_browser_*.tar.bz2 -C {self._target_dir} >/dev/null",
            f"rm -rf {self._target_dir}/*.zip {self._target_dir}/*.tar.bz2",
        ]
        for entry in command_list:
            _, stderr, return_code = self._executor(entry)
            if stderr or return_code:
                self._executor(f"rm -rf {self._target_dir}/doxygen {self._target_dir}/browser")
                self._abort(f"Interrupted due to a failure of the \"{entry}\" command.")

    def _get_redirect_location(self, url: str, headers: dict[str, str]) -> str:
        class NoRedirectHandler(urllib.request.HTTPRedirectHandler):
            def redirect_request(self, *args, **kwargs) -> None:  # pylint: disable=unused-argument
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

    def _executor(self, command: str) -> tuple[str, str, int]:
        self._logger.debug(command)
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

    def _notice(self, message: str):
        self._logger.info(message)

    def _abort(self, message: str):
        self._logger.warning(message)
        sys.exit(1)


def create_logger(log_level: int, log_file: str) -> logging.Logger:
    logger = logging.getLogger(__name__)
    logger.setLevel(log_level)

    file_handler = logging.FileHandler(log_file, mode="at", encoding="utf-8")
    formatter = logging.Formatter("%(asctime)s - %(levelname)-8s - %(message)s", datefmt="%Y-%m-%d %H:%M:%S")
    file_handler.setFormatter(formatter)
    logger.addHandler(file_handler)
    return logger


if __name__ == "__main__":
    LOGGER = create_logger(logging.INFO, f"/tmp/foo_{os.path.splitext(os.path.basename(__file__))[0]}.log")
    try:
        Schedule(LOGGER).pull_artifact()
    except Exception:  # pylint: disable=broad-except
        LOGGER.error(traceback.format_exc().replace("\n", "\\n"))
