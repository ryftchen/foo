#!/usr/bin/env bash

declare -r PROJECT_FOLDER="foo"
declare -r APPLICATION_FOLDER="application"
declare -r UTILITY_FOLDER="utility"
declare -r ALGORITHM_FOLDER="algorithm"
declare -r DATA_STRUCTURE_FOLDER="data_structure"
declare -r DESIGN_PATTERN_FOLDER="design_pattern"
declare -r NUMERIC_FOLDER="numeric"
declare -r TEST_FOLDER="test"
declare -r SCRIPT_FOLDER="script"
declare -r DOCKER_FOLDER="docker"
declare -r DOCUMENT_FOLDER="document"
declare -r BUILD_FOLDER="build"
declare -r TEMPORARY_FOLDER="temporary"
declare -r COMPILE_COMMANDS="compile_commands.json"

ARGS_HELP=false
ARGS_CLEANUP=false
ARGS_ENVIRONMENT=false
ARGS_CONTAINER=false
ARGS_TEST=false
ARGS_RELEASE=false
ARGS_FORMAT=false
ARGS_LINT=false
ARGS_BROWSER=false
ARGS_DOXYGEN=false
ENHANCED_DEV_PARALLEL=0
ENHANCED_DEV_PCH=false
ENHANCED_DEV_CCACHE=false
ENHANCED_DEV_DISTCC=false
ENHANCED_DEV_TMPFS=false

CMAKE_CACHE_ENTRY=""
CMAKE_BUILD_OPTION=""
BUILD_TYPE="Debug"
TO_COMPILE_SOURCE_ONLY=false

shellCommand()
{
    echo
    echo "$(date "+%b %d %T") $* START"
    /bin/bash -c "$@"
    echo "$(date "+%b %d %T") $* FINISH"
}

exception()
{
    echo
    echo "build.sh: $*"
    exit 1
}

signalHandler()
{
    tput sgr0
    if [[ "${ARGS_LINT}" = true ]]; then
        if [[ -f ./"${BUILD_FOLDER}"/"${COMPILE_COMMANDS}".bak ]]; then
            rm -rf ./"${BUILD_FOLDER}"/"${COMPILE_COMMANDS}" \
                && mv ./"${BUILD_FOLDER}"/"${COMPILE_COMMANDS}".bak ./"${BUILD_FOLDER}"/"${COMPILE_COMMANDS}"
        fi
        local testBuildFolder="${TEST_FOLDER}/${BUILD_FOLDER}"
        if [[ -f ./"${testBuildFolder}"/"${COMPILE_COMMANDS}".bak ]]; then
            rm -rf ./"${testBuildFolder}"/"${COMPILE_COMMANDS}" \
                && mv ./"${testBuildFolder}"/"${COMPILE_COMMANDS}".bak ./"${testBuildFolder}"/"${COMPILE_COMMANDS}"
        fi
    fi
    if [[ "${ARGS_DOXYGEN}" = true ]] && [[ "${ARGS_RELEASE}" = false ]]; then
        local doxygenConfig="Doxyfile"
        sed -i "s/\(^PROJECT_NUMBER[ ]\+=\)\([ ]\+.*\)/\1/" ./"${DOCUMENT_FOLDER}"/"${doxygenConfig}"
        sed -i "s/\(^HTML_TIMESTAMP[ ]\+=\)\([ ]\+YES\)/\1 NO/" ./"${DOCUMENT_FOLDER}"/"${doxygenConfig}"
    fi
    exit 1
}

parseArguments()
{
    while [[ "$#" -gt 0 ]]; do
        case $1 in
        -h | --help) ARGS_HELP=true ;;
        -C | --cleanup) ARGS_CLEANUP=true ;;
        -e | --environment) ARGS_ENVIRONMENT=true ;;
        -c | --container) ARGS_CONTAINER=true ;;
        -t | --test) ARGS_TEST=true ;;
        -r | --release) ARGS_RELEASE=true ;;
        -f | --format) ARGS_FORMAT=true ;;
        -l | --lint) ARGS_LINT=true ;;
        -b | --browser) ARGS_BROWSER=true ;;
        -d | --doxygen) ARGS_DOXYGEN=true ;;
        *) exception "Unknown command line option: $1. Try using the --help option for information." ;;
        esac
        shift
    done
}

