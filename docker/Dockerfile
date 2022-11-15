FROM ubuntu:20.04

SHELL ["/bin/bash", "-c"]

RUN apt-get update \
&& DEBIAN_FRONTEND=noninteractive apt-get install -y sudo tzdata ca-certificates curl rsync zip \
&& sudo apt-get update \
&& sudo apt-get install -y git python3 python3-pip build-essential cmake ninja-build gdb \
llvm-12 clang-12 libclang-12-dev libstdc++-10-dev libreadline-dev libmpfrc++-dev \
clang-format-12 clang-tidy-12 valgrind graphviz \
&& sudo apt-get clean \
&& sudo pip3 install -v requests pylint==2.15.5 black==22.10.0 shellcheck-py==0.8.0.4 shfmt-py==3.4.3.1 \
compdb==0.2.0 ValgrindCI==0.2.0 \
&& cd /root \
&& git clone https://github.com/KDAB/codebrowser.git \
&& git -C ./codebrowser reset --hard e7bce1618726657d0615f0976f0db11a4f70f1f7 \
&& cmake -S ./codebrowser -B ./codebrowser/build -DCMAKE_CXX_COMPILER=clang++-12 -DCMAKE_BUILD_TYPE=Release \
&& make -C ./codebrowser/build -j \
&& sudo make -C ./codebrowser/build install \
&& rm -rf ./codebrowser \
&& curl -L https://downloads.sourceforge.net/project/doxygen/rel-1.9.5/doxygen-1.9.5.linux.bin.tar.gz >doxygen-1.9.5.linux.bin.tar.gz \
&& tar -zxvf doxygen-1.9.5.linux.bin.tar.gz \
&& sudo make -C ./doxygen-1.9.5 install \
&& rm -rf ./doxygen-* \
&& curl -L https://github.com/jothepro/doxygen-awesome-css/archive/refs/tags/v2.1.0.tar.gz >doxygen-awesome-css-2.1.0.tar.gz \
&& tar -zxvf doxygen-awesome-css-2.1.0.tar.gz \
&& sudo mkdir -p /usr/local/share/doxygen-awesome-css \
&& sudo install -m 644 ./doxygen-awesome-css-2.1.0/doxygen-awesome* /usr/local/share/doxygen-awesome-css/ \
&& rm -rf ./doxygen-awesome-css-*
