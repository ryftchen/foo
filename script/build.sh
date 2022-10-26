#!/usr/bin/env bash

ARGS_FORMAT=false
ARGS_LINT=false
ARGS_BROWSER=false
ARGS_DOCKER=false
ARGS_RELEASE=false
PERFORM_COMPILE=false

PROJECT_FOLDER="foo"
APPLICATION_FOLDER="application"
UTILITY_FOLDER="utility"
ALGORITHM_FOLDER="algorithm"
DESIGN_PATTERN_FOLDER="design_pattern"
NUMERIC_FOLDER="numeric"
SCRIPT_FOLDER="script"
DOCKER_FOLDER="docker"
BUILD_FOLDER="build"
TEMPORARY_FOLDER="temporary"
CMAKE_LISTS="CMakeLists.txt"
COMPILE_COMMANDS="compile_commands.json"
FORMAT_CONFIG_CPP=".clang-format"
FORMAT_CONFIG_PY=".toml"
FORMAT_CONFIG_SH=".editorconfig"
LINT_CONFIG_CPP=".clang-tidy"
LINT_CONFIG_PY=".pylintrc"
LINT_CONFIG_SH=".shellcheckrc"
DOCKER_FILE="Dockerfile"

bashCommand()
{
    echo
    echo "$(date "+%b %d %T") $* START"
    /bin/bash -c "$@"
    echo "$(date "+%b %d %T") $* FINISH"
}

printException()
{
    echo
    echo "Shell script build.sh: $*"
    exit 1
}

showHelp()
{
    echo "Usage: build.sh <options...>"
    echo
    echo "Optional:"
    echo "-h, --help       show help"
    echo "-f, --format     format code"
    echo "-l, --lint       lint code"
    echo "-c, --cleanup    cleanup project"
    echo "-b, --browser    generate browser"
    echo "-d, --docker     construct docker"
    echo "-r, --release    build release"
    exit 0
}

cleanupProject()
{
    bashCommand "rm -rf ./${BUILD_FOLDER} ./${TEMPORARY_FOLDER}"
    bashCommand "rm -rf ./core*"
}

parseArgs()
{
    while [ "$#" -gt 0 ]; do
        case $1 in
        -h | --help) showHelp ;;
        -f | --format) ARGS_FORMAT=true ;;
        -l | --lint) ARGS_LINT=true ;;
        -c | --cleanup) cleanupProject ;;
        -b | --browser) ARGS_BROWSER=true ;;
        -d | --docker) ARGS_DOCKER=true ;;
        -r | --release) ARGS_RELEASE=true ;;
        *) printException "Unknown command line option: $1. Try with --help to get information." ;;
        esac
        shift
    done
}

