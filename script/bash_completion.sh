#!/usr/bin/env bash

function _foo()
{
    COMPREPLY=()
    local curr prev
    curr=$(_get_cword)
    prev=${COMP_WORDS[COMP_CWORD - 1]}

    _expand || return

    case ${prev} in
    -@(h|-help|v|-version|r|-reconfig|c|-console))
        return
        ;;
    app-algo)
        mapfile -t COMPREPLY < <(compgen -W "-h --help -m  --match -n --notation -o --optimal -s --search \
-S --sort" -- "${curr}")
        return
        ;;
    app-dp)
        mapfile -t COMPREPLY < <(compgen -W "-h --help -b --behavioral -c --creational -s --structural" -- "${curr}")
        return
        ;;
    app-ds)
        mapfile -t COMPREPLY < <(compgen -W "-h --help -l --linear -t --tree" -- "${curr}")
        return
        ;;
    app-num)
        mapfile -t COMPREPLY < <(compgen -W "-h --help -a --arithmetic -d --divisor -i --integral \
-p- -prime" -- "${curr}")
        return
        ;;
    -*)
        return
        ;;
    *)
        _filedir
        ;;
    esac

    case ${curr} in
    *)
        mapfile -t COMPREPLY < <(compgen -W "-h --help -v --version -r --reconfig -c --console \
app-algo app-dp app-ds app-num" -- "${curr}")
        ;;
    esac
}

complete -F _foo foo
