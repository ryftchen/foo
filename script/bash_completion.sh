#!/usr/bin/env bash

function _foo()
{
    COMPREPLY=()
    local curr prev
    curr=$(_get_cword)
    prev=${COMP_WORDS[COMP_CWORD - 1]}

    _expand || return 0

    case ${prev} in
    -@(h|-help))
        mapfile -t COMPREPLY < <(compgen -W "-a -ds -dp -n \
--algorithm --data-structure --design-pattern --numeric" -- "${curr}")
        return 0
        ;;
    -@(v|-version|c|-console))
        return 0
        ;;
    -@(a|-algorithm))
        mapfile -t COMPREPLY < <(compgen -W "match notation optimal search sort" -- "${curr}")
        return 0
        ;;
    -@(ds|-data-structure))
        mapfile -t COMPREPLY < <(compgen -W "linear tree" -- "${curr}")
        return 0
        ;;
    -@(dp|-design-pattern))
        mapfile -t COMPREPLY < <(compgen -W "behavioral creational structural" -- "${curr}")
        return 0
        ;;
    -@(n|-numeric))
        mapfile -t COMPREPLY < <(compgen -W "arithmetic divisor integral prime" -- "${curr}")
        return 0
        ;;
    match | notation | optimal | search | sort | \
        linear | tree | \
        behavioral | creational | structural | \
        arithmetic | divisor | integral | prime)
        mapfile -t COMPREPLY < <(compgen -W "-h --help" -- "${curr}")
        return 0
        ;;
    *)
        _filedir
        ;;
    esac

    case ${curr} in
    -*)
        mapfile -t COMPREPLY < <(compgen -W "-h -v -c -a -ds -dp -n \
--help --version --console --algorithm --data-structure --design-pattern --numeric" -- "${curr}")
        ;;
    *) ;;
    esac

    return 0
}

complete -F _foo foo
