#!/usr/bin/env bash

ARGS_FORMAT=false
ARGS_LINT=false
ARGS_BROWSER=false
ARGS_DOXYGEN=false
ARGS_CONTAINER=false
ARGS_RELEASE=false
PERFORM_COMPILE=false

PROJECT_FOLDER="foo"
APPLICATION_FOLDER="application"
UTILITY_FOLDER="utility"
ALGORITHM_FOLDER="algorithm"
DATA_STRUCTURE_FOLDER="data_structure"
DESIGN_PATTERN_FOLDER="design_pattern"
NUMERIC_FOLDER="numeric"
SCRIPT_FOLDER="script"
DOCKER_FOLDER="docker"
DOCUMENT_FOLDER="document"
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
DOXYGEN_FILE="Doxyfile"

bashCommand()
{
    echo
    echo "$(date "+%b %d %T") $* START" || true
    /bin/bash -c "$@"
    echo "$(date "+%b %d %T") $* FINISH" || true
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
    echo "-h, --help         show help and exit"
    echo "-f, --format       format code"
    echo "-l, --lint         lint code"
    echo "-C, --cleanup      cleanup project"
    echo "-b, --browser      generate code browser"
    echo "-d, --doxygen      generate doxygen document"
    echo "-c, --container    construct docker container"
    echo "-r, --release      build release version"
    exit 0
}

cleanupProject()
{
    bashCommand "rm -rf ./${BUILD_FOLDER} ./${SCRIPT_FOLDER}/__pycache__ ./${TEMPORARY_FOLDER}"
    bashCommand "find ./${DOCUMENT_FOLDER} -maxdepth 1 -type d | sed 1d | grep -E 'browser|doxygen' \
| xargs -i rm -rf {}"
    bashCommand "rm -rf ./core* ./vgcore* ./*.profraw"
}

parseArgs()
{
    while [[ "$#" -gt 0 ]]; do
        case $1 in
        -h | --help) showHelp ;;
        -f | --format) ARGS_FORMAT=true ;;
        -l | --lint) ARGS_LINT=true ;;
        -C | --cleanup) cleanupProject ;;
        -b | --browser) ARGS_BROWSER=true ;;
        -d | --doxygen) ARGS_DOXYGEN=true ;;
        -c | --container) ARGS_CONTAINER=true ;;
        -r | --release) ARGS_RELEASE=true ;;
        *) printException "Unknown command line option: $1. Try with --help to get information." ;;
        esac
        shift
    done
}

