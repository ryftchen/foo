#!/usr/bin/env bash

PROJECT_URL="https://github.com/ryftchen/foo.git"
PROJECT_FOLDER="foo"
BROWSER_FOLDER="browser"
NETRC_FILE=".netrc"
ARTIFACT_URL="https://api.github.com/repos/ryftchen/foo/actions/artifacts?per_page=1"
ARTIFACT_FILE="foo_artifact"
SHFMT_URL="https://github.com/mvdan/sh/releases/download/v3.4.2/shfmt_v3.4.2_linux_amd64"
WOBOQ_URL="https://github.com/KDAB/codebrowser.git"
WOBOQ_COMMIT="73fce32fc696b3f6eb2a678397328d9ce1ad4cf6"

shCommand()
{
    echo
    echo "$(date "+%b %d %T") $*" BEGIN
    sh -c "$@"
    echo "$(date "+%b %d %T") $*" END
}

printAbort()
{
    echo "Shell script ci.sh: $*"
    exit 1
}

installDependencies()
{
    echo
    echo "$(date "+%b %d %T") TRIGGER INSTALL DEPENDENCIES"

    shCommand "sudo apt-get install -y llvm-10 clang-10 libclang-10-dev \
clang-format-10 clang-tidy-10 cmake python3 pylint black shellcheck global valgrind"
    shCommand "sudo curl -L ${SHFMT_URL} >/usr/local/bin/shfmt \
&& sudo chmod +x /usr/local/bin/shfmt"
    shCommand "git clone ${WOBOQ_URL} && cd ./codebrowser && git reset --hard ${WOBOQ_COMMIT} \
&& cmake . -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_COMPILER=clang++-10 && make -j4 \
&& sudo make install && cd .. && rm -rf ./codebrowser"
}

downloadArtifact()
{
    echo
    echo "$(date "+%b %d %T") TRIGGER DOWNLOAD ARTIFACT"

    if [ ! -d ~/"${BROWSER_FOLDER}" ]; then
        printAbort "Please create ${BROWSER_FOLDER} folder in ~/ folder."
    fi
    if [ ! -f ~/"${NETRC_FILE}" ]; then
        printAbort "There is no ${NETRC_FILE} file in ~/ folder."
    fi
    localCommitId=$(git rev-parse HEAD)
    remoteCommitId=$(git ls-remote "${PROJECT_URL}" refs/heads/master | cut -f 1)
    htmlFolder="${PROJECT_FOLDER}_html"
    if [ -n "${remoteCommitId}" ]; then
        if [ "${localCommitId}" != "${remoteCommitId}" ]; then
            shCommand "git pull origin master"
        elif [ -d ~/"${BROWSER_FOLDER}"/"${htmlFolder}" ]; then
            printAbort "No change in ${PROJECT_FOLDER} project."
        fi
    else
        printAbort "Failed to get the latest commit id."
    fi

    actionUrl=$(curl -s "${ARTIFACT_URL}" \
        | jq '[.artifacts[] | {name : .name, archive_download_url : .archive_download_url}]' \
        | jq -r '.[] | select (.name == "'"${ARTIFACT_FILE}"'") | .archive_download_url' | head -n1)
    localUrl=$(curl -netrc -silent "${actionUrl}" | grep -oP 'location: \K.*' | tr -d '\r')
    shCommand "curl -L -X GET \"${localUrl}\" --output ~/${BROWSER_FOLDER}/${ARTIFACT_FILE}.zip"
    if
        zip -T ~/"${BROWSER_FOLDER}"/"${ARTIFACT_FILE}".zip | grep 'zip error' >/dev/null 2>&1
    then
        shCommand "rm -rf ~/${BROWSER_FOLDER}/${ARTIFACT_FILE}.zip"
        shCommand "git reset --hard ${localCommitId}"
        printAbort "The zip file ${ARTIFACT_FILE}.zip in ~/${BROWSER_FOLDER} folder is corrupted."
    else
        shCommand "rm -rf ~/${BROWSER_FOLDER}/${htmlFolder}"
        shCommand "unzip ~/${BROWSER_FOLDER}/${ARTIFACT_FILE}.zip -d ~/${BROWSER_FOLDER}"
        shCommand "tar -jxvf ~/${BROWSER_FOLDER}/${htmlFolder}_*.tar.bz2 -C ~/${BROWSER_FOLDER} \
>/dev/null"
        shCommand "rm -rf ~/${BROWSER_FOLDER}/*.zip ~/${BROWSER_FOLDER}/*.tar.bz2"
    fi
}

main()
{
    cd "$(dirname "$0")" || exit 1
    localDir=$(git rev-parse --show-toplevel)
    cd "${localDir}" || exit 1

    if [ -n "${FOO_ENV:?}" ]; then
        if [ "${FOO_ENV}" = "GITHUB_ACTION" ]; then
            installDependencies
        elif [ "${FOO_ENV}" = "CODE_BROWSER" ]; then
            downloadArtifact
        fi
    else
        printAbort "Please set environment variable FOO_ENV."
    fi
}

main "$@"
