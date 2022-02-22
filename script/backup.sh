#!/usr/bin/env bash
# chmod 755 backup.sh
# tar -zxvf foo_XXXXXXXXXXXXXX.tar.gz

PROJECT_FOLDER="foo"
INCLUDE_FOLDER="include"
SOURCE_FOLDER="source"
LIBRARY_FOLDER="library"
SCRIPT_FOLDER="script"
BUILD_FOLDER="build"
BACKUP_FOLDER="backup"
TEMP_FOLDER="temp"
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
    if [ ! -d ./"${INCLUDE_FOLDER}" ] | [ ! -d ./"${SOURCE_FOLDER}" ] \
        | [ ! -d ./"${LIBRARY_FOLDER}" ] | [ ! -d ./"${SCRIPT_FOLDER}" ] \
        | [ ! -f "${CMAKE_FILE}" ] | [ ! -f "${README_FILE}" ]; then
        printAbort "There are missing files in ${PROJECT_FOLDER} folder."
    fi

    if [ -d "${BACKUP_FOLDER}" ]; then
        files=$(find "${BACKUP_FOLDER}"/ -type f -name "${PROJECT_FOLDER}_*.tar.gz" 2>/dev/null \
            | wc -l)
        if [ "${files}" != "0" ]; then
            lastTar=$(find "${BACKUP_FOLDER}"/ -type f -name "${PROJECT_FOLDER}_*.tar.gz" -print0 \
                | xargs --null ls -at | head -n 1)
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
}

main "$@"