checkDependencies()
{
    if {
        [[ "$#" -eq 1 ]] && [[ "${ARGS_RELEASE}" = true ]]
    } || [[ "$#" -eq 0 ]]; then
        PERFORM_COMPILE=true
    fi

    if [[ ! -d ./"${APPLICATION_FOLDER}" ]] || [[ ! -d ./"${UTILITY_FOLDER}" ]] || [[ ! -d ./"${ALGORITHM_FOLDER}" ]] \
        || [[ ! -d ./"${DATA_STRUCTURE_FOLDER}" ]] || [[ ! -d ./"${DESIGN_PATTERN_FOLDER}" ]] \
        || [[ ! -d ./"${NUMERIC_FOLDER}" ]] || [[ ! -d ./"${SCRIPT_FOLDER}" ]]; then
        printException "Missing code folders in ${PROJECT_FOLDER} folder. Please check it."
    fi

    if
        ! command -v cmake >/dev/null 2>&1 \
            || ! command -v ninja >/dev/null 2>&1
    then
        printException "No cmake or ninja program. Please check it."
    fi

    if [[ "${ARGS_FORMAT}" = true ]]; then
        if
            command -v clang-format-12 >/dev/null 2>&1 \
                && command -v shfmt >/dev/null 2>&1 \
                && command -v black >/dev/null 2>&1
        then
            if [[ ! -f ./"${FORMAT_CONFIG_CPP}" ]] || [[ ! -f ./"${FORMAT_CONFIG_PY}" ]] \
                || [[ ! -f ./"${FORMAT_CONFIG_SH}" ]]; then
                printException "Missing format config files in ${PROJECT_FOLDER} folder. Please check it."
            fi
        else
            printException "No clang-format, shfmt or black program. Please check it."
        fi
    fi

    if [[ "${ARGS_LINT}" = true ]]; then
        if
            command -v clang-tidy-12 >/dev/null 2>&1 \
                && command -v run-clang-tidy-12.py >/dev/null 2>&1 \
                && command -v compdb >/dev/null 2>&1 \
                && command -v shellcheck >/dev/null 2>&1 \
                && command -v pylint >/dev/null 2>&1
        then
            if [[ ! -f ./"${LINT_CONFIG_CPP}" ]] || [[ ! -f ./"${LINT_CONFIG_PY}" ]] \
                || [[ ! -f ./"${LINT_CONFIG_SH}" ]]; then
                printException "Missing lint config files in ${PROJECT_FOLDER} folder. Please check it."
            fi
            if [[ ! -f ./"${BUILD_FOLDER}"/"${COMPILE_COMMANDS}" ]]; then
                printException "No ${COMPILE_COMMANDS} file in ${BUILD_FOLDER} folder. Please generate it."
            fi
        else
            printException "No clang-tidy (involving run-clang-tidy-12.py, compdb), shellcheck or pylint program. \
Please check it."
        fi
    fi

    if [[ "${ARGS_BROWSER}" = true ]]; then
        if [[ ! -d ./"${DOCUMENT_FOLDER}" ]]; then
            printException "Missing ${DOCUMENT_FOLDER} folder in ${PROJECT_FOLDER} folder. Please check it."
        fi
        if
            ! command -v codebrowser_generator >/dev/null 2>&1 \
                || ! command -v codebrowser_indexgenerator >/dev/null 2>&1
        then
            printException "No codebrowser_generator or codebrowser_indexgenerator program. Please check it."
        fi
    fi

    if [[ "${ARGS_DOXYGEN}" = true ]]; then
        if [[ ! -d ./"${DOCUMENT_FOLDER}" ]]; then
            printException "Missing ${DOCUMENT_FOLDER} folder in ${PROJECT_FOLDER} folder. Please check it."
        fi
        if
            command -v doxygen >/dev/null 2>&1 \
                && command -v dot >/dev/null 2>&1
        then
            if [[ ! -f ./"${DOCUMENT_FOLDER}"/"${DOXYGEN_FILE}" ]]; then
                printException "No ${DOXYGEN_FILE} file in ${DOCUMENT_FOLDER} folder. Please check it."
            fi
        else
            printException "No doxygen or dot program. Please check it."
        fi
    fi

    if [[ "${ARGS_CONTAINER}" = true ]]; then
        if [[ ! -d ./"${DOCKER_FOLDER}" ]]; then
            printException "Missing ${DOCKER_FOLDER} folder in ${PROJECT_FOLDER} folder. Please check it."
        fi
        if command -v docker >/dev/null 2>&1; then
            if [[ ! -f ./"${DOCKER_FOLDER}"/"${DOCKER_FILE}" ]]; then
                printException "No ${DOCKER_FILE} file in ${DOCKER_FOLDER} folder. Please check it."
            fi

            echo "Please confirm further whether construct docker container. (y or n)"
            oldStty=$(stty -g)
            stty raw -echo
            answer=$(while ! head -c 1 | grep -i '[ny]' || true; do true; done)
            stty "${oldStty}"
            if echo "${answer}" | grep -iq "^y"; then
                echo "Yes"
            else
                echo "No"
                ARGS_CONTAINER=false
            fi
        else
            printException "No docker program. Please check it."
        fi
    fi
}

generateCMakeFiles()
{
    if [[ -f ./"${CMAKE_LISTS}" ]]; then
        if [[ ! -d ./"${BUILD_FOLDER}" ]]; then
            bashCommand "mkdir ./${BUILD_FOLDER}"
        fi

        export CC=/usr/bin/clang-12 CXX=/usr/bin/clang++-12
        if [[ "${ARGS_RELEASE}" = true ]]; then
            buildType="Release"
        else
            buildType="Debug"
        fi
        bashCommand "cmake -S . -B ./${BUILD_FOLDER} -G Ninja -DCMAKE_CXX_COMPILER=clang++-12 \
-DCMAKE_BUILD_TYPE=${buildType}"
    else
        printException "No ${CMAKE_LISTS} file in ${PROJECT_FOLDER} folder. Please check it."
    fi
}

