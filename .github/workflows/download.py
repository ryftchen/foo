#!/usr/bin/env python3

import json
import os
import subprocess
import sys
from datetime import datetime
import requests

PROJECT_GIT = "https://github.com/ryftchen/foo.git"
PROJECT_FOLDER = "foo"
BROWSER_FOLDER = "/var/www/code_browser"
ARTIFACT_URL = "https://api.github.com/repos/ryftchen/foo/actions/artifacts?per_page=1"
ARTIFACT_FILE = "foo_artifact"


def executeCommand(cmd, output=True):
    if output:
        print("\r\n{} {} START".format(datetime.strftime(datetime.now(), "%b %d %H:%M:%S"), cmd))
    try:
        out = subprocess.Popen(
            cmd,
            shell=True,
            executable="/bin/bash",
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            encoding="utf-8",
        )
    except RuntimeError:
        sys.exit(-1)
    if output:
        stdout, stderr = out.communicate()
        if stdout.strip():
            print(stdout.strip())
        if stderr.strip():
            print(stderr.strip())
        print("{} {} FINISH".format(datetime.strftime(datetime.now(), "%b %d %H:%M:%S"), cmd))
    return out


def printAbort(message):
    print(f"Python script download.py: {message}")
    sys.exit(-1)


def downloadArtifacts():
    os.chdir(os.path.split(os.path.realpath(__file__))[0])
    localDir = (
        executeCommand("git rev-parse --show-toplevel", output=False).stdout.read().splitlines()[0]
    )
    os.chdir(localDir)

    if not os.path.exists(f"{BROWSER_FOLDER}"):
        printAbort(f"Please create code_browser folder in /var/www directory.")
    localCommitId = executeCommand("git rev-parse HEAD", output=False).stdout.read().splitlines()[0]
    remoteCommitId = executeCommand(
        f"git ls-remote {PROJECT_GIT} refs/heads/master | cut -f 1", output=False
    ).stdout.read()
    if len(remoteCommitId) != 0:
        remoteCommitId = remoteCommitId.splitlines()[0]
    else:
        printAbort("Failed to get the latest commit id.")
    htmlFolder = f"{PROJECT_FOLDER}_html"
    if localCommitId != remoteCommitId:
        executeCommand("git pull origin master")
    elif os.path.exists(f"{BROWSER_FOLDER}/{htmlFolder}"):
        printAbort(f"No change in {PROJECT_FOLDER} project.")

    try:
        response = requests.get(ARTIFACT_URL)
        response.raise_for_status()

        jsonInfo = json.loads(response.text)
        if jsonInfo["total_count"] != 0:
            if jsonInfo["artifacts"][0]["name"] == ARTIFACT_FILE:
                downloadUrl = jsonInfo["artifacts"][0]["archive_download_url"]

        response = requests.get(downloadUrl, allow_redirects=False)
        response.raise_for_status()
        locationUrl = response.headers["location"]

        response = requests.get(locationUrl)
        response.raise_for_status()
        with open(f"{BROWSER_FOLDER}/{ARTIFACT_FILE}.zip", "wb") as outputFile:
            outputFile.write(response.content)
    except requests.exceptions.HTTPError as error:
        printAbort(error)

    zipCheck = executeCommand(
        f"zip -T {BROWSER_FOLDER}/{ARTIFACT_FILE}.zip", output=False
    ).stdout.read()
    if zipCheck.find("zip error") == -1:
        executeCommand(f"rm -rf {BROWSER_FOLDER}/{htmlFolder}")
        executeCommand(f"unzip {BROWSER_FOLDER}/{ARTIFACT_FILE}.zip -d {BROWSER_FOLDER}")
        executeCommand(
            f"tar -jxvf {BROWSER_FOLDER}/{htmlFolder}_*.tar.bz2 -C {BROWSER_FOLDER} >/dev/null"
        )
        executeCommand(f"rm -rf {BROWSER_FOLDER}/*.zip {BROWSER_FOLDER}/*.tar.bz2")
    else:
        executeCommand(f"rm -rf {BROWSER_FOLDER}/{ARTIFACT_FILE}.zip")
        executeCommand(f"git reset --hard {localCommitId}")
        printAbort(f"The zip file {ARTIFACT_FILE}.zip in {BROWSER_FOLDER} folder is corrupted.")


if __name__ == "__main__":
    print(
        "\r\n{} >>>>>>>>>> DOWNLOAD ARTIFACTS START <<<<<<<<<<".format(
            datetime.strftime(datetime.now(), "%b %d %H:%M:%S")
        )
    )

    if os.getenv("FOO_ENV"):
        if os.getenv("FOO_ENV") == "CODE_BROWSER":
            downloadArtifacts()
        else:
            printAbort("The environment variable FOO_ENV is not CODE_BROWSER.")
    else:
        printAbort("Please set environment variable FOO_ENV firstly.")

    print(
        "\r\n{} >>>>>>>>>> DOWNLOAD ARTIFACTS FINISH <<<<<<<<<<".format(
            datetime.strftime(datetime.now(), "%b %d %H:%M:%S")
        )
    )
