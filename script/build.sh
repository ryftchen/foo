#!/usr/bin/env bash

declare -rA FOLDER=([proj]="foo" [app]="application" [util]="utility" [algo]="algorithm" [ds]="data_structure"
    [dp]="design_pattern" [num]="numeric" [tst]="test" [scr]="script" [doc]="document" [dock]="docker" [bld]="build"
    [rep]="report" [cac]=".cache")
declare -r COMPILE_DB="compile_commands.json"
declare -r BASH_RC=".bashrc"
declare -r GIT_CHANGE_CMD="git status --porcelain -z | cut -z -c4- | tr '\0' '\n'"
declare -rA ESC_COLOR=([exec]="\033[0;33;40m\033[1m\033[49m" [succ]="\033[0;32;40m\033[1m\033[49m"
    [fail]="\033[0;31;40m\033[1m\033[49m" [time]="\033[0;39;40m\033[1m\033[2m\033[49m")
declare -r ESC_OFF="\033[0m"
declare -A ARGS=([help]=false [assume]=false [quick]=false [dry]=false [initialize]=false [clean]=false [install]=false
    [uninstall]=false [container]=false [archive]=false [test]=false [release]=false [hook]=false [spell]=false
    [statistics]=false [format]=false [lint]=false [query]=false [doxygen]=false [browser]=false)
declare -A DEV_OPT=([compiler]="gcc" [parallel]=0 [pch]=false [unity]=false [ccache]=false [distcc]="localhost"
    [tmpfs]=false)
declare SUDO_PREFIX=""
declare STATUS=0
declare CMAKE_CACHE_ENTRY=""
declare CMAKE_BUILD_OPTION=""
declare BUILD_TYPE="Debug"

function shell()
{
    /bin/bash -c "$@"
}

function die()
{
    echo "$(basename "${0}"): $*"

    exit 1
} >&2

function shell_command()
{
    if [[ ${ARGS[dry]} != false ]]; then
        printf "${ESC_COLOR[exec]}[ exec ] ${ESC_COLOR[time]}$(date "+%b %d %T")${ESC_OFF} $ %s\n" "$*"
        echo
        return
    fi

    printf "${ESC_COLOR[exec]}[ exec ] ${ESC_COLOR[time]}$(date "+%b %d %T")${ESC_OFF} $ %s\n" "$*"
    if shell "$@"; then
        printf "${ESC_COLOR[succ]}[ succ ] ${ESC_COLOR[time]}$(date "+%b %d %T")${ESC_OFF} $ %s\n" "$*"
    else
        printf "${ESC_COLOR[fail]}[ fail ] ${ESC_COLOR[time]}$(date "+%b %d %T")${ESC_OFF} $ %s\n" "$*"
        STATUS=1
    fi
    echo
}

function wait_until_get_input()
{
    if [[ ${ARGS[assume]} != false ]]; then
        echo "${ARGS[assume]}"
        return 0
    fi

    local old_stty
    old_stty=$(stty -g)
    stty raw -echo
    local answer
    answer=$(while ! head -c 1 | grep -i '[yn]'; do true; done)
    stty "${old_stty}"
    echo "${answer}"

    return 0
}

function validate_single_choice_exclusivity()
{
    if [[ $(count_enabled_single_choice_parameters) -gt 0 ]] || [[ $(count_enabled_multiple_choice_parameters) -gt 0 ]]; then
        die "Mutually exclusive option: $1 is not allowed."
    fi
}

function validate_multiple_choice_exclusivity()
{
    if [[ $(count_enabled_single_choice_parameters) -gt 0 ]]; then
        die "Mutually exclusive option: $1 is not allowed."
    fi
}

function count_enabled_single_choice_parameters()
{
    local number=0
    for key in "${!ARGS[@]}"; do
        if [[ ${key} == "initialize" ]] || [[ ${key} == "clean" ]] || [[ ${key} == "install" ]] \
            || [[ ${key} == "uninstall" ]] || [[ ${key} == "container" ]] || [[ ${key} == "archive" ]] \
            || [[ ${key} == "test" ]]; then
            if [[ ${ARGS[${key}]} != false ]]; then
                number+=1
            fi
        fi
    done
    echo "${number}"

    return 0
}

function count_enabled_multiple_choice_parameters()
{
    local number=0
    for key in "${!ARGS[@]}"; do
        if [[ ${key} == "release" ]] || [[ ${key} == "hook" ]] || [[ ${key} == "spell" ]] \
            || [[ ${key} == "statistics" ]] || [[ ${key} == "format" ]] || [[ ${key} == "lint" ]] \
            || [[ ${key} == "query" ]] || [[ ${key} == "doxygen" ]] || [[ ${key} == "browser" ]]; then
            if [[ ${ARGS[${key}]} != false ]]; then
                number+=1
            fi
        fi
    done
    echo "${number}"

    return 0
}

function handle_type_related_parameter()
{
    local option=$1
    shift

    if [[ ${ARGS[${option}]} != false ]]; then
        die "Set the required parameter repeatedly for the ${option} option: $1."
    fi
    if [[ $# -gt 1 ]]; then
        case $2 in
        cpp | sh | py | rs)
            ARGS[${option}]=$2
            return 1
            ;;
        -*)
            ARGS[${option}]=true
            return 0
            ;;
        *)
            die "Invalid argument for the $1 option: $2. Only 'cpp', 'sh', 'py' or 'rs' are allowed."
            ;;
        esac
    else
        ARGS[${option}]=true
        return 0
    fi
}

function parse_parameters()
{
    while [[ $# -gt 0 ]]; do
        case $1 in
        -h | --help)
            ARGS[help]=true
            ;;
        -A | --assume)
            if [[ ${ARGS[assume]} != false ]]; then
                die "Set the required parameter repeatedly for the $1 option: $2."
            fi
            if [[ $# -lt 2 ]]; then
                die "The $1 command line option requires an argument."
            fi
            case $2 in
            y | n)
                ARGS[assume]=$2
                shift
                ;;
            *)
                die "Invalid argument for the $1 option: $2. Only 'y' or 'n' are allowed."
                ;;
            esac
            ;;
        -Q | --quick)
            ARGS[quick]=true
            ;;
        -D | --dry)
            ARGS[dry]=true
            ;;
        -I | --initialize)
            validate_single_choice_exclusivity "$1"
            ARGS[initialize]=true
            ;;
        -C | --clean)
            validate_single_choice_exclusivity "$1"
            ARGS[clean]=true
            ;;
        -i | --install)
            validate_single_choice_exclusivity "$1"
            ARGS[install]=true
            ;;
        -u | --uninstall)
            validate_single_choice_exclusivity "$1"
            ARGS[uninstall]=true
            ;;
        -c | --container)
            validate_single_choice_exclusivity "$1"
            ARGS[container]=true
            ;;
        -a | --archive)
            validate_single_choice_exclusivity "$1"
            ARGS[archive]=true
            ;;
        -t | --test)
            if {
                [[ ${ARGS[release]} != false ]] && [[ $(count_enabled_multiple_choice_parameters) -gt 1 ]]
            } || {
                [[ ${ARGS[release]} = false ]] && [[ $(count_enabled_multiple_choice_parameters) -gt 0 ]]
            } || [[ $(count_enabled_single_choice_parameters) -gt 0 ]]; then
                die "Mutually exclusive option: $1 is not allowed."
            fi
            ARGS[test]=true
            ;;
        -r | --release)
            if {
                [[ ${ARGS[test]} != false ]] && [[ $(count_enabled_single_choice_parameters) -gt 1 ]]
            } || {
                [[ ${ARGS[test]} = false ]] && [[ $(count_enabled_single_choice_parameters) -gt 0 ]]
            }; then
                die "Mutually exclusive option: $1 is not allowed."
            fi
            ARGS[release]=true
            ;;
        -H | --hook)
            validate_multiple_choice_exclusivity "$1"
            ARGS[hook]=true
            ;;
        -s | --spell)
            validate_multiple_choice_exclusivity "$1"
            ARGS[spell]=true
            ;;
        -S | --statistics)
            validate_multiple_choice_exclusivity "$1"
            ARGS[statistics]=true
            ;;
        -f | --format)
            validate_multiple_choice_exclusivity "$1"
            handle_type_related_parameter "format" "$@"
            shift $?
            ;;
        -l | --lint)
            validate_multiple_choice_exclusivity "$1"
            handle_type_related_parameter "lint" "$@"
            shift $?
            ;;
        -q | --query)
            validate_multiple_choice_exclusivity "$1"
            ARGS[query]=true
            ;;
        -d | --doxygen)
            validate_multiple_choice_exclusivity "$1"
            ARGS[doxygen]=true
            ;;
        -b | --browser)
            validate_multiple_choice_exclusivity "$1"
            ARGS[browser]=true
            ;;
        *)
            die "Unknown command line option: $1. Try using the --help option for information."
            ;;
        esac
        shift
    done
}

