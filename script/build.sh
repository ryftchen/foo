#!/usr/bin/env bash

ARGS_CLEANUP=false
ARGS_FORMAT=false
ARGS_LINT=false
ARGS_HTML=false
ARGS_BACKUP=false
ARGS_TAG=false
ARGS_RELEASE=false

PROJECT_FOLDER="foo"
INCLUDE_FOLDER="include"
SOURCE_FOLDER="source"
UTILITY_FOLDER="utility"
SCRIPT_FOLDER="script"
BUILD_FOLDER="build"
TEMP_FOLDER="temp"
BUILD_SCRIPT="build.sh"
TEST_SCRIPT="test.py"
CMAKE_FILE="CMakeLists.txt"
COMPILE_COMMANDS="compile_commands.json"
FORMAT_CONFIG_CPP=".clang-format"
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

printAbort()
{
    echo "Shell script build.sh: $*"
    exit 1
}

printInstruction()
{
    echo "Usage    : build.sh [Options...]"
    echo
    echo "[Options]:"
    echo
    echo "    -c, --cleanup                      Cleanup"
    echo
    echo "    -f, --format                       Format"
    echo
    echo "    -l, --lint                         Lint"
    echo
    echo "    -h, --html                         Html"
    echo
    echo "    -b, --backup                       Backup"
    echo
    echo "    -t, --tag                          Tag"
    echo
    echo "    --release                          Release"
    echo
    echo "    --help                             Help"
    exit 0
}

parseArgs()
{
    while [ "$#" -gt 0 ]; do
        case $1 in
        -c | --cleanup) ARGS_CLEANUP=true ;;
        -f | --format) ARGS_FORMAT=true ;;
        -l | --lint) ARGS_LINT=true ;;
        -h | --html) ARGS_HTML=true ;;
        -b | --backup) ARGS_BACKUP=true ;;
        -t | --tag) ARGS_TAG=true ;;
        --release) ARGS_RELEASE=true ;;
        --help) printInstruction ;;
        *) printAbort "Unknown command line option: $1. Try with --help to get information." ;;
        esac
        shift
    done
}

checkDependencies()
{
    if [ "$#" -eq 1 ] && [ "${ARGS_RELEASE}" = true ] || [ "$#" -eq 0 ]; then
        PERFORM_COMPILE=true
    fi

    if [ ! -d ./"${INCLUDE_FOLDER}" ] || [ ! -d ./"${SOURCE_FOLDER}" ] \
        || [ ! -d ./"${UTILITY_FOLDER}" ] || [ ! -d ./"${SCRIPT_FOLDER}" ] \
        || [ ! -f ./"${CMAKE_FILE}" ]; then
        printAbort "There are missing files in ${PROJECT_FOLDER} folder."
    fi

    if [ "${ARGS_FORMAT}" = true ] || [ "${ARGS_LINT}" = true ]; then
        if [ ! -f ./"${SCRIPT_FOLDER}"/"${BUILD_SCRIPT}" ] \
            || [ ! -f ./"${SCRIPT_FOLDER}"/"${TEST_SCRIPT}" ]; then
            printAbort "There are missing files in ${SCRIPT_FOLDER} folder."
        fi
    fi
}

generateCMakeFiles()
{
    if [ -f ./"${CMAKE_FILE}" ]; then
        if [ ! -d ./"${BUILD_FOLDER}" ]; then
            bashCommand "mkdir ./${BUILD_FOLDER}"
        fi
        if [ "${ARGS_RELEASE}" = true ]; then
            bashCommand "cmake -S . -B ./${BUILD_FOLDER} \
-DCMAKE_CXX_COMPILER=clang++-12 -DCMAKE_BUILD_TYPE=Release"
        else
            bashCommand "cmake -S . -B ./${BUILD_FOLDER} \
-DCMAKE_CXX_COMPILER=clang++-12 -DCMAKE_BUILD_TYPE=Debug"
        fi
    else
        printAbort "There is no ${CMAKE_FILE} file in ${PROJECT_FOLDER} folder."
    fi
}

