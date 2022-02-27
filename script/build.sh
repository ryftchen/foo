#!/usr/bin/env bash
# chmod 755 build.sh
# tar -zxvf foo_XXXXXXXXXXXXXX.tar.gz

ARGS_RELEASE=0
ARGS_CLEANUP=0
ARGS_ANALYSIS=0
ARGS_FORMAT=0
ARGS_BACKUP=0
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
TEST_FILE="test.py"
CMAKE_FILE="CMakeLists.txt"
README_FILE="README.md"
COMPILE_COMMAND="compile_commands.json"
ANALYSIS_STYLE=".clang-tidy"
FORMAT_STYLE=".clang-format"

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
    echo "    -b, --backup                       Backup"
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
        -b | --backup) ARGS_BACKUP=1 ;;
        -t | --tag) ARGS_TAG=1 ;;
        --help)
            printInstruction
            ;;
        *) printAbort "Unknown command line option: $1. Try with --help to get information." ;;
        esac
        shift
    done
}

tarProject()
{
    tarFolder="${PROJECT_FOLDER}_$(date "+%Y%m%d%H%M%S")"
    shCommand "mkdir ${BACKUP_FOLDER}/${PROJECT_FOLDER}"
    shCommand "find . -type f -o \( -path ./${BUILD_FOLDER} -o -path ./${BACKUP_FOLDER} \
-o -path ./${TEMP_FOLDER} -o -path './.*' \) -prune -o -print | sed 1d \
| grep -E '${INCLUDE_FOLDER}|${SOURCE_FOLDER}|${LIBRARY_FOLDER}|${SCRIPT_FOLDER}' \
| xargs -i cp -R {} ${BACKUP_FOLDER}/${PROJECT_FOLDER}/"
    shCommand "find . -maxdepth 1 -type d -o -print | grep -E '*\.txt|*\.md' \
| xargs -i cp -R {} ${BACKUP_FOLDER}/${PROJECT_FOLDER}/"
    shCommand "tar -zcvf ${BACKUP_FOLDER}/${tarFolder}.tar.gz -C ./${BACKUP_FOLDER} \
${PROJECT_FOLDER}"
    shCommand "rm -rf ${BACKUP_FOLDER}/${PROJECT_FOLDER}"
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
            | [ ! -f ./"${SCRIPT_FOLDER}"/"${TEST_FILE}" ]; then
            printAbort "There are missing file in ${SCRIPT_FOLDER} folder."
        fi
    fi

    if [ "${ARGS_FORMAT}" = "1" ]; then
        if
            command -v clang-format >/dev/null 2>&1 &
            command -v shfmt >/dev/null 2>&1 &
            command -v black >/dev/null 2>&1
        then
            if [ -f "${FORMAT_STYLE}" ]; then
                shCommand "clang-format -i --verbose ./${INCLUDE_FOLDER}/*.hpp \
./${SOURCE_FOLDER}/*.cpp ./${LIBRARY_FOLDER}/*.cpp"
            else
                printAbort "There is no ${FORMAT_STYLE} file in ${PROJECT_FOLDER} folder. \
Please generate it."
            fi
            shCommand "shfmt -l -w -i 4 -bn -fn ./${SCRIPT_FOLDER}/${BUILD_FILE}"
            shCommand "black -l 100 -S -v ./${SCRIPT_FOLDER}/${TEST_FILE}"
        else
            printAbort "There is no clang-format, shfmt or black program. Please check it."
        fi
        if [ "$#" -eq 1 ]; then
            exit 0
        fi
    fi

    if [ "${ARGS_BACKUP}" = "1" ]; then
        if [ -d "${BACKUP_FOLDER}" ]; then
            files=$(find "${BACKUP_FOLDER}"/ -type f -name "${PROJECT_FOLDER}_*.tar.gz" \
                2>/dev/null | wc -l)
            if [ "${files}" != "0" ]; then
                lastTar=$(find "${BACKUP_FOLDER}"/ -type f -name "${PROJECT_FOLDER}_*.tar.gz" \
                    -print0 | xargs --null ls -at | head -n 1)
                timeDiff=$(($(date +%s) - $(stat -L --format %Y "${lastTar}")))
                if [ "${timeDiff}" -gt "10" ]; then
                    tarProject
                else
                    printAbort "The latest backup file ${lastTar} has been generated since \
${timeDiff}s ago."
                fi
            else
                tarProject
            fi
        else
            shCommand "mkdir ${BACKUP_FOLDER}"
            tarProject
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
            command -v clang-tidy >/dev/null 2>&1 &
            command -v shellcheck >/dev/null 2>&1 &
            command -v pylint >/dev/null 2>&1
        then
            if [ -f ./"${BUILD_FOLDER}"/"${COMPILE_COMMAND}" ]; then
                if [ -f "${ANALYSIS_STYLE}" ]; then
                    shCommand "clang-tidy -p ./${BUILD_FOLDER}/compile_commands.json \
./${INCLUDE_FOLDER}/*.hpp ./${SOURCE_FOLDER}/*.cpp ./${LIBRARY_FOLDER}/*.cpp"
                else
                    printAbort "There is no ${ANALYSIS_STYLE} file in ${PROJECT_FOLDER} folder. \
Please generate it."
                fi
            else
                printAbort "There is no ${COMPILE_COMMAND} file in ${BUILD_FOLDER} folder. \
Please generate it."
            fi
            shCommand "shellcheck ./${SCRIPT_FOLDER}/${BUILD_FILE} --enable=all"
            shCommand "pylint ./${SCRIPT_FOLDER}/${TEST_FILE} --enable=all \
--argument-naming-style=camelCase --attr-naming-style=camelCase --function-naming-style=camelCase \
--method-naming-style=camelCase --module-naming-style=camelCase --variable-naming-style=camelCase \
--disable=missing-module-docstring,missing-class-docstring,missing-function-docstring,\
global-statement"
        else
            printAbort "There is no clang-tidy, shellcheck or pylint program. Please check it."
        fi
    fi
}

main "$@"