function perform_help_option()
{
    if [[ ${ARGS[help]} = false ]]; then
        return
    fi

    echo "usage: $(basename "${0}") [-h] [-A {y,n}] [-Q] [-D] [-I] [-C] [-i] [-u] [-q] [-c] [-a] [-t [-r]] \
[[{-H,-c,-f [cpp,sh,py,rs],-l [cpp,sh,py,rs],-S,-b,-d} ...] [-r]]"
    echo
    echo "build script"
    echo
    echo "options:"
    echo "  -h, --help            show this help message and exit"
    echo "  -A {y,n}, --assume {y,n}"
    echo "                        assume confirmation is a yes or no"
    echo "  -Q, --quick           quick process if support filter by type"
    echo "  -D, --dry             dry run for script to preview"
    echo "  -I, --initialize      initialize environment and exit"
    echo "  -C, --clean           clean up project folder and exit"
    echo "  -i, --install         install binary with libraries and exit"
    echo "  -u, --uninstall       uninstall binary with libraries and exit"
    echo "  -c, --container       construct docker container and exit"
    echo "  -a, --archive         start or stop archive server and exit"
    echo "  -t, --test            only build unit test and exit"
    echo "  -r, --release         set as release version globally"
    echo "  -H, --hook            run hook before commit for precheck"
    echo "  -s, --spell           spell check against dictionaries"
    echo "  -S, --statistics      lines of code classification statistics"
    echo "  -f [cpp,sh,py,rs], --format [cpp,sh,py,rs]"
    echo "                        format all code files or by type"
    echo "  -l [cpp,sh,py,rs], --lint [cpp,sh,py,rs]"
    echo "                        lint all code files or by type"
    echo "  -q, --query           scan and query source code only"
    echo "  -d, --doxygen         project documentation with doxygen"
    echo "  -b, --browser         generate web-based code browser like IDE"

    exit "${STATUS}"
}

function perform_initialize_option()
{
    if [[ ${ARGS[initialize]} = false ]]; then
        return
    fi

    local export_cmd="export FOO_ENV=${FOLDER[proj]}_dev"
    if {
        [[ -z ${FOO_ENV} ]] || [[ ${FOO_ENV} != "${FOLDER[proj]}_dev" ]]
    } && ! grep -Fxq "${export_cmd}" ~/"${BASH_RC}" 2>/dev/null; then
        shell_command "echo '${export_cmd}' >>~/${BASH_RC}"
    fi

    local validate_proj="[[ \\\"\\\$(basename \\\"\\\$(pwd)\\\")\\\" == \\\"${FOLDER[proj]}\\\" ]] \
&& git rev-parse --git-dir >/dev/null 2>&1 && source ~/${BASH_RC}"
    local alias_cmd
    if ! grep -Fwq "alias ${FOLDER[proj]:0:1}build" ~/"${BASH_RC}" 2>/dev/null; then
        alias_cmd="alias ${FOLDER[proj]:0:1}build='${validate_proj} && ./${FOLDER[scr]}/build.sh'"
        shell_command "echo \"${alias_cmd}\" >>~/${BASH_RC}"
    fi
    if ! grep -Fwq "alias ${FOLDER[proj]:0:1}run" ~/"${BASH_RC}" 2>/dev/null; then
        alias_cmd="alias ${FOLDER[proj]:0:1}run='${validate_proj} && ./${FOLDER[scr]}/run.py'"
        shell_command "echo \"${alias_cmd}\" >>~/${BASH_RC}"
    fi

    shell_command "cat <<EOF >./${FOLDER[scr]}/.build_env
#!/bin/false

FOO_BLD_COMPILER=gcc # gcc / clang
FOO_BLD_PARALLEL=0 # NUMBER
FOO_BLD_PCH=off # on / off
FOO_BLD_UNITY=off # on / off
FOO_BLD_CCACHE=on # on / off
FOO_BLD_DISTCC=localhost # HOST
FOO_BLD_TMPFS=off # on / off

export FOO_BLD_COMPILER FOO_BLD_PARALLEL FOO_BLD_PCH FOO_BLD_UNITY FOO_BLD_CCACHE FOO_BLD_DISTCC FOO_BLD_TMPFS

return 0
EOF"
    local gdb_config_folder=".config/gdb"
    if [[ ! -d ~/${gdb_config_folder} ]]; then
        shell_command "mkdir -p ~/${gdb_config_folder}"
    fi
    local gdb_load_cmd="set auto-load safe-path /"
    if ! grep -Fxq "${gdb_load_cmd}" ~/"${gdb_config_folder}/gdbinit" 2>/dev/null; then
        shell_command "echo '${gdb_load_cmd}' >>~/${gdb_config_folder}/gdbinit"
    fi
    shell_command "echo 'core.%s.%e.%p' | ${SUDO_PREFIX}tee /proc/sys/kernel/core_pattern"
    shell_command "git config --local commit.template ./.gitcommit.template"

    echo "To initialize for effect, type \"exec bash\" manually."

    exit "${STATUS}"
}

function perform_clean_option()
{
    if [[ ${ARGS[clean]} = false ]]; then
        return
    fi

    if df -h -t tmpfs | grep -q "${FOLDER[proj]}/${FOLDER[bld]}" 2>/dev/null; then
        shell_command "${SUDO_PREFIX}umount ./${FOLDER[bld]}"
    fi
    if df -h -t tmpfs | grep -q "${FOLDER[proj]}/${FOLDER[tst]}/${FOLDER[bld]}" 2>/dev/null; then
        shell_command "${SUDO_PREFIX}umount ./${FOLDER[tst]}/${FOLDER[bld]}"
    fi

    if [[ -f ~/${BASH_RC} ]]; then
        shell_command "sed -i '/export FOO_ENV=${FOLDER[proj]}_dev/d' ~/${BASH_RC}"
        shell_command "sed -i '/alias ${FOLDER[proj]:0:1}\(build\|run\)/d' ~/${BASH_RC}"
    fi
    shell_command "find ./ -maxdepth 3 -type d | sed 1d \
| grep -E '(${FOLDER[bld]}|${FOLDER[rep]}|${FOLDER[cac]}|__pycache__|artifact|browser|doxygen|target)$' \
| xargs -i rm -rf {}"
    shell_command "rm -rf ./${FOLDER[scr]}/.* ./${FOLDER[doc]}/*.{css,html} ./${FOLDER[doc]}/server/Cargo.lock \
./core.* ./vgcore.* ./*.profraw"
    shell_command "git config --local --unset commit.template || true"

    if [[ -f ./.git/hooks/pre-commit ]]; then
        shell_command "pre-commit uninstall"
    fi

    echo "To clean up for effect, type \"exec bash\" manually."

    exit "${STATUS}"
}

