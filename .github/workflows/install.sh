#!/usr/bin/env bash

SHFMT_URL="https://github.com/mvdan/sh/releases/download/v3.4.2/shfmt_v3.4.2_linux_amd64"
SHFMT_PATH="/usr/local/bin"
WOBOQ_GIT="https://github.com/KDAB/codebrowser.git"
WOBOQ_COMMIT="73fce32fc696b3f6eb2a678397328d9ce1ad4cf6"
WOBOQ_FOLDER="codebrowser"

bashCommand()
{
    echo
    echo "$(date "+%b %d %T") $* START"
    /bin/bash -c "$@"
    echo "$(date "+%b %d %T") $* FINISH"
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

    bashCommand "sudo apt-get install -y git curl python3 \
build-essential llvm-12 clang-12 libclang-12-dev libreadline-dev cmake gdb valgrind \
clang-format-12 clang-tidy-12 python3 pylint black shellcheck"
    bashCommand "sudo curl -L ${SHFMT_URL} >${SHFMT_PATH}/shfmt \
&& sudo chmod +x ${SHFMT_PATH}/shfmt"
    bashCommand "git clone ${WOBOQ_GIT} -b master \
&& git -C ./${WOBOQ_FOLDER} reset --hard ${WOBOQ_COMMIT} \
&& cmake -S ./${WOBOQ_FOLDER} -B ./${WOBOQ_FOLDER} \
-DCMAKE_CXX_COMPILER=clang++-12 -DCMAKE_BUILD_TYPE=Release && make -C ./${WOBOQ_FOLDER} -j \
&& sudo make -C ./${WOBOQ_FOLDER} install && rm -rf ./${WOBOQ_FOLDER}"
}

main()
{
    echo
    echo "$(date "+%b %d %T") >>>>>>>>>> INSTALL DEPENDENCIES START <<<<<<<<<<"

    if [ -n "${FOO_ENV:=}" ]; then
        if [ "${FOO_ENV}" = "CODE_ACTION" ]; then
            installDependencies
        else
            printAbort "The environment variable FOO_ENV is not CODE_ACTION."
        fi
    else
        printAbort "Please set environment variable FOO_ENV firstly."
    fi

    echo
    echo "$(date "+%b %d %T") >>>>>>>>>> INSTALL DEPENDENCIES FINISH <<<<<<<<<<"
}

main "$@"
