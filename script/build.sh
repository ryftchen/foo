#!/usr/bin/env bash

declare -rA FOLDER=([proj]="foo" [app]="application" [util]="utility" [algo]="algorithm" [ds]="data_structure"
    [dp]="design_pattern" [num]="numeric" [tst]="test" [scr]="script" [doc]="document" [dock]="docker" [bld]="build"
    [cac]=".cache")
declare -r COMP_CMD="compile_commands.json"
declare -r BASH_RC=".bashrc"
declare -A ARGS=([help]=false [initialize]=false [clean]=false [install]=false [uninstall]=false [container]=false
    [website]=false [test]=false [release]=false [hook]=false [spell]=false [format]=false [lint]=false
    [statistics]=false [doxygen]=false [browser]=false [dry]=false)
declare -A DEV_OPT=([parallel]=0 [pch]=false [unity]=false [ccache]=false [distcc]=false [tmpfs]=false)
declare SUDO=""
declare CMAKE_CACHE_ENTRY=""
declare CMAKE_BUILD_OPTION=""
declare BUILD_TYPE="Debug"

function shell_command()
{
    echo
    if [[ ${ARGS[dry]} = false ]]; then
        printf "\033[0;37;40m\033[1m\033[49m%s%-40s%s\033[0m\n" "[ $(date "+%b %d %T") # CMD: " "$*" " # START  ]"
        /bin/bash -c "$@"
        printf "\033[0;37;40m\033[1m\033[49m%s%-40s%s\033[0m\n" "[ $(date "+%b %d %T") # CMD: " "$*" " # FINISH ]"
    else
        printf "\033[0;37;40m\033[1m\033[49m%s%-40s%s\033[0m\n" "[ $(date "+%b %d %T") # CMD: " "$*" " # PRACTICE ]"
    fi
}

function die()
{
    echo
    echo "$(basename "${0}"): $*"
    exit 1
}

function wait_until_get_input()
{
    local old_stty
    old_stty=$(stty -g)
    stty raw -echo
    local answer
    answer=$(while ! head -c 1 | grep -i '[ny]'; do true; done)
    stty "${old_stty}"

    echo "${answer}"
    return 0
}

function parse_parameters()
{
    while [[ $# -gt 0 ]]; do
        case $1 in
        -h | --help)
            check_single_choice_parameters_validity "$1"
            ARGS[help]=true
            ;;
        -I | --initialize)
            check_single_choice_parameters_validity "$1"
            ARGS[initialize]=true
            ;;
        -C | --clean)
            check_single_choice_parameters_validity "$1"
            ARGS[clean]=true
            ;;
        -i | --install)
            check_single_choice_parameters_validity "$1"
            ARGS[install]=true
            ;;
        -u | --uninstall)
            check_single_choice_parameters_validity "$1"
            ARGS[uninstall]=true
            ;;
        -c | --container)
            check_single_choice_parameters_validity "$1"
            ARGS[container]=true
            ;;
        -w | --website)
            check_single_choice_parameters_validity "$1"
            ARGS[website]=true
            ;;
        -t | --test)
            if {
                [[ ${ARGS[release]} = true ]] && [[ $(exist_multiple_choice_parameters) -gt 1 ]]
            } || {
                [[ ${ARGS[release]} = false ]] && [[ $(exist_multiple_choice_parameters) -gt 0 ]]
            } || [[ $(exist_single_choice_parameters) -gt 0 ]]; then
                die "Mutually exclusive option: $1 is not allowed."
            fi
            ARGS[test]=true
            ;;
        -r | --release)
            if {
                [[ ${ARGS[test]} = true ]] && [[ $(exist_single_choice_parameters) -gt 1 ]]
            } || {
                [[ ${ARGS[test]} = false ]] && [[ $(exist_single_choice_parameters) -gt 0 ]]
            }; then
                die "Mutually exclusive option: $1 is not allowed."
            fi
            ARGS[release]=true
            ;;
        -H | --hook)
            check_multiple_choice_parameters_validity "$1"
            ARGS[hook]=true
            ;;
        -s | --spell)
            check_multiple_choice_parameters_validity "$1"
            ARGS[spell]=true
            ;;
        -f | --format)
            check_multiple_choice_parameters_validity "$1"
            ARGS[format]=true
            ;;
        -l | --lint)
            check_multiple_choice_parameters_validity "$1"
            ARGS[lint]=true
            ;;
        -S | --statistics)
            check_multiple_choice_parameters_validity "$1"
            ARGS[statistics]=true
            ;;
        -d | --doxygen)
            check_multiple_choice_parameters_validity "$1"
            ARGS[doxygen]=true
            ;;
        -b | --browser)
            check_multiple_choice_parameters_validity "$1"
            ARGS[browser]=true
            ;;
        -D | --dry)
            ARGS[dry]=true
            ;;
        *)
            die "Unknown command line option: $1. Try using the --help option for information."
            ;;
        esac
        shift
    done
}

