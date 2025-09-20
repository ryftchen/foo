FROM ubuntu:24.04

SHELL ["/bin/bash", "-c"]

RUN apt-get update \
&& DEBIAN_FRONTEND=noninteractive apt-get install --assume-yes --no-install-recommends sudo tzdata ca-certificates \
&& sudo apt-get update \
&& sudo apt-get install --assume-yes --no-install-recommends bash coreutils findutils procps grep sed mawk \
build-essential binutils gcc-14 g++-14 binutils-gold clang-19 clang++-19 lld-19 ninja-build make cmake \
libstdc++-14-dev libgtest-dev libssl-dev liblz4-dev libreadline-dev libgsl-dev libmpfrc++-dev llvm-19-dev libclang-19-dev libclang-rt-19-dev \
clang-format-19 clang-tidy-19 shfmt shellcheck black pylint rustc cargo rustfmt rust-clippy gdb ccache distcc valgrind \
cloc pre-commit doxygen doxygen-awesome-css graphviz git python3 python3-pip docker.io docker-compose-v2 npm \
elfutils psmisc net-tools curl tar zip rsync bash-completion \
&& sudo apt-get clean \
&& sudo rm -rf /var/lib/apt/lists/* \
&& sudo pip3 install --no-cache-dir --break-system-packages compdb==0.2.0 ValgrindCI==0.3.0 sarif-tools==2.0.0 \
git+https://github.com/yuriisk/clang-tidy-converter.git@f07de2eca78637c47893ddb0dfdf150163572ab5 \
&& sudo npm install --no-save --global --no-package-lock cspell@7.3.9 \
&& sudo npm cache clean --force \
&& cd /root \
&& curl -L https://github.com/github/codeql-action/releases/download/codeql-bundle-v2.22.4/codeql-bundle-linux64.tar.gz | tee codeql-bundle-linux64.tar.gz >/dev/null \
&& sudo tar -zxvf codeql-bundle-linux64.tar.gz -C /usr/local/share >/dev/null \
&& sudo rm -rf /usr/local/share/codeql/{actions,csharp,csv,go,html,java,javascript,properties,python,ruby,rust,swift,xml,yaml} \
/usr/local/share/codeql/qlpacks/codeql/{actions-*,csharp-*,go-*,java-*,javascript-*,python-*,ruby-*,rust-*,swift-*} \
&& sudo chmod +x /usr/local/share/codeql/codeql \
&& sudo ln -s /usr/local/share/codeql/codeql /usr/local/bin/codeql \
&& rm -rf codeql-bundle-linux64.tar.gz \
&& sudo mkdir /usr/local/share/doxygen-tag \
&& curl -L https://gcc.gnu.org/onlinedocs/gcc-14.2.0/libstdc++/api/libstdc++.tag | sudo tee /usr/local/share/doxygen-tag/libstdc++.tag >/dev/null \
&& git clone https://github.com/KDAB/codebrowser.git \
&& git -C ./codebrowser reset --hard 9c819d5072dba9ba8eb07646d712b46229f7ecb5 \
&& cmake -S ./codebrowser -B ./codebrowser/build -DCMAKE_CXX_COMPILER=clang++-19 -DCMAKE_BUILD_TYPE=Release \
&& sudo make -C ./codebrowser/build install \
&& rm -rf ./codebrowser
