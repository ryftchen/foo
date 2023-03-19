#!/usr/bin/env bash

declare -rA FOLDER=([proj]="foo" [app]="application" [util]="utility" [algo]="algorithm" [ds]="data_structure"
    [dp]="design_pattern" [num]="numeric" [tst]="test" [scr]="script" [doc]="document" [bld]="build" [temp]="temporary")
declare -r COMP_CMD="compile_commands.json"
declare -A ARGS=([help]=false [cleanup]=false [environment]=false [docker]=false [test]=false [release]=false
    [format]=false [lint]=false [count]=false [browser]=false [doxygen]=false)
declare -A DEV_OPT=([parallel]=0 [pch]=false [unity]=false [ccache]=false [distcc]=false [tmpfs]=false)
declare CMAKE_CACHE_ENTRY=""
declare CMAKE_BUILD_OPTION=""
declare BUILD_TYPE="Debug"
declare BUILD_APP_ONLY=false

function shellCommand()
{
    echo
    echo "$(date "+%b %d %T") $* START"
    /bin/bash -c "$@"
    echo "$(date "+%b %d %T") $* FINISH"
}

function abort()
{
    echo
    echo "build.sh: $*"
    exit 1
}

function signalHandler()
{
    tput sgr0
    if [[ ${ARGS[lint]} = true ]]; then
        local appCompCmd=${FOLDER[bld]}/${COMP_CMD}
        if [[ -f ./${appCompCmd}.bak ]]; then
            rm -rf "./${appCompCmd}" && mv "./${appCompCmd}.bak" "./${appCompCmd}"
        fi
        local tstCompCmd=${FOLDER[tst]}/${FOLDER[bld]}/${COMP_CMD}
        if [[ -f ./${tstCompCmd}.bak ]]; then
            rm -rf "./${tstCompCmd}" && mv "./${tstCompCmd}.bak" "./${tstCompCmd}"
        fi
    fi
    if [[ ${ARGS[doxygen]} = true ]] && [[ ${ARGS[release]} = false ]]; then
        sed -i "s/\(^PROJECT_NUMBER[ ]\+=\)\([ ]\+.*\)/\1/" "./${FOLDER[doc]}/Doxyfile"
        sed -i "s/\(^HTML_TIMESTAMP[ ]\+=\)\([ ]\+YES\)/\1 NO/" "./${FOLDER[doc]}/Doxyfile"
    fi
    exit 1
}

function parseArguments()
{
    while [[ $# -gt 0 ]]; do
        case $1 in
        -h | --help) ARGS[help]=true ;;
        -C | --cleanup) ARGS[cleanup]=true ;;
        -e | --environment) ARGS[environment]=true ;;
        -D | --docker) ARGS[docker]=true ;;
        -t | --test) ARGS[test]=true ;;
        -r | --release) ARGS[release]=true ;;
        -f | --format) ARGS[format]=true ;;
        -l | --lint) ARGS[lint]=true ;;
        -c | --count) ARGS[count]=true ;;
        -b | --browser) ARGS[browser]=true ;;
        -d | --doxygen) ARGS[doxygen]=true ;;
        *) abort "Unknown command line option: $1. Try using the --help option for information." ;;
        esac
        shift
    done
}