compileCode()
{
    if [[ "${PERFORM_COMPILE}" = true ]]; then
        bashCommand "tput setaf 2; tput bold; cmake --build ./${BUILD_FOLDER}; tput sgr0"
    fi
}

performFormatOption()
{
    if [[ "${ARGS_FORMAT}" = true ]]; then
        bashCommand "find ./${APPLICATION_FOLDER} ./${UTILITY_FOLDER} ./${ALGORITHM_FOLDER} ./${DATA_STRUCTURE_FOLDER} \
./${DESIGN_PATTERN_FOLDER} ./${NUMERIC_FOLDER} -name *.cpp -o -name *.hpp | xargs clang-format-12 -i --verbose"
        bashCommand "shfmt -l -w ./${SCRIPT_FOLDER}/*.sh"
        bashCommand "black --config ./${FORMAT_CONFIG_PY} ./${SCRIPT_FOLDER}/*.py"
    fi
}

performLintOption()
{
    if [[ "${ARGS_LINT}" = true ]]; then
        bashCommand "compdb -p ./${BUILD_FOLDER} list > ./${COMPILE_COMMANDS} && \
mv ./${COMPILE_COMMANDS} ./${BUILD_FOLDER}"
        bashCommand "find ./${APPLICATION_FOLDER} ./${UTILITY_FOLDER} \
./${ALGORITHM_FOLDER} ./${DATA_STRUCTURE_FOLDER} ./${DESIGN_PATTERN_FOLDER} ./${NUMERIC_FOLDER} \
-name *.cpp -o -name *.hpp | xargs run-clang-tidy-12.py -p ./${BUILD_FOLDER} -quiet"
        generateCMakeFiles
        bashCommand "shellcheck ./${SCRIPT_FOLDER}/*.sh"
        bashCommand "pylint --rcfile=${LINT_CONFIG_PY} ./${SCRIPT_FOLDER}/*.py"
    fi
}

performBrowserOption()
{
    if [[ "${ARGS_BROWSER}" = true ]]; then
        commitId=$(git rev-parse --short @)
        if [[ -z "${commitId}" ]]; then
            commitId="local"
        fi
        if [[ -d ./"${TEMPORARY_FOLDER}" ]]; then
            lastTar="${PROJECT_FOLDER}_browser_${commitId}.tar.bz2"
            if [[ -f ./"${TEMPORARY_FOLDER}"/"${lastTar}" ]]; then
                timeDiff=$(($(date +%s) - $(stat -L --format %Y "./${TEMPORARY_FOLDER}/${lastTar}")))
                if [[ "${timeDiff}" -lt "10" ]]; then
                    printException "The latest browser tarball ${TEMPORARY_FOLDER}/${lastTar} has been generated since \
${timeDiff}s ago."
                fi
            fi
            tarHtmlForBrowser "${commitId}"
        else
            bashCommand "mkdir ./${TEMPORARY_FOLDER}"
            tarHtmlForBrowser "${commitId}"
        fi
    fi
}

tarHtmlForBrowser()
{
    browserFolder="browser"
    tarFile="${PROJECT_FOLDER}_${browserFolder}_$1.tar.bz2"
    rm -rf ./"${DOCUMENT_FOLDER}"/"${browserFolder}" ./"${TEMPORARY_FOLDER}"/"${tarFile}"

    bashCommand "mkdir -p ./${DOCUMENT_FOLDER}/${browserFolder}"
    bashCommand "codebrowser_generator -color -a -b ./${BUILD_FOLDER}/${COMPILE_COMMANDS} \
-o ./${DOCUMENT_FOLDER}/${browserFolder} -p ${PROJECT_FOLDER}:.:$1 -d ./data"
    bashCommand "codebrowser_indexgenerator ./${DOCUMENT_FOLDER}/${browserFolder} -d ./data"
    bashCommand "cp -rf /usr/local/share/woboq/data ./${DOCUMENT_FOLDER}/${browserFolder}/"
    bashCommand "tar -jcvf ./${TEMPORARY_FOLDER}/${tarFile} -C ./${DOCUMENT_FOLDER} ${browserFolder} >/dev/null"
}

