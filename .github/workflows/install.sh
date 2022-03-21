#!/usr/bin/env bash

STATUS_SPLIT_LINE="-----"
STATUS_INSTALL_DEPENDENCIES="INSTALL DEPENDENCIES"
SHFMT_URL="https://github.com/mvdan/sh/releases/download/v3.4.2/shfmt_v3.4.2_linux_amd64"
WOBOQ_GIT="https://github.com/KDAB/codebrowser.git"
WOBOQ_COMMIT="73fce32fc696b3f6eb2a678397328d9ce1ad4cf6"

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
    bashCommand "sudo curl -L ${SHFMT_URL} >/usr/local/bin/shfmt \
&& sudo chmod +x /usr/local/bin/shfmt"
    bashCommand "git clone ${WOBOQ_GIT} -b master \
&& cd ./codebrowser && git reset --hard ${WOBOQ_COMMIT} \
&& cmake . -DCMAKE_CXX_COMPILER=clang++-11 -DCMAKE_BUILD_TYPE=Release && make -j4 \
&& sudo make install && cd .. && rm -rf ./codebrowser"
}

main()
{
    echo
    echo "$(date "+%b %d %T") \
${STATUS_SPLIT_LINE} ${STATUS_INSTALL_DEPENDENCIES} BEGIN ${STATUS_SPLIT_LINE}"

    if [ -n "${FOO_ENV:?}" ]; then
        if [ "${FOO_ENV}" = "GITHUB_ACTION" ]; then
            installDependencies
        fi
    else
        printAbort "Please set environment variable FOO_ENV."
    fi

    echo
    echo "$(date "+%b %d %T") \
${STATUS_SPLIT_LINE}  ${STATUS_INSTALL_DEPENDENCIES} END  ${STATUS_SPLIT_LINE}"
}

main "$@"