checkDependencies()
{
    if {
        [ "$#" -eq 1 ] && [ "${ARGS_RELEASE}" = true ]
    } || [ "$#" -eq 0 ]; then
        PERFORM_COMPILE=true
    fi

    if [ ! -d ./"${APPLICATION_FOLDER}" ] || [ ! -d ./"${UTILITY_FOLDER}" ] || [ ! -d ./"${ALGORITHM_FOLDER}" ] \
        || [ ! -d ./"${DESIGN_PATTERN_FOLDER}" ] || [ ! -d ./"${NUMERIC_FOLDER}" ] \
        || [ ! -d ./"${SCRIPT_FOLDER}" ]; then
        printException "Missing code folders in ${PROJECT_FOLDER} folder. Please check it."
    fi

    if ! command -v cmake >/dev/null 2>&1; then
        printException "No cmake program. Please check it."
    fi

    if [ "${ARGS_FORMAT}" = true ]; then
        if
            command -v clang-format-12 >/dev/null 2>&1 \
                && command -v shfmt >/dev/null 2>&1 \
                && command -v black >/dev/null 2>&1
        then
            if [ ! -f ./"${FORMAT_CONFIG_CPP}" ] || [ ! -f ./"${FORMAT_CONFIG_PY}" ] \
                || [ ! -f ./"${FORMAT_CONFIG_SH}" ]; then
                printException "Missing format config files in ${PROJECT_FOLDER} folder. Please check it."
            fi
        else
            printException "No clang-format, shfmt or black program. Please check it."
        fi
    fi

    if [ "${ARGS_LINT}" = true ]; then
        if
            command -v clang-tidy-12 >/dev/null 2>&1 \
                && command -v shellcheck >/dev/null 2>&1 \
                && command -v pylint >/dev/null 2>&1
        then
            if [ ! -f ./"${LINT_CONFIG_CPP}" ] || [ ! -f ./"${LINT_CONFIG_PY}" ] || [ ! -f ./"${LINT_CONFIG_SH}" ]; then
                printException "Missing lint config files in ${PROJECT_FOLDER} folder. Please check it."
            fi
            if [ ! -f ./"${BUILD_FOLDER}"/"${COMPILE_COMMANDS}" ]; then
                printException "No ${COMPILE_COMMANDS} file in ${BUILD_FOLDER} folder. Please generate it."
            fi
        else
            printException "No clang-tidy, shellcheck or pylint program. Please check it."
        fi
    fi

    if [ "${ARGS_BROWSER}" = true ]; then
        if
            ! command -v codebrowser_generator >/dev/null 2>&1 \
                || ! command -v codebrowser_indexgenerator >/dev/null 2>&1
        then
            printException "No codebrowser_generator or codebrowser_indexgenerator program. Please check it."
        fi
    fi

    if [ "${ARGS_DOCKER}" = true ]; then
        if [ ! -d ./"${DOCKER_FOLDER}" ]; then
            printException "Missing construct folders in ${PROJECT_FOLDER} folder. Please check it."
        fi
        if command -v docker >/dev/null 2>&1; then
            if [ ! -f ./"${DOCKER_FOLDER}"/"${DOCKER_FILE}" ]; then
                printException "No ${DOCKER_FILE} file in ${DOCKER_FOLDER} folder. Please check it."
            fi

            printf "Please confirm further whether construct container. [y/n]: "
            oldStty=$(stty -g)
            stty raw -echo
            answer=$(while ! head -c 1 | grep -i '[ny]'; do true; done)
            stty "${oldStty}"
            if echo "${answer}" | grep -iq "^y"; then
                echo "Yes"
            else
                echo "No"
                ARGS_DOCKER=false
            fi
        else
            printException "No docker program. Please check it."
        fi
    fi
}

generateCMakeFiles()
{
    if [ -f ./"${CMAKE_LISTS}" ]; then
        if [ ! -d ./"${BUILD_FOLDER}" ]; then
            bashCommand "mkdir ./${BUILD_FOLDER}"
        fi

        export CC=/usr/bin/clang-12 CXX=/usr/bin/clang++-12
        if [ "${ARGS_RELEASE}" = true ]; then
            bashCommand "cmake -S . -B ./${BUILD_FOLDER} -DCMAKE_CXX_COMPILER=clang++-12 -DCMAKE_BUILD_TYPE=Release"
        else
            bashCommand "cmake -S . -B ./${BUILD_FOLDER} -DCMAKE_CXX_COMPILER=clang++-12 -DCMAKE_BUILD_TYPE=Debug"
        fi
    else
        printException "No ${CMAKE_LISTS} file in ${PROJECT_FOLDER} folder. Please check it."
    fi
}

compileCode()
{
    if [ "${PERFORM_COMPILE}" = true ]; then
        bashCommand "make -C ./${BUILD_FOLDER} -j"
    fi
}

performFormatOption()
{
    if [ "${ARGS_FORMAT}" = true ]; then
        bashCommand "find ./${APPLICATION_FOLDER} ./${UTILITY_FOLDER} ./${ALGORITHM_FOLDER} ./${DESIGN_PATTERN_FOLDER} \
./${NUMERIC_FOLDER} -name *.cpp -o -name *.hpp | xargs clang-format-12 -i --verbose"
        bashCommand "shfmt -l -w ./${SCRIPT_FOLDER}/*.sh"
        bashCommand "black --config ./${FORMAT_CONFIG_PY} ./${SCRIPT_FOLDER}/*.py"
    fi
}

