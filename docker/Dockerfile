FROM ubuntu:22.04

SHELL ["/bin/bash", "-c"]

RUN apt-get update \
&& DEBIAN_FRONTEND=noninteractive apt-get install -y sudo tzdata ca-certificates gpg psmisc bash-completion curl rsync zip \
&& curl -fsSL https://apt.llvm.org/llvm-snapshot.gpg.key | sudo gpg --dearmor -o /etc/apt/keyrings/llvm-snapshot.gpg \
&& sudo chmod a+r /etc/apt/keyrings/llvm-snapshot.gpg \
&& echo 'deb [signed-by=/etc/apt/keyrings/llvm-snapshot.gpg] http://apt.llvm.org/focal/ llvm-toolchain-focal-17 main' | sudo tee /etc/apt/sources.list.d/llvm-snapshot.list \
&& echo 'deb-src [signed-by=/etc/apt/keyrings/llvm-snapshot.gpg] http://apt.llvm.org/focal/ llvm-toolchain-focal-17 main' | sudo tee -a /etc/apt/sources.list.d/llvm-snapshot.list \
&& sudo apt-get update \
&& sudo apt-get install -y git python3 python3-pip build-essential cmake ninja-build ccache distcc gdb strace \
clang-17 lld-17 libstdc++-12-dev libgtest-dev libssl-dev libreadline-dev libmpfrc++-dev llvm-17-dev libclang-17-dev \
clang-format-17 clang-tidy-17 shfmt shellcheck black pylint rustc cargo rustfmt rust-clippy \
valgrind cloc pre-commit graphviz docker docker-compose npm \
&& sudo apt-get clean \
&& sudo pip3 install --no-cache-dir compdb ValgrindCI \
&& sudo npm install --no-save --global --no-package-lock cspell@5 https://github.com/jothepro/doxygen-awesome-css#v2.2.1 \
&& sudo npm cache clean --force \
&& cd /root \
&& curl -L https://github.com/doxygen/doxygen/releases/download/Release_1_9_7/doxygen-1.9.7.linux.bin.tar.gz >doxygen-1.9.7.linux.bin.tar.gz \
&& tar -zxvf ./doxygen-1.9.7.linux.bin.tar.gz \
&& sudo make -C ./doxygen-1.9.7 install \
&& rm -rf ./doxygen-* \
&& sudo mkdir /usr/local/share/doxygen-tag \
&& sudo curl -L https://gcc.gnu.org/onlinedocs/gcc-12.3.0/libstdc++/api/libstdc++.tag >/usr/local/share/doxygen-tag/libstdc++.tag \
&& git clone https://github.com/KDAB/codebrowser.git \
&& git -C ./codebrowser reset --hard 1808e96d4c62417952389d536576d53bb6da4053 \
&& cmake -S ./codebrowser -B ./codebrowser/build -DCMAKE_CXX_COMPILER=clang++-17 -DCMAKE_BUILD_TYPE=Release \
&& sudo make -C ./codebrowser/build install \
&& rm -rf ./codebrowser
