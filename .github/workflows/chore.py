#!/usr/bin/env python3

import argparse
import json
import os
import subprocess
import sys
from datetime import datetime
import requests

ENV_BROWSER = "CODE_BROWSER"
ENV_CONTAINER = "CODE_CONTAINER"
PROJECT_GIT = "https://github.com/ryftchen/foo.git"
PROJECT_NAME = "foo"
BROWSER_DIR = "/var/www/code_browser"
ARTIFACT_URL = "https://api.github.com/repos/ryftchen/foo/actions/artifacts?per_page=1"
ARTIFACT_NAME = "foo_artifact"
IMAGE_REPO = "ryftchen/foo"
CODE_DIR = "/root/code"


def command(cmd):
    print("{} {}".format(datetime.strftime(datetime.now(), "%b %d %H:%M:%S"), cmd))
    try:
        proc = subprocess.Popen(
            cmd,
            shell=True,
            executable="/bin/bash",
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            encoding="utf-8",
        )
    except RuntimeError:
        sys.exit(-1)
    stdout, stderr = proc.communicate()
    if stdout:
        print(stdout.strip())
    if stderr:
        print(stderr.strip())
    return stdout.strip()


def abort(msg):
    print(
        "{} {}".format(
            datetime.strftime(datetime.now(), "%b %d %H:%M:%S"), f"Python script chore.py: {msg}"
        )
    )
    sys.exit(-1)


def constructCodeBrowser():
    if not os.path.exists(BROWSER_DIR):
        abort(f"Please create code_browser folder in /var/www directory.")
    localCommitId = command("git rev-parse HEAD")
    remoteCommitId = command(f"git ls-remote {PROJECT_GIT} refs/heads/master | cut -f 1")
    if not remoteCommitId:
        abort("Failed to get the latest commit id.")
    htmlFolder = f"{PROJECT_NAME}_html"
    if localCommitId != remoteCommitId:
        command("git pull origin master")
    elif os.path.exists(f"{BROWSER_DIR}/{htmlFolder}"):
        abort(f"No change in {PROJECT_NAME} project.")

    try:
        response = requests.get(ARTIFACT_URL)
        response.raise_for_status()

        jsonInfo = json.loads(response.text)
        if jsonInfo["total_count"] != 0:
            if jsonInfo["artifacts"][0]["name"] == ARTIFACT_NAME:
                downloadUrl = jsonInfo["artifacts"][0]["archive_download_url"]

        response = requests.get(downloadUrl, allow_redirects=False)
        response.raise_for_status()
        locationUrl = response.headers["location"]

        response = requests.get(locationUrl)
        response.raise_for_status()
        with open(f"{BROWSER_DIR}/{ARTIFACT_NAME}.zip", "wb") as outputFile:
            outputFile.write(response.content)
    except requests.exceptions.HTTPError as error:
        abort(error)

    zipValidation = command(f"zip -T {BROWSER_DIR}/{ARTIFACT_NAME}.zip")
    if "zip error" in zipValidation:
        command(f"rm -rf {BROWSER_DIR}/{ARTIFACT_NAME}.zip")
        command(f"git reset --hard {localCommitId}")
        abort(f"The zip file {ARTIFACT_NAME}.zip in {BROWSER_DIR} folder is corrupted.")
    command(f"rm -rf {BROWSER_DIR}/{htmlFolder}")
    command(f"unzip {BROWSER_DIR}/{ARTIFACT_NAME}.zip -d {BROWSER_DIR}")
    command(f"tar -jxvf {BROWSER_DIR}/{htmlFolder}_*.tar.bz2 -C {BROWSER_DIR} >/dev/null")
    command(f"rm -rf {BROWSER_DIR}/*.zip {BROWSER_DIR}/*.tar.bz2")


def constructCodeContainer():
    containerInfo = command("docker ps -a")
    if IMAGE_REPO not in containerInfo:
        localImageInfo = command("docker image ls -a")
        if IMAGE_REPO not in localImageInfo:
            remoteImageInfo = command(f"docker search {IMAGE_REPO}")
            if IMAGE_REPO in remoteImageInfo:
                command(f"docker pull {IMAGE_REPO}:latest")
            else:
                command(f"docker build -t {IMAGE_REPO}:latest -f Dockerfile .")
        command(
            f"docker run -it --name {PROJECT_NAME} -v {CODE_DIR}:{CODE_DIR} \
-d {IMAGE_REPO}:latest /bin/bash"
        )


def chore():
    parser = argparse.ArgumentParser(description="chore script")
    parser.add_argument("-b", "--browser", action="store_true", help="construct code browser")
    parser.add_argument("-c", "--container", action="store_true", help="construct code container")
    args = parser.parse_args()

    print(
        "\r\n{} >>>>>>>>>> chore.py <<<<<<<<<<".format(
            datetime.strftime(datetime.now(), "%b %d %H:%M:%S")
        )
    )

    env = os.getenv("FOO_ENV")
    if env:
        if env not in (ENV_BROWSER, ENV_CONTAINER):
            abort("Unknown type of FOO_ENV.")
    else:
        abort("Please export FOO_ENV firstly.")
    os.chdir(os.path.split(os.path.realpath(__file__))[0])
    localDir = command("git rev-parse --show-toplevel")
    os.chdir(localDir)

    if args.browser:
        if env != ENV_BROWSER:
            abort(f"FOO_ENV should be {ENV_BROWSER}.")
        constructCodeBrowser()
    elif args.container:
        if env != ENV_CONTAINER:
            abort(f"FOO_ENV should be {ENV_CONTAINER}.")
        constructCodeContainer()


if __name__ == "__main__":
    chore()