function check_single_choice_parameters_validity()
{
    if [[ $(exist_single_choice_parameters) -gt 0 ]] || [[ $(exist_multiple_choice_parameters) -gt 0 ]]; then
        die "Mutually exclusive option: $1 is not allowed."
    fi
}

function check_multiple_choice_parameters_validity()
{
    if [[ $(exist_single_choice_parameters) -gt 0 ]]; then
        die "Mutually exclusive option: $1 is not allowed."
    fi
}

function exist_single_choice_parameters()
{
    local number=0
    for key in "${!ARGS[@]}"; do
        if [[ ${ARGS[${key}]} = true ]]; then
            if [[ ${key} == "help" ]] || [[ ${key} == "initialize" ]] || [[ ${key} == "clean" ]] \
                || [[ ${key} == "install" ]] || [[ ${key} == "uninstall" ]] || [[ ${key} == "container" ]] \
                || [[ ${key} == "website" ]] || [[ ${key} == "test" ]]; then
                number+=1
            fi
        fi
    done
    echo "${number}"
    return 0
}

function exist_multiple_choice_parameters()
{
    local number=0
    for key in "${!ARGS[@]}"; do
        if [[ ${ARGS[${key}]} = true ]]; then
            if [[ ${key} == "release" ]] || [[ ${key} == "hook" ]] || [[ ${key} == "spell" ]] \
                || [[ ${key} == "format" ]] || [[ ${key} == "lint" ]] || [[ ${key} == "statistics" ]] \
                || [[ ${key} == "doxygen" ]] || [[ ${key} == "browser" ]]; then
                number+=1
            fi
        fi
    done
    echo "${number}"
    return 0
}

function try_to_perform_single_choice_options()
{
    perform_help_option
    perform_initialize_option
    perform_clean_option
    perform_install_option
    perform_uninstall_option
    perform_container_option
    perform_website_option
}

function perform_help_option()
{
    if [[ ${ARGS[help]} = false ]]; then
        return
    fi

    echo "usage: $(basename "${0}") [-h] [-I] [-C] [-i] [-u] [-c] [-w] [-t {-r}] \
[[{-H, -c, -f, -l, -S, -b, -d} ...] {-r}] {-D}"
    echo
    echo "build script"
    echo
    echo "options:"
    echo "  -h, --help            show help and exit"
    echo "  -I, --initialize      initialize environment and exit"
    echo "  -C, --clean           clean up project folder and exit"
    echo "  -i, --install         install binary with libraries and exit"
    echo "  -u, --uninstall       uninstall binary with libraries and exit"
    echo "  -c, --container       construct docker container and exit"
    echo "  -w, --website         launch/terminate web server and exit"
    echo "  -t, --test            build unit test and exit"
    echo "  -r, --release         set as release version"
    echo "  -H, --hook            run hook before commit"
    echo "  -s, --spell           spell check against dictionaries"
    echo "  -f, --format          format all code files"
    echo "  -l, --lint            lint all code files"
    echo "  -S, --statistics      code line statistics"
    echo "  -d, --doxygen         documentation with doxygen"
    echo "  -b, --browser         generate code browser like IDE"
    echo "  -D, --dry             dry run for script"
    exit 0
}

