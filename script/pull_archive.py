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


class Documentation:
    github_url = "https://github.com/ryftchen/foo.git"
    artifact_url = "https://api.github.com/repos/ryftchen/foo/actions/artifacts?per_page=5"
    artifact_file = "foo_artifact"
    website_dir = "/var/www/foo_web"

    def __init__(self):
        self.project_path = ""
        self.proxy_port = ""
        env = os.getenv("FOO_ENV")
        if env is not None:
            if env != "foo_doc":
                abort("The environment variable FOO_ENV must be foo_doc.")
        else:
            abort("Please export the environment variable FOO_ENV.")

    def pull_archive(self):
        script_path = os.path.split(os.path.realpath(__file__))[0]
        if not fnmatch.fnmatch(script_path, "*foo/script"):
            abort("Illegal path to current script.")
        self.project_path = os.path.dirname(script_path)

        parser = argparse.ArgumentParser(description="pull archive script")
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
        if args.port is not None:
            self.proxy_port = args.port

        print(
            f"\r\n{datetime.strftime(datetime.now(), '%b %d %H:%M:%S')} \
>>>>>>>>>>>>>>>>>>>>>>>>> PULL ARCHIVE <<<<<<<<<<<<<<<<<<<<<<<<<"
        )
        self.download_artifact()
        self.update_document()

    def download_artifact(self):
        if not os.path.exists(self.website_dir):
            abort("Please create a foo_web folder in the /var/www directory.")
        local_commit_id, _, _ = execute(f"git -C {self.project_path} rev-parse HEAD")
        remote_commit_id, _, _ = execute(
            f"git -C {self.project_path} ls-remote {self.github_url} refs/heads/master | cut -f 1"
        )
        if not remote_commit_id:
            abort("Failed to get the latest commit id.")
        if local_commit_id != remote_commit_id:
            execute(f"git -C {self.project_path} pull origin master")
        elif os.path.exists(f"{self.website_dir}/browser") and os.path.exists(f"{self.website_dir}/doxygen"):
            abort("No commit change.")

        try:
            response = requests.get(self.artifact_url, timeout=60)
            response.raise_for_status()

            download_url = ""
            json_info = json.loads(response.text)
            for index in range(json_info["total_count"]):
                if json_info["artifacts"][index]["name"] == self.artifact_file:
                    download_url = json_info["artifacts"][index]["archive_download_url"]
                    break

            response = requests.get(download_url, timeout=60, allow_redirects=False)
            response.raise_for_status()
            location_url = response.headers["location"]
            proxy = {}
            if self.proxy_port:
                proxy = {
                    "http": f"http://localhost:{self.proxy_port}",
                    "https": f"https://localhost:{self.proxy_port}",
                    "ftp": f"ftp://localhost:{self.proxy_port}",
                }
            response = requests.get(location_url, timeout=60, proxies=proxy)
            response.raise_for_status()
            with open(f"{self.website_dir}/{self.artifact_file}.zip", "wb") as output_file:
                output_file.write(response.content)
        except requests.exceptions.HTTPError as error:
            execute(f"rm -rf {self.website_dir}/{self.artifact_file}.zip")
            execute(f"git -C {self.project_path} reset --hard {local_commit_id}")
            abort(error)

        validation, _, _ = execute(f"zip -T {self.website_dir}/{self.artifact_file}.zip")
        if "zip error" in validation:
            execute(f"rm -rf {self.website_dir}/{self.artifact_file}.zip")
            execute(f"git -C {self.project_path} reset --hard {local_commit_id}")
            abort(f"The {self.artifact_file}.zip file in the {self.website_dir} folder is corrupted.")

    def update_document(self):
        execute(f"rm -rf {self.website_dir}/browser {self.website_dir}/doxygen")
        execute(f"unzip {self.website_dir}/{self.artifact_file}.zip -d {self.website_dir}")
        execute(f"tar -jxvf {self.website_dir}/foo_browser_*.tar.bz2 -C {self.website_dir} >/dev/null")
        execute(f"tar -jxvf {self.website_dir}/foo_doxygen_*.tar.bz2 -C {self.website_dir} >/dev/null")
        execute(f"rm -rf {self.website_dir}/*.zip {self.website_dir}/*.tar.bz2")


def execute(cmd):
    print(f"{datetime.strftime(datetime.now(), '%b %d %H:%M:%S')} EXECUTE: {cmd}")
    return common.execute_command(cmd)


def abort(msg):
    print(f"{datetime.strftime(datetime.now(), '%b %d %H:%M:%S')} ABORT: {msg}")
    sys.exit(1)


if __name__ == "__main__":
    try:
        Documentation().pull_archive()
    except Exception:  # pylint: disable=broad-except
        abort(traceback.format_exc())
