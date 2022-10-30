FROM ubuntu:20.04

SHELL ["/bin/bash", "-c"]

RUN apt-get update \
&& DEBIAN_FRONTEND=noninteractive apt-get install -y sudo tzdata ca-certificates curl zip \
&& sudo apt-get update \
&& sudo apt-get install -y git python3 python3-pip build-essential cmake ninja-build gdb \
llvm-12 clang-12 libclang-12-dev libstdc++-10-dev libreadline-dev libmpfrc++-dev \
clang-format-12 clang-tidy-12 pylint black shellcheck valgrind \
&& sudo apt-get clean \
&& sudo pip3 install compdb requests \
&& cd /root \
&& sudo curl -L https://github.com/mvdan/sh/releases/download/v3.4.2/shfmt_v3.4.2_linux_amd64 >/usr/local/bin/shfmt \
&& sudo chmod +x /usr/local/bin/shfmt \
&& git clone https://github.com/KDAB/codebrowser.git -b master \
&& git -C ./codebrowser reset --hard 73fce32fc696b3f6eb2a678397328d9ce1ad4cf6 \
&& cmake -S ./codebrowser -B ./codebrowser -DCMAKE_CXX_COMPILER=clang++-12 -DCMAKE_BUILD_TYPE=Release \
&& make -C ./codebrowser -j \
&& sudo make -C ./codebrowser install \
&& rm -rf ./codebrowser
