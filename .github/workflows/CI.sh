#!/usr/bin/env bash

shCommand()
{
    echo
    echo "$(date "+%b %d %T") $*" BEGIN
    sh -c "$@"
    echo "$(date "+%b %d %T") $*" END
}

printAbort()
{
    echo "Shell script build.sh: $*"
    exit 1
}

main()
{
    shCommand "sudo apt-get install -y llvm-10 clang-10 libclang-10-dev \
clang-format-10 clang-tidy-10 cmake python3 pylint black shellcheck valgrind"
    shCommand "wget https://github.com/mvdan/sh/releases/download/v3.4.2/shfmt_v3.4.2_linux_amd64 \
&& sudo mv shfmt_v3.4.2_linux_amd64 shfmt && sudo mv shfmt /usr/local/bin/ \
&& sudo chmod +x /usr/local/bin/shfmt"
    shCommand "git clone https://github.com/KDAB/codebrowser.git && cd ./codebrowser \
&& git reset --hard 73fce32fc696b3f6eb2a678397328d9ce1ad4cf6 \
&& cmake . -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_COMPILER=clang++-10 && make -j4 \
&& sudo make install && cd .."
}

main "$@"