performDoxygenOption()
{
    if [[ "${ARGS_DOXYGEN}" = true ]]; then
        commitId=$(git rev-parse --short @)
        if [[ -z "${commitId}" ]]; then
            commitId="local"
        fi
        if [[ -d ./"${TEMPORARY_FOLDER}" ]]; then
            lastTar="${PROJECT_FOLDER}_doxygen_${commitId}.tar.bz2"
            if [[ -f ./"${TEMPORARY_FOLDER}"/"${lastTar}" ]]; then
                timeDiff=$(($(date +%s) - $(stat -L --format %Y "./${TEMPORARY_FOLDER}/${lastTar}")))
                if [[ "${timeDiff}" -lt "10" ]]; then
                    printException "The latest doxygen tarball ${TEMPORARY_FOLDER}/${lastTar} has been generated since \
${timeDiff}s ago."
                fi
            fi
            tarHtmlForDoxygen "${commitId}"
        else
            bashCommand "mkdir ./${TEMPORARY_FOLDER}"
            tarHtmlForDoxygen "${commitId}"
        fi
    fi
}

tarHtmlForDoxygen()
{
    doxygenFolder="doxygen"
    tarFile="${PROJECT_FOLDER}_${doxygenFolder}_$1.tar.bz2"
    rm -rf ./"${DOCUMENT_FOLDER}"/"${doxygenFolder}" ./"${TEMPORARY_FOLDER}"/"${tarFile}"

    bashCommand "mkdir -p ./${DOCUMENT_FOLDER}/${doxygenFolder}"
    bashCommand "doxygen ./${DOCUMENT_FOLDER}/${DOXYGEN_FILE} >/dev/null"
    bashCommand "tar -jcvf ./${TEMPORARY_FOLDER}/${tarFile} -C ./${DOCUMENT_FOLDER} ${doxygenFolder} >/dev/null"
}

performContainerOption()
{
    if [[ "${ARGS_CONTAINER}" = true ]]; then
        toBuildImage=false
        if service docker status | grep -q "active (running)" 2>/dev/null || true; then
            imageRepo="ryftchen/${PROJECT_FOLDER}"
            if ! docker ps -a --format "{{lower .Image}}" | grep -q "${imageRepo}":latest 2>/dev/null || true; then
                if {
                    ! docker image ls -a --format "{{lower .Repository}}" | grep -q "${imageRepo}" 2>/dev/null || true
                } || {
                    ! docker image ls -a | tail -n +2 | grep "${imageRepo}" | awk '{split($0, a, " "); print a[2]}' \
                        | grep -q "latest" 2>/dev/null || true
                }; then
                    if docker search "${imageRepo}" --format "{{lower .Name}}" \
                        | grep -q "${imageRepo}" 2>/dev/null || true; then
                        tags=$(curl -sS "https://registry.hub.docker.com/v2/repositories/${imageRepo}/tags" \
                            | sed -Ee 's/("name":)"([^"]*)"/\n\1\2\n/g' | grep '"name":' \
                            | awk -F: '{printf("%s\n", $2)}' || true)
                        if echo "${tags}" | grep -q "latest" 2>/dev/null; then
                            bashCommand "docker pull ${imageRepo}:latest"
                        else
                            toBuildImage=true
                        fi
                    else
                        toBuildImage=true
                    fi
                    if [[ "${toBuildImage}" = true ]]; then
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
    cd "${0%%"${SCRIPT_FOLDER}"*}" || exit 1
    trap "tput sgr0" INT TERM

    parseArgs "$@"
    checkDependencies "$@"
    generateCMakeFiles

    performFormatOption
    performLintOption
    performBrowserOption
    performDoxygenOption
    performContainerOption

    compileCode
}

main "$@"
