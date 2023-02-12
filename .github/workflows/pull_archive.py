#!/usr/bin/env python3

try:
    import json
    import os
    import subprocess
    import sys
    from datetime import datetime
    import requests
except ImportError as err:
    raise ImportError(err) from err

GIT_URL = "https://github.com/ryftchen/foo.git"
ARTIFACT_URL = "https://api.github.com/repos/ryftchen/foo/actions/artifacts?per_page=5"
ARTIFACT_NAME = "foo_artifact"
WEBSITE_DIR = "/var/www/foo_web"


def command(cmd):
    print(f'{datetime.strftime(datetime.now(), "%b %d %H:%M:%S")} command: {cmd}')
    try:
        process = subprocess.run(
            cmd,
            shell=True,
            executable="/bin/bash",
            universal_newlines=True,
            capture_output=True,
            check=True,
            encoding="utf-8",
            timeout=300,
        )
        if process.stdout:
            print(process.stdout.strip())
        if process.stderr:
            print(process.stderr.strip())
        return process.stdout.strip(), process.stderr.strip()
    except subprocess.CalledProcessError as error:
        print(error)
        sys.exit(-1)
    except subprocess.TimeoutExpired as error:
        print(error)
        sys.exit(-1)


def exception(msg):
    print(f'{datetime.strftime(datetime.now(), "%b %d %H:%M:%S")} exception: {msg}')
    sys.exit(-1)


def downloadArtifact():
    filePath = os.path.split(os.path.realpath(__file__))[0]
    os.chdir(filePath.replace(filePath[filePath.index(".github") :], ''))

    if not os.path.exists(WEBSITE_DIR):
        exception("Please create a foo_web folder in the /var/www directory.")
    localCommitId, _ = command("git rev-parse HEAD")
    remoteCommitId, _ = command(f"git ls-remote {GIT_URL} refs/heads/master | cut -f 1")
    if not remoteCommitId:
        exception("Failed to get the latest commit id.")
    if localCommitId != remoteCommitId:
        command("git pull origin master")
    elif os.path.exists(f"{WEBSITE_DIR}/browser") and os.path.exists(f"{WEBSITE_DIR}/doxygen"):
        exception("No change in git project.")

    try:
        response = requests.get(ARTIFACT_URL, timeout=60)
        response.raise_for_status()

        downloadUrl = ""
        jsonInfo = json.loads(response.text)
        for index in range(jsonInfo["total_count"]):
            if jsonInfo["artifacts"][index]["name"] == ARTIFACT_NAME:
                downloadUrl = jsonInfo["artifacts"][index]["archive_download_url"]
                break

        response = requests.get(downloadUrl, timeout=60, allow_redirects=False)
        response.raise_for_status()
        locationUrl = response.headers["location"]
        proxy = {
            # "http": "http://localhost:80",
            # "https": "https://localhost:80",
            # "ftp": "ftp://localhost:80"
        }
        response = requests.get(locationUrl, timeout=60, proxies=proxy)
        response.raise_for_status()
        with open(f"{WEBSITE_DIR}/{ARTIFACT_NAME}.zip", "wb") as outputFile:
            outputFile.write(response.content)
    except requests.exceptions.HTTPError as error:
        command(f"rm -rf {WEBSITE_DIR}/{ARTIFACT_NAME}.zip")
        command(f"git reset --hard {localCommitId}")
        exception(error)

    validation, _ = command(f"zip -T {WEBSITE_DIR}/{ARTIFACT_NAME}.zip")
    if "zip error" in validation:
        command(f"rm -rf {WEBSITE_DIR}/{ARTIFACT_NAME}.zip")
        command(f"git reset --hard {localCommitId}")
        exception(f"The {ARTIFACT_NAME}.zip file in the {WEBSITE_DIR} folder is corrupted.")


def archiveDocument():
    command(f"rm -rf {WEBSITE_DIR}/browser {WEBSITE_DIR}/doxygen")
    command(f"unzip {WEBSITE_DIR}/{ARTIFACT_NAME}.zip -d {WEBSITE_DIR}")
    command(f"tar -jxvf {WEBSITE_DIR}/foo_browser_*.tar.bz2 -C {WEBSITE_DIR} >/dev/null")
    command(f"tar -jxvf {WEBSITE_DIR}/foo_doxygen_*.tar.bz2 -C {WEBSITE_DIR} >/dev/null")
    command(f"rm -rf {WEBSITE_DIR}/*.zip {WEBSITE_DIR}/*.tar.bz2")


def pull():
    print(
        f'\r\n{datetime.strftime(datetime.now(), "%b %d %H:%M:%S")} \
>>>>>>>>>>>>>>>>>>>>>>>>> ARCHIVE <<<<<<<<<<<<<<<<<<<<<<<<<'
    )
    env = os.getenv("FOO_ENV")
    if env:
        if env != "FOO_DOC":
            exception("The environment variable FOO_ENV must be FOO_DOC.")
    else:
        exception("Please export the environment variable FOO_ENV.")

    downloadArtifact()
    archiveDocument()


if __name__ == "__main__":
    pull()