function perform_initialize_option()
{
    if [[ ${ARGS[initialize]} = false ]]; then
        return
    fi

    local export_cmd="export FOO_ENV=${FOLDER[proj]}_dev"
    if ! grep -Fxq "${export_cmd}" ~/"${BASH_RC}" 2>/dev/null && {
        [[ -z ${FOO_ENV} ]] || [[ ${FOO_ENV} != "${FOLDER[proj]}_dev" ]]
    }; then
        shell_command "echo '${export_cmd}' >>~/${BASH_RC}"
    fi

    local validate_proj="[[ \\\"\\\$(basename \\\"\\\$(pwd)\\\")\\\" == \\\"${FOLDER[proj]}\\\" ]] && \
git rev-parse --git-dir >/dev/null 2>&1 &&"
    local alias_cmd
    if ! grep -Fwq "alias ${FOLDER[proj]}-build" ~/"${BASH_RC}" 2>/dev/null; then
        alias_cmd="alias ${FOLDER[proj]}-build='${validate_proj} ./${FOLDER[scr]}/build.sh'"
        shell_command "echo \"${alias_cmd}\" >>~/${BASH_RC}"
    fi
    if ! grep -Fwq "alias ${FOLDER[proj]}-run" ~/"${BASH_RC}" 2>/dev/null; then
        alias_cmd="alias ${FOLDER[proj]}-run='${validate_proj} ./${FOLDER[scr]}/run.py'"
        shell_command "echo \"${alias_cmd}\" >>~/${BASH_RC}"
    fi
    if ! grep -Fwq "alias ${FOLDER[proj]}-exec" ~/"${BASH_RC}" 2>/dev/null; then
        alias_cmd="alias ${FOLDER[proj]}-exec='${validate_proj} ./${FOLDER[bld]}/bin/foo'"
        shell_command "echo \"${alias_cmd}\" >>~/${BASH_RC}"
    fi

    shell_command "cat <<EOF >./${FOLDER[scr]}/.env
#!/bin/false

FOO_BLD_PARALLEL=0
FOO_BLD_PCH=off
FOO_BLD_UNITY=off
FOO_BLD_CCACHE=on
FOO_BLD_DISTCC=off
FOO_BLD_TMPFS=off

export FOO_BLD_PARALLEL FOO_BLD_PCH FOO_BLD_UNITY FOO_BLD_CCACHE FOO_BLD_DISTCC FOO_BLD_TMPFS
return 0
EOF"
    shell_command "echo 'core.%s.%e.%p' | ${SUDO}tee /proc/sys/kernel/core_pattern"
    shell_command "git config --local commit.template ./.gitcommit.template"

    echo
    echo "To initialize for effect, type 'exec bash' manually."
    exit 0
}

function perform_clean_option()
{
    if [[ ${ARGS[clean]} = false ]]; then
        return
    fi

    shell_command "sed -i '/export FOO_ENV=${FOLDER[proj]}_dev/d' ~/${BASH_RC} 2>/dev/null"
    shell_command "sed -i '/alias ${FOLDER[proj]}-/d' ~/${BASH_RC} 2>/dev/null"
    shell_command "find ./ -maxdepth 3 -type d | sed 1d \
| grep -E '(${FOLDER[bld]}|${FOLDER[cac]}|target|doxygen|browser|archive|__pycache__)$' | xargs -i rm -rf {}"
    shell_command "rm -rf ./${FOLDER[scr]}/.env ./${FOLDER[doc]}/server/Cargo.lock ./core.* ./vgcore.* ./*.profraw"
    shell_command "git config --local --unset commit.template"

    if [[ -f ./.git/hooks/pre-commit ]]; then
        shell_command "pre-commit uninstall"
    fi

    echo
    echo "To clean up for effect, type 'exec bash' manually."
    exit 0
}

