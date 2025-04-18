#!/usr/bin/env python3

try:
    import argparse
    import fnmatch
    import http
    import json
    import netrc
    import os
    import sys
    import traceback
    import urllib.error
    import urllib.parse
    import urllib.request
    import zipfile
    from datetime import datetime
    from common import execute_command as executor, Log as Logger
except ImportError as err:
    raise ImportError(err) from err

STDOUT = sys.stdout


class Schedule:
    repo_url = "https://github.com/ryftchen/foo.git"
    api_url = "https://api.github.com/repos/ryftchen/foo/actions/artifacts?per_page=5"
    artifact_name = "foo_artifact"
    target_dir = "/var/www/foo_doc"
    netrc_file = "~/.netrc"
    log_file = "/tmp/foo_pull_artifact.log"

    def __init__(self):
        self.logger = sys.stdout
        self.forced_pull = False
        self.proxy_port = None

        env = os.getenv("FOO_ENV")
        if env is not None:
            if env != "foo_doc":
                abort("The environment variable FOO_ENV must be foo_doc.")
        else:
            abort("Please export the environment variable FOO_ENV.")
        script_path = os.path.split(os.path.realpath(__file__))[0]
        if not fnmatch.fnmatch(script_path, "*foo/script"):
            abort("Illegal path to current script.")
        self.project_path = os.path.dirname(script_path)

    def __del__(self):
        sys.stdout = STDOUT
        del self.logger

    def parse(self):
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
        self.parse()
        self.logger = Logger(self.log_file, "at")
        sys.stdout = self.logger
        print()

        print(f"[ {datetime.now()} ] >>>>>>>>>>>>>>>>> PULL ARTIFACT >>>>>>>>>>>>>>>>>")
        if not os.path.exists(self.target_dir):
            abort(f"Please create a {self.target_dir} folder for storing pages.")
        self.download_artifact()
        self.update_document()
        print(f"[ {datetime.now()} ] <<<<<<<<<<<<<<<<< PULL ARTIFACT <<<<<<<<<<<<<<<<<")

    def download_artifact(self):
        print(f"[ {datetime.now()} ] ############### DOWNLOAD ARTIFACT ###############")
        local_commit_id, _, _ = executor(f"git -C {self.project_path} rev-parse HEAD")
        remote_commit_id, _, _ = executor(
            f"git -C {self.project_path} ls-remote {self.repo_url} refs/heads/master | cut -f 1"
        )
        if not remote_commit_id:
            abort("Could not get the latest commit id.")
        if local_commit_id != remote_commit_id:
            executor(f"git -C {self.project_path} pull origin master")
        elif (
            not self.forced_pull
            and os.path.exists(f"{self.target_dir}/doxygen")
            and os.path.exists(f"{self.target_dir}/browser")
        ):
            abort("No commit change.")

        try:
            if self.proxy_port:
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

            download_url = None
            request = urllib.request.Request(self.api_url, headers=headers)
            with urllib.request.urlopen(request, timeout=60) as response:
                if response.status != http.HTTPStatus.OK:
                    raise urllib.error.HTTPError(
                        self.api_url, response.status, "HTTP request failed.", response.headers, None
                    ) from None
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
                    ) from None
                with open(f"{self.target_dir}/{self.artifact_name}.zip", "wb") as output_file:
                    output_file.write(response.read())

            with zipfile.ZipFile(f"{self.target_dir}/{self.artifact_name}.zip", "r") as zip_file:
                if zip_file.testzip() is not None:
                    raise zipfile.BadZipFile("Corrupted zip file.") from None
        except Exception:  # pylint: disable=broad-except
            executor(f"rm -rf {self.target_dir}/{self.artifact_name}.zip")
            executor(f"git -C {self.project_path} reset --hard {local_commit_id}")
            raise

    def update_document(self):
        print(f"[ {datetime.now()} ] ################ UPDATE DOCUMENT ################")
        executor(f"rm -rf {self.target_dir}/doxygen {self.target_dir}/browser")
        executor(f"unzip {self.target_dir}/{self.artifact_name}.zip -d {self.target_dir}")
        executor(f"tar -jxvf {self.target_dir}/foo_doxygen_*.tar.bz2 -C {self.target_dir} >/dev/null")
        executor(f"tar -jxvf {self.target_dir}/foo_browser_*.tar.bz2 -C {self.target_dir} >/dev/null")
        executor(f"rm -rf {self.target_dir}/*.zip {self.target_dir}/*.tar.bz2")

    def get_redirect_location(self, url, headers):
        class NoRedirectHandler(urllib.request.HTTPRedirectHandler):
            def redirect_request(self, *args, **kwargs):  # pylint: disable=unused-argument
                return None

        location = None
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


def abort(msg):
    print(f"[ {datetime.now()} ] {msg}")
    sys.exit(1)


if __name__ == "__main__":
    try:
        Schedule().pull_artifact()
    except Exception:  # pylint: disable=broad-except
        abort(traceback.format_exc())
