#!/usr/bin/env bash

SHFMT_URL="https://github.com/mvdan/sh/releases/download/v3.4.2/shfmt_v3.4.2_linux_amd64"
SHFMT_PATH="/usr/local/bin"
WOBOQ_GIT="https://github.com/KDAB/codebrowser.git"
WOBOQ_COMMIT="73fce32fc696b3f6eb2a678397328d9ce1ad4cf6"
WOBOQ_FOLDER="codebrowser"

bashCommand()
{
    echo
    echo "$(date "+%b %d %T") $* BEGIN"
    /bin/bash -c "$@"
    echo "$(date "+%b %d %T") $* END"
}

printAbort()
{
    echo "Shell script install.sh: $*"
    exit 1
}

installDependencies()
{
    cd "$(dirname "$0")" || exit 1
    localDir=$(git rev-parse --show-toplevel)
    cd "${localDir}" || exit 1

    bashCommand "sudo apt-get install -y llvm-11 clang-11 libclang-11-dev \
clang-format-11 clang-tidy-11 cmake python3 pylint black shellcheck valgrind global sysvbanner"
    bashCommand "sudo curl -L ${SHFMT_URL} >${SHFMT_PATH}/shfmt \
&& sudo chmod +x ${SHFMT_PATH}/shfmt"
    bashCommand "git clone ${WOBOQ_GIT} -b master \
&& git -C ./${WOBOQ_FOLDER} reset --hard ${WOBOQ_COMMIT} \
&& cmake -S ./${WOBOQ_FOLDER} -B ./${WOBOQ_FOLDER} \
-DCMAKE_CXX_COMPILER=clang++-11 -DCMAKE_BUILD_TYPE=Release && make -C ./${WOBOQ_FOLDER} -j \
&& sudo make -C ./${WOBOQ_FOLDER} install && rm -rf ./${WOBOQ_FOLDER}"
}

main()
{
    echo
    echo "$(date "+%b %d %T") ---------- INSTALL DEPENDENCIES BEGIN"

    if [ -n "${FOO_ENV:?}" ]; then
        if [ "${FOO_ENV}" = "GITHUB_ACTION" ]; then
            installDependencies
        fi
    else
        printAbort "Please set environment variable FOO_ENV."
    fi

    echo
    echo "$(date "+%b %d %T") ---------- INSTALL DEPENDENCIES END "
}

main "$@"