checkBasicDependencies()
{
    if [[ "${ARGS_RELEASE}" = true ]]; then
        BUILD_TYPE="Release"
    fi
    if [[ "$#" -eq 0 ]] || {
        [[ "$#" -eq 1 ]] && [[ "${ARGS_RELEASE}" = true ]]
    }; then
        TO_COMPILE_SOURCE_ONLY=true
    fi

    if
        ! command -v cmake >/dev/null 2>&1 \
            || ! command -v ninja >/dev/null 2>&1
    then
        exception "No cmake or ninja program. Please install it."
    fi
    if
        ! command -v clang-12 >/dev/null 2>&1 \
            || ! command -v clang++-12 >/dev/null 2>&1
    then
        exception "No clang-12 or clang++-12 program. Please install it."
    fi
}

checkExtraDependencies()
{
    if [[ "${ARGS_FORMAT}" = true ]]; then
        if
            ! command -v clang-format-12 >/dev/null 2>&1 \
                || ! command -v shfmt >/dev/null 2>&1 \
                || ! command -v black >/dev/null 2>&1
        then
            exception "No clang-format, shfmt or black program. Please install it."
        fi
    fi

    if [[ "${ARGS_LINT}" = true ]]; then
        if
            ! command -v clang-tidy-12 >/dev/null 2>&1 \
                || ! command -v run-clang-tidy-12 >/dev/null 2>&1 \
                || ! command -v compdb >/dev/null 2>&1 \
                || ! command -v shellcheck >/dev/null 2>&1 \
                || ! command -v pylint >/dev/null 2>&1
        then
            exception "No clang-tidy (including run-clang-tidy-12, compdb), shellcheck or pylint program. \
Please install it."
        fi
    fi

    if [[ "${ARGS_BROWSER}" = true ]]; then
        if
            ! command -v codebrowser_generator >/dev/null 2>&1 \
                || ! command -v codebrowser_indexgenerator >/dev/null 2>&1
        then
            exception "No codebrowser_generator or codebrowser_indexgenerator program. Please install it."
        fi
    fi

    if [[ "${ARGS_DOXYGEN}" = true ]]; then
        if
            ! command -v doxygen >/dev/null 2>&1 \
                || ! command -v dot >/dev/null 2>&1
        then
            exception "No doxygen or dot program. Please install it."
        fi
    fi
}

