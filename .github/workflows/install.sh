#!/usr/bin/env bash

SHFMT_URL="https://github.com/mvdan/sh/releases/download/v3.4.2/shfmt_v3.4.2_linux_amd64"
WOBOQ_GIT="https://github.com/KDAB/codebrowser.git"
WOBOQ_COMMIT="73fce32fc696b3f6eb2a678397328d9ce1ad4cf6"

shCommand()
{
    echo
    echo "$(date "+%b %d %T") $* BEGIN"
    sh -c "$@"
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

    shCommand "sudo apt-get install -y llvm-10 clang-10 libclang-10-dev \
clang-format-10 clang-tidy-10 cmake python3 pylint black shellcheck global valgrind"
    shCommand "sudo curl -L ${SHFMT_URL} >/usr/local/bin/shfmt \
&& sudo chmod +x /usr/local/bin/shfmt"
    shCommand "git clone ${WOBOQ_GIT} -b master \
&& cd ./codebrowser && git reset --hard ${WOBOQ_COMMIT} \
&& cmake . -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_COMPILER=clang++-10 && make -j4 \
&& sudo make install && cd .. && rm -rf ./codebrowser"
}

main()
{
    echo
    echo "$(date "+%b %d %T") ----- INSTALL DEPENDENCIES BEGIN -----"
    if [ -n "${FOO_ENV:?}" ]; then
        if [ "${FOO_ENV}" = "GITHUB_ACTION" ]; then
            installDependencies
        fi
    else
        printAbort "Please set environment variable FOO_ENV."
    fi
    echo
    echo "$(date "+%b %d %T") -----  INSTALL DEPENDENCIES END  -----"
}

main "$@"