function perform_install_option()
{
    if [[ ${ARGS[install]} = false ]]; then
        return
    fi

    if [[ ! -f ./${FOLDER[bld]}/bin/${FOLDER[proj]} ]]; then
        die "There is no binary file in the ${FOLDER[bld]} folder. Please finish compiling first."
    fi

    shell_command "${SUDO_PREFIX}cmake --install ./${FOLDER[bld]}"
    local install_path=/opt/${FOLDER[proj]}
    if [[ -d ${install_path} ]]; then
        local bin_path=${install_path}/bin
        local export_cmd="export PATH=${bin_path}:\$PATH"
        if [[ :${PATH}: != *:${bin_path}:* ]] && ! grep -Fxq "${export_cmd}" ~/"${BASH_RC}" 2>/dev/null; then
            shell_command "echo '${export_cmd}' >>~/${BASH_RC}"
        fi

        local completion_file="bash_completion"
        export_cmd="[ \"\${BASH_COMPLETION_VERSINFO}\" != \"\" ] && [ -s ${install_path}/${completion_file} ] \
&& \. ${install_path}/${completion_file}"
        shell_command "${SUDO_PREFIX}cp ./${FOLDER[scr]}/${completion_file}.sh ${install_path}/${completion_file}"
        if ! grep -Fxq "${export_cmd}" ~/"${BASH_RC}" 2>/dev/null; then
            shell_command "echo '${export_cmd}' >>~/${BASH_RC}"
        fi

        local man_path=${install_path}/man
        export_cmd="MANDATORY_MANPATH ${man_path}"
        shell_command "${SUDO_PREFIX}mkdir -p ${man_path}/man1 \
&& ${SUDO_PREFIX}cp ./${FOLDER[doc]}/man.1 ${man_path}/man1/${FOLDER[proj]}.1"
        if ! grep -Fxq "${export_cmd}" ~/.manpath 2>/dev/null; then
            shell_command "echo '${export_cmd}' >>~/.manpath"
        fi
    fi

    echo "Manually type \"exec bash\" to install for effect."

    exit "${STATUS}"
}

function perform_uninstall_option()
{
    if [[ ${ARGS[uninstall]} = false ]]; then
        return
    fi

    local manifest_file="install_manifest.txt"
    if [[ ! -f ./${FOLDER[bld]}/${manifest_file} ]]; then
        die "There is no ${manifest_file} file in the ${FOLDER[bld]} folder. Please generate it."
    fi

    local completion_file="bash_completion"
    shell_command "rm -rf ~/.${FOLDER[proj]}"
    shell_command "cat ./${FOLDER[bld]}/${manifest_file} | xargs ${SUDO_PREFIX}rm -rf \
&& ${SUDO_PREFIX}rm -rf /opt/${FOLDER[proj]}/${completion_file} /opt/${FOLDER[proj]}/man"
    shell_command "cat ./${FOLDER[bld]}/${manifest_file} | xargs -L1 dirname \
| xargs ${SUDO_PREFIX}rmdir -p 2>/dev/null || true"
    if [[ -f ~/${BASH_RC} ]]; then
        shell_command "sed -i '/export PATH=\/opt\/${FOLDER[proj]}\/bin:\$PATH/d' ~/${BASH_RC}"
        shell_command "sed -i '/\\\. \/opt\/${FOLDER[proj]}\/${completion_file}/d' ~/${BASH_RC}"
    fi
    if [[ -f ~/.manpath ]]; then
        shell_command "sed -i '/MANDATORY_MANPATH \/opt\/${FOLDER[proj]}\/man/d' ~/.manpath"
    fi

    echo "Manually type \"exec bash\" to uninstall for effect."

    exit "${STATUS}"
}

function perform_container_option()
{
    if [[ ${ARGS[container]} = false ]]; then
        return
    fi

    if ! command -v docker >/dev/null 2>&1; then
        die "No docker program. Please install it."
    fi

    echo "Please confirm whether continue constructing the docker container. (y or n)"
    local input
    input=$(wait_until_get_input)
    if echo "${input}" | grep -iq '^y'; then
        echo "Yes"
    else
        echo "No"

        exit "${STATUS}"
    fi

    if ! docker ps -a --format "{{lower .Image}} {{lower .Names}}" \
        | grep -q "ryftchen/${FOLDER[proj]}:latest ${FOLDER[proj]}_dev" 2>/dev/null; then
        shell_command "docker compose -f ./${FOLDER[dock]}/docker-compose.yml up -d --force-recreate"
    else
        die "The container exists."
    fi

    exit "${STATUS}"
}

function perform_archive_option()
{
    if [[ ${ARGS[archive]} = false ]]; then
        return
    fi

    if ! command -v rustc >/dev/null 2>&1 || ! command -v cargo >/dev/null 2>&1; then
        die "No rustc or cargo program. Please install it."
    fi

    shell_command "cargo build --release --manifest-path ./${FOLDER[doc]}/server/Cargo.toml"
    local html_file="index.html" css_file="main.css"
    if [[ ! -f ./${FOLDER[doc]}/${html_file} ]] || [[ ! -f ./${FOLDER[doc]}/${css_file} ]]; then
        shell_command "cp ./${FOLDER[doc]}/template/archive_${html_file} ./${FOLDER[doc]}/${html_file} \
&& cp ./${FOLDER[doc]}/template/archive_${css_file} ./${FOLDER[doc]}/${css_file}"
        shell_command "sed -i 's/unamed list/document list/g' ./${FOLDER[doc]}/index.html"
        local item_rows
        item_rows=$(
            cat <<EOF
<li><a href="./doxygen/index.html" target="_blank" rel="noopener">doxygen document</a></li>
<li><a href="./browser/index.html" target="_blank" rel="noopener">codebrowser document</a></li>
EOF
        )
        local item_row_single_line
        item_row_single_line=$(echo "${item_rows}" | tr -d '\n')
        shell_command "sed -i 's|<li>unamed item</li>|${item_row_single_line}|g' ./${FOLDER[doc]}/index.html"
    fi
    if [[ ! -f ./${FOLDER[rep]}/${html_file} ]] || [[ ! -f ./${FOLDER[rep]}/${css_file} ]]; then
        if [[ ! -d ./${FOLDER[rep]} ]]; then
            shell_command "mkdir ./${FOLDER[rep]}"
        fi
        shell_command "cp ./${FOLDER[doc]}/template/archive_${html_file} ./${FOLDER[rep]}/${html_file} \
&& cp ./${FOLDER[doc]}/template/archive_${css_file} ./${FOLDER[rep]}/${css_file}"
        shell_command "sed -i 's/unamed list/report list/g' ./${FOLDER[rep]}/index.html"
        local item_rows
        item_rows=$(
            cat <<EOF
<li>dca</li>
<ul>
<li><a href="./dca/chk_cov/index.html" target="_blank" rel="noopener">llvm-cov report</a></li>
<li><a href="./dca/chk_mem/index.html" target="_blank" rel="noopener">valgrind report</a></li>
</ul>
<li>sca</li>
<ul>
<li><a href="./sca/lint/index.html" target="_blank" rel="noopener">clang-tidy report</a></li>
<li><a href="./sca/query/index.html" target="_blank" rel="noopener">codeql report</a></li>
</ul>
EOF
        )
        local item_row_single_line
        item_row_single_line=$(echo "${item_rows}" | tr -d '\n')
        shell_command "sed -i 's|<li>unamed item</li>|${item_row_single_line}|g' ./${FOLDER[rep]}/index.html"
    fi

    local host_addr="127.0.0.1" start_port=61503
    local server_daemon="./${FOLDER[doc]}/server/target/release/${FOLDER[proj]}_arc --root-dir ./${FOLDER[doc]} \
--root-dir ./${FOLDER[rep]} --host ${host_addr} --port ${start_port}"
    if ! pgrep -f "${server_daemon}" >/dev/null 2>&1; then
        echo "Please confirm whether continue starting the archive server. (y or n)"
        local input
        input=$(wait_until_get_input)
        if echo "${input}" | grep -iq '^y'; then
            echo "Yes"
            shell_command "${server_daemon} & sleep 0.5s"
        else
            echo "No"
        fi
    else
        echo "Please confirm whether continue stopping the archive server. (y or n)"
        local input
        input=$(wait_until_get_input)
        if echo "${input}" | grep -iq '^y'; then
            echo "Yes"
            local port1=${start_port}
            if netstat -tuln | grep ":${port1} " >/dev/null 2>&1; then
                shell_command "fuser -k ${port1}/tcp || true"
            fi
            local port2=$((port1 + 1))
            if netstat -tuln | grep ":${port2} " >/dev/null 2>&1; then
                shell_command "fuser -k ${port2}/tcp || true"
            fi
        else
            echo "No"
        fi
    fi

    exit "${STATUS}"
}