setCompileEnvironment()
{
    local tmpfsSubFolder=$1 tmpfsSize=$2

    export CC=/usr/bin/clang-12 CXX=/usr/bin/clang++-12
    if [[ -f ./"${SCRIPT_FOLDER}"/.env ]]; then
        # shellcheck source=/dev/null
        source ./"${SCRIPT_FOLDER}"/.env
        if [[ -n "${FOO_BLD_PARALLEL}" ]] && [[ "${FOO_BLD_PARALLEL}" =~ ^[0-9]+$ ]]; then
            ENHANCED_DEV_PARALLEL=${FOO_BLD_PARALLEL}
        fi
        if [[ -n "${FOO_BLD_PCH}" ]] && [[ "${FOO_BLD_PCH}" = "on" ]]; then
            ENHANCED_DEV_PCH=true
        fi
        if [[ -n "${FOO_BLD_CCACHE}" ]] && [[ "${FOO_BLD_CCACHE}" = "on" ]]; then
            ENHANCED_DEV_CCACHE=true
        fi
        if [[ -n "${FOO_BLD_DISTCC}" ]] && [[ "${FOO_BLD_DISTCC}" = "on" ]]; then
            ENHANCED_DEV_DISTCC=true
        fi
        if [[ -n "${FOO_BLD_TMPFS}" ]] && [[ "${FOO_BLD_TMPFS}" = "on" ]]; then
            ENHANCED_DEV_TMPFS=true
        fi
    fi

    if [[ ! "${ENHANCED_DEV_PARALLEL}" -eq 0 ]]; then
        CMAKE_BUILD_OPTION="-j ${ENHANCED_DEV_PARALLEL}"
    fi
    CMAKE_CACHE_ENTRY="-DCMAKE_BUILD_TYPE=${BUILD_TYPE}"
    if [[ "${ENHANCED_DEV_PCH}" = true ]]; then
        CMAKE_CACHE_ENTRY="${CMAKE_CACHE_ENTRY} -D_TOOLCHAIN_PCH=ON"
    fi
    if [[ "${ENHANCED_DEV_CCACHE}" = true ]]; then
        CMAKE_CACHE_ENTRY="${CMAKE_CACHE_ENTRY} -D_TOOLCHAIN_CCACHE=ON"
        if [[ "${ENHANCED_DEV_DISTCC}" = true ]] \
            && command -v ccache >/dev/null 2>&1 && command -v distcc >/dev/null 2>&1; then
            export CCACHE_PREFIX=distcc
        fi
    fi
    if [[ "${ENHANCED_DEV_DISTCC}" = true ]]; then
        CMAKE_CACHE_ENTRY="${CMAKE_CACHE_ENTRY} -D_TOOLCHAIN_DISTCC=ON"
        if [[ -z "${DISTCC_HOSTS}" ]]; then
            export DISTCC_HOSTS=localhost
        fi
    fi
    if [[ "${ENHANCED_DEV_TMPFS}" = true ]]; then
        if [[ ! -d ./${tmpfsSubFolder} ]]; then
            mkdir "./${tmpfsSubFolder}"
        fi
        if ! df -h -t tmpfs | grep -q "${PROJECT_FOLDER}/${tmpfsSubFolder}" 2>/dev/null; then
            shellCommand "mount -t tmpfs -o size=${tmpfsSize} tmpfs ./${tmpfsSubFolder}"
        fi
    elif df -h -t tmpfs | grep -q "${PROJECT_FOLDER}/${tmpfsSubFolder}" 2>/dev/null; then
        shellCommand "umount ./${tmpfsSubFolder}"
    fi
}

performBuilding()
{
    setCompileEnvironment "${BUILD_FOLDER}" "256m"

    shellCommand "cmake -S . -B ./${BUILD_FOLDER} -G Ninja ${CMAKE_CACHE_ENTRY}"
    if [[ "${TO_COMPILE_SOURCE_ONLY}" = true ]]; then
        tput setaf 2 && tput bold
        shellCommand "cmake --build ./${BUILD_FOLDER} ${CMAKE_BUILD_OPTION}"
        tput sgr0
        exit 0
    fi
    shellCommand "cmake -S ./${TEST_FOLDER} -B ./${TEST_FOLDER}/${BUILD_FOLDER} -G Ninja ${CMAKE_CACHE_ENTRY}"
}

performHelpOption()
{
    if [[ "${ARGS_HELP}" = true ]]; then
        echo "Usage: build.sh <options...>"
        echo
        echo "Optional:"
        echo "-h, --help           show help and exit"
        echo "-C, --cleanup        cleanup folder and exit"
        echo "-e, --environment    create env configuration and exit"
        echo "-c, --container      construct docker container and exit"
        echo "-t, --test           build unit test and exit"
        echo "-r, --release        set as release version"
        echo "-f, --format         format all code"
        echo "-l, --lint           lint all code"
        echo "-b, --browser        document by code browser"
        echo "-d, --doxygen        document by doxygen"
        exit 0
    fi
}

performCleanupOption()
{
    if [[ "${ARGS_CLEANUP}" = true ]]; then
        shellCommand "find ./ -maxdepth 2 -type d | sed 1d \
| grep -E '(${BUILD_FOLDER}|${TEMPORARY_FOLDER}|browser|doxygen|__pycache__)$' | xargs -i rm -rf {}"
        shellCommand "rm -rf ./${SCRIPT_FOLDER}/.env ./core* ./vgcore* ./*.profraw"
        exit 0
    fi
}

performEnvironmentOption()
{
    if [[ "${ARGS_ENVIRONMENT}" = true ]]; then
        shellCommand "cat <<EOF >./${SCRIPT_FOLDER}/.env
#!/bin/false

FOO_BLD_PARALLEL=0
FOO_BLD_PCH=off
FOO_BLD_CCACHE=off
FOO_BLD_DISTCC=off
FOO_BLD_TMPFS=off

export FOO_BLD_PARALLEL FOO_BLD_PCH FOO_BLD_CCACHE FOO_BLD_DISTCC FOO_BLD_TMPFS
return 0
EOF"
        exit 0
    fi
}