function perform_install_option()
{
    if [[ ${ARGS[install]} = false ]]; then
        return
    fi

    if [[ ! -f ./${FOLDER[bld]}/bin/${FOLDER[proj]} ]]; then
        die "There is no binary file in the ${FOLDER[bld]} folder. Please finish compiling first."
    fi

    shell_command "${SUDO}cmake --install ./${FOLDER[bld]}"
    local install_path=/opt/${FOLDER[proj]}
    if [[ -d ${install_path} ]]; then
        local bin_path=${install_path}/bin
        local export_cmd="export PATH=${bin_path}:\$PATH"
        if [[ :${PATH}: != *:${bin_path}:* ]] && ! grep -Fxq "${export_cmd}" ~/"${BASH_RC}" 2>/dev/null; then
            shell_command "echo '${export_cmd}' >>~/${BASH_RC}"
        fi

        local completion_file="bash_completion"
        export_cmd="[ \"\${BASH_COMPLETION_VERSINFO}\" != \"\" ] && [ -s ${install_path}/${completion_file} ] && \
\. ${install_path}/${completion_file}"
        shell_command "${SUDO}cp ./${FOLDER[scr]}/${completion_file}.sh ${install_path}/${completion_file}"
        if ! grep -Fxq "${export_cmd}" ~/"${BASH_RC}" 2>/dev/null; then
            shell_command "echo '${export_cmd}' >>~/${BASH_RC}"
        fi

        local man_path=${install_path}/man
        export_cmd="MANDATORY_MANPATH ${man_path}"
        shell_command "${SUDO}mkdir -p ${man_path}/man1 && \
${SUDO}cp ./${FOLDER[doc]}/man.1 ${man_path}/man1/${FOLDER[proj]}.1"
        if ! grep -Fxq "${export_cmd}" ~/.manpath 2>/dev/null; then
            shell_command "echo '${export_cmd}' >>~/.manpath"
        fi
    fi

    echo
    echo "Manually type 'exec bash' to install for effect."
    exit 0
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
    shell_command "cat ./${FOLDER[bld]}/${manifest_file} | xargs ${SUDO}rm -rf && \
${SUDO}rm -rf /opt/${FOLDER[proj]}/${completion_file} /opt/${FOLDER[proj]}/man"
    shell_command "cat ./${FOLDER[bld]}/${manifest_file} | xargs -L1 dirname | xargs ${SUDO}rmdir -p 2>/dev/null"
    if [[ -f ~/${BASH_RC} ]]; then
        shell_command "sed -i '/export PATH=\/opt\/${FOLDER[proj]}\/bin:\$PATH/d' ~/${BASH_RC}"
        shell_command "sed -i '/\\\. \/opt\/${FOLDER[proj]}\/${completion_file}/d' ~/${BASH_RC}"
    fi
    if [[ -f ~/.manpath ]]; then
        shell_command "sed -i '/MANDATORY_MANPATH \/opt\/${FOLDER[proj]}\/man/d' ~/.manpath"
    fi

    echo
    echo "Manually type 'exec bash' to uninstall for effect."
    exit 0
}

function perform_container_option()
{
    if [[ ${ARGS[container]} = false ]]; then
        return
    fi

    if command -v docker >/dev/null 2>&1 && command -v docker-compose >/dev/null 2>&1; then
        echo "Please confirm whether continue constructing the docker container. (y or n)"
        local input
        input=$(wait_until_get_input)
        if echo "${input}" | grep -iq '^y'; then
            echo "Yes"
        else
            echo "No"
            exit 0
        fi
    else
        die "No docker or docker-compose program. Please install it."
    fi

    if ! docker ps -a --format "{{lower .Image}} {{lower .Names}}" \
        | grep -q "ryftchen/${FOLDER[proj]}:latest ${FOLDER[proj]}_dev" 2>/dev/null; then
        shell_command "docker-compose -f ./${FOLDER[dock]}/docker-compose.yml up -d --force-recreate"
    else
        die "The container exists."
    fi
    exit 0
}

function perform_website_option()
{
    if [[ ${ARGS[website]} = false ]]; then
        return
    fi

    if command -v rustc >/dev/null 2>&1 && command -v cargo >/dev/null 2>&1; then
        shell_command "cargo build --release --manifest-path ./${FOLDER[doc]}/server/Cargo.toml"
        echo ""
        if ! pgrep -f "${FOLDER[proj]}_doc" >/dev/null 2>&1; then
            echo "Please confirm whether continue launching the document server. (y or n)"
            local input
            input=$(wait_until_get_input)
            if echo "${input}" | grep -iq '^y'; then
                echo "Yes"
                shell_command "./${FOLDER[doc]}/server/target/release/${FOLDER[proj]}_doc --root-dir . &"
                sleep 0.2s
            else
                echo "No"
            fi
        else
            echo "The document server starts listening under the ${PWD} directory..."
            echo "=> ${FOLDER[doc]}/doxygen online: http://127.0.0.1:61503/"
            echo "=> ${FOLDER[doc]}/browser online: http://127.0.0.1:61504/"
            echo "Please confirm whether continue terminating the document server. (y or n)"
            local input
            input=$(wait_until_get_input)
            if echo "${input}" | grep -iq '^y'; then
                echo "Yes"
                shell_command "fuser -k 61503/tcp && fuser -k 61504/tcp"
            else
                echo "No"
            fi
        fi
    else
        die "No rustc or cargo program. Please install it."
    fi
    exit 0
}

function try_to_perform_multiple_choice_options()
{
    check_extra_dependencies

    perform_hook_option
    perform_spell_option
    perform_format_option
    perform_lint_option
    perform_statistics_option
    perform_doxygen_option
    perform_browser_option
}