function try_perform_single_choice_options()
{
    perform_help_option
    perform_initialize_option
    perform_clean_option
    perform_install_option
    perform_uninstall_option
    perform_container_option
    perform_archive_option
}

function check_potential_dependencies()
{
    if [[ ${ARGS[hook]} != false ]]; then
        if ! command -v pre-commit >/dev/null 2>&1; then
            die "No pre-commit program. Please install it."
        fi
    fi

    if [[ ${ARGS[spell]} != false ]]; then
        if ! command -v cspell >/dev/null 2>&1; then
            die "No cspell program. Please install it."
        fi
    fi

    if [[ ${ARGS[statistics]} != false ]]; then
        if ! command -v cloc >/dev/null 2>&1; then
            die "No cloc program. Please install it."
        fi
    fi

    if [[ ${ARGS[format]} != false ]]; then
        if ! command -v clang-format-19 >/dev/null 2>&1 || ! command -v shfmt >/dev/null 2>&1 \
            || ! command -v black >/dev/null 2>&1 || ! command -v rustfmt >/dev/null 2>&1; then
            die "No clang-format, shfmt, black or rustfmt program. Please install it."
        fi
    fi

    if [[ ${ARGS[lint]} != false ]]; then
        if ! command -v clang-tidy-19 >/dev/null 2>&1 || ! command -v run-clang-tidy-19 >/dev/null 2>&1 \
            || ! command -v compdb >/dev/null 2>&1 || ! pip3 show clang-tidy-converter >/dev/null 2>&1 \
            || ! command -v shellcheck >/dev/null 2>&1 || ! command -v pylint >/dev/null 2>&1 \
            || ! command -v clippy-driver >/dev/null 2>&1; then
            die "No clang-tidy (including run-clang-tidy-19, compdb, clang-tidy-converter), shellcheck, pylint \
or clippy program. Please install it."
        fi
        if [[ ${DEV_OPT[pch]} = true ]] || [[ ${DEV_OPT[unity]} = true ]]; then
            die "Due to the unconventional ${COMPILE_DB} file, the --lint option cannot run if the FOO_BLD_PCH or \
FOO_BLD_UNITY is turned on."
        fi
    fi

    if [[ ${ARGS[query]} = false ]]; then
        if ! command -v codeql >/dev/null 2>&1 || ! command -v sarif >/dev/null 2>&1; then
            die "No codeql (including sarif) program. Please install it."
        fi
    fi

    if [[ ${ARGS[doxygen]} != false ]]; then
        if ! command -v doxygen >/dev/null 2>&1 || ! command -v dot >/dev/null 2>&1; then
            die "No doxygen (including dot) program. Please install it."
        fi
    fi

    if [[ ${ARGS[browser]} != false ]]; then
        if ! command -v codebrowser_generator >/dev/null 2>&1 \
            || ! command -v codebrowser_indexgenerator >/dev/null 2>&1; then
            die "No codebrowser_generator or codebrowser_indexgenerator program. Please install it."
        fi
        if [[ ${DEV_OPT[pch]} = true ]] || [[ ${DEV_OPT[unity]} = true ]]; then
            die "Due to the unconventional ${COMPILE_DB} file, the --browser option cannot run if the FOO_BLD_PCH or \
FOO_BLD_UNITY is turned on."
        fi
    fi
}

function perform_hook_option()
{
    if [[ ${ARGS[hook]} = false ]]; then
        return
    fi

    shell_command "pre-commit install --config ./.pre-commit"
    shell_command "pre-commit run --all-files --config ./.pre-commit"
}

function perform_spell_option()
{
    if [[ ${ARGS[spell]} = false ]]; then
        return
    fi

    shell_command "cspell lint --config ./.cspell --show-context --no-cache"
}

function perform_statistics_option()
{
    if [[ ${ARGS[statistics]} = false ]]; then
        return
    fi

    shell_command "printf \"C,C++,C/C++ Header\nBourne Shell\nPython\nRust\n\" \
| xargs -I {} -n 1 -P 1 cloc --config ./.cloc --include-lang='{}'"
}

function perform_format_option()
{
    if [[ ${ARGS[format]} = false ]]; then
        return
    fi

    if [[ ${ARGS[format]} = true ]] || [[ ${ARGS[format]} = "cpp" ]]; then
        if [[ ${ARGS[quick]} = false ]]; then
            shell_command "find ./${FOLDER[app]} ./${FOLDER[util]} ./${FOLDER[algo]} ./${FOLDER[ds]} ./${FOLDER[dp]} \
./${FOLDER[num]} ./${FOLDER[tst]} -name '*.cpp' -o -name '*.hpp' -o -name '*.tpp' | grep -v '/${FOLDER[bld]}/' \
| xargs clang-format-19 --Werror -i --style=file:./.clang-format --verbose"
        else
            local format_changed_cpp="${GIT_CHANGE_CMD} | grep -E '\.(cpp|hpp|tpp)$'"
            if eval "${format_changed_cpp}" >/dev/null; then
                shell_command "${format_changed_cpp} \
| xargs clang-format-19 --Werror -i --style=file:./.clang-format --verbose"
            fi
        fi
    fi

    if [[ ${ARGS[format]} = true ]] || [[ ${ARGS[format]} = "sh" ]]; then
        if [[ ${ARGS[quick]} = false ]]; then
            shell_command "shfmt -l -w ./${FOLDER[scr]}/*.sh"
        else
            local format_changed_sh="${GIT_CHANGE_CMD} | grep -E '\.sh$'"
            if eval "${format_changed_sh}" >/dev/null; then
                shell_command "${format_changed_sh} | xargs shfmt -l -w"
            fi
        fi
    fi

    if [[ ${ARGS[format]} = true ]] || [[ ${ARGS[format]} = "py" ]]; then
        if [[ ${ARGS[quick]} = false ]]; then
            shell_command "black --config ./.toml ./${FOLDER[scr]}/*.py"
        else
            local format_changed_py="${GIT_CHANGE_CMD} | grep -E '\.py$'"
            if eval "${format_changed_py}" >/dev/null; then
                shell_command "${format_changed_py} | xargs black --config ./.toml"
            fi
        fi
    fi

    if [[ ${ARGS[format]} = true ]] || [[ ${ARGS[format]} = "rs" ]]; then
        local cargo_toml="Cargo.toml"
        local crate_path="./${FOLDER[doc]}/server/${cargo_toml}"
        if [[ ${ARGS[quick]} = false ]]; then
            shell_command "cargo fmt --all --verbose --manifest-path ${crate_path}"
        else
            local format_changed_rs="${GIT_CHANGE_CMD} | grep -E '\.rs$'"
            if eval "${format_changed_rs}" >/dev/null; then
                local split_crate="${format_changed_rs} | xargs -I {} dirname {} | while read path; \
do while [[ ! -f \${path}/${cargo_toml} ]] && [[ \${path} != \"/\" ]]; do path=\$(dirname \"\${path}\"); done; \
[[ -f \${path}/${cargo_toml} ]] && [[ \$(realpath \${path}/${cargo_toml}) == \$(realpath ${crate_path}) ]] \
&& echo \"\${path}\"; done | sort -u"
                if eval "${split_crate}" >/dev/null; then
                    shell_command "cd \$(dirname ${crate_path}) && cargo fmt --verbose"
                fi
            fi
        fi
    fi
}