performContainerOption()
{
    if [[ "${ARGS_CONTAINER}" = true ]]; then
        if command -v docker >/dev/null 2>&1 && command -v docker-compose >/dev/null 2>&1; then
            echo "Please confirm whether continue constructing the docker container. (y or n)"
            local oldStty answer
            oldStty=$(stty -g)
            stty raw -echo
            answer=$(while ! head -c 1 | grep -i '[ny]'; do true; done)
            stty "${oldStty}"
            if echo "${answer}" | grep -iq '^y'; then
                echo "Yes"
            else
                echo "No"
                exit 0
            fi
        else
            exception "No docker or docker-compose program. Please install it."
        fi

        if ! docker ps -a --format "{{lower .Image}} {{lower .Names}}" \
            | grep -q "ryftchen/${PROJECT_FOLDER}:latest ${PROJECT_FOLDER}_dev" 2>/dev/null; then
            shellCommand "docker-compose -f ./${DOCKER_FOLDER}/docker-compose.yml up -d"
            exit 0
        else
            exception "The container exists."
        fi
    fi
}

performTestOption()
{
    if [[ "${ARGS_TEST}" = true ]]; then
        setCompileEnvironment "${TEST_FOLDER}/${BUILD_FOLDER}" "128m"

        shellCommand "cmake -S ./${TEST_FOLDER} -B ./${TEST_FOLDER}/${BUILD_FOLDER} -G Ninja ${CMAKE_CACHE_ENTRY}"
        tput setaf 2 && tput bold
        shellCommand "cmake --build ./${TEST_FOLDER}/${BUILD_FOLDER} ${CMAKE_BUILD_OPTION}"
        tput sgr0
        exit 0
    fi
}

performFormatOption()
{
    if [[ "${ARGS_FORMAT}" = true ]]; then
        shellCommand "find ./${APPLICATION_FOLDER} ./${UTILITY_FOLDER} ./${ALGORITHM_FOLDER} \
./${DATA_STRUCTURE_FOLDER} ./${DESIGN_PATTERN_FOLDER} ./${NUMERIC_FOLDER} ./${TEST_FOLDER} \
-name *.cpp -o -name *.hpp -o -name *.tpp | grep -v '/${BUILD_FOLDER}/' | xargs clang-format-12 -i --verbose --Werror"
        shellCommand "shfmt -l -w ./${SCRIPT_FOLDER}/*.sh"
        shellCommand "black --config ./.toml ./${SCRIPT_FOLDER}/*.py"
    fi
}

