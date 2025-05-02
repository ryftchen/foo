set auto-load safe-path /
set backtrace past-main on
set breakpoint pending on
set history filename ~/.gdbhistory
set history save on
set history size 1000
set pagination off
set print array on
set print elements 0
set print frame-arguments all
set print object on
set print pretty on
set print repeats 0
set print thread-events on
set style enabled on

define fp
    set follow-fork-mode parent
    echo set follow-fork-mode parent\n
end
define fc
    set follow-fork-mode child
    echo set follow-fork-mode child\n
end

define sl
    set scheduler-locking on
    echo set scheduler-locking on\n
end
define su
    set scheduler-locking off
    echo set scheduler-locking off\n
end
