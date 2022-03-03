#!/usr/bin/env python3

import argparse
import curses
import fcntl
import os
import signal
import subprocess
import sys
import time
from datetime import datetime

RUN_CMD = "foo"
RUN_DIR = "./build/"
OPTION_TYPE_1 = ["-o", "-i", "-s"]
OPTIMUM = ["fib", "gra", "ann", "par", "gen"]
INTEGRAL = ["tra", "sim", "rom", "gau", "mon"]
SORT = ["bub", "sel", "ins", "she", "mer", "qui", "hea", "cou", "buc", "rad"]
OPTION_TYPE_2 = ["--optimum", "--integral", "--sort", "--log", "--verbose", "--help"]
CURRENT_STEP = 0
WHOLE_STEP = (
    (len(OPTION_TYPE_1) + 2)
    + (len(OPTIMUM) + 1)
    + (len(INTEGRAL) + 1)
    + (len(SORT) + 1)
    + len(OPTION_TYPE_2)
)

STDOUT_DEFAULT = sys.stdout
STDOUT_LOG = sys.stdout
SET_VALGRIND = False
VALGRIND_CMD = "valgrind --tool=memcheck --show-reachable=yes --leak-check=full \
--leak-resolution=high --log-fd=1"
TEMP_LOG = "./temp/test.log"
TEMP_PATH = "./temp"
BUILD_CMD = "./script/build.sh"
BUILD_COMPILER_START = "Configuring done"
BUILD_COMPILER_FINISH = "Built target"
FORE_RED = "\033[0;31;40m"
FORE_GREEN = "\033[0;32;40m"
FORE_YELLOW = "\033[0;33;40m"
FORE_BLUE = "\033[0;36;40m"
SPLIT_LINE = "==============="

SAVE_CURSOR = "\033[s"
RESTORE_CURSOR = "\033[u"
MOVE_UP_CURSOR = "\033[1A"
FORE_COLOR = "\033[30m"
BACK_COLOR = "\033[42m"
FORE_DEFAULT = "\033[39m"
BACK_DEFAULT = "\033[49m"
CURRENT_LINES = 0
SET_TRAP = False
SIGNAL_DEFAULT = None


class Log:
    def __init__(self, filename=TEMP_LOG, stream=sys.stdout):
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
            printAbort("Failed to write log file test.log.")

    def flush(self):
        pass

    def uninit(self):
        fcntl.flock(self.log, fcntl.LOCK_UN)
        self.log.close()
        sys.stdout = STDOUT_DEFAULT


def setupProgressBar():
    global CURRENT_LINES
    curses.setupterm()

    trapDueToInterrupt()

    CURRENT_LINES = tputLines()
    lines = CURRENT_LINES - 1
    printProgress("\n")

    printProgress(SAVE_CURSOR)
    printProgress("\033[0;" + str(lines) + "r")

    printProgress(RESTORE_CURSOR)
    printProgress(MOVE_UP_CURSOR)
    drawProgressBar(0)


def drawProgressBar(percentage):
    lines = tputLines()
    if lines != CURRENT_LINES:
        setupProgressBar()

    printProgress(SAVE_CURSOR)
    printProgress("\033[" + str(lines) + ";0f")

    tput()
    printBar(percentage)
    printProgress(RESTORE_CURSOR)
    time.sleep(0.1)


def destroyProgressBar():
    lines = tputLines()
    printProgress(SAVE_CURSOR)
    printProgress("\033[0;" + str(lines) + "r")

    printProgress(RESTORE_CURSOR)
    printProgress(MOVE_UP_CURSOR)

    clearProgressBar()
    printProgress("\n\n")
    if SET_TRAP:
        signal.signal(signal.SIGINT, SIGNAL_DEFAULT)


def clearProgressBar():
    lines = tputLines()
    printProgress(SAVE_CURSOR)
    printProgress("\033[" + str(lines) + ";0f")

    tput()
    printProgress(RESTORE_CURSOR)


def trapDueToInterrupt():
    global SET_TRAP
    global SIGNAL_DEFAULT
    SET_TRAP = True
    SIGNAL_DEFAULT = signal.getsignal(signal.SIGINT)
    signal.signal(signal.SIGINT, clearDueToInterrupt)


def clearDueToInterrupt(sign, frame):
    destroyProgressBar()
    raise KeyboardInterrupt


def printBar(percentage):
    cols = tputCols()
    barSize = cols - 18
    color = f"{FORE_COLOR}{BACK_COLOR}"

    completeSize = int((barSize * percentage) / 100)
    remainderSize = barSize - completeSize
    progressBar = (
        f"[{color}{'#' * int(completeSize)}{FORE_DEFAULT}{BACK_DEFAULT}{'.' * int(remainderSize)}]"
    )
    printProgress(f" Progress {percentage:>3}% {progressBar}\r")


def printProgress(text):
    print(text, end="")


def tputLines():
    cmd = tryCommand("tput lines")
    output = cmd.stdout.read()
    return int(output)


def tputCols():
    cmd = tryCommand("tput cols")
    output = cmd.stdout.read()
    return int(output)


def tput():
    print(curses.tparm(curses.tigetstr("el")).decode(), end="")


def tryCommand(command):
    cmd = subprocess.Popen(
        command, shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE, encoding="utf-8"
    )
    cmd.wait()
    return cmd


def printAbort(message):
    print("Python script test.py: " + message)
    sys.exit(-1)


