FROM ubuntu:24.04

SHELL ["/bin/bash", "-c"]

RUN apt-get update \
&& DEBIAN_FRONTEND=noninteractive apt-get install --assume-yes --no-install-recommends sudo tzdata ca-certificates \
&& sudo apt-get update \
&& sudo apt-get install --assume-yes --no-install-recommends git python3 python3-pip build-essential cmake ninja-build gcc-13 g++-13 binutils-common \
clang-16 clang++-16 lld-16 libstdc++-13-dev libgtest-dev libssl-dev libreadline-dev libmpfrc++-dev llvm-16-dev libclang-16-dev libclang-rt-16-dev \
clang-format-16 clang-tidy-16 shfmt shellcheck black pylint rustc cargo rustfmt rust-clippy gdb ccache distcc \
valgrind cloc pre-commit doxygen graphviz docker.io docker-compose-v2 npm elfutils psmisc net-tools curl rsync zip bash-completion \
&& sudo apt-get clean \
&& sudo rm -rf /var/lib/apt/lists/* \
&& sudo pip3 install --no-cache-dir --break-system-packages compdb ValgrindCI sarif-tools \
&& sudo npm install --no-save --global --no-package-lock cspell@7 https://github.com/jothepro/doxygen-awesome-css#v2.3.1 \
&& sudo npm cache clean --force \
&& cd /root \
&& curl -L https://github.com/github/codeql-action/releases/download/codeql-bundle-v2.18.0/codeql-bundle-linux64.tar.gz | tee codeql-bundle-linux64.tar.gz >/dev/null \
&& sudo tar -zxvf codeql-bundle-linux64.tar.gz -C /usr/local/share \
&& sudo rm -rf /usr/local/share/codeql/{csharp,java,swift} \
&& sudo chmod +x /usr/local/share/codeql/codeql \
&& sudo ln -s /usr/local/share/codeql/codeql /usr/local/bin/codeql \
&& rm -rf codeql-bundle-linux64.tar.gz \
&& sudo mkdir /usr/local/share/doxygen-tag \
&& curl -L https://gcc.gnu.org/onlinedocs/gcc-13.2.0/libstdc++/api/libstdc++.tag | sudo tee /usr/local/share/doxygen-tag/libstdc++.tag >/dev/null \
&& git clone https://github.com/KDAB/codebrowser.git \
&& git -C ./codebrowser reset --hard 1808e96d4c62417952389d536576d53bb6da4053 \
&& cmake -S ./codebrowser -B ./codebrowser/build -DCMAKE_CXX_COMPILER=clang++-16 -DCMAKE_BUILD_TYPE=Release \
&& sudo make -C ./codebrowser/build install \
&& rm -rf ./codebrowser