function perform_lint_option()
{
    if [[ ${ARGS[lint]} = false ]]; then
        return
    fi

    if [[ ${ARGS[lint]} = true ]] || [[ ${ARGS[lint]} = "cpp" ]]; then
        local app_comp_db=${FOLDER[bld]}/${COMPILE_DB}
        if [[ ! -f ./${app_comp_db} ]]; then
            die "There is no ${COMPILE_DB} file in the ${FOLDER[bld]} folder. Please generate it."
        fi
        shell_command "compdb -p ./${FOLDER[bld]} list >./${COMPILE_DB} && mv ./${app_comp_db} ./${app_comp_db}.bak \
&& mv ./${COMPILE_DB} ./${FOLDER[bld]}"
        local exist_file_extention=false
        while true; do
            local line
            line=$(grep -n '.tpp' "./${app_comp_db}" | head -n 1 | cut -d : -f 1)
            if ! [[ ${line} =~ ^[0-9]+$ ]]; then
                break
            fi
            exist_file_extention=true
            if ! sed -i $((line - 2)),$((line + 3))d ./"${app_comp_db}" >/dev/null 2>&1; then
                die "Failed to remove redundant implementation file objects from the ${app_comp_db} file."
            fi
        done

        local clang_tidy_output_path=./${FOLDER[rep]}/sca/lint
        local clang_tidy_log=${clang_tidy_output_path}/clang-tidy.log
        if [[ ! -d ${clang_tidy_output_path} ]]; then
            shell_command "mkdir -p ${clang_tidy_output_path}"
        elif [[ -f ${clang_tidy_log} ]]; then
            shell_command "rm -rf ${clang_tidy_log}"
        fi
        if [[ ${ARGS[quick]} = false ]]; then
            shell_command "set -o pipefail && find ./${FOLDER[app]} ./${FOLDER[util]} ./${FOLDER[algo]} \
./${FOLDER[ds]} ./${FOLDER[dp]} ./${FOLDER[num]} -name '*.cpp' -o -name '*.hpp' \
| xargs run-clang-tidy-19 -config-file=./.clang-tidy -p ./${FOLDER[bld]} -quiet | tee -a ${clang_tidy_log}"
        else
            local lint_changed_cpp_for_app="${GIT_CHANGE_CMD} \
| grep -E '^(${FOLDER[app]}|${FOLDER[util]}|${FOLDER[algo]}|${FOLDER[ds]}|${FOLDER[dp]}|${FOLDER[num]})/.*\.(cpp|hpp)$'"
            if eval "${lint_changed_cpp_for_app}" >/dev/null; then
                shell_command "set -o pipefail && ${lint_changed_cpp_for_app} \
| xargs run-clang-tidy-19 -config-file=./.clang-tidy -p ./${FOLDER[bld]} -quiet | tee -a ${clang_tidy_log}"
            fi
        fi
        if [[ ${exist_file_extention} = true ]]; then
            if [[ ${ARGS[quick]} = false ]]; then
                shell_command "set -o pipefail && find ./${FOLDER[app]} ./${FOLDER[util]} ./${FOLDER[algo]} \
./${FOLDER[ds]} ./${FOLDER[dp]} ./${FOLDER[num]} -name '*.tpp' \
| xargs clang-tidy-19 --config-file=./.clang-tidy -p ./${FOLDER[bld]} --quiet | tee -a ${clang_tidy_log}"
            else
                local lint_changed_cpp_for_app_ext="${GIT_CHANGE_CMD} \
| grep -E '^(${FOLDER[app]}|${FOLDER[util]}|${FOLDER[algo]}|${FOLDER[ds]}|${FOLDER[dp]}|${FOLDER[num]})/.*\.tpp$'"
                if eval "${lint_changed_cpp_for_app_ext}" >/dev/null; then
                    shell_command "set -o pipefail && ${lint_changed_cpp_for_app_ext} \
| xargs clang-tidy-19 --config-file=./.clang-tidy -p ./${FOLDER[bld]} --quiet | tee -a ${clang_tidy_log}"
                fi
            fi
        fi
        shell_command "rm -rf ./${app_comp_db} && mv ./${app_comp_db}.bak ./${app_comp_db}"

        local tst_comp_db=${FOLDER[tst]}/${FOLDER[bld]}/${COMPILE_DB}
        if [[ ! -f ./${tst_comp_db} ]]; then
            die "There is no ${COMPILE_DB} file in the ${FOLDER[tst]}/${FOLDER[bld]} folder. Please generate it."
        fi
        shell_command "compdb -p ./${FOLDER[tst]}/${FOLDER[bld]} list >./${COMPILE_DB} \
&& mv ./${tst_comp_db} ./${tst_comp_db}.bak && mv ./${COMPILE_DB} ./${FOLDER[tst]}/${FOLDER[bld]}"
        if [[ ${ARGS[quick]} = false ]]; then
            shell_command "set -o pipefail && find ./${FOLDER[tst]} -name '*.cpp' \
| xargs run-clang-tidy-19 -config-file=./.clang-tidy -p ./${FOLDER[tst]}/${FOLDER[bld]} -quiet \
| tee -a ${clang_tidy_log}"
        else
            local lint_changed_cpp_for_tst="${GIT_CHANGE_CMD} | grep -E '^${FOLDER[tst]}/.*\.cpp$'"
            if eval "${lint_changed_cpp_for_tst}" >/dev/null; then
                shell_command "set -o pipefail && ${lint_changed_cpp_for_tst} \
| xargs run-clang-tidy-19 -config-file=./.clang-tidy -p ./${FOLDER[tst]}/${FOLDER[bld]} -quiet \
| tee -a ${clang_tidy_log}"
            fi
        fi
        shell_command "rm -rf ./${tst_comp_db} && mv ./${tst_comp_db}.bak ./${tst_comp_db}"
        shell_command "(test -f ${clang_tidy_log} && cat ${clang_tidy_log}) | sed 's/\x1b\[[0-9;]*m//g' \
| python3 -m clang_tidy_converter --project_root ./ html >${clang_tidy_output_path}/index.html"
    fi

    if [[ ${ARGS[lint]} = true ]] || [[ ${ARGS[lint]} = "sh" ]]; then
        if [[ ${ARGS[quick]} = false ]]; then
            shell_command "shellcheck -a ./${FOLDER[scr]}/*.sh"
        else
            local lint_changed_sh="${GIT_CHANGE_CMD} | grep -E '\.sh$'"
            if eval "${lint_changed_sh}" >/dev/null; then
                shell_command "${lint_changed_sh} | xargs shellcheck -a"
            fi
        fi
    fi

    if [[ ${ARGS[lint]} = true ]] || [[ ${ARGS[lint]} = "py" ]]; then
        if [[ ${ARGS[quick]} = false ]]; then
            shell_command "pylint --rcfile=./.pylintrc ./${FOLDER[scr]}/*.py"
        else
            local lint_changed_py="${GIT_CHANGE_CMD} | grep -E '\.py$'"
            if eval "${lint_changed_py}" >/dev/null; then
                shell_command "${lint_changed_py} | xargs pylint --rcfile=./.pylintrc"
            fi
        fi
    fi

    if [[ ${ARGS[lint]} = true ]] || [[ ${ARGS[lint]} = "rs" ]]; then
        local build_type
        if [[ ${BUILD_TYPE} = "Release" ]]; then
            build_type=" --release"
        fi
        local cargo_toml="Cargo.toml"
        local crate_path="./${FOLDER[doc]}/server/${cargo_toml}"
        if [[ ${ARGS[quick]} = false ]]; then
            shell_command "cargo clippy --manifest-path ${crate_path}""${build_type}"
        else
            local lint_changed_rs="${GIT_CHANGE_CMD} | grep -E '\.rs$'"
            if eval "${lint_changed_rs}" >/dev/null; then
                local split_crate="${lint_changed_rs} | xargs -I {} dirname {} | while read path; \
do while [[ ! -f \${path}/${cargo_toml} ]] && [[ \${path} != \"/\" ]]; do path=\$(dirname \"\${path}\"); done; \
[[ -f \${path}/${cargo_toml} ]] && [[ \$(realpath \${path}/${cargo_toml}) == \$(realpath ${crate_path}) ]] \
&& echo \"\${path}\"; done | sort -u"
                if eval "${split_crate}" >/dev/null; then
                    shell_command "cd \$(dirname ${crate_path}) && cargo clippy""${build_type}"
                fi
            fi
        fi
    fi
}

