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

function shell_command()
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

function signal_handler()
{
    tput sgr0
    if [[ ${ARGS[lint]} = true ]]; then
        local app_comp_cmd=${FOLDER[bld]}/${COMP_CMD}
        if [[ -f ./${app_comp_cmd}.bak ]]; then
            rm -rf "./${app_comp_cmd}" && mv "./${app_comp_cmd}.bak" "./${app_comp_cmd}"
        fi
        local tst_comp_cmd=${FOLDER[tst]}/${FOLDER[bld]}/${COMP_CMD}
        if [[ -f ./${tst_comp_cmd}.bak ]]; then
            rm -rf "./${tst_comp_cmd}" && mv "./${tst_comp_cmd}.bak" "./${tst_comp_cmd}"
        fi
    fi
    if [[ ${ARGS[doxygen]} = true ]] && [[ ${ARGS[release]} = false ]]; then
        sed -i "s/\(^PROJECT_NUMBER[ ]\+=\)\([ ]\+.*\)/\1/" "./${FOLDER[doc]}/Doxyfile"
        sed -i "s/\(^HTML_TIMESTAMP[ ]\+=\)\([ ]\+YES\)/\1 NO/" "./${FOLDER[doc]}/Doxyfile"
    fi
    exit 1
}

function parse_arguments()
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

function check_basic_dependencies()
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

function check_extra_dependencies()
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

function set_compile_environment()
{
    local tmpfs_subfolder=$1 tmpfs_size=$2

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
        if [[ ! -d ./${tmpfs_subfolder} ]]; then
            mkdir "./${tmpfs_subfolder}"
        fi
        if ! df -h -t tmpfs | grep -q "${FOLDER[proj]}/${tmpfs_subfolder}" 2>/dev/null; then
            shell_command "mount -t tmpfs -o size=${tmpfs_size} tmpfs ./${tmpfs_subfolder}"
        fi
    elif df -h -t tmpfs | grep -q "${FOLDER[proj]}/${tmpfs_subfolder}" 2>/dev/null; then
        shell_command "umount ./${tmpfs_subfolder}"
    fi
}

function build_target()
{
    set_compile_environment "${FOLDER[bld]}" "256m"

    shell_command "cmake -S . -B ./${FOLDER[bld]} -G Ninja""${CMAKE_CACHE_ENTRY}"
    if [[ ${BUILD_APP_ONLY} = true ]]; then
        tput setaf 2 && tput bold
        shell_command "cmake --build ./${FOLDER[bld]}""${CMAKE_BUILD_OPTION}"
        tput sgr0
        exit 0
    fi
    shell_command "cmake -S ./${FOLDER[tst]} -B ./${FOLDER[tst]}/${FOLDER[bld]} -G Ninja""${CMAKE_CACHE_ENTRY}"
}

function perform_help_option()
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

function perform_cleanup_option()
{
    if [[ ${ARGS[cleanup]} = true ]]; then
        shell_command "find ./ -maxdepth 2 -type d | sed 1d \
| grep -E '(${FOLDER[bld]}|${FOLDER[temp]}|browser|doxygen|__pycache__)$' | xargs -i rm -rf {}"
        shell_command "rm -rf ./${FOLDER[scr]}/.env ./core.* ./vgcore.* ./*.profraw"
        exit 0
    fi
}

