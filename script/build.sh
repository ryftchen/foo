#!/usr/bin/env bash

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
ENHANCED_DEV_TMPFS=false
ENHANCED_DEV_CCACHE=true
ENHANCED_DEV_DISTCC=true

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

CMAKE_BUILD_TYPE="Debug"
CMAKE_EXTRA_FLAG=""
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
        CMAKE_BUILD_TYPE="Release"
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

setCompileEnv()
{
    export CC=/usr/bin/clang-12 CXX=/usr/bin/clang++-12
    if [[ -f ./"${SCRIPT_FOLDER}"/.env ]]; then
        # shellcheck source=/dev/null
        source ./"${SCRIPT_FOLDER}"/.env
        if [[ -n "${FOO_ENHANCED_DEV_TMPFS}" ]] && [[ "${FOO_ENHANCED_DEV_TMPFS}" = "ON" ]]; then
            ENHANCED_DEV_TMPFS=true
        fi
        if [[ -n "${FOO_ENHANCED_DEV_CCACHE}" ]] && [[ "${FOO_ENHANCED_DEV_CCACHE}" = "ON" ]]; then
            ENHANCED_DEV_CCACHE=true
        fi
        if [[ -n "${FOO_ENHANCED_DEV_DISTCC}" ]] && [[ "${FOO_ENHANCED_DEV_DISTCC}" = "ON" ]]; then
            ENHANCED_DEV_DISTCC=true
        fi
    fi

    if [[ "${ENHANCED_DEV_CCACHE}" = true ]]; then
        CMAKE_EXTRA_FLAG="${CMAKE_EXTRA_FLAG} -D_TOOLCHAIN_CCACHE=ON"
        if [[ "${ENHANCED_DEV_DISTCC}" = true ]] \
            && command -v ccache >/dev/null 2>&1 && command -v distcc >/dev/null 2>&1; then
            export CCACHE_PREFIX=distcc
        fi
    fi
    if [[ "${ENHANCED_DEV_DISTCC}" = true ]]; then
        CMAKE_EXTRA_FLAG="${CMAKE_EXTRA_FLAG} -D_TOOLCHAIN_DISTCC=ON"
        if [[ -z "${DISTCC_HOSTS}" ]]; then
            export DISTCC_HOSTS=localhost
        fi
    fi
}

performBuilding()
{
    setCompileEnv
    if [[ "${ENHANCED_DEV_TMPFS}" = true ]]; then
        if [[ ! -d ./${BUILD_FOLDER} ]]; then
            mkdir "./${BUILD_FOLDER}"
        fi
        if ! df -h -t tmpfs | grep -q "${PROJECT_FOLDER}/${BUILD_FOLDER}" 2>/dev/null; then
            shellCommand "mount -t tmpfs -o size=96m tmpfs ./${BUILD_FOLDER}"
        fi
    fi

    shellCommand "cmake -S . -B ./${BUILD_FOLDER} -G Ninja -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}${CMAKE_EXTRA_FLAG}"
    if [[ "${TO_COMPILE_SOURCE_ONLY}" = true ]]; then
        tput setaf 2 && tput bold
        shellCommand "cmake --build ./${BUILD_FOLDER}"
        tput sgr0
        exit 0
    fi
    shellCommand "cmake -S ./${TEST_FOLDER} -B ./${TEST_FOLDER}/${BUILD_FOLDER} -G Ninja \
-DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}${CMAKE_EXTRA_FLAG}"
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

FOO_ENHANCED_DEV_TMPFS=OFF
FOO_ENHANCED_DEV_CCACHE=OFF
FOO_ENHANCED_DEV_DISTCC=OFF

export FOO_ENHANCED_DEV_TMPFS FOO_ENHANCED_DEV_CCACHE FOO_ENHANCED_DEV_DISTCC
return 0
EOF"
        exit 0
    fi
}