function perform_query_option()
{
    if [[ ${ARGS[query]} = false ]]; then
        return
    fi

    local build_script other_option
    build_script=./${FOLDER[scr]}/$(basename "$0")
    if [[ ${ARGS[release]} != false ]]; then
        other_option=" --release"
    fi
    local rebuild_script=./${FOLDER[scr]}/.build_afresh
    if [[ ! -f ${rebuild_script} ]]; then
        shell_command "cat <<EOF >./${rebuild_script}
#!/usr/bin/env bash

set -e

export FOO_BLD_FORCE=on
$(realpath "$0") \"\\\$@\"
EOF"
        shell_command "chmod +x ${rebuild_script}"
    fi

    echo "Please confirm whether need to temporarily force a full recompile (using the default .build_env) \
to improve accuracy. (y or n)"
    local input
    input=$(wait_until_get_input)
    if echo "${input}" | grep -iq '^y'; then
        echo "Yes"
        build_script=${rebuild_script}
        shell_command "rm -rf ./${FOLDER[bld]} ./${FOLDER[tst]}/${FOLDER[bld]}"
    else
        echo "No"
    fi

    local codeql_db_path=./${FOLDER[rep]}/sca/query
    shell_command "rm -rf ${codeql_db_path} && mkdir -p ${codeql_db_path}"
    shell_command "codeql database create ${codeql_db_path} --codescanning-config=./.codeql --language=cpp \
--source-root=./ --command='${build_script}${other_option}' --command='${build_script} --test${other_option}'"
    local codeql_sarif=${codeql_db_path}/codeql.sarif
    shell_command "codeql database analyze ${codeql_db_path} --format=sarif-latest --output=${codeql_sarif}"
    if echo "${input}" | grep -iq '^y'; then
        build_script=./${FOLDER[scr]}/$(basename "$0")
        shell_command "${build_script}${other_option} >/dev/null && ${build_script} --test${other_option} >/dev/null"
    fi

    if [[ -f ${codeql_sarif} ]]; then
        local sarif_sum="sarif summary ${codeql_sarif}" sarif_sum_output
        sarif_sum_output=$(eval "${sarif_sum}")
        if {
            echo "${sarif_sum_output}" | grep -q 'error: 0'
        } && {
            echo "${sarif_sum_output}" | grep -q 'warning: 0'
        } && {
            echo "${sarif_sum_output}" | grep -q 'note: 0'
        }; then
            shell_command "${sarif_sum}"
        else
            shell_command "! ${sarif_sum}"
        fi
        shell_command "sarif html ${codeql_sarif} --output ${codeql_db_path}/index.html"
    else
        die "Could not find sarif file in codeql database."
    fi
}

function package_for_doxygen()
{
    local commit_id=$1

    local doxygen_folder="doxygen"
    local tar_file="${FOLDER[proj]}_${doxygen_folder}_${commit_id}.tar.bz2"
    shell_command "rm -rf ./${FOLDER[doc]}/artifact/${FOLDER[proj]}_${doxygen_folder}_*.tar.bz2 \
./${FOLDER[doc]}/${doxygen_folder} && mkdir ./${FOLDER[doc]}/${doxygen_folder}"

    local check_format="grep -nE '\/\/! @((brief (([a-z].+)|(.+[^.])))|((param|tparam) (.+[.]))|(return (.+[.])))$' \
-R './${FOLDER[app]}' './${FOLDER[util]}' './${FOLDER[algo]}' './${FOLDER[ds]}' './${FOLDER[dp]}' './${FOLDER[num]}' \
'./${FOLDER[tst]}' --include '*.cpp' --include '*.hpp' --include '*.tpp'"
    if eval "${check_format}" >/dev/null; then
        shell_command "! ${check_format}"
    fi
    shell_command "(cat ./${FOLDER[doc]}/Doxyfile; echo 'PROJECT_NUMBER=\"@ $(git rev-parse --short @)\"') | doxygen -"
    shell_command "tar -jcvf ./${FOLDER[doc]}/artifact/${tar_file} -C ./${FOLDER[doc]} ${doxygen_folder} >/dev/null"
}

function perform_doxygen_option()
{
    if [[ ${ARGS[doxygen]} = false ]]; then
        return
    fi

    local commit_id
    commit_id=$(git rev-parse --short @)
    if [[ -z ${commit_id} ]]; then
        commit_id="local"
    fi
    if [[ -d ./${FOLDER[doc]}/artifact ]]; then
        local last_tar="${FOLDER[proj]}_doxygen_${commit_id}.tar.bz2"
        if [[ -f ./${FOLDER[doc]}/artifact/${last_tar} ]]; then
            local time_interval=$(($(date +%s) - $(stat -L --format %Y "./${FOLDER[doc]}/artifact/${last_tar}")))
            if [[ ${time_interval} -lt 60 ]]; then
                die "The latest doxygen artifact ${last_tar} has been generated since ${time_interval} s ago."
            fi
        fi
        package_for_doxygen "${commit_id}"
    else
        shell_command "mkdir ./${FOLDER[doc]}/artifact"
        package_for_doxygen "${commit_id}"
    fi
}

function package_for_browser()
{
    local commit_id=$1

    if [[ ! -f ./${FOLDER[bld]}/${COMPILE_DB} ]]; then
        die "There is no ${COMPILE_DB} file in the ${FOLDER[bld]} folder. Please generate it."
    fi
    local browser_folder="browser"
    local tar_file="${FOLDER[proj]}_${browser_folder}_${commit_id}.tar.bz2"
    shell_command "rm -rf ./${FOLDER[doc]}/artifact/${FOLDER[proj]}_${browser_folder}_*.tar.bz2 \
./${FOLDER[doc]}/${browser_folder} && mkdir ./${FOLDER[doc]}/${browser_folder}"

    shell_command "cp -rf /usr/local/share/woboq/data ./${FOLDER[doc]}/${browser_folder}/"
    shell_command "sed -i \"s|'><img src='|'><img src='https://web.archive.org/web/20220224111803/|g\" \
./${FOLDER[doc]}/${browser_folder}/data/codebrowser.js"
    shell_command "codebrowser_generator -color -a -b ./${FOLDER[bld]}/${COMPILE_DB} \
-o ./${FOLDER[doc]}/${browser_folder} -p ${FOLDER[proj]}:.:${commit_id} -d ./data"
    shell_command "codebrowser_generator -color -a -b ./${FOLDER[tst]}/${FOLDER[bld]}/${COMPILE_DB} \
-o ./${FOLDER[doc]}/${browser_folder} -p ${FOLDER[proj]}:.:${commit_id} -d ./data"
    shell_command "codebrowser_indexgenerator ./${FOLDER[doc]}/${browser_folder} -d ./data"

    shell_command "find \"./${FOLDER[doc]}/${browser_folder}/index.html\" \
\"./${FOLDER[doc]}/${browser_folder}/${FOLDER[proj]}\" \"./${FOLDER[doc]}/${browser_folder}/include\" -name \"*.html\" \
-exec sed -i '/^<\/head>$/i <link rel=\"shortcut icon\" \
href=\"https://web.archive.org/web/20220309195008if_/https://code.woboq.org/favicon.ico\" type=\"image/x-icon\"/>' {} +"
    shell_command "find \"./${FOLDER[doc]}/${browser_folder}/index.html\" \
\"./${FOLDER[doc]}/${browser_folder}/${FOLDER[proj]}\" \"./${FOLDER[doc]}/${browser_folder}/include\" -name \"*.html\" \
-exec sed -i 's|https://code.woboq.org/woboq-16.png\
|https://web.archive.org/web/20220224111804if_/https://code.woboq.org/woboq-16.png|g' {} +"
    shell_command "tar -jcvf ./${FOLDER[doc]}/artifact/${tar_file} -C ./${FOLDER[doc]} ${browser_folder} >/dev/null"
}

