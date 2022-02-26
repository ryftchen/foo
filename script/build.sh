#!/usr/bin/env bash
# chmod 755 build.sh

ARGS_RELEASE=0
ARGS_CLEANUP=0
ARGS_ANALYSIS=0
ARGS_FORMAT=0
ARGS_TAG=0
PROJECT_FOLDER="foo"
INCLUDE_FOLDER="include"
SOURCE_FOLDER="source"
LIBRARY_FOLDER="library"
SCRIPT_FOLDER="script"
BUILD_FOLDER="build"
BACKUP_FOLDER="backup"
TEMP_FOLDER="temp"
BUILD_FILE="build.sh"
BACKUP_FILE="backup.sh"
TEST_FILE="test.py"
CMAKE_FILE="CMakeLists.txt"
README_FILE="README.md"

shCommand()
{
    echo
    echo "$(date "+%b %d %T") $*" BEGIN
    sh -c "$@"
    echo "$(date "+%b %d %T") $*" END
}

printAbort()
{
    echo "Shell script build.sh: $*"
    exit 1
}

printInstruction()
{
    echo "### Usage    : build.sh [Options...]"
    echo
    echo "### [Options]:"
    echo
    echo "    -r, --release                      Release"
    echo
    echo "    -c, --cleanup                      Cleanup"
    echo
    echo "    -f, --format                       Format"
    echo
    echo "    -a, --analysis                     Analysis"
    echo
    echo "    -t, --tag                          Tag"
    echo
    echo "    --help                             Help"
    exit 0
}

parseArgs()
{
    while [ "$#" -gt 0 ]; do
        case $1 in
        -r | --release) ARGS_RELEASE=1 ;;
        -c | --cleanup) ARGS_CLEANUP=1 ;;
        -f | --format) ARGS_FORMAT=1 ;;
        -a | --analysis) ARGS_ANALYSIS=1 ;;
        -t | --tag) ARGS_TAG=1 ;;
        --help)
            printInstruction
            ;;
        *) printAbort "Unknown command line option: $1. Try with --help to get information." ;;
        esac
        shift
    done
}

main()
{
    cd "$(dirname "$0")" && cd ..
    parseArgs "$@"

    if [ ! -d ./"${INCLUDE_FOLDER}" ] | [ ! -d ./"${SOURCE_FOLDER}" ] \
        | [ ! -d ./"${LIBRARY_FOLDER}" ] | [ ! -d ./"${SCRIPT_FOLDER}" ] \
        | [ ! -f "${CMAKE_FILE}" ] | [ ! -f "${README_FILE}" ]; then
        printAbort "There are missing files in ${PROJECT_FOLDER} folder."
    fi

    if [ "${ARGS_CLEANUP}" = "1" ]; then
        shCommand "rm -rf ./${BUILD_FOLDER} ./${BACKUP_FOLDER} ./${TEMP_FOLDER}"
        shCommand "rm -rf GPATH GRTAGS GTAGS"
        if [ "$#" -eq 1 ]; then
            exit 0
        fi
    fi

    if [ "${ARGS_FORMAT}" = "1" ] | [ "${ARGS_ANALYSIS}" = "1" ]; then
        if [ ! -f ./"${SCRIPT_FOLDER}"/"${BUILD_FILE}" ] \
            | [ ! -f ./"${SCRIPT_FOLDER}"/"${BACKUP_FILE}" ] \
            | [ ! -f ./"${SCRIPT_FOLDER}"/"${TEST_FILE}" ]; then
            printAbort "There are missing file in ${SCRIPT_FOLDER} folder."
        fi
    fi

    if [ "${ARGS_FORMAT}" = "1" ]; then
        if
            command -v astyle >/dev/null 2>&1 &
            command -v shfmt >/dev/null 2>&1 &
            command -v black >/dev/null 2>&1
        then
            shCommand "astyle --style=ansi -s -xU -w -xg -k3 -W3 -c -n -xC100 -xL -v \
./${INCLUDE_FOLDER}/*.hpp ./${SOURCE_FOLDER}/*.cpp ./${LIBRARY_FOLDER}/*.cpp"
            shCommand "shfmt -l -w -i 4 -bn -fn ./${SCRIPT_FOLDER}/${BUILD_FILE} \
./${SCRIPT_FOLDER}/${BACKUP_FILE}"
            shCommand "black -l 100 -S -v ./${SCRIPT_FOLDER}/${TEST_FILE}"
        else
            printAbort "There is no astyle, shfmt or black program. Please check it."
        fi
        if [ "$#" -eq 1 ]; then
            exit 0
        fi
    fi

    if [ "${ARGS_TAG}" = "1" ]; then
        if
            command -v gtags >/dev/null 2>&1
        then
            shCommand "find ./${INCLUDE_FOLDER} ./${SOURCE_FOLDER} ./${LIBRARY_FOLDER} -type f \
-print | grep -E '*\.cpp|*\.hpp' | grep -v dummy | gtags -i -v -f -"
        else
            printAbort "There is no gtags program. Please check it."
        fi
        if [ "$#" -eq 1 ]; then
            exit 0
        fi
    fi

    if [ -f "${CMAKE_FILE}" ]; then
        if [ ! -d ./"${BUILD_FOLDER}" ]; then
            shCommand "mkdir ${BUILD_FOLDER}"
        fi
        if [ "${ARGS_RELEASE}" = "1" ]; then
            shCommand "cd ${BUILD_FOLDER} && cmake .. -DCMAKE_BUILD_TYPE=Release"
        else
            shCommand "cd ${BUILD_FOLDER} && cmake .. -DCMAKE_BUILD_TYPE=Debug"
        fi
        shCommand "cd ${BUILD_FOLDER} && make -j4"
    else
        printAbort "There is no ${CMAKE_FILE} file in ${PROJECT_FOLDER} folder."
    fi

    if [ "${ARGS_ANALYSIS}" = "1" ]; then
        if
            command -v cppcheck >/dev/null 2>&1 &
            command -v shellcheck >/dev/null 2>&1 &
            command -v pylint >/dev/null 2>&1
        then
            if [ -f ./"${BUILD_FOLDER}"/compile_commands.json ]; then
                shCommand "cppcheck ./ --std=c++20 --enable=all --suppress=missingIncludeSystem \
--project=./${BUILD_FOLDER}/compile_commands.json"
            else
                printAbort "There is no compile_commands.json file in ${BUILD_FOLDER} folder. \
Please generate it."
            fi
            shCommand "shellcheck ./${SCRIPT_FOLDER}/${BUILD_FILE} \
./${SCRIPT_FOLDER}/${BACKUP_FILE} --enable=all"
            shCommand "pylint ./${SCRIPT_FOLDER}/${TEST_FILE} --enable=all \
--argument-naming-style=camelCase --attr-naming-style=camelCase --function-naming-style=camelCase \
--method-naming-style=camelCase --module-naming-style=camelCase --variable-naming-style=camelCase \
--disable=missing-module-docstring,missing-class-docstring,missing-function-docstring,\
global-statement"
        else
            printAbort "There is no cppcheck, shellcheck or pylint program. Please check it."
        fi
    fi
}

main "$@"