compileCode()
{
    if [ "${PERFORM_COMPILE}" = true ]; then
        bashCommand "make -C ./${BUILD_FOLDER} -j"
    fi
}

performOptionCleanup()
{
    if [ "${ARGS_CLEANUP}" = true ]; then
        bashCommand "rm -rf ./${BUILD_FOLDER} ./${TEMP_FOLDER}"
        bashCommand "rm -rf ./core* ./GPATH ./GRTAGS ./GTAGS"
        exit 0
    fi
}

performOptionFormat()
{
    if [ "${ARGS_FORMAT}" = true ]; then
        if
            command -v clang-format-12 >/dev/null 2>&1 \
                && command -v shfmt >/dev/null 2>&1 \
                && command -v black >/dev/null 2>&1
        then
            if [ -f ./"${FORMAT_CONFIG_CPP}" ]; then
                bashCommand "clang-format-12 -i --verbose ./${INCLUDE_FOLDER}/*.hpp \
./${SOURCE_FOLDER}/*.cpp ./${UTILITY_FOLDER}/*.cpp"
            else
                printAbort "There is no ${FORMAT_CONFIG_CPP} file in ${PROJECT_FOLDER} folder. \
Please generate it."
            fi
            bashCommand "shfmt -l -w -ln bash -i 4 -bn -fn ./${SCRIPT_FOLDER}/${BUILD_SCRIPT}"
            bashCommand "black -l 100 -S -v ./${SCRIPT_FOLDER}/${TEST_SCRIPT}"
        else
            printAbort "There is no clang-format, shfmt or black program. Please check it."
        fi
    fi
}

performOptionLint()
{
    if [ "${ARGS_LINT}" = true ]; then
        if
            command -v clang-tidy-12 >/dev/null 2>&1 \
                && command -v shellcheck >/dev/null 2>&1 \
                && command -v pylint >/dev/null 2>&1
        then
            if [ -f ./"${BUILD_FOLDER}"/"${COMPILE_COMMANDS}" ]; then
                if [ -f ./"${LINT_CONFIG_CPP}" ]; then
                    bashCommand "clang-tidy-12 -p ./${BUILD_FOLDER}/${COMPILE_COMMANDS} \
./${INCLUDE_FOLDER}/*.hpp ./${SOURCE_FOLDER}/*.cpp ./${UTILITY_FOLDER}/*.cpp"
                else
                    printAbort "There is no ${LINT_CONFIG_CPP} file in ${PROJECT_FOLDER} folder. \
Please generate it."
                fi
            else
                printAbort "There is no ${COMPILE_COMMANDS} file in ${BUILD_FOLDER} folder. \
Please generate it."
            fi
            if [ -f ./"${LINT_CONFIG_SH}" ]; then
                bashCommand "shellcheck --enable=all ./${SCRIPT_FOLDER}/${BUILD_SCRIPT}"
            else
                printAbort "There is no ${LINT_CONFIG_SH} file in ${PROJECT_FOLDER} folder. \
Please generate it."
            fi
            if [ -f ./"${LINT_CONFIG_PY}" ]; then
                bashCommand "pylint --rcfile=${LINT_CONFIG_PY} ./${SCRIPT_FOLDER}/${TEST_SCRIPT}"
            else
                printAbort "There is no ${LINT_CONFIG_PY} file in ${PROJECT_FOLDER} folder. \
Please generate it."
            fi
        else
            printAbort "There is no clang-tidy, shellcheck or pylint program. Please check it."
        fi
    fi
}

