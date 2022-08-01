#!/usr/bin/env python3

import curses
import fcntl
import signal
import sys
import subprocess
import time

BAR_SAVE_CURSOR = "\033[s"
BAR_RESTORE_CURSOR = "\033[u"
BAR_MOVE_UP_CURSOR = "\033[1A"
BAR_FORE_COLOR = "\033[30m"
BAR_BACK_COLOR = "\033[42m"
BAR_FORE_COLOR_DEFAULT = "\033[39m"
BAR_BACK_COLOR_DEFAULT = "\033[49m"
BAR_CURRENT_LINES = 0
BAR_PLACEHOLDER_LENGTH = 20
BAR_SET_TRAP = False
BAR_SIGNAL_DEFAULT = None

STDOUT_DEFAULT = sys.stdout


class Log:
    def __init__(self, filename, stream=sys.stdout):
        self.terminal = stream
        self.log = open(filename, "w")
        fcntl.flock(self.log, fcntl.LOCK_EX | fcntl.LOCK_NB)

    def write(self, message):
        self.terminal.write(message)
        try:
            self.log.write(message)
        except IOError:
            fcntl.flock(self.log, fcntl.LOCK_UN)
            self.log.close()
            sys.stdout = STDOUT_DEFAULT

    def flush(self):
        pass

    def __del__(self):
        fcntl.flock(self.log, fcntl.LOCK_UN)
        self.log.close()
        sys.stdout = STDOUT_DEFAULT


def executeCommand(cmd):
    try:
        out = subprocess.Popen(
            cmd,
            shell=True,
            executable="/bin/bash",
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            encoding="utf-8",
        )
    except RuntimeError as err:
        return "", err.args[0], 255
    stdout, stderr = out.communicate()
    error = out.returncode
    return stdout.strip(), stderr.strip(), error


def setupProgressBar():
    global BAR_CURRENT_LINES
    curses.setupterm()

    trapDueToInterrupt()

    BAR_CURRENT_LINES = tputLines()
    lines = BAR_CURRENT_LINES - 1
    printProgress("\n")

    printProgress(BAR_SAVE_CURSOR)
    printProgress(f"\033[0;{str(lines)}r")

    printProgress(BAR_RESTORE_CURSOR)
    printProgress(BAR_MOVE_UP_CURSOR)
    drawProgressBar(0)


def drawProgressBar(percentage):
    lines = tputLines()
    if lines != BAR_CURRENT_LINES:
        setupProgressBar()

    printProgress(BAR_SAVE_CURSOR)
    printProgress(f"\033[{str(lines)};0f")

    tput()
    printBar(percentage)
    printProgress(BAR_RESTORE_CURSOR)
    time.sleep(0.05)


def destroyProgressBar():
    lines = tputLines()
    printProgress(BAR_SAVE_CURSOR)
    printProgress(f"\033[0;{str(lines)}r")

    printProgress(BAR_RESTORE_CURSOR)
    printProgress(BAR_MOVE_UP_CURSOR)

    clearProgressBar()
    printProgress("\n\n")
    if BAR_SET_TRAP:
        signal.signal(signal.SIGINT, BAR_SIGNAL_DEFAULT)


def clearProgressBar():
    lines = tputLines()
    printProgress(BAR_SAVE_CURSOR)
    printProgress(f"\033[{str(lines)};0f")

    tput()
    printProgress(BAR_RESTORE_CURSOR)


def trapDueToInterrupt():
    global BAR_SET_TRAP
    global BAR_SIGNAL_DEFAULT
    BAR_SET_TRAP = True
    BAR_SIGNAL_DEFAULT = signal.getsignal(signal.SIGINT)
    signal.signal(signal.SIGINT, clearDueToInterrupt)


def clearDueToInterrupt(sign, frame):
    destroyProgressBar()
    raise KeyboardInterrupt


def printBar(percentage):
    cols = tputCols()
    barSize = cols - BAR_PLACEHOLDER_LENGTH
    color = f"{BAR_FORE_COLOR}{BAR_BACK_COLOR}"
    defaultColor = f"{BAR_FORE_COLOR_DEFAULT}{BAR_BACK_COLOR_DEFAULT}"

    completeSize = int((barSize * percentage) / 100)
    remainderSize = barSize - completeSize
    progressBar = f"[{color}{'#' * int(completeSize)}{defaultColor}{'.' * int(remainderSize)}]"
    printProgress(f" Progress {percentage:>3}% {progressBar}\r")


def printProgress(text):
    print(text, end="")


def tputLines():
    stdout, _, _ = executeCommand("tput lines")
    return int(stdout)


def tputCols():
    stdout, _, _ = executeCommand("tput cols")
    return int(stdout)


def tput():
    print(curses.tparm(curses.tigetstr("el")).decode(), end="")