function checkBasicDependencies()
{
    if [[ ${ARGS[release]} = true ]]; then
        BUILD_TYPE="Release"
    fi
    if [[ $# -eq 0 ]] || {
        [[ $# -eq 1 ]] && [[ ${ARGS[release]} = true ]]
    }; then
        BUILD_APP_ONLY=true
    fi

    if ! command -v cmake >/dev/null 2>&1 || ! command -v ninja >/dev/null 2>&1; then
        abort "No cmake or ninja program. Please install it."
    fi
    if ! command -v clang-12 >/dev/null 2>&1 || ! command -v clang++-12 >/dev/null 2>&1; then
        abort "No clang-12 or clang++-12 program. Please install it."
    fi
}

function checkExtraDependencies()
{
    if [[ ${ARGS[format]} = true ]]; then
        if ! command -v clang-format-12 >/dev/null 2>&1 || ! command -v shfmt >/dev/null 2>&1 \
            || ! command -v black >/dev/null 2>&1; then
            abort "No clang-format, shfmt or black program. Please install it."
        fi
    fi

    if [[ ${ARGS[lint]} = true ]]; then
        if ! command -v clang-tidy-12 >/dev/null 2>&1 || ! command -v run-clang-tidy-12 >/dev/null 2>&1 \
            || ! command -v compdb >/dev/null 2>&1 || ! command -v shellcheck >/dev/null 2>&1 \
            || ! command -v pylint >/dev/null 2>&1; then
            abort "No clang-tidy (including run-clang-tidy-12, compdb), shellcheck or pylint program. \
Please install it."
        fi
        if [[ ${DEV_OPT[pch]} = true ]] || [[ ${DEV_OPT[unity]} = true ]]; then
            abort "Due to the unconventional ${COMP_CMD} file, the --lint option cannot run if the FOO_BLD_PCH or \
FOO_BLD_UNITY is turned on."
        fi
    fi

    if [[ ${ARGS[count]} = true ]]; then
        if ! command -v cloc >/dev/null 2>&1; then
            abort "No cloc program. Please install it."
        fi
    fi

    if [[ ${ARGS[browser]} = true ]]; then
        if ! command -v codebrowser_generator >/dev/null 2>&1 \
            || ! command -v codebrowser_indexgenerator >/dev/null 2>&1; then
            abort "No codebrowser_generator or codebrowser_indexgenerator program. Please install it."
        fi
        if [[ ${DEV_OPT[pch]} = true ]] || [[ ${DEV_OPT[unity]} = true ]]; then
            abort "Due to the unconventional ${COMP_CMD} file, the --browser option cannot run if the FOO_BLD_PCH or \
FOO_BLD_UNITY is turned on."
        fi
    fi

    if [[ ${ARGS[doxygen]} = true ]]; then
        if ! command -v doxygen >/dev/null 2>&1 || ! command -v dot >/dev/null 2>&1; then
            abort "No doxygen or dot program. Please install it."
        fi
    fi
}

function setCompileEnvironment()
{
    local tmpfsSubFolder=$1 tmpfsSize=$2

    export CC=/usr/bin/clang-12 CXX=/usr/bin/clang++-12
    if [[ -f ./${FOLDER[scr]}/.env ]]; then
        # shellcheck source=/dev/null
        source "./${FOLDER[scr]}/.env"
        if [[ -n ${FOO_BLD_PARALLEL} ]] && [[ ${FOO_BLD_PARALLEL} =~ ^[0-9]+$ ]]; then
            DEV_OPT[parallel]=${FOO_BLD_PARALLEL}
        fi
        if [[ -n ${FOO_BLD_PCH} ]] && [[ ${FOO_BLD_PCH} = "on" ]]; then
            DEV_OPT[pch]=true
        fi
        if [[ -n ${FOO_BLD_UNITY} ]] && [[ ${FOO_BLD_UNITY} = "on" ]]; then
            DEV_OPT[unity]=true
        fi
        if [[ -n ${FOO_BLD_CCACHE} ]] && [[ ${FOO_BLD_CCACHE} = "on" ]]; then
            DEV_OPT[ccache]=true
        fi
        if [[ -n ${FOO_BLD_DISTCC} ]] && [[ ${FOO_BLD_DISTCC} = "on" ]]; then
            DEV_OPT[distcc]=true
        fi
        if [[ -n ${FOO_BLD_TMPFS} ]] && [[ ${FOO_BLD_TMPFS} = "on" ]]; then
            DEV_OPT[tmpfs]=true
        fi
    fi

    if [[ ! ${DEV_OPT[parallel]} -eq 0 ]]; then
        CMAKE_BUILD_OPTION=" -j ${DEV_OPT[parallel]}"
    fi
    CMAKE_CACHE_ENTRY=" -DCMAKE_BUILD_TYPE=${BUILD_TYPE}"
    if [[ ${DEV_OPT[pch]} = true ]]; then
        CMAKE_CACHE_ENTRY="${CMAKE_CACHE_ENTRY} -D_TOOLCHAIN_PCH=ON"
    fi
    if [[ ${DEV_OPT[unity]} = true ]]; then
        CMAKE_CACHE_ENTRY="${CMAKE_CACHE_ENTRY} -D_TOOLCHAIN_UNITY=ON"
    fi
    if [[ ${DEV_OPT[ccache]} = true ]]; then
        CMAKE_CACHE_ENTRY="${CMAKE_CACHE_ENTRY} -D_TOOLCHAIN_CCACHE=ON"
        if [[ ${DEV_OPT[distcc]} = true ]]; then
            if command -v ccache >/dev/null 2>&1 && command -v distcc >/dev/null 2>&1; then
                export CCACHE_PREFIX=distcc
            fi
        fi
    fi
    if [[ ${DEV_OPT[distcc]} = true ]]; then
        CMAKE_CACHE_ENTRY="${CMAKE_CACHE_ENTRY} -D_TOOLCHAIN_DISTCC=ON"
        if [[ -z ${DISTCC_HOSTS} ]]; then
            export DISTCC_HOSTS=localhost
        fi
    fi
    if [[ ${DEV_OPT[tmpfs]} = true ]]; then
        if [[ ! -d ./${tmpfsSubFolder} ]]; then
            mkdir "./${tmpfsSubFolder}"
        fi
        if ! df -h -t tmpfs | grep -q "${FOLDER[proj]}/${tmpfsSubFolder}" 2>/dev/null; then
            shellCommand "mount -t tmpfs -o size=${tmpfsSize} tmpfs ./${tmpfsSubFolder}"
        fi
    elif df -h -t tmpfs | grep -q "${FOLDER[proj]}/${tmpfsSubFolder}" 2>/dev/null; then
        shellCommand "umount ./${tmpfsSubFolder}"
    fi
}

function performBuilding()
{
    setCompileEnvironment "${FOLDER[bld]}" "256m"

    shellCommand "cmake -S . -B ./${FOLDER[bld]} -G Ninja""${CMAKE_CACHE_ENTRY}"
    if [[ ${BUILD_APP_ONLY} = true ]]; then
        tput setaf 2 && tput bold
        shellCommand "cmake --build ./${FOLDER[bld]}""${CMAKE_BUILD_OPTION}"
        tput sgr0
        exit 0
    fi
    shellCommand "cmake -S ./${FOLDER[tst]} -B ./${FOLDER[tst]}/${FOLDER[bld]} -G Ninja""${CMAKE_CACHE_ENTRY}"
}

function performHelpOption()
{
    if [[ ${ARGS[help]} = true ]]; then
        echo "Usage: build.sh <options...>"
        echo
        echo "Optional:"
        echo "-h, --help           show help and exit"
        echo "-C, --cleanup        cleanup folder and exit"
        echo "-e, --environment    create env configuration and exit"
        echo "-D, --docker         construct docker container and exit"
        echo "-t, --test           build unit test and exit"
        echo "-r, --release        set as release version"
        echo "-f, --format         format all code"
        echo "-l, --lint           lint all code"
        echo "-c, --count          count lines of code"
        echo "-b, --browser        document by code browser"
        echo "-d, --doxygen        document by doxygen"
        exit 0
    fi
}

function performCleanupOption()
{
    if [[ ${ARGS[cleanup]} = true ]]; then
        shellCommand "find ./ -maxdepth 2 -type d | sed 1d \
| grep -E '(${FOLDER[bld]}|${FOLDER[temp]}|browser|doxygen|__pycache__)$' | xargs -i rm -rf {}"
        shellCommand "rm -rf ./${FOLDER[scr]}/.env ./core.* ./vgcore.* ./*.profraw"
        exit 0
    fi
}

function performEnvironmentOption()
{
    if [[ ${ARGS[environment]} = true ]]; then
        shellCommand "cat <<EOF >./${FOLDER[scr]}/.env
#!/bin/false

FOO_BLD_PARALLEL=0
FOO_BLD_PCH=off
FOO_BLD_UNITY=off
FOO_BLD_CCACHE=off
FOO_BLD_DISTCC=off
FOO_BLD_TMPFS=off

export FOO_BLD_PARALLEL FOO_BLD_PCH FOO_BLD_UNITY FOO_BLD_CCACHE FOO_BLD_DISTCC FOO_BLD_TMPFS
return 0
EOF"
        exit 0
    fi
}

function performContainerOption()
{
    if [[ ${ARGS[docker]} = true ]]; then
        if command -v docker >/dev/null 2>&1 && command -v docker-compose >/dev/null 2>&1; then
            echo "Please confirm whether continue constructing the docker container. (y or n)"
            local oldStty
            oldStty=$(stty -g)
            stty raw -echo
            local answer
            answer=$(while ! head -c 1 | grep -i '[ny]'; do true; done)
            stty "${oldStty}"
            if echo "${answer}" | grep -iq '^y'; then
                echo "Yes"
            else
                echo "No"
                exit 0
            fi
        else
            abort "No docker or docker-compose program. Please install it."
        fi

        if ! docker ps -a --format "{{lower .Image}} {{lower .Names}}" \
            | grep -q "ryftchen/${FOLDER[proj]}:latest" "${FOLDER[proj]}_dev" 2>/dev/null; then
            shellCommand "docker-compose -f ./docker/docker-compose.yml up -d"
            exit 0
        else
            abort "The container exists."
        fi
    fi
}

function performTestOption()
{
    if [[ ${ARGS[test]} = true ]]; then
        setCompileEnvironment "${FOLDER[tst]}/${FOLDER[bld]}" "128m"

        shellCommand "cmake -S ./${FOLDER[tst]} -B ./${FOLDER[tst]}/${FOLDER[bld]} -G Ninja""${CMAKE_CACHE_ENTRY}"
        tput setaf 2 && tput bold
        shellCommand "cmake --build ./${FOLDER[tst]}/${FOLDER[bld]}""${CMAKE_BUILD_OPTION}"
        tput sgr0
        exit 0
    fi
}

function performFormatOption()
{
    if [[ ${ARGS[format]} = true ]]; then
        shellCommand "find ./${FOLDER[app]} ./${FOLDER[util]} ./${FOLDER[algo]} ./${FOLDER[ds]} ./${FOLDER[dp]} \
./${FOLDER[num]} ./${FOLDER[tst]} -name *.cpp -o -name *.hpp -o -name *.tpp | grep -v '/${FOLDER[bld]}/' \
| xargs clang-format-12 -i --verbose --Werror"
        shellCommand "shfmt -l -w ./${FOLDER[scr]}/*.sh"
        shellCommand "black --config ./.toml ./${FOLDER[scr]}/*.py"
    fi
}

function performLintOption()
{
    if [[ ${ARGS[lint]} = true ]]; then
        local appCompCmd=${FOLDER[bld]}/${COMP_CMD}
        if [[ ! -f ./${appCompCmd} ]]; then
            abort "There is no ${COMP_CMD} file in the ${FOLDER[bld]} folder. Please generate it."
        fi
        compdb -p "./${FOLDER[bld]}" list >"./${COMP_CMD}" && mv "./${appCompCmd}" "./${appCompCmd}.bak" \
            && mv "./${COMP_CMD}" "./${FOLDER[bld]}"
        while true; do
            local line
            line=$(grep -n '.tpp' "./${appCompCmd}" | head -n 1 | cut -d : -f 1)
            if ! [[ ${line} =~ ^[0-9]+$ ]]; then
                break
            fi
            sed -i $(("${line}" - 2)),$(("${line}" + 3))d "./${appCompCmd}"
        done
        shellCommand "find ./${FOLDER[app]} ./${FOLDER[util]} ./${FOLDER[algo]} ./${FOLDER[ds]} ./${FOLDER[dp]} \
./${FOLDER[num]} -name *.cpp -o -name *.hpp | xargs run-clang-tidy-12 -p ./${FOLDER[bld]} -quiet"
        shellCommand "find ./${FOLDER[app]} ./${FOLDER[util]} ./${FOLDER[algo]} ./${FOLDER[ds]} ./${FOLDER[dp]} \
./${FOLDER[num]} -name *.tpp | xargs clang-tidy-12 --use-color -p ./${FOLDER[bld]} -quiet"
        rm -rf "./${appCompCmd}" && mv "./${appCompCmd}.bak" "./${appCompCmd}"

        local tstCompCmd=${FOLDER[tst]}/${FOLDER[bld]}/${COMP_CMD}
        if [[ ! -f ./${tstCompCmd} ]]; then
            abort "There is no ${COMP_CMD} file in the ${FOLDER[tst]}/${FOLDER[bld]} folder. Please generate it."
        fi
        compdb -p "./${FOLDER[tst]}/${FOLDER[bld]}" list >"./${COMP_CMD}" \
            && mv "./${tstCompCmd}" "./${tstCompCmd}.bak" && mv "./${COMP_CMD}" "./${FOLDER[tst]}/${FOLDER[bld]}"
        shellCommand "find ./${FOLDER[tst]} -name *.cpp \
| xargs run-clang-tidy-12 -p ./${FOLDER[tst]}/${FOLDER[bld]} -quiet"
        rm -rf "./${tstCompCmd}" && mv "./${tstCompCmd}.bak" "./${tstCompCmd}"

        shellCommand "shellcheck ./${FOLDER[scr]}/*.sh"
        shellCommand "pylint --rcfile=./.pylintrc ./${FOLDER[scr]}/*.py"
    fi
}

function performCountOption()
{
    if [[ ${ARGS[count]} = true ]]; then
        shellCommand "find ./${FOLDER[app]} ./${FOLDER[util]} ./${FOLDER[algo]} ./${FOLDER[ds]} ./${FOLDER[dp]} \
./${FOLDER[num]} ./${FOLDER[tst]} -name *.cpp -o -name *.hpp -o -name *.tpp | grep -v '/${FOLDER[bld]}/' \
| xargs cloc --by-file-by-lang --force-lang='C++',tpp --include-lang='C++','C/C++ Header' --by-percent=cmb"
    fi
}

function performBrowserOption()
{
    if [[ ${ARGS[browser]} = true ]]; then
        local commitId
        commitId=$(git rev-parse --short @)
        if [[ -z ${commitId} ]]; then
            commitId="local"
        fi
        if [[ -d ./${FOLDER[temp]} ]]; then
            local lastTar="${FOLDER[proj]}_browser_${commitId}.tar.bz2"
            if [[ -f ./${FOLDER[temp]}/${lastTar} ]]; then
                local timeInterval=$(($(date +%s) - $(stat -L --format %Y "./${FOLDER[temp]}/${lastTar}")))
                if [[ ${timeInterval} -lt 10 ]]; then
                    abort "The latest browser tarball ${FOLDER[temp]}/${lastTar} has been generated since \
${timeInterval}s ago."
                fi
            fi
            packageForBrowser "${commitId}"
        else
            mkdir "./${FOLDER[temp]}"
            packageForBrowser "${commitId}"
        fi
    fi
}

function packageForBrowser()
{
    local commitId=$1

    if [[ ! -f ./${FOLDER[bld]}/${COMP_CMD} ]]; then
        abort "There is no ${COMP_CMD} file in the ${FOLDER[bld]} folder. Please generate it."
    fi
    local browserFolder="browser"
    local tarFile="${FOLDER[proj]}_${browserFolder}_${commitId}.tar.bz2"
    rm -rf "./${FOLDER[temp]}/${FOLDER[proj]}_${browserFolder}"_*.tar.bz2 "./${FOLDER[doc]}/${browserFolder}"

    mkdir -p "./${FOLDER[doc]}/${browserFolder}"
    shellCommand "codebrowser_generator -color -a -b ./${FOLDER[bld]}/${COMP_CMD} -o ./${FOLDER[doc]}/${browserFolder} \
-p ${FOLDER[proj]}:.:${commitId} -d ./data"
    shellCommand "codebrowser_generator -color -a -b ./${FOLDER[tst]}/${FOLDER[bld]}/${COMP_CMD} \
-o ./${FOLDER[doc]}/${browserFolder} -p ${FOLDER[proj]}:.:${commitId} -d ./data"
    shellCommand "codebrowser_indexgenerator ./${FOLDER[doc]}/${browserFolder} -d ./data"
    shellCommand "cp -rf /usr/local/share/woboq/data ./${FOLDER[doc]}/${browserFolder}/"
    shellCommand "tar -jcvf ./${FOLDER[temp]}/${tarFile} -C ./${FOLDER[doc]} ${browserFolder} >/dev/null"
}

function performDoxygenOption()
{
    if [[ ${ARGS[doxygen]} = true ]]; then
        local commitId
        commitId=$(git rev-parse --short @)
        if [[ -z ${commitId} ]]; then
            commitId="local"
        fi
        if [[ -d ./${FOLDER[temp]} ]]; then
            local lastTar="${FOLDER[proj]}_doxygen_${commitId}.tar.bz2"
            if [[ -f ./${FOLDER[temp]}/${lastTar} ]]; then
                local timeInterval=$(($(date +%s) - $(stat -L --format %Y "./${FOLDER[temp]}/${lastTar}")))
                if [[ ${timeInterval} -lt 10 ]]; then
                    abort "The latest doxygen tarball ${FOLDER[temp]}/${lastTar} has been generated since \
${timeInterval}s ago."
                fi
            fi
            packageForDoxygen "${commitId}"
        else
            mkdir "./${FOLDER[temp]}"
            packageForDoxygen "${commitId}"
        fi
    fi
}

function packageForDoxygen()
{
    local commitId=$1

    local doxygenFolder="doxygen"
    local tarFile="${FOLDER[proj]}_${doxygenFolder}_${commitId}.tar.bz2"
    rm -rf "./${FOLDER[temp]}/${FOLDER[proj]}_${doxygenFolder}"_*.tar.bz2 "./${FOLDER[doc]}/${doxygenFolder}"

    mkdir -p "./${FOLDER[doc]}/${doxygenFolder}"
    if [[ ${ARGS[release]} = false ]]; then
        sed -i "s/\(^PROJECT_NUMBER[ ]\+=\)/\1 \"@ $(git rev-parse --short @)\"/" "./${FOLDER[doc]}/Doxyfile"
        sed -i "s/\(^HTML_TIMESTAMP[ ]\+=\)\([ ]\+NO\)/\1 YES/" "./${FOLDER[doc]}/Doxyfile"
    fi
    shellCommand "doxygen ./${FOLDER[doc]}/Doxyfile >/dev/null"
    shellCommand "tar -jcvf ./${FOLDER[temp]}/${tarFile} -C ./${FOLDER[doc]} ${doxygenFolder} >/dev/null"
    if [[ ${ARGS[release]} = false ]]; then
        sed -i "s/\(^PROJECT_NUMBER[ ]\+=\)\([ ]\+.*\)/\1/" "./${FOLDER[doc]}/Doxyfile"
        sed -i "s/\(^HTML_TIMESTAMP[ ]\+=\)\([ ]\+YES\)/\1 NO/" "./${FOLDER[doc]}/Doxyfile"
    fi
}

function main()
{
    cd "${0%%"${FOLDER[scr]}"*}" || exit 1
    export TERM=linux TERMINFO=/etc/terminfo
    trap "signalHandler" INT TERM

    parseArguments "$@"
    performHelpOption
    performCleanupOption
    performEnvironmentOption
    performContainerOption

    checkBasicDependencies "$@"
    performTestOption
    performBuilding

    checkExtraDependencies
    performFormatOption
    performLintOption
    performCountOption
    performBrowserOption
    performDoxygenOption
}

main "$@"