performOptionHtml()
{
    if [ "${ARGS_HTML}" = true ]; then
        if
            command -v codebrowser_generator >/dev/null 2>&1 \
                && command -v codebrowser_indexgenerator >/dev/null 2>&1
        then
            if [ -d ./"${TEMP_FOLDER}" ]; then
                commitId=$(git rev-parse --short @)
                if [ -z "${commitId}" ]; then
                    commitId="local"
                fi
                lastTar="${PROJECT_FOLDER}_html_${commitId}.tar.bz2"
                if [ -f ./"${TEMP_FOLDER}"/"${lastTar}" ]; then
                    printAbort "The latest html file ${TEMP_FOLDER}/${lastTar} has been generated."
                else
                    tarHtml
                fi
            else
                bashCommand "mkdir ./${TEMP_FOLDER}"
                tarHtml
            fi
        else
            printAbort "There is no codebrowser_generator or codebrowser_indexgenerator program. \
Please check it."
        fi
    fi
}
tarHtml()
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
    bashCommand "tar -jcvf ./${TEMP_FOLDER}/${tarFile} -C ./${TEMP_FOLDER} ${browserFolder} \
>/dev/null"
    bashCommand "rm -rf ./${TEMP_FOLDER}/${browserFolder}"
}

performOptionBackup()
{
    if [ "${ARGS_BACKUP}" = true ]; then
        if [ -d ./"${TEMP_FOLDER}" ]; then
            files=$(find "${TEMP_FOLDER}"/ -type f -name "${PROJECT_FOLDER}_backup_*.tar.gz" \
                2>/dev/null | wc -l)
            if [ "${files}" != "0" ]; then
                lastTar=$(find "${TEMP_FOLDER}"/ -type f -name "${PROJECT_FOLDER}_backup_*.tar.gz" \
                    -print0 | xargs --null ls -at | head -n 1)
                timeDiff=$(($(date +%s) - $(stat -L --format %Y "${lastTar}")))
                if [ "${timeDiff}" -gt "10" ]; then
                    tarBackup
                else
                    printAbort "The latest backup file ${lastTar} has been generated since \
${timeDiff} seconds ago."
                fi
            else
                tarBackup
            fi
        else
            bashCommand "mkdir ./${TEMP_FOLDER}"
            tarBackup
        fi
    fi
}
tarBackup()
{
    tarFolder="${PROJECT_FOLDER}_backup_$(date "+%Y%m%d%H%M%S")"
    if [ -d ./"${TEMP_FOLDER}"/"${PROJECT_FOLDER}" ]; then
        rm -rf ./"${TEMP_FOLDER}"/"${PROJECT_FOLDER}"
    fi
    bashCommand "mkdir -p ./${TEMP_FOLDER}/${PROJECT_FOLDER}"
    bashCommand "find . -type f -o \( -path ./${BUILD_FOLDER} -o -path ./${TEMP_FOLDER} \
-o -path './.*' \) -prune -o -print | sed 1d \
| grep -E '${INCLUDE_FOLDER}|${SOURCE_FOLDER}|${UTILITY_FOLDER}|${SCRIPT_FOLDER}' \
| xargs -i cp -R {} ${TEMP_FOLDER}/${PROJECT_FOLDER}/"
    bashCommand "find . -maxdepth 1 -type d -o -print | grep -E '*\.txt' \
| xargs -i cp -R {} ${TEMP_FOLDER}/${PROJECT_FOLDER}/"
    bashCommand "tar -zcvf ${TEMP_FOLDER}/${tarFolder}.tar.gz -C ./${TEMP_FOLDER} \
${PROJECT_FOLDER}"
    bashCommand "rm -rf ./${TEMP_FOLDER}/${PROJECT_FOLDER}"
}

performOptionTag()
{
    if [ "${ARGS_TAG}" = true ]; then
        if
            command -v gtags >/dev/null 2>&1
        then
            bashCommand "find ./${INCLUDE_FOLDER} ./${SOURCE_FOLDER} ./${UTILITY_FOLDER} -type f \
-print | grep -E '*\.cpp|*\.hpp' | gtags -i -v -f -"
        else
            printAbort "There is no gtags program. Please check it."
        fi
    fi
}

main()
{
    cd "$(dirname "$(dirname "$0")")" || exit 1

    parseArgs "$@"
    checkDependencies "$@"

    performOptionCleanup
    generateCMakeFiles

    performOptionFormat
    performOptionLint
    performOptionHtml
    performOptionBackup
    performOptionTag

    compileCode
}

main "$@"
