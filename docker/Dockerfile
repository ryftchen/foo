FROM ubuntu:22.04

SHELL ["/bin/bash", "-c"]

RUN apt-get update \
&& DEBIAN_FRONTEND=noninteractive apt-get install -y sudo tzdata ca-certificates curl bash-completion rsync zip \
&& sudo apt-get update \
&& sudo apt-get install -y git python3 python3-pip build-essential cmake ninja-build ccache distcc gdb strace \
clang-15 lld-15 libgtest-dev libssl-dev libreadline-dev libmpfrc++-dev llvm-15-dev libclang-15-dev \
clang-format-15 clang-tidy-15 shfmt shellcheck black pylint rustfmt rust-clippy rustc cargo \
valgrind cloc pre-commit graphviz docker docker-compose npm \
&& sudo apt-get clean \
&& sudo pip3 install --no-cache-dir compdb ValgrindCI \
&& sudo npm install --no-save --global --no-package-lock cspell@5 \
&& sudo npm cache clean --force \
&& cd /root \
&& curl -L https://github.com/doxygen/doxygen/releases/download/Release_1_9_6/doxygen-1.9.6.linux.bin.tar.gz >doxygen-1.9.6.linux.bin.tar.gz \
&& tar -zxvf ./doxygen-1.9.6.linux.bin.tar.gz \
&& sudo make -C ./doxygen-1.9.6 install \
&& rm -rf ./doxygen-* \
&& curl -L https://github.com/jothepro/doxygen-awesome-css/archive/refs/tags/v2.2.0.tar.gz >doxygen-awesome-css-2.2.0.tar.gz \
&& tar -zxvf ./doxygen-awesome-css-2.2.0.tar.gz \
&& sudo make -C ./doxygen-awesome-css-2.2.0 install \
&& rm -rf ./doxygen-awesome-css-* \
&& git clone https://github.com/KDAB/codebrowser.git \
&& git -C ./codebrowser reset --hard 85834c9cb99d36051912d3007697087d871faa83 \
&& cmake -S ./codebrowser -B ./codebrowser/build -DCMAKE_CXX_COMPILER=clang++-15 -DCMAKE_BUILD_TYPE=Release \
&& sudo make -C ./codebrowser/build install \
&& rm -rf ./codebrowser
