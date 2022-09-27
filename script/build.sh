#!/usr/bin/env bash

ARGS_FORMAT=false
ARGS_LINT=false
ARGS_RELEASE=false
ARGS_REPORT=false

PROJECT_FOLDER="foo"
APPLICATION_FOLDER="application"
UTILITY_FOLDER="utility"
ALGORITHM_FOLDER="algorithm"
SCRIPT_FOLDER="script"
BUILD_FOLDER="build"
TEMP_FOLDER="temp"
CMAKE_CONTENT="CMakeLists.txt"
COMPILE_COMMANDS="compile_commands.json"
FORMAT_CONFIG_CPP=".clang-format"
FORMAT_CONFIG_PY=".toml"
FORMAT_CONFIG_SH=".editorconfig"
LINT_CONFIG_CPP=".clang-tidy"
LINT_CONFIG_PY=".pylintrc"
LINT_CONFIG_SH=".shellcheckrc"
PERFORM_COMPILE=false

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

printInstruction()
{
    echo "Usage: build.sh <options...>"
    echo
    echo "Optional:"
    echo "--help           show help"
    echo "--release        build with release version"
    echo "-f, --format     format code"
    echo "-l, --lint       lint code"
    echo "-c, --cleanup    cleanup project"
    echo "-r, --report     report by html" #
    exit 0
}

parseArgs()
{
    while [ "$#" -gt 0 ]; do
        case $1 in
        --help) printInstruction ;;
        --release) ARGS_RELEASE=true ;;
        -f | --format) ARGS_FORMAT=true ;;
        -l | --lint) ARGS_LINT=true ;;
        -c | --cleanup) performCleanupOption ;;
        -r | --report) ARGS_REPORT=true ;;
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
        || [ ! -d ./"${SCRIPT_FOLDER}" ]; then
        printException "Missing code folders in ${PROJECT_FOLDER} folder. Please check it."
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

    if [ "${ARGS_REPORT}" = true ]; then
        if
            ! command -v codebrowser_generator >/dev/null 2>&1 \
                || ! command -v codebrowser_indexgenerator >/dev/null 2>&1
        then
            printException "No codebrowser_generator or codebrowser_indexgenerator program. Please check it."
        fi
    fi
}

generateCMakeFiles()
{
    if [ -f ./"${CMAKE_CONTENT}" ]; then
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
        printException "No ${CMAKE_CONTENT} file in ${PROJECT_FOLDER} folder."
    fi
}

compileCode()
{
    if [ "${PERFORM_COMPILE}" = true ]; then
        bashCommand "make -C ./${BUILD_FOLDER} -j"
    fi
}

performCleanupOption()
{
    bashCommand "rm -rf ./${BUILD_FOLDER} ./${TEMP_FOLDER}"
    bashCommand "rm -rf ./core*"
}

performFormatOption()
{
    if [ "${ARGS_FORMAT}" = true ]; then
        bashCommand "find ./${APPLICATION_FOLDER} ./${UTILITY_FOLDER} ./${ALGORITHM_FOLDER} -name *.cpp -o -name *.hpp \
| xargs clang-format-12 -i --verbose"
        bashCommand "shfmt -l -w ./${SCRIPT_FOLDER}/*.sh"
        bashCommand "black --config ./${FORMAT_CONFIG_PY} ./${SCRIPT_FOLDER}/*.py"
    fi
}

performLintOption()
{
    if [ "${ARGS_LINT}" = true ]; then
        bashCommand "find ./${APPLICATION_FOLDER} ./${UTILITY_FOLDER} ./${ALGORITHM_FOLDER} -name *.cpp -o -name *.hpp \
| xargs clang-tidy-12 -p ./${BUILD_FOLDER}/${COMPILE_COMMANDS}"
        bashCommand "shellcheck ./${SCRIPT_FOLDER}/*.sh"
        bashCommand "pylint --rcfile=${LINT_CONFIG_PY} ./${SCRIPT_FOLDER}/*.py"
    fi
}

performReportOption()
{
    if [ "${ARGS_REPORT}" = true ]; then
        if [ -d ./"${TEMP_FOLDER}" ]; then
            commitId=$(git rev-parse --short @)
            if [ -z "${commitId}" ]; then
                commitId="local"
            fi
            lastTar="${PROJECT_FOLDER}_html_${commitId}.tar.bz2"
            if [ -f ./"${TEMP_FOLDER}"/"${lastTar}" ]; then
                printException "The latest html file ${TEMP_FOLDER}/${lastTar} has been generated."
            else
                tarHtmlReport
            fi
        else
            bashCommand "mkdir ./${TEMP_FOLDER}"
            tarHtmlReport
        fi
    fi
}

tarHtmlReport()
{
    commitId=$(git rev-parse --short @)
    browserFolder="${PROJECT_FOLDER}_html"
    tarFile="${browserFolder}_${commitId}.tar.bz2"
    if [ -d ./"${TEMP_FOLDER}"/"${browserFolder}" ]; then
        rm -rf ./"${TEMP_FOLDER}"/"${browserFolder}"
    fi
    bashCommand "mkdir -p ./${TEMP_FOLDER}/${browserFolder}"
    bashCommand "codebrowser_generator -color -a -b ./${BUILD_FOLDER}/${COMPILE_COMMANDS} \
-o ./${TEMP_FOLDER}/${browserFolder} -p ${PROJECT_FOLDER}:.:${commitId} -d ./data"
    bashCommand "codebrowser_indexgenerator ./${TEMP_FOLDER}/${browserFolder} -d ./data"
    bashCommand "cp -rf /usr/local/share/woboq/data ./${TEMP_FOLDER}/${browserFolder}/"
    bashCommand "tar -jcvf ./${TEMP_FOLDER}/${tarFile} -C ./${TEMP_FOLDER} ${browserFolder} >/dev/null"
    bashCommand "rm -rf ./${TEMP_FOLDER}/${browserFolder}"
}

main()
{
    cd "${0%%${SCRIPT_FOLDER}*}" || exit 1

    parseArgs "$@"
    checkDependencies "$@"
    generateCMakeFiles

    performFormatOption
    performLintOption
    performReportOption

    compileCode
}

main "$@"