function check_extra_dependencies()
{
    if [[ ${ARGS[hook]} = true ]]; then
        if ! command -v pre-commit >/dev/null 2>&1; then
            die "No pre-commit program. Please install it."
        fi
    fi

    if [[ ${ARGS[spell]} = true ]]; then
        if ! command -v cspell >/dev/null 2>&1; then
            die "No cspell program. Please install it."
        fi
    fi

    if [[ ${ARGS[format]} = true ]]; then
        if ! command -v clang-format-16 >/dev/null 2>&1 || ! command -v shfmt >/dev/null 2>&1 \
            || ! command -v black >/dev/null 2>&1 || ! command -v rustfmt >/dev/null 2>&1; then
            die "No clang-format, shfmt, black or rustfmt program. Please install it."
        fi
    fi

    if [[ ${ARGS[lint]} = true ]]; then
        if ! command -v clang-tidy-16 >/dev/null 2>&1 || ! command -v run-clang-tidy-16 >/dev/null 2>&1 \
            || ! command -v compdb >/dev/null 2>&1 || ! command -v shellcheck >/dev/null 2>&1 \
            || ! command -v pylint >/dev/null 2>&1 || ! command -v clippy-driver >/dev/null 2>&1; then
            die "No clang-tidy (including run-clang-tidy-16, compdb), shellcheck, pylint or clippy program. \
Please install it."
        fi
        if [[ ${DEV_OPT[pch]} = true ]] || [[ ${DEV_OPT[unity]} = true ]]; then
            die "Due to the unconventional ${COMP_CMD} file, the --lint option cannot run if the FOO_BLD_PCH or \
FOO_BLD_UNITY is turned on."
        fi
    fi

    if [[ ${ARGS[statistics]} = true ]]; then
        if ! command -v cloc >/dev/null 2>&1; then
            die "No cloc program. Please install it."
        fi
    fi

    if [[ ${ARGS[doxygen]} = true ]]; then
        if ! command -v doxygen >/dev/null 2>&1 || ! command -v dot >/dev/null 2>&1; then
            die "No doxygen or dot program. Please install it."
        fi
    fi

    if [[ ${ARGS[browser]} = true ]]; then
        if ! command -v codebrowser_generator >/dev/null 2>&1 \
            || ! command -v codebrowser_indexgenerator >/dev/null 2>&1; then
            die "No codebrowser_generator or codebrowser_indexgenerator program. Please install it."
        fi
        if [[ ${DEV_OPT[pch]} = true ]] || [[ ${DEV_OPT[unity]} = true ]]; then
            die "Due to the unconventional ${COMP_CMD} file, the --browser option cannot run if the FOO_BLD_PCH or \
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

function perform_format_option()
{
    if [[ ${ARGS[format]} = false ]]; then
        return
    fi

    shell_command "find ./${FOLDER[app]} ./${FOLDER[util]} ./${FOLDER[algo]} ./${FOLDER[ds]} ./${FOLDER[dp]} \
./${FOLDER[num]} ./${FOLDER[tst]} -name '*.cpp' -o -name '*.hpp' -o -name '*.tpp' | grep -v '/${FOLDER[bld]}/' \
| xargs clang-format-16 --Werror -i --style=file:./.clang-format --verbose"
    shell_command "shfmt -l -w ./${FOLDER[scr]}/*.sh"
    shell_command "black --config ./.toml ./${FOLDER[scr]}/*.py"
    shell_command "cargo fmt --all --verbose --manifest-path ./${FOLDER[doc]}/server/Cargo.toml"
}

function perform_lint_option()
{
    if [[ ${ARGS[lint]} = false ]]; then
        return
    fi

    local app_comp_cmd=${FOLDER[bld]}/${COMP_CMD}
    if [[ ! -f ./${app_comp_cmd} ]]; then
        die "There is no ${COMP_CMD} file in the ${FOLDER[bld]} folder. Please generate it."
    fi
    shell_command "compdb -p ./${FOLDER[bld]} list >./${COMP_CMD} && mv ./${app_comp_cmd} ./${app_comp_cmd}.bak \
&& mv ./${COMP_CMD} ./${FOLDER[bld]}"
    while true; do
        local line
        line=$(grep -n '.tpp' "./${app_comp_cmd}" | head -n 1 | cut -d : -f 1)
        if ! [[ ${line} =~ ^[0-9]+$ ]]; then
            break
        fi
        shell_command "sed -i \$((${line} - 2)),\$((${line} + 3))d ./${app_comp_cmd}"
    done
    shell_command "find ./${FOLDER[app]} ./${FOLDER[util]} ./${FOLDER[algo]} ./${FOLDER[ds]} ./${FOLDER[dp]} \
./${FOLDER[num]} -name '*.cpp' -o -name '*.hpp' | xargs run-clang-tidy-16 -config-file=./.clang-tidy \
-p ./${FOLDER[bld]} -quiet"
    shell_command "find ./${FOLDER[app]} ./${FOLDER[util]} ./${FOLDER[algo]} ./${FOLDER[ds]} ./${FOLDER[dp]} \
./${FOLDER[num]} -name '*.tpp' | xargs clang-tidy-16 --config-file=./.clang-tidy -p ./${FOLDER[bld]} --quiet"
    shell_command "rm -rf ./${app_comp_cmd} && mv ./${app_comp_cmd}.bak ./${app_comp_cmd}"

    local tst_comp_cmd=${FOLDER[tst]}/${FOLDER[bld]}/${COMP_CMD}
    if [[ ! -f ./${tst_comp_cmd} ]]; then
        die "There is no ${COMP_CMD} file in the ${FOLDER[tst]}/${FOLDER[bld]} folder. Please generate it."
    fi
    shell_command "compdb -p ./${FOLDER[tst]}/${FOLDER[bld]} list >./${COMP_CMD} \
&& mv ./${tst_comp_cmd} ./${tst_comp_cmd}.bak && mv ./${COMP_CMD} ./${FOLDER[tst]}/${FOLDER[bld]}"
    shell_command "find ./${FOLDER[tst]} -name '*.cpp' | xargs run-clang-tidy-16 -config-file=./.clang-tidy \
-p ./${FOLDER[tst]}/${FOLDER[bld]} -quiet"
    shell_command "rm -rf ./${tst_comp_cmd} && mv ./${tst_comp_cmd}.bak ./${tst_comp_cmd}"

    shell_command "shellcheck -a ./${FOLDER[scr]}/*.sh"
    shell_command "pylint --rcfile=./.pylintrc ./${FOLDER[scr]}/*.py"
    shell_command "cargo clippy --release --manifest-path ./${FOLDER[doc]}/server/Cargo.toml"
}

function perform_statistics_option()
{
    if [[ ${ARGS[statistics]} = false ]]; then
        return
    fi

    shell_command "cloc --config ./.cloc --include-lang='C,C++,C/C++ Header'"
    shell_command "cloc --config ./.cloc --include-lang='Bourne Shell'"
    shell_command "cloc --config ./.cloc --include-lang='Python'"
    shell_command "cloc --config ./.cloc --include-lang='Rust'"
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
    if [[ -d ./${FOLDER[doc]}/archive ]]; then
        local last_tar="${FOLDER[proj]}_doxygen_${commit_id}.tar.bz2"
        if [[ -f ./${FOLDER[doc]}/archive/${last_tar} ]]; then
            local time_interval=$(($(date +%s) - $(stat -L --format %Y "./${FOLDER[doc]}/archive/${last_tar}")))
            if [[ ${time_interval} -lt 60 ]]; then
                die "The latest doxygen archive ${last_tar} has been generated since ${time_interval}s ago."
            fi
        fi
        package_for_doxygen "${commit_id}"
    else
        shell_command "mkdir ./${FOLDER[doc]}/archive"
        package_for_doxygen "${commit_id}"
    fi
}

function package_for_doxygen()
{
    local commit_id=$1

    local doxygen_folder="doxygen"
    local tar_file="${FOLDER[proj]}_${doxygen_folder}_${commit_id}.tar.bz2"
    shell_command "rm -rf ./${FOLDER[doc]}/archive/${FOLDER[proj]}_${doxygen_folder}_*.tar.bz2 \
./${FOLDER[doc]}/${doxygen_folder}"

    shell_command "mkdir -p ./${FOLDER[doc]}/${doxygen_folder}"
    shell_command "(cat ./${FOLDER[doc]}/Doxyfile ; echo 'PROJECT_NUMBER=\"@ $(git rev-parse --short @)\"') \
| doxygen -"
    shell_command "tar -jcvf ./${FOLDER[doc]}/archive/${tar_file} -C ./${FOLDER[doc]} ${doxygen_folder} >/dev/null"
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
    if [[ -d ./${FOLDER[doc]}/archive ]]; then
        local last_tar="${FOLDER[proj]}_browser_${commit_id}.tar.bz2"
        if [[ -f ./${FOLDER[doc]}/archive/${last_tar} ]]; then
            local time_interval=$(($(date +%s) - $(stat -L --format %Y "./${FOLDER[doc]}/archive/${last_tar}")))
            if [[ ${time_interval} -lt 60 ]]; then
                die "The latest browser archive ${last_tar} has been generated since ${time_interval}s ago."
            fi
        fi
        package_for_browser "${commit_id}"
    else
        shell_command "mkdir ./${FOLDER[doc]}/archive"
        package_for_browser "${commit_id}"
    fi
}

function package_for_browser()
{
    local commit_id=$1

    if [[ ! -f ./${FOLDER[bld]}/${COMP_CMD} ]]; then
        die "There is no ${COMP_CMD} file in the ${FOLDER[bld]} folder. Please generate it."
    fi
    local browser_folder="browser"
    local tar_file="${FOLDER[proj]}_${browser_folder}_${commit_id}.tar.bz2"
    shell_command "rm -rf ./${FOLDER[doc]}/archive/${FOLDER[proj]}_${browser_folder}_*.tar.bz2 \
./${FOLDER[doc]}/${browser_folder}"

    shell_command "mkdir -p ./${FOLDER[doc]}/${browser_folder}"
    shell_command "cp -rf /usr/local/share/woboq/data ./${FOLDER[doc]}/${browser_folder}/"
    shell_command "codebrowser_generator -color -a -b ./${FOLDER[bld]}/${COMP_CMD} \
-o ./${FOLDER[doc]}/${browser_folder} -p ${FOLDER[proj]}:.:${commit_id} -d ./data"
    shell_command "codebrowser_generator -color -a -b ./${FOLDER[tst]}/${FOLDER[bld]}/${COMP_CMD} \
-o ./${FOLDER[doc]}/${browser_folder} -p ${FOLDER[proj]}:.:${commit_id} -d ./data"
    shell_command "codebrowser_indexgenerator ./${FOLDER[doc]}/${browser_folder} -d ./data"

    shell_command "find \"./${FOLDER[doc]}/${browser_folder}/index.html\" \
\"./${FOLDER[doc]}/${browser_folder}/${FOLDER[proj]}\" \"./${FOLDER[doc]}/${browser_folder}/include\" -name \"*.html\" \
-exec sed -i '/^<\/head>$/i <link rel=\\\"shortcut icon\\\" href=\\\"https://woboq.com/favicon.ico\\\" \
type=\\\"image/x-icon\\\" />' {} +"
    shell_command "tar -jcvf ./${FOLDER[doc]}/archive/${tar_file} -C ./${FOLDER[doc]} ${browser_folder} >/dev/null"
}

function build_target()
{
    if ! command -v cmake >/dev/null 2>&1 || ! command -v ninja >/dev/null 2>&1; then
        die "No cmake or ninja program. Please install it."
    fi
    if ! command -v clang-16 >/dev/null 2>&1 || ! command -v clang++-16 >/dev/null 2>&1; then
        die "No clang-16 or clang++-16 program. Please install it."
    fi
    if [[ ${ARGS[release]} = true ]]; then
        BUILD_TYPE="Release"
    fi

    if [[ ${ARGS[test]} = true ]]; then
        set_compile_condition "${FOLDER[tst]}/${FOLDER[bld]}" "128m"
        shell_command "cmake -S ./${FOLDER[tst]} -B ./${FOLDER[tst]}/${FOLDER[bld]} -G Ninja""${CMAKE_CACHE_ENTRY}"
        NINJA_STATUS=$(echo -e "\e[92m[\e[92m%f/\e[92m%t\e[92m]\e[39m\e[49m ")
        export NINJA_STATUS
        shell_command "cmake --build ./${FOLDER[tst]}/${FOLDER[bld]}""${CMAKE_BUILD_OPTION}"
        exit 0
    fi

    set_compile_condition "${FOLDER[bld]}" "256m"
    shell_command "cmake -S . -B ./${FOLDER[bld]} -G Ninja""${CMAKE_CACHE_ENTRY}"
    if [[ $# -eq 0 ]] || {
        [[ $# -eq 1 ]] && [[ ${ARGS[release]} = true ]]
    }; then
        NINJA_STATUS=$(echo -e "\e[92m[\e[92m%f/\e[92m%t\e[92m]\e[39m\e[49m ")
        export NINJA_STATUS
        shell_command "cmake --build ./${FOLDER[bld]}""${CMAKE_BUILD_OPTION}"
        exit 0
    fi
    shell_command "cmake -S ./${FOLDER[tst]} -B ./${FOLDER[tst]}/${FOLDER[bld]} -G Ninja""${CMAKE_CACHE_ENTRY}"
}

function set_compile_condition()
{
    local tmpfs_subfolder=$1 tmpfs_size=$2

    export CC=/usr/bin/clang-16 CXX=/usr/bin/clang++-16
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
    CMAKE_CACHE_ENTRY=" -D CMAKE_BUILD_TYPE=${BUILD_TYPE}"
    if [[ ${DEV_OPT[pch]} = true ]]; then
        CMAKE_CACHE_ENTRY="${CMAKE_CACHE_ENTRY} -D TOOLCHAIN_PCH=ON"
        if [[ ${DEV_OPT[ccache]} = true ]]; then
            export CCACHE_PCH_EXTSUM=true
        fi
    fi
    if [[ ${DEV_OPT[unity]} = true ]]; then
        CMAKE_CACHE_ENTRY="${CMAKE_CACHE_ENTRY} -D TOOLCHAIN_UNITY=ON"
    fi
    if [[ ${DEV_OPT[ccache]} = true ]]; then
        CMAKE_CACHE_ENTRY="${CMAKE_CACHE_ENTRY} -D TOOLCHAIN_CCACHE=ON"
        if [[ ${ARGS[test]} = false ]]; then
            export CCACHE_DIR=${PWD}/${FOLDER[cac]}/ccache
        else
            export CCACHE_DIR=${PWD}/${FOLDER[tst]}/${FOLDER[cac]}/ccache
        fi
        if [[ ${DEV_OPT[distcc]} = true ]]; then
            if command -v ccache >/dev/null 2>&1 && command -v distcc >/dev/null 2>&1; then
                export CCACHE_PREFIX=distcc
            fi
        fi
    fi
    if [[ ${DEV_OPT[distcc]} = true ]]; then
        CMAKE_CACHE_ENTRY="${CMAKE_CACHE_ENTRY} -D TOOLCHAIN_DISTCC=ON"
        if [[ -z ${DISTCC_HOSTS} ]]; then
            export DISTCC_HOSTS=localhost
        fi
    fi
    if [[ ${DEV_OPT[tmpfs]} = true ]]; then
        if [[ ! -d ./${tmpfs_subfolder} ]]; then
            shell_command "mkdir ./${tmpfs_subfolder}"
        fi
        if ! df -h -t tmpfs | grep -q "${FOLDER[proj]}/${tmpfs_subfolder}" 2>/dev/null; then
            shell_command "${SUDO}mount -t tmpfs -o size=${tmpfs_size} tmpfs ./${tmpfs_subfolder}"
        fi
    elif df -h -t tmpfs | grep -q "${FOLDER[proj]}/${tmpfs_subfolder}" 2>/dev/null; then
        shell_command "${SUDO}umount ./${tmpfs_subfolder}"
    fi
}

function remove_temporary_files()
{
    local app_comp_cmd=${FOLDER[bld]}/${COMP_CMD}
    if [[ -f ./${app_comp_cmd}.bak ]]; then
        shell_command "rm -rf ./${app_comp_cmd} && mv ./${app_comp_cmd}.bak ./${app_comp_cmd}"
    fi
    local tst_comp_cmd=${FOLDER[tst]}/${FOLDER[bld]}/${COMP_CMD}
    if [[ -f ./${tst_comp_cmd}.bak ]]; then
        shell_command "rm -rf ./${tst_comp_cmd} && mv ./${tst_comp_cmd}.bak ./${tst_comp_cmd}"
    fi
}

function signal_handler()
{
    remove_temporary_files
    exit 1
}

function main()
{
    local script_path
    script_path=$(cd "$(dirname "${0}")" &>/dev/null && pwd)
    if [[ ${script_path} != *"${FOLDER[proj]}/${FOLDER[scr]}" ]]; then
        die "Illegal path to current script."
    fi
    cd "$(dirname "${script_path}")" || exit 1

    export TERM=linux TERMINFO=/etc/terminfo
    if [[ ${EUID} -ne 0 ]]; then
        SUDO="sudo "
    fi
    trap signal_handler SIGINT SIGTERM
    remove_temporary_files

    parse_parameters "$@"
    try_to_perform_single_choice_options
    build_target "$@"
    try_to_perform_multiple_choice_options
}

main "$@"