function perform_browser_option()
{
    if [[ ${ARGS[browser]} = false ]]; then
        return
    fi

    local commit_id
    commit_id=$(git rev-parse --short @)
    if [[ -z ${commit_id} ]]; then
        commit_id="local"
    fi
    if [[ -d ./${FOLDER[doc]}/artifact ]]; then
        local last_tar="${FOLDER[proj]}_browser_${commit_id}.tar.bz2"
        if [[ -f ./${FOLDER[doc]}/artifact/${last_tar} ]]; then
            local time_interval=$(($(date +%s) - $(stat -L --format %Y "./${FOLDER[doc]}/artifact/${last_tar}")))
            if [[ ${time_interval} -lt 60 ]]; then
                die "The latest browser artifact ${last_tar} has been generated since ${time_interval} s ago."
            fi
        fi
        package_for_browser "${commit_id}"
    else
        shell_command "mkdir ./${FOLDER[doc]}/artifact"
        package_for_browser "${commit_id}"
    fi
}

function try_perform_multiple_choice_options()
{
    check_potential_dependencies

    perform_hook_option
    perform_spell_option
    perform_statistics_option
    perform_format_option
    perform_lint_option
    perform_query_option
    perform_doxygen_option
    perform_browser_option
}

function set_compile_condition()
{
    local tmpfs_subfolder=$1 tmpfs_size=$2

    if [[ -f ./${FOLDER[scr]}/.build_env ]] && ! {
        [[ -n ${FOO_BLD_FORCE} ]] && [[ ${FOO_BLD_FORCE} = "on" ]]
    }; then
        # shellcheck source=/dev/null
        source "./${FOLDER[scr]}/.build_env"
        if [[ -n ${FOO_BLD_COMPILER} ]]; then
            if [[ ! ${FOO_BLD_COMPILER} =~ ^(gcc|clang)$ ]]; then
                die "The FOO_BLD_COMPILER must be gcc or clang."
            fi
            DEV_OPT[compiler]=${FOO_BLD_COMPILER}
        fi
        if [[ -n ${FOO_BLD_PARALLEL} ]]; then
            if [[ ! ${FOO_BLD_PARALLEL} =~ ^[0-9]+$ ]]; then
                die "The FOO_BLD_PARALLEL must be a positive integer."
            fi
            DEV_OPT[parallel]=${FOO_BLD_PARALLEL}
        fi
        if [[ -n ${FOO_BLD_PCH} ]]; then
            if [[ ! ${FOO_BLD_PCH} =~ ^(on|off)$ ]]; then
                die "The FOO_BLD_PCH must be on or off."
            fi
            DEV_OPT[pch]=$([[ ${FOO_BLD_PCH} = "on" ]] && echo true || echo false)
        fi
        if [[ -n ${FOO_BLD_UNITY} ]]; then
            if [[ ! ${FOO_BLD_UNITY} =~ ^(on|off)$ ]]; then
                die "The FOO_BLD_UNITY must be on or off."
            fi
            DEV_OPT[unity]=$([[ ${FOO_BLD_UNITY} = "on" ]] && echo true || echo false)
        fi
        if [[ -n ${FOO_BLD_CCACHE} ]]; then
            if [[ ! ${FOO_BLD_CCACHE} =~ ^(on|off)$ ]]; then
                die "The FOO_BLD_CCACHE must be on or off."
            fi
            DEV_OPT[ccache]=$([[ ${FOO_BLD_CCACHE} = "on" ]] && echo true || echo false)
        fi
        if [[ -n "${FOO_BLD_DISTCC}" ]]; then
            if [[ -z "${FOO_BLD_DISTCC// /}" ]]; then
                die "The FOO_BLD_DISTCC must be a non-empty string."
            fi
            DEV_OPT[distcc]=${FOO_BLD_DISTCC}
        fi
        if [[ -n ${FOO_BLD_TMPFS} ]]; then
            if [[ ! ${FOO_BLD_TMPFS} =~ ^(on|off)$ ]]; then
                die "The FOO_BLD_TMPFS must be on or off."
            fi
            DEV_OPT[tmpfs]=$([[ ${FOO_BLD_TMPFS} = "on" ]] && echo true || echo false)
        fi
    fi
    if {
        [[ -n ${FOO_BLD_COV} ]] && [[ ${FOO_BLD_COV} = "llvm-cov" ]]
    } || {
        [[ -n ${FOO_BLD_SAN} ]] && [[ ${FOO_BLD_SAN} =~ ^(asan|tsan|ubsan)$ ]]
    }; then
        DEV_OPT[compiler]="clang"
    fi

    CMAKE_CACHE_ENTRY=" -D CMAKE_BUILD_TYPE=${BUILD_TYPE}"
    if [[ ${DEV_OPT[compiler]} = "gcc" ]]; then
        local ver=14
        export CC=gcc-${ver} CXX=g++-${ver}
        if ! command -v "${CC}" >/dev/null 2>&1 || ! command -v "${CXX}" >/dev/null 2>&1; then
            die "No ${CC} or ${CXX} program. Please install it."
        fi
    elif [[ ${DEV_OPT[compiler]} = "clang" ]]; then
        local ver=19
        export CC=clang-${ver} CXX=clang++-${ver}
        if ! command -v "${CC}" >/dev/null 2>&1 || ! command -v "${CXX}" >/dev/null 2>&1; then
            die "No ${CC} or ${CXX} program. Please install it."
        fi
    fi
    CMAKE_CACHE_ENTRY="${CMAKE_CACHE_ENTRY} -D CMAKE_C_COMPILER=${CC} -D CMAKE_CXX_COMPILER=${CXX}"
    local gnu_lib_ver=14
    if command -v "gcc-${gnu_lib_ver}" >/dev/null 2>&1 && command -v "g++-${gnu_lib_ver}" >/dev/null 2>&1; then
        local gcc_processor gxx_processor
        gcc_processor=$("gcc-${gnu_lib_ver}" -dumpmachine)
        gxx_processor=$("g++-${gnu_lib_ver}" -dumpmachine)
        export C_INCLUDE_PATH=/usr/include:/usr/lib/gcc/${gcc_processor}/${gnu_lib_ver}/include \
            CPLUS_INCLUDE_PATH=/usr/include/c++/${gnu_lib_ver}:/usr/include/${gxx_processor}/c++/${gnu_lib_ver}
    fi

    if [[ ! ${DEV_OPT[parallel]} -eq 0 ]]; then
        CMAKE_BUILD_OPTION=" -j ${DEV_OPT[parallel]}"
    fi
    if [[ ${DEV_OPT[pch]} = true ]]; then
        CMAKE_CACHE_ENTRY="${CMAKE_CACHE_ENTRY} -D TOOLCHAIN_PCH=ON"
    fi
    if [[ ${DEV_OPT[unity]} = true ]]; then
        CMAKE_CACHE_ENTRY="${CMAKE_CACHE_ENTRY} -D TOOLCHAIN_UNITY=ON"
    fi
    if [[ ${DEV_OPT[ccache]} = true ]]; then
        if ! command -v ccache >/dev/null; then
            die "No ccache program. Please install it."
        fi
        CMAKE_CACHE_ENTRY="${CMAKE_CACHE_ENTRY} -D TOOLCHAIN_CCACHE=ON"
        if [[ ${ARGS[test]} = false ]]; then
            export CCACHE_DIR=${PWD}/${FOLDER[cac]}/ccache
        else
            export CCACHE_DIR=${PWD}/${FOLDER[tst]}/${FOLDER[cac]}/ccache
        fi
    fi
    if [[ ${DEV_OPT[distcc]} != "localhost" ]]; then
        if ! command -v distcc >/dev/null; then
            die "No distcc program. Please install it."
        fi
        if [[ ${DEV_OPT[pch]} = true ]] || [[ ${DEV_OPT[unity]} = true ]]; then
            die "Compilation relationships may have potential conflicts if the FOO_BLD_DISTCC is not localhost and the \
FOO_BLD_PCH or FOO_BLD_UNITY is turned on."
        fi
        if [[ -n ${FOO_BLD_COV} ]] && [[ ${FOO_BLD_COV} = "llvm-cov" ]]; then
            die "Code coverage may be affected if the FOO_BLD_DISTCC is not localhost."
        fi
        if [[ ${DEV_OPT[distcc]} = *"127.0.0.1"* ]]; then
            local local_client="127.0.0.1/32" escaped_local_client
            escaped_local_client=$(printf "%s\n" "${local_client}" | sed -e 's/[]\/$*.^[]/\\&/g')
            if ! pgrep -a distccd \
                | grep -e "-a ${escaped_local_client}\|--allow ${escaped_local_client}" >/dev/null 2>&1; then
                die "No local distcc server has been detected, please start it manually, \
e.g. with \"distccd --daemon --allow ${local_client}\"."
            fi
        fi
        CMAKE_CACHE_ENTRY="${CMAKE_CACHE_ENTRY} -D TOOLCHAIN_DISTCC=ON"
        export DISTCC_HOSTS="localhost ${DEV_OPT[distcc]}"
    fi
    if [[ ${DEV_OPT[ccache]} = true ]] && [[ ${DEV_OPT[pch]} = true ]]; then
        export CCACHE_PCH_EXTSUM=true CCACHE_SLOPPINESS=pch_defines,time_macros
    fi
    if [[ ${DEV_OPT[ccache]} = true ]] && [[ ${DEV_OPT[distcc]} != "localhost" ]]; then
        export CCACHE_PREFIX=distcc
    fi
    if [[ ${DEV_OPT[tmpfs]} = true ]]; then
        if [[ ! -d ./${tmpfs_subfolder} ]]; then
            shell_command "mkdir -p ./${tmpfs_subfolder}"
        fi
        if ! df -h -t tmpfs | grep -q "${FOLDER[proj]}/${tmpfs_subfolder}" 2>/dev/null; then
            shell_command "${SUDO_PREFIX}mount -t tmpfs -o size=${tmpfs_size} tmpfs ./${tmpfs_subfolder}"
        fi
    elif df -h -t tmpfs | grep -q "${FOLDER[proj]}/${tmpfs_subfolder}" 2>/dev/null; then
        shell_command "${SUDO_PREFIX}umount ./${tmpfs_subfolder}"
    fi
}