performLintOption()
{
    if [[ "${ARGS_LINT}" = true ]]; then
        if [[ ! -f ./"${BUILD_FOLDER}"/"${COMPILE_COMMANDS}" ]]; then
            exception "There is no ${COMPILE_COMMANDS} file in the ${BUILD_FOLDER} folder. Please generate it."
        fi
        compdb -p ./"${BUILD_FOLDER}" list >./"${COMPILE_COMMANDS}" \
            && mv ./"${BUILD_FOLDER}"/"${COMPILE_COMMANDS}" ./"${BUILD_FOLDER}"/"${COMPILE_COMMANDS}".bak \
            && mv ./"${COMPILE_COMMANDS}" ./"${BUILD_FOLDER}"
        while true; do
            local line
            line=$(grep -n '.tpp' ./"${BUILD_FOLDER}"/"${COMPILE_COMMANDS}" | head -n 1 | cut -d : -f 1)
            if ! [[ "${line}" =~ ^[0-9]+$ ]]; then
                break
            fi
            sed -i $(("${line}" - 2)),$(("${line}" + 3))d ./"${BUILD_FOLDER}"/"${COMPILE_COMMANDS}"
        done
        shellCommand "find ./${APPLICATION_FOLDER} ./${UTILITY_FOLDER} ./${ALGORITHM_FOLDER} \
./${DATA_STRUCTURE_FOLDER} ./${DESIGN_PATTERN_FOLDER} ./${NUMERIC_FOLDER} -name *.cpp -o -name *.hpp \
| xargs run-clang-tidy-12 -p ./${BUILD_FOLDER} -quiet"
        shellCommand "find ./${APPLICATION_FOLDER} ./${UTILITY_FOLDER} ./${ALGORITHM_FOLDER} \
./${DATA_STRUCTURE_FOLDER} ./${DESIGN_PATTERN_FOLDER} ./${NUMERIC_FOLDER} -name *.tpp \
| xargs clang-tidy-12 --use-color -p ./${BUILD_FOLDER} -quiet"
        rm -rf ./"${BUILD_FOLDER}"/"${COMPILE_COMMANDS}" \
            && mv ./"${BUILD_FOLDER}"/"${COMPILE_COMMANDS}".bak ./"${BUILD_FOLDER}"/"${COMPILE_COMMANDS}"

        local testBuildFolder="${TEST_FOLDER}/${BUILD_FOLDER}"
        if [[ ! -f ./"${testBuildFolder}"/"${COMPILE_COMMANDS}" ]]; then
            exception "There is no ${COMPILE_COMMANDS} file in the ${testBuildFolder} folder. Please generate it."
        fi
        compdb -p ./"${testBuildFolder}" list >./"${COMPILE_COMMANDS}" \
            && mv ./"${testBuildFolder}"/"${COMPILE_COMMANDS}" ./"${testBuildFolder}"/"${COMPILE_COMMANDS}".bak \
            && mv ./"${COMPILE_COMMANDS}" ./"${testBuildFolder}"
        shellCommand "find ./${TEST_FOLDER} -name *.cpp | xargs run-clang-tidy-12 -p ./${testBuildFolder} -quiet"
        rm -rf ./"${testBuildFolder}"/"${COMPILE_COMMANDS}" \
            && mv ./"${testBuildFolder}"/"${COMPILE_COMMANDS}".bak ./"${testBuildFolder}"/"${COMPILE_COMMANDS}"

        shellCommand "shellcheck ./${SCRIPT_FOLDER}/*.sh"
        shellCommand "pylint --rcfile=./.pylintrc ./${SCRIPT_FOLDER}/*.py"
    fi
}

performBrowserOption()
{
    if [[ "${ARGS_BROWSER}" = true ]]; then
        local commitId
        commitId=$(git rev-parse --short @)
        if [[ -z "${commitId}" ]]; then
            commitId="local"
        fi
        if [[ -d ./"${TEMPORARY_FOLDER}" ]]; then
            local lastTar="${PROJECT_FOLDER}_browser_${commitId}.tar.bz2"
            if [[ -f ./"${TEMPORARY_FOLDER}"/"${lastTar}" ]]; then
                local timeInterval=$(($(date +%s) - $(stat -L --format %Y "./${TEMPORARY_FOLDER}/${lastTar}")))
                if [[ "${timeInterval}" -lt "10" ]]; then
                    exception "The latest browser tarball ${TEMPORARY_FOLDER}/${lastTar} has been generated since \
${timeInterval}s ago."
                fi
            fi
            tarHtmlForBrowser "${commitId}"
        else
            mkdir ./"${TEMPORARY_FOLDER}"
            tarHtmlForBrowser "${commitId}"
        fi
    fi
}

