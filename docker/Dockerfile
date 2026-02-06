FROM ubuntu:24.04

SHELL ["/bin/bash", "-c"]

RUN apt-get update \
&& DEBIAN_FRONTEND=noninteractive apt-get install --assume-yes --no-install-recommends \
sudo tzdata ca-certificates git bash python3 python3-pip coreutils findutils procps grep sed mawk curl tar bzip2 \
build-essential binutils gcc-14 g++-14 binutils-gold clang-19 clang++-19 lld-19 ninja-build make cmake pkg-config \
libstdc++-14-dev libgtest-dev libssl-dev liblz4-dev libreadline-dev libgsl-dev libmpfrc++-dev llvm-19-dev libclang-19-dev libclang-rt-19-dev \
clang-format-19 clang-tidy-19 shfmt shellcheck black pylint rustc cargo rustfmt rust-clippy gdb ccache distcc valgrind elfutils \
cloc pre-commit doxygen doxygen-awesome-css graphviz supervisor docker.io docker-compose-v2 bash-completion man-db \
zip rsync \
&& apt-get clean \
&& rm -rf /var/lib/apt/lists/* \
&& pip3 install --no-cache-dir --break-system-packages compdb==0.2.0 ValgrindCI==0.3.0 sarif-tools==2.0.0 nodeenv==1.8.0 \
git+https://github.com/yuriisk/clang-tidy-converter.git@f07de2eca78637c47893ddb0dfdf150163572ab5 \
&& curl -L https://github.com/github/codeql-action/releases/download/codeql-bundle-v2.22.4/codeql-bundle-linux64.tar.gz | tar -xz -C /usr/local/share \
&& rm -rf /usr/local/share/codeql/{actions,csharp,csv,go,html,java,javascript,properties,python,ruby,rust,swift,xml,yaml} \
/usr/local/share/codeql/qlpacks/codeql/{actions-*,csharp-*,go-*,java-*,javascript-*,python-*,ruby-*,rust-*,swift-*} \
&& chmod +x /usr/local/share/codeql/codeql \
&& ln -s /usr/local/share/codeql/codeql /usr/local/bin/codeql \
&& mkdir -p /usr/local/share/doxygen-tag \
&& curl -L https://gcc.gnu.org/onlinedocs/gcc-14.2.0/libstdc++/api/libstdc++.tag -o /usr/local/share/doxygen-tag/libstdc++.tag \
&& git clone https://github.com/KDAB/codebrowser.git /tmp/codebrowser \
&& git -C /tmp/codebrowser reset --hard 9c819d5072dba9ba8eb07646d712b46229f7ecb5 \
&& cmake -S /tmp/codebrowser -B /tmp/codebrowser/build -DCMAKE_CXX_COMPILER=clang++-19 -DCMAKE_BUILD_TYPE=Release \
&& make -C /tmp/codebrowser/build install \
&& rm -rf /tmp/codebrowser