function build_native_if_needed()
{
    if ! command -v cmake >/dev/null 2>&1 || ! command -v ninja >/dev/null 2>&1; then
        die "No cmake or ninja program. Please install it."
    fi
    if [[ ${ARGS[release]} != false ]]; then
        BUILD_TYPE="Release"
    fi

    local cmake_cache="CMakeCache.txt"
    if [[ ${ARGS[test]} != false ]]; then
        set_compile_condition "${FOLDER[tst]}/${FOLDER[bld]}" "256m"
        if [[ -f ./${FOLDER[tst]}/${FOLDER[bld]}/${cmake_cache} ]] \
            && ! grep -Fwq "${DEV_OPT[compiler]}" "./${FOLDER[tst]}/${FOLDER[bld]}/${cmake_cache}" 2>/dev/null; then
            shell_command "rm -rf ./${FOLDER[tst]}/${FOLDER[bld]}/${cmake_cache}"
        fi
        shell_command "cmake -S ./${FOLDER[tst]} -B ./${FOLDER[tst]}/${FOLDER[bld]} -G Ninja""${CMAKE_CACHE_ENTRY}"
        NINJA_STATUS=$(echo -e "\e[92m[\e[92m%f/\e[92m%t\e[92m]\e[39m\e[49m ")
        export NINJA_STATUS CLICOLOR_FORCE=1 TERM=dumb
        shell_command "cmake --build ./${FOLDER[tst]}/${FOLDER[bld]}""${CMAKE_BUILD_OPTION}"

        exit "${STATUS}"
    fi

    set_compile_condition "${FOLDER[bld]}" "256m"
    if [[ -f ./${FOLDER[bld]}/${cmake_cache} ]] \
        && ! grep -Fwq "${DEV_OPT[compiler]}" "./${FOLDER[bld]}/${cmake_cache}" 2>/dev/null; then
        shell_command "rm -rf ./${FOLDER[bld]}/${cmake_cache}"
    fi
    shell_command "cmake -S ./ -B ./${FOLDER[bld]} -G Ninja""${CMAKE_CACHE_ENTRY}"

    local valid_param_num
    valid_param_num=$(($(count_enabled_single_choice_parameters) + $(count_enabled_multiple_choice_parameters)))
    if [[ valid_param_num -eq 0 ]] || {
        [[ valid_param_num -eq 1 ]] && [[ ${ARGS[release]} != false ]]
    }; then
        NINJA_STATUS=$(echo -e "\e[92m[\e[92m%f/\e[92m%t\e[92m]\e[39m\e[49m ")
        export NINJA_STATUS CLICOLOR_FORCE=1 TERM=dumb
        shell_command "cmake --build ./${FOLDER[bld]}""${CMAKE_BUILD_OPTION}"

        exit "${STATUS}"
    fi
    shell_command "cmake -S ./${FOLDER[tst]} -B ./${FOLDER[tst]}/${FOLDER[bld]} -G Ninja""${CMAKE_CACHE_ENTRY}"
}

function clean_up_temporary_files()
{
    local app_comp_db=${FOLDER[bld]}/${COMPILE_DB}
    if [[ -f ./${app_comp_db}.bak ]]; then
        shell_command "rm -rf ./${app_comp_db} && mv ./${app_comp_db}.bak ./${app_comp_db}"
    fi
    local tst_comp_db=${FOLDER[tst]}/${FOLDER[bld]}/${COMPILE_DB}
    if [[ -f ./${tst_comp_db}.bak ]]; then
        shell_command "rm -rf ./${tst_comp_db} && mv ./${tst_comp_db}.bak ./${tst_comp_db}"
    fi
}

# shellcheck disable=SC2317
function signal_handler()
{
    clean_up_temporary_files

    exit 1
}

function prepare_environment()
{
    export TERM=linux TERMINFO=/etc/terminfo
    if [[ -n ${FOO_ENV} ]]; then
        if [[ ${FOO_ENV} != "foo_dev" ]]; then
            die "The environment variable FOO_ENV must be foo_dev."
        fi
    else
        die "Please export the environment variable FOO_ENV."
    fi

    local script_path
    script_path=$(cd "$(dirname "${0}")" &>/dev/null && pwd)
    if [[ ${script_path} != *"${FOLDER[proj]}/${FOLDER[scr]}" ]]; then
        die "Illegal path to current script."
    fi
    cd "$(dirname "${script_path}")" || exit 1

    if [[ ${EUID} -ne 0 ]]; then
        SUDO_PREFIX="sudo "
    fi
    trap signal_handler SIGINT SIGTERM
    clean_up_temporary_files
}

function main()
{
    prepare_environment
    parse_parameters "$@"
    try_perform_single_choice_options
    build_native_if_needed
    try_perform_multiple_choice_options

    exit "${STATUS}"
}

main "$@"