performLintOption()
{
    if [ "${ARGS_LINT}" = true ]; then
        bashCommand "find ./${APPLICATION_FOLDER} ./${UTILITY_FOLDER} ./${ALGORITHM_FOLDER} ./${DESIGN_PATTERN_FOLDER} \
./${NUMERIC_FOLDER} -name *.cpp -o -name *.hpp | xargs clang-tidy-12 -p ./${BUILD_FOLDER}/${COMPILE_COMMANDS}"
        bashCommand "shellcheck ./${SCRIPT_FOLDER}/*.sh"
        bashCommand "pylint --rcfile=${LINT_CONFIG_PY} ./${SCRIPT_FOLDER}/*.py"
    fi
}

performBrowserOption()
{
    if [ "${ARGS_BROWSER}" = true ]; then
        if [ -d ./"${TEMPORARY_FOLDER}" ]; then
            commitId=$(git rev-parse --short @)
            if [ -z "${commitId}" ]; then
                commitId="local"
            fi
            lastTar="${PROJECT_FOLDER}_html_${commitId}.tar.bz2"
            if [ -f ./"${TEMPORARY_FOLDER}"/"${lastTar}" ]; then
                printException "The latest html file ${TEMPORARY_FOLDER}/${lastTar} has been generated."
            else
                tarHtmlForBrowser
            fi
        else
            bashCommand "mkdir ./${TEMPORARY_FOLDER}"
            tarHtmlForBrowser
        fi
    fi
}

tarHtmlForBrowser()
{
    commitId=$(git rev-parse --short @)
    browserFolder="${PROJECT_FOLDER}_html"
    tarFile="${browserFolder}_${commitId}.tar.bz2"
    if [ -d ./"${TEMPORARY_FOLDER}"/"${browserFolder}" ]; then
        rm -rf ./"${TEMPORARY_FOLDER}"/"${browserFolder}"
    fi
    bashCommand "mkdir -p ./${TEMPORARY_FOLDER}/${browserFolder}"
    bashCommand "codebrowser_generator -color -a -b ./${BUILD_FOLDER}/${COMPILE_COMMANDS} \
-o ./${TEMPORARY_FOLDER}/${browserFolder} -p ${PROJECT_FOLDER}:.:${commitId} -d ./data"
    bashCommand "codebrowser_indexgenerator ./${TEMPORARY_FOLDER}/${browserFolder} -d ./data"
    bashCommand "cp -rf /usr/local/share/woboq/data ./${TEMPORARY_FOLDER}/${browserFolder}/"
    bashCommand "tar -jcvf ./${TEMPORARY_FOLDER}/${tarFile} -C ./${TEMPORARY_FOLDER} ${browserFolder} >/dev/null"
    bashCommand "rm -rf ./${TEMPORARY_FOLDER}/${browserFolder}"
}

performDockerOption()
{
    if [ "${ARGS_DOCKER}" = true ]; then
        if service docker status | grep -q "active (running)" 2>/dev/null; then
            imageRepo="ryftchen/${PROJECT_FOLDER}"
            if ! docker ps -a | tail -n +2 | awk '{split($0, a, " "); print a[2]}' \
                | grep "${imageRepo}" >/dev/null 2>&1; then
                if ! docker image ls -a | tail -n +2 | awk '{split($0, a, " "); print a[1]}' \
                    | grep "${imageRepo}" >/dev/null 2>&1; then
                    if docker search "${imageRepo}" | tail -n +2 | awk '{split($0, a, " "); print a[1]}' \
                        | grep "${imageRepo}" >/dev/null 2>&1; then
                        bashCommand "docker pull ${imageRepo}:latest"
                    else
                        bashCommand "docker build -t ${imageRepo}:latest -f ./${DOCKER_FOLDER}/${DOCKER_FILE} \
./${DOCKER_FOLDER}/"
                    fi
                fi
                bashCommand "docker run -it --name ${PROJECT_FOLDER} -v ${PWD}:/root/${PROJECT_FOLDER} \
-d ${imageRepo}:latest /bin/bash"
            fi
        else
            printException "Service docker status is not active."
        fi
    fi
}

main()
{
    cd "${0%%${SCRIPT_FOLDER}*}" || exit 1

    parseArgs "$@"
    checkDependencies "$@"
    generateCMakeFiles

    performFormatOption
    performLintOption
    performBrowserOption
    performDockerOption

    compileCode
}

main "$@"