def buildProject(command):
    cmd = tryCommand(command)
    output = cmd.stdout.read()
    print(output)
    if cmd.returncode != 0:
        printAbort("Failed to execute shell script build.sh.")
    elif output.find(BUILD_COMPILER_START) != -1 and output.find(BUILD_COMPILER_FINISH) == -1:
        printAbort("Failed to build project by shell script build.sh.")


def executeCommand(command):
    completeCommand = RUN_DIR + command
    if SET_VALGRIND:
        completeCommand = VALGRIND_CMD + " " + completeCommand
    align = max(len(command) + 20, 30)
    print(
        "\r\n"
        + FORE_BLUE
        + SPLIT_LINE
        + "[ "
        + datetime.strftime(datetime.now(), "%b %d %H:%M:%S")
        + " | TEST TASK: "
        + "%-10s" % command
        + " | START  ]"
        + SPLIT_LINE
        + "\033[0m\n"
    )
    cmd = tryCommand(completeCommand)
    output = cmd.stdout.read()
    print(output)
    if cmd.returncode == 0:
        global CURRENT_STEP
        CURRENT_STEP += 1
    else:
        print(
            FORE_RED
            + SPLIT_LINE
            + "[ "
            + datetime.strftime(datetime.now(), "%b %d %H:%M:%S")
            + " | "
            + "%-*s" % (align, "TEST TASK ERROR")
            + " ]"
            + SPLIT_LINE
            + "\033[0m"
        )
    print(
        "\r\n"
        + FORE_BLUE
        + SPLIT_LINE
        + "[ "
        + datetime.strftime(datetime.now(), "%b %d %H:%M:%S")
        + " | TEST TASK: "
        + "%-10s" % command
        + " | FINISH ]"
        + SPLIT_LINE
        + "\033[0m"
    )

    if CURRENT_STEP != WHOLE_STEP:
        statusColor = FORE_YELLOW
    else:
        statusColor = FORE_GREEN
    print(
        statusColor
        + SPLIT_LINE
        + "[ "
        + datetime.strftime(datetime.now(), "%b %d %H:%M:%S")
        + " | "
        + "%-*s"
        % (
            align,
            (
                "TEST TASK COMPLETION: "
                + "%2s" % str(CURRENT_STEP)
                + " / "
                + "%2s" % str(WHOLE_STEP)
            ),
        )
        + " ]"
        + SPLIT_LINE
        + "\033[0m\n"
    )
    sys.stdout = STDOUT_DEFAULT
    drawProgressBar(int(CURRENT_STEP / WHOLE_STEP * 100))
    sys.stdout = STDOUT_LOG


def parseArgs():
    parser = argparse.ArgumentParser(description="test script")
    parser.add_argument(
        "--build",
        "-b",
        nargs="?",
        choices=[
            "default",
            "cleanup",
            "format",
            "analysis",
            "html",
            "backup",
            "tag",
            "release",
            "help",
        ],
        const="default",
        help="test with build.sh",
    )
    parser.add_argument(
        "--valgrind", "-v", action="store_true", default=False, help="test with valgrind"
    )
    args = parser.parse_args()

    if args.build:
        if os.path.isfile(BUILD_CMD):
            if args.build == "default":
                buildProject(BUILD_CMD + " 2>&1")
            else:
                buildProject(BUILD_CMD + " --" + args.build + " 2>&1")
                sys.exit(0)
        else:
            printAbort("There is no shell script build.sh.")
    if args.valgrind:
        cmd = tryCommand("command -v valgrind 2>&1")
        output = cmd.stdout.read()
        if output.find("valgrind") != -1:
            global SET_VALGRIND
            SET_VALGRIND = True
        else:
            printAbort("There is no valgrind program. Please check it.")


def prepareTest():
    os.chdir(os.path.split(os.path.realpath(__file__))[0])
    os.chdir("..")
    parseArgs()
    if not os.path.isfile(RUN_DIR + RUN_CMD):
        printAbort("There is no executable file. Please build it.")
    if not os.path.exists(TEMP_PATH):
        os.makedirs(TEMP_PATH)

    setupProgressBar()
    sys.stdout = Log(TEMP_LOG, sys.stdout)
    global STDOUT_LOG
    STDOUT_LOG = sys.stdout


def completeTest():
    sys.stdout = STDOUT_DEFAULT
    destroyProgressBar()
    sys.stdout = STDOUT_LOG
    sys.stdout.uninit()


def testOptionType1():
    executeCommand(RUN_CMD)
    for each in OPTION_TYPE_1:
        executeCommand(RUN_CMD + " " + each)
    executeCommand(RUN_CMD + " " + " ".join(OPTION_TYPE_1))


def testOptimum():
    for each in OPTIMUM:
        executeCommand(RUN_CMD + " " + OPTION_TYPE_1[0] + " " + each)
    executeCommand(RUN_CMD + " " + OPTION_TYPE_1[0] + " " + " ".join(OPTIMUM))


def testIntegral():
    for each in INTEGRAL:
        executeCommand(RUN_CMD + " " + OPTION_TYPE_1[1] + " " + each)
    executeCommand(RUN_CMD + " " + OPTION_TYPE_1[1] + " " + " ".join(INTEGRAL))


def testSort():
    for each in SORT:
        executeCommand(RUN_CMD + " " + OPTION_TYPE_1[2] + " " + each)
    executeCommand(RUN_CMD + " " + OPTION_TYPE_1[2] + " " + " ".join(SORT))


def testOptionType2():
    for each in OPTION_TYPE_2:
        executeCommand(RUN_CMD + " " + each)


if __name__ == "__main__":
    prepareTest()

    testOptionType1()
    testOptimum()
    testIntegral()
    testSort()
    testOptionType2()

    completeTest()