function perform_environment_option()
{
    if [[ ${ARGS[environment]} = true ]]; then
        shell_command "cat <<EOF >./${FOLDER[scr]}/.env
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

function perform_container_option()
{
    if [[ ${ARGS[docker]} = true ]]; then
        if command -v docker >/dev/null 2>&1 && command -v docker-compose >/dev/null 2>&1; then
            echo "Please confirm whether continue constructing the docker container. (y or n)"
            local old_stty
            old_stty=$(stty -g)
            stty raw -echo
            local answer
            answer=$(while ! head -c 1 | grep -i '[ny]'; do true; done)
            stty "${old_stty}"
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
            shell_command "docker-compose -f ./docker/docker-compose.yml up -d"
            exit 0
        else
            abort "The container exists."
        fi
    fi
}

function perform_test_option()
{
    if [[ ${ARGS[test]} = true ]]; then
        set_compile_environment "${FOLDER[tst]}/${FOLDER[bld]}" "128m"

        shell_command "cmake -S ./${FOLDER[tst]} -B ./${FOLDER[tst]}/${FOLDER[bld]} -G Ninja""${CMAKE_CACHE_ENTRY}"
        tput setaf 2 && tput bold
        shell_command "cmake --build ./${FOLDER[tst]}/${FOLDER[bld]}""${CMAKE_BUILD_OPTION}"
        tput sgr0
        exit 0
    fi
}

function perform_format_option()
{
    if [[ ${ARGS[format]} = true ]]; then
        shell_command "find ./${FOLDER[app]} ./${FOLDER[util]} ./${FOLDER[algo]} ./${FOLDER[ds]} ./${FOLDER[dp]} \
./${FOLDER[num]} ./${FOLDER[tst]} -name *.cpp -o -name *.hpp -o -name *.tpp | grep -v '/${FOLDER[bld]}/' \
| xargs clang-format-12 -i --verbose --Werror"
        shell_command "shfmt -l -w ./${FOLDER[scr]}/*.sh"
        shell_command "black --config ./.toml ./${FOLDER[scr]}/*.py"
    fi
}

function perform_lint_option()
{
    if [[ ${ARGS[lint]} = true ]]; then
        local app_comp_cmd=${FOLDER[bld]}/${COMP_CMD}
        if [[ ! -f ./${app_comp_cmd} ]]; then
            abort "There is no ${COMP_CMD} file in the ${FOLDER[bld]} folder. Please generate it."
        fi
        compdb -p "./${FOLDER[bld]}" list >"./${COMP_CMD}" && mv "./${app_comp_cmd}" "./${app_comp_cmd}.bak" \
            && mv "./${COMP_CMD}" "./${FOLDER[bld]}"
        while true; do
            local line
            line=$(grep -n '.tpp' "./${app_comp_cmd}" | head -n 1 | cut -d : -f 1)
            if ! [[ ${line} =~ ^[0-9]+$ ]]; then
                break
            fi
            sed -i $(("${line}" - 2)),$(("${line}" + 3))d "./${app_comp_cmd}"
        done
        shell_command "find ./${FOLDER[app]} ./${FOLDER[util]} ./${FOLDER[algo]} ./${FOLDER[ds]} ./${FOLDER[dp]} \
./${FOLDER[num]} -name *.cpp -o -name *.hpp | xargs run-clang-tidy-12 -p ./${FOLDER[bld]} -quiet"
        shell_command "find ./${FOLDER[app]} ./${FOLDER[util]} ./${FOLDER[algo]} ./${FOLDER[ds]} ./${FOLDER[dp]} \
./${FOLDER[num]} -name *.tpp | xargs clang-tidy-12 --use-color -p ./${FOLDER[bld]} -quiet"
        rm -rf "./${app_comp_cmd}" && mv "./${app_comp_cmd}.bak" "./${app_comp_cmd}"

        local tst_comp_cmd=${FOLDER[tst]}/${FOLDER[bld]}/${COMP_CMD}
        if [[ ! -f ./${tst_comp_cmd} ]]; then
            abort "There is no ${COMP_CMD} file in the ${FOLDER[tst]}/${FOLDER[bld]} folder. Please generate it."
        fi
        compdb -p "./${FOLDER[tst]}/${FOLDER[bld]}" list >"./${COMP_CMD}" \
            && mv "./${tst_comp_cmd}" "./${tst_comp_cmd}.bak" && mv "./${COMP_CMD}" "./${FOLDER[tst]}/${FOLDER[bld]}"
        shell_command "find ./${FOLDER[tst]} -name *.cpp \
| xargs run-clang-tidy-12 -p ./${FOLDER[tst]}/${FOLDER[bld]} -quiet"
        rm -rf "./${tst_comp_cmd}" && mv "./${tst_comp_cmd}.bak" "./${tst_comp_cmd}"

        shell_command "shellcheck ./${FOLDER[scr]}/*.sh"
        shell_command "pylint --rcfile=./.pylintrc ./${FOLDER[scr]}/*.py"
    fi
}

function perform_count_option()
{
    if [[ ${ARGS[count]} = true ]]; then
        shell_command "find ./${FOLDER[app]} ./${FOLDER[util]} ./${FOLDER[algo]} ./${FOLDER[ds]} ./${FOLDER[dp]} \
./${FOLDER[num]} ./${FOLDER[tst]} -name *.cpp -o -name *.hpp -o -name *.tpp | grep -v '/${FOLDER[bld]}/' \
| xargs cloc --by-file-by-lang --force-lang='C++',tpp --include-lang='C++','C/C++ Header' --by-percent=cmb"
    fi
}

function perform_browser_option()
{
    if [[ ${ARGS[browser]} = true ]]; then
        local commit_id
        commit_id=$(git rev-parse --short @)
        if [[ -z ${commit_id} ]]; then
            commit_id="local"
        fi
        if [[ -d ./${FOLDER[temp]} ]]; then
            local last_tar="${FOLDER[proj]}_browser_${commit_id}.tar.bz2"
            if [[ -f ./${FOLDER[temp]}/${last_tar} ]]; then
                local time_interval=$(($(date +%s) - $(stat -L --format %Y "./${FOLDER[temp]}/${last_tar}")))
                if [[ ${time_interval} -lt 10 ]]; then
                    abort "The latest browser tarball ${FOLDER[temp]}/${last_tar} has been generated since \
${time_interval}s ago."
                fi
            fi
            package_for_browser "${commit_id}"
        else
            mkdir "./${FOLDER[temp]}"
            package_for_browser "${commit_id}"
        fi
    fi
}

function package_for_browser()
{
    local commit_id=$1

    if [[ ! -f ./${FOLDER[bld]}/${COMP_CMD} ]]; then
        abort "There is no ${COMP_CMD} file in the ${FOLDER[bld]} folder. Please generate it."
    fi
    local browser_folder="browser"
    local tar_file="${FOLDER[proj]}_${browser_folder}_${commit_id}.tar.bz2"
    rm -rf "./${FOLDER[temp]}/${FOLDER[proj]}_${browser_folder}"_*.tar.bz2 "./${FOLDER[doc]}/${browser_folder}"

    mkdir -p "./${FOLDER[doc]}/${browser_folder}"
    shell_command "codebrowser_generator -color -a -b ./${FOLDER[bld]}/${COMP_CMD} \
-o ./${FOLDER[doc]}/${browser_folder} -p ${FOLDER[proj]}:.:${commit_id} -d ./data"
    shell_command "codebrowser_generator -color -a -b ./${FOLDER[tst]}/${FOLDER[bld]}/${COMP_CMD} \
-o ./${FOLDER[doc]}/${browser_folder} -p ${FOLDER[proj]}:.:${commit_id} -d ./data"
    shell_command "codebrowser_indexgenerator ./${FOLDER[doc]}/${browser_folder} -d ./data"
    shell_command "cp -rf /usr/local/share/woboq/data ./${FOLDER[doc]}/${browser_folder}/"
    shell_command "tar -jcvf ./${FOLDER[temp]}/${tar_file} -C ./${FOLDER[doc]} ${browser_folder} >/dev/null"
}

function perform_doxygen_option()
{
    if [[ ${ARGS[doxygen]} = true ]]; then
        local commit_id
        commit_id=$(git rev-parse --short @)
        if [[ -z ${commit_id} ]]; then
            commit_id="local"
        fi
        if [[ -d ./${FOLDER[temp]} ]]; then
            local last_tar="${FOLDER[proj]}_doxygen_${commit_id}.tar.bz2"
            if [[ -f ./${FOLDER[temp]}/${last_tar} ]]; then
                local time_interval=$(($(date +%s) - $(stat -L --format %Y "./${FOLDER[temp]}/${last_tar}")))
                if [[ ${time_interval} -lt 10 ]]; then
                    abort "The latest doxygen tarball ${FOLDER[temp]}/${last_tar} has been generated since \
${time_interval}s ago."
                fi
            fi
            package_for_doxygen "${commit_id}"
        else
            mkdir "./${FOLDER[temp]}"
            package_for_doxygen "${commit_id}"
        fi
    fi
}

function package_for_doxygen()
{
    local commit_id=$1

    local doxygen_folder="doxygen"
    local tar_file="${FOLDER[proj]}_${doxygen_folder}_${commit_id}.tar.bz2"
    rm -rf "./${FOLDER[temp]}/${FOLDER[proj]}_${doxygen_folder}"_*.tar.bz2 "./${FOLDER[doc]}/${doxygen_folder}"

    mkdir -p "./${FOLDER[doc]}/${doxygen_folder}"
    if [[ ${ARGS[release]} = false ]]; then
        sed -i "s/\(^PROJECT_NUMBER[ ]\+=\)/\1 \"@ $(git rev-parse --short @)\"/" "./${FOLDER[doc]}/Doxyfile"
        sed -i "s/\(^HTML_TIMESTAMP[ ]\+=\)\([ ]\+NO\)/\1 YES/" "./${FOLDER[doc]}/Doxyfile"
    fi
    shell_command "doxygen ./${FOLDER[doc]}/Doxyfile >/dev/null"
    shell_command "tar -jcvf ./${FOLDER[temp]}/${tar_file} -C ./${FOLDER[doc]} ${doxygen_folder} >/dev/null"
    if [[ ${ARGS[release]} = false ]]; then
        sed -i "s/\(^PROJECT_NUMBER[ ]\+=\)\([ ]\+.*\)/\1/" "./${FOLDER[doc]}/Doxyfile"
        sed -i "s/\(^HTML_TIMESTAMP[ ]\+=\)\([ ]\+YES\)/\1 NO/" "./${FOLDER[doc]}/Doxyfile"
    fi
}

function main()
{
    cd "${0%%"${FOLDER[scr]}"*}" || exit 1
    export TERM=linux TERMINFO=/etc/terminfo
    trap "signal_handler" INT TERM

    parse_arguments "$@"
    perform_help_option
    perform_cleanup_option
    perform_environment_option
    perform_container_option

    check_basic_dependencies "$@"
    perform_test_option
    build_target

    check_extra_dependencies
    perform_format_option
    perform_lint_option
    perform_count_option
    perform_browser_option
    perform_doxygen_option
}

main "$@"
