#!/usr/bin/env python3

try:
    import argparse
    import fnmatch
    import json
    import os
    import requests
    import sys
    import traceback
    from datetime import datetime
    import common
except ImportError as err:
    raise ImportError(err) from err

STDOUT = sys.stdout


class Documentation:
    repo_url = "https://github.com/ryftchen/foo.git"
    api_url = "https://api.github.com/repos/ryftchen/foo/actions/artifacts?per_page=5"
    artifact_name = "foo_artifact"
    target_dir = "/var/www/foo_doc"
    netrc_file = os.path.expanduser("~/.netrc")
    log_file = "/tmp/foo_pull_artifact.log"

    def __init__(self):
        self.forced_pull = False
        self.proxy_port = ""

        env = os.getenv("FOO_ENV")
        if env is not None:
            if env != "foo_doc":
                interrupt("The environment variable FOO_ENV must be foo_doc.")
        else:
            interrupt("Please export the environment variable FOO_ENV.")
        script_path = os.path.split(os.path.realpath(__file__))[0]
        if not fnmatch.fnmatch(script_path, "*foo/script"):
            interrupt("Illegal path to current script.")
        parser = argparse.ArgumentParser(description="pull artifact script")
        parser.add_argument("-f", "--force", action="store_true", default=False, help="forced pull")
        parser.add_argument(
            "-p",
            "--port",
            nargs="?",
            type=int,
            choices=range(0, 65535 + 1),
            help="proxy port",
            metavar="[0-65535]",
        )
        args = parser.parse_args()
        if args.force:
            self.forced_pull = True
        if args.port is not None:
            self.proxy_port = args.port

        self.project_path = os.path.dirname(script_path)
        self.logger = common.Log(self.log_file, "at")
        sys.stdout = self.logger

    def pull_artifact(self):
        print(f"\n[ {datetime.now()} ] ################# PULL ARTIFACT #################")
        if not os.path.exists(self.target_dir):
            interrupt(f"Please create a {self.target_dir} folder for storing pages.")
        if not os.path.exists(self.netrc_file):
            interrupt(f"Please create a {self.netrc_file} file for authentication.")
        self.download_artifact()
        self.update_document()
        sys.stdout = STDOUT
        del self.logger

    def download_artifact(self):
        print(f"[ {datetime.now()} ] +++++++++++++++ DOWNLOAD ARTIFACT +++++++++++++++")
        local_commit_id, _, _ = execute(f"git -C {self.project_path} rev-parse HEAD")
        remote_commit_id, _, _ = execute(
            f"git -C {self.project_path} ls-remote {self.repo_url} refs/heads/master | cut -f 1"
        )
        if not remote_commit_id:
            interrupt("Could not get the latest commit id.")
        if local_commit_id != remote_commit_id:
            execute(f"git -C {self.project_path} pull origin master")
        elif (
            not self.forced_pull
            and os.path.exists(f"{self.target_dir}/doxygen")
            and os.path.exists(f"{self.target_dir}/browser")
        ):
            interrupt("No commit change.")

        try:
            response = requests.get(self.api_url, timeout=60)
            response.raise_for_status()

            download_url = ""
            json_detail = json.loads(response.text)
            for index in range(json_detail["total_count"]):
                if json_detail["artifacts"][index]["name"] == self.artifact_name:
                    download_url = json_detail["artifacts"][index]["archive_download_url"]
                    break

            response = requests.get(download_url, timeout=60, allow_redirects=False)
            response.raise_for_status()
            location_url = response.headers["location"]
            proxy = {}
            if len(str(self.proxy_port)) != 0:
                proxy = {
                    "http": f"http://localhost:{self.proxy_port}",
                    "https": f"https://localhost:{self.proxy_port}",
                    "ftp": f"ftp://localhost:{self.proxy_port}",
                }
            response = requests.get(location_url, timeout=60, proxies=proxy)
            response.raise_for_status()
            with open(f"{self.target_dir}/{self.artifact_name}.zip", "wb") as output_file:
                output_file.write(response.content)
        except requests.exceptions.RequestException as error:
            execute(f"rm -rf {self.target_dir}/{self.artifact_name}.zip")
            execute(f"git -C {self.project_path} reset --hard {local_commit_id}")
            interrupt(error)

        validation, _, _ = execute(f"zip -T {self.target_dir}/{self.artifact_name}.zip")
        if "zip error" in validation:
            execute(f"rm -rf {self.target_dir}/{self.artifact_name}.zip")
            execute(f"git -C {self.project_path} reset --hard {local_commit_id}")
            interrupt(f"The {self.artifact_name}.zip file in the {self.target_dir} folder is corrupted.")

    def update_document(self):
        print(f"[ {datetime.now()} ] ++++++++++++++++ UPDATE DOCUMENT ++++++++++++++++")
        execute(f"rm -rf {self.target_dir}/doxygen {self.target_dir}/browser")
        execute(f"unzip {self.target_dir}/{self.artifact_name}.zip -d {self.target_dir}")
        execute(f"tar -jxvf {self.target_dir}/foo_doxygen_*.tar.bz2 -C {self.target_dir} >/dev/null")
        execute(f"tar -jxvf {self.target_dir}/foo_browser_*.tar.bz2 -C {self.target_dir} >/dev/null")
        execute(f"rm -rf {self.target_dir}/*.zip {self.target_dir}/*.tar.bz2")


def execute(cmd):
    print(f"[ {datetime.now()} ] {execute.__name__.upper()}: {cmd}")
    return common.execute_command(cmd)


def interrupt(msg):
    print(f"[ {datetime.now()} ] {interrupt.__name__.upper()}: {msg}")
    sys.exit(1)


if __name__ == "__main__":
    try:
        Documentation().pull_artifact()
    except Exception:  # pylint: disable=broad-except
        interrupt(traceback.format_exc())
