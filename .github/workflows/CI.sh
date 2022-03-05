#!/usr/bin/env bash

GITHUB_FOLDER="/home/runner/work"
PROJECT_FOLDER="foo"
BROWSER_FOLDER="browser"
NETRC_FILE=".netrc"
ARTIFACT_FILE="foo_artifact"
ARTIFACT_URL="https://api.github.com/repos/ryftchen/foo/actions/artifacts?per_page=1"
SHFMT_URL="https://github.com/mvdan/sh/releases/download/v3.4.2/shfmt_v3.4.2_linux_amd64"
SHFMT_ORIGINAL="shfmt_v3.4.2_linux_amd64"
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
    echo "Shell script CI.sh: $*"
    exit 1
}

installDependencies()
{
    shCommand "sudo apt-get install -y llvm-10 clang-10 libclang-10-dev \
clang-format-10 clang-tidy-10 cmake python3 pylint black shellcheck valgrind"
    shCommand "curl -O ${SHFMT_URL} && sudo mv ${SHFMT_ORIGINAL} shfmt \
&& sudo mv shfmt /usr/local/bin/ && sudo chmod +x /usr/local/bin/shfmt"
    shCommand "git clone ${WOBOQ_URL} && cd ./codebrowser && git reset --hard ${WOBOQ_COMMIT} \
&& cmake . -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_COMPILER=clang++-10 && make -j4 \
&& sudo make install && cd .. && rm -rf ./codebrowser"
}

downloadArtifact()
{
    if [ ! -f ~/"${NETRC_FILE}" ]; then
        printAbort "There is no ${NETRC_FILE} file in ~/ folder."
    fi
    shCommand "git remote -v update"
    localCommitId=$(git rev-parse @)
    remoteCommitId=$(git rev-parse @\{u\})
    htmlFolder="${PROJECT_FOLDER}_html"
    if [ "${localCommitId}" != "${remoteCommitId}" ]; then
        shCommand "git pull origin master"
    elif [ -d ~/"${BROWSER_FOLDER}"/"${htmlFolder}" ]; then
        printAbort "No change in ${PROJECT_FOLDER} project."
    fi
    files=$(find ~/"${BROWSER_FOLDER}" -name "*" 2>/dev/null | sed 1d | wc -l)
    if [ "${files}" != "0" ]; then
        shCommand "rm -rf ~/${BROWSER_FOLDER}/*"
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
        printAbort "The zip file ${ARTIFACT_FILE}.zip in ~/${BROWSER_FOLDER} folder is corrupted."
    else
        shCommand "unzip ~/${BROWSER_FOLDER}/${ARTIFACT_FILE}.zip -d ~/${BROWSER_FOLDER}"
        shCommand "tar -jxvf ~/${BROWSER_FOLDER}/${htmlFolder}_*.tar.bz2 -C ~/${BROWSER_FOLDER} \
>/dev/null"
        shCommand "rm -rf ~/${BROWSER_FOLDER}/*.zip ~/${BROWSER_FOLDER}/*.tar.bz2"
    fi
}

main()
{
    cd "$(dirname "$0")" || exit 1
    localEnv=$(git rev-parse --show-toplevel)
    cd "${localEnv}" || exit 1
    ciEnv="${GITHUB_FOLDER}/${PROJECT_FOLDER}/${PROJECT_FOLDER}"

    if [ "${localEnv}" = "${ciEnv}" ]; then
        echo "$(date "+%b %d %T") INSTALL DEPENDENCIES"
        installDependencies
    elif [ -d ~/"${BROWSER_FOLDER}" ]; then
        echo "$(date "+%b %d %T") DOWNLOAD ARTIFACT"
        downloadArtifact
    fi
}

main "$@"