performContainerOption()
{
    if [[ "${ARGS_CONTAINER}" = true ]]; then
        if command -v docker >/dev/null 2>&1; then
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
            exception "No docker program. Please install it."
        fi

        local imageRepo="ryftchen/${PROJECT_FOLDER}"
        local containerName="${PROJECT_FOLDER}_dev"
        if ! docker ps -a --format "{{lower .Image}} {{lower .Names}}" \
            | grep -q "${imageRepo}:latest ${containerName}" 2>/dev/null; then
            if ! docker image ls -a --format "{{lower .Repository}} {{lower .Tag}}" \
                | grep -q "${imageRepo} latest" 2>/dev/null; then
                local toBuildImage=false
                if docker search "${imageRepo}" --format "{{lower .Name}}" | grep -q "${imageRepo}" 2>/dev/null; then
                    local tags
                    tags=$(curl -sS "https://registry.hub.docker.com/v2/repositories/${imageRepo}/tags" \
                        | sed -Ee 's/("name":)"([^"]*)"/\n\1\2\n/g' | grep '"name":' \
                        | awk -F: '{printf("%s\n", $2)}')
                    if echo "${tags}" | grep -q 'latest' 2>/dev/null; then
                        shellCommand "docker pull ${imageRepo}:latest"
                    else
                        toBuildImage=true
                    fi
                else
                    toBuildImage=true
                fi
                if [[ "${toBuildImage}" = true ]]; then
                    shellCommand "docker build -t ${imageRepo}:latest -f ./${DOCKER_FOLDER}/Dockerfile \
./${DOCKER_FOLDER}/"
                fi
            fi
            shellCommand "docker run -it --name ${containerName} -v ${PWD}:/root/${PROJECT_FOLDER} -d --privileged \
${imageRepo}:latest /bin/bash"
            exit 0
        else
            exception "The container exists."
        fi
    fi
}

performTestOption()
{
    if [[ "${ARGS_TEST}" = true ]]; then
        setCompileEnv
        if [[ "${ENHANCED_DEV_TMPFS}" = true ]]; then
            if [[ ! -d ./${TEST_FOLDER}/${BUILD_FOLDER} ]]; then
                mkdir -p "./${TEST_FOLDER}/${BUILD_FOLDER}"
            fi
            if ! df -h -t tmpfs | grep -q "${PROJECT_FOLDER}/${TEST_FOLDER}/${BUILD_FOLDER}" 2>/dev/null; then
                shellCommand "mount -t tmpfs -o size=48m tmpfs ./${TEST_FOLDER}/${BUILD_FOLDER}"
            fi
        fi

        shellCommand "cmake -S ./${TEST_FOLDER} -B ./${TEST_FOLDER}/${BUILD_FOLDER} -G Ninja \
-DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}${CMAKE_EXTRA_FLAG}"
        tput setaf 2 && tput bold
        shellCommand "cmake --build ./${TEST_FOLDER}/${BUILD_FOLDER}"
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
        local numberRegex="^[0-9]+$"
        while true; do
            local line
            line=$(grep -n '.tpp' ./"${BUILD_FOLDER}"/"${COMPILE_COMMANDS}" | head -n 1 | cut -d : -f 1)
            if ! [[ "${line}" =~ ${numberRegex} ]]; then
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
                local timeDiff=$(($(date +%s) - $(stat -L --format %Y "./${TEMPORARY_FOLDER}/${lastTar}")))
                if [[ "${timeDiff}" -lt "10" ]]; then
                    exception "The latest browser tarball ${TEMPORARY_FOLDER}/${lastTar} has been generated since \
${timeDiff}s ago."
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
    if [[ ! -f ./"${BUILD_FOLDER}"/"${COMPILE_COMMANDS}" ]]; then
        exception "There is no ${COMPILE_COMMANDS} file in the ${BUILD_FOLDER} folder. Please generate it."
    fi
    local browserFolder="browser"
    local tarFile="${PROJECT_FOLDER}_${browserFolder}_$1.tar.bz2"
    rm -rf ./"${TEMPORARY_FOLDER}"/"${PROJECT_FOLDER}"_"${browserFolder}"_*.tar.bz2 \
        ./"${DOCUMENT_FOLDER}"/"${browserFolder}"

    mkdir -p ./"${DOCUMENT_FOLDER}"/"${browserFolder}"
    shellCommand "codebrowser_generator -color -a -b ./${BUILD_FOLDER}/${COMPILE_COMMANDS} \
-o ./${DOCUMENT_FOLDER}/${browserFolder} -p ${PROJECT_FOLDER}:.:$1 -d ./data"
    shellCommand "codebrowser_generator -color -a -b ./${TEST_FOLDER}/${BUILD_FOLDER}/${COMPILE_COMMANDS} \
-o ./${DOCUMENT_FOLDER}/${browserFolder} -p ${PROJECT_FOLDER}:.:$1 -d ./data"
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
                local timeDiff=$(($(date +%s) - $(stat -L --format %Y "./${TEMPORARY_FOLDER}/${lastTar}")))
                if [[ "${timeDiff}" -lt "10" ]]; then
                    exception "The latest doxygen tarball ${TEMPORARY_FOLDER}/${lastTar} has been generated since \
${timeDiff}s ago."
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
    local doxygenFolder="doxygen"
    local tarFile="${PROJECT_FOLDER}_${doxygenFolder}_$1.tar.bz2"
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