tarHtmlForBrowser()
{
    local commitId=$1

    if [[ ! -f ./"${BUILD_FOLDER}"/"${COMPILE_COMMANDS}" ]]; then
        exception "There is no ${COMPILE_COMMANDS} file in the ${BUILD_FOLDER} folder. Please generate it."
    fi
    local browserFolder="browser" tarFile="${PROJECT_FOLDER}_${browserFolder}_${commitId}.tar.bz2"
    rm -rf ./"${TEMPORARY_FOLDER}"/"${PROJECT_FOLDER}"_"${browserFolder}"_*.tar.bz2 \
        ./"${DOCUMENT_FOLDER}"/"${browserFolder}"

    mkdir -p ./"${DOCUMENT_FOLDER}"/"${browserFolder}"
    shellCommand "codebrowser_generator -color -a -b ./${BUILD_FOLDER}/${COMPILE_COMMANDS} \
-o ./${DOCUMENT_FOLDER}/${browserFolder} -p ${PROJECT_FOLDER}:.:${commitId} -d ./data"
    shellCommand "codebrowser_generator -color -a -b ./${TEST_FOLDER}/${BUILD_FOLDER}/${COMPILE_COMMANDS} \
-o ./${DOCUMENT_FOLDER}/${browserFolder} -p ${PROJECT_FOLDER}:.:${commitId} -d ./data"
    shellCommand "codebrowser_indexgenerator ./${DOCUMENT_FOLDER}/${browserFolder} -d ./data"
    shellCommand "cp -rf /usr/local/share/woboq/data ./${DOCUMENT_FOLDER}/${browserFolder}/"
    shellCommand "tar -jcvf ./${TEMPORARY_FOLDER}/${tarFile} -C ./${DOCUMENT_FOLDER} ${browserFolder} >/dev/null"
}

performDoxygenOption()
{
    if [[ "${ARGS_DOXYGEN}" = true ]]; then
        local commitId
        commitId=$(git rev-parse --short @)
        if [[ -z "${commitId}" ]]; then
            commitId="local"
        fi
        if [[ -d ./"${TEMPORARY_FOLDER}" ]]; then
            local lastTar="${PROJECT_FOLDER}_doxygen_${commitId}.tar.bz2"
            if [[ -f ./"${TEMPORARY_FOLDER}"/"${lastTar}" ]]; then
                local timeInterval=$(($(date +%s) - $(stat -L --format %Y "./${TEMPORARY_FOLDER}/${lastTar}")))
                if [[ "${timeInterval}" -lt "10" ]]; then
                    exception "The latest doxygen tarball ${TEMPORARY_FOLDER}/${lastTar} has been generated since \
${timeInterval}s ago."
                fi
            fi
            tarHtmlForDoxygen "${commitId}"
        else
            mkdir ./"${TEMPORARY_FOLDER}"
            tarHtmlForDoxygen "${commitId}"
        fi
    fi
}

tarHtmlForDoxygen()
{
    local commitId=$1

    local doxygenFolder="doxygen" tarFile="${PROJECT_FOLDER}_${doxygenFolder}_${commitId}.tar.bz2"
    rm -rf ./"${TEMPORARY_FOLDER}"/"${PROJECT_FOLDER}"_"${doxygenFolder}"_*.tar.bz2 \
        ./"${DOCUMENT_FOLDER}"/"${doxygenFolder}"

    mkdir -p ./"${DOCUMENT_FOLDER}"/"${doxygenFolder}"
    local doxygenConfig="Doxyfile"
    if [[ "${ARGS_RELEASE}" = false ]]; then
        sed -i "s/\(^PROJECT_NUMBER[ ]\+=\)/\1 \"@ $(git rev-parse --short @)\"/" \
            ./"${DOCUMENT_FOLDER}"/"${doxygenConfig}"
        sed -i "s/\(^HTML_TIMESTAMP[ ]\+=\)\([ ]\+NO\)/\1 YES/" ./"${DOCUMENT_FOLDER}"/"${doxygenConfig}"
    fi
    shellCommand "doxygen ./${DOCUMENT_FOLDER}/${doxygenConfig} >/dev/null"
    shellCommand "tar -jcvf ./${TEMPORARY_FOLDER}/${tarFile} -C ./${DOCUMENT_FOLDER} ${doxygenFolder} >/dev/null"
    if [[ "${ARGS_RELEASE}" = false ]]; then
        sed -i "s/\(^PROJECT_NUMBER[ ]\+=\)\([ ]\+.*\)/\1/" ./"${DOCUMENT_FOLDER}"/"${doxygenConfig}"
        sed -i "s/\(^HTML_TIMESTAMP[ ]\+=\)\([ ]\+YES\)/\1 NO/" ./"${DOCUMENT_FOLDER}"/"${doxygenConfig}"
    fi
}

main()
{
    cd "${0%%"${SCRIPT_FOLDER}"*}" || exit 1
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
    performBrowserOption
    performDoxygenOption
}

main "$@"
