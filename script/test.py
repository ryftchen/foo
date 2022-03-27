#!/usr/bin/env python3

import argparse
import curses
import fcntl
import os
import re
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
OPTION_TYPE_2 = ["--optimum", "--integral", "--sort", "--log", "--help"]
CURRENT_STEP = 0
WHOLE_STEP = (
    (len(OPTION_TYPE_1) + 1)
    + (len(OPTIMUM) + 1)
    + (len(INTEGRAL) + 1)
    + (len(SORT) + 1)
    + (len(OPTION_TYPE_2) + 1)
)
TEMP_LOG = "./temp/foo_test.log"
TEMP_PATH = "./temp"
BUILD_CMD = "./script/build.sh"
BUILD_COMPILER_START = "Configuring done"
BUILD_COMPILER_FINISH = "Built target"
SET_VALGRIND = False
VALGRIND_CMD = "valgrind --tool=memcheck --show-reachable=yes --leak-check=full \
--leak-resolution=high --log-fd=1"
VALGRIND_INFO = "ERROR SUMMARY: 0 errors from 0 contexts (suppressed: 0 from 0)"
STATUS_RED = "\033[0;31;40m"
STATUS_GREEN = "\033[0;32;40m"
STATUS_YELLOW = "\033[0;33;40m"
STATUS_BLUE = "\033[0;36;40m"
STATUS_END = "\033[0m"
STATUS_ESC_REGEX = "\\033.*?m"
STATUS_SPLIT_LINE = "=========="
ALIGN_MAX = 30
ALIGN_CMD = 10

STDOUT_DEFAULT = sys.stdout
STDOUT_LOG = sys.stdout
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
    global BAR_CURRENT_LINES
    curses.setupterm()

    trapDueToInterrupt()

    BAR_CURRENT_LINES = tputLines()
    lines = BAR_CURRENT_LINES - 1
    printProgress("\n")

    printProgress(BAR_SAVE_CURSOR)
    printProgress("\033[0;{}r".format(str(lines)))

    printProgress(BAR_RESTORE_CURSOR)
    printProgress(BAR_MOVE_UP_CURSOR)
    drawProgressBar(0)


def drawProgressBar(percentage):
    lines = tputLines()
    if lines != BAR_CURRENT_LINES:
        setupProgressBar()

    printProgress(BAR_SAVE_CURSOR)
    printProgress("\033[{};0f".format(str(lines)))

    tput()
    printBar(percentage)
    printProgress(BAR_RESTORE_CURSOR)
    time.sleep(0.05)


def destroyProgressBar():
    lines = tputLines()
    printProgress(BAR_SAVE_CURSOR)
    printProgress("\033[0;{}r".format(str(lines)))

    printProgress(BAR_RESTORE_CURSOR)
    printProgress(BAR_MOVE_UP_CURSOR)

    clearProgressBar()
    printProgress("\n\n")
    if BAR_SET_TRAP:
        signal.signal(signal.SIGINT, BAR_SIGNAL_DEFAULT)


def clearProgressBar():
    lines = tputLines()
    printProgress(BAR_SAVE_CURSOR)
    printProgress("\033[{};0f".format(str(lines)))

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
    cmd = executeCommand("tput lines", output=False)
    out = cmd.stdout.read()
    return int(out)


def tputCols():
    cmd = executeCommand("tput cols", output=False)
    out = cmd.stdout.read()
    return int(out)


def tput():
    print(curses.tparm(curses.tigetstr("el")).decode(), end="")


def executeCommand(command, output=True):
    try:
        cmd = subprocess.Popen(
            command,
            shell=True,
            executable="/bin/bash",
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            encoding="utf-8",
        )
        cmd.wait()
    except RuntimeError():
        printAbort(f"Failed to execute command: \"{command}\".")
    if output:
        out, err = cmd.communicate()
        if len(out) != 0:
            print(out)
        if len(err) != 0:
            print(err)
    return cmd


def printAbort(message):
    print("Python script test.py: " + message)
    sys.exit(-1)


def buildProject(command):
    cmd = executeCommand(command, output=False)
    out = cmd.stdout.read()
    print(out)
    if cmd.returncode != 0:
        printAbort("Failed to execute shell script build.sh.")
    elif out.find(BUILD_COMPILER_START) != -1 and out.find(BUILD_COMPILER_FINISH) == -1:
        printAbort("Failed to build project by shell script build.sh.")


def runTestTask(command):
    fullCommand = RUN_DIR + command
    if SET_VALGRIND:
        fullCommand = VALGRIND_CMD + " " + fullCommand
    align = max(len(command) + (ALIGN_MAX - ALIGN_CMD), ALIGN_MAX)
    print(
        "\r\n{0}{2}[ {3} | TEST TASK: {4:<{x}} | START  ]{2}{1}\n".format(
            STATUS_BLUE,
            STATUS_END,
            STATUS_SPLIT_LINE,
            datetime.strftime(datetime.now(), "%b %d %H:%M:%S"),
            command,
            x=ALIGN_CMD,
        )
    )
    cmd = executeCommand(fullCommand, output=False)
    out = cmd.stdout.read()
    print(out)
    if cmd.returncode == 0:
        global CURRENT_STEP
        CURRENT_STEP += 1
        if SET_VALGRIND:
            if out.find(VALGRIND_INFO) == -1:
                CURRENT_STEP -= 1
                print(
                    "{0}{2}[ {3} | {4:<{x}} ]{2}{1}".format(
                        STATUS_RED,
                        STATUS_END,
                        STATUS_SPLIT_LINE,
                        datetime.strftime(datetime.now(), "%b %d %H:%M:%S"),
                        "TEST TASK ERROR",
                        x=align,
                    )
                )
    else:
        print(
            "{0}{2}[ {3} | {4:<{x}} ]{2}{1}".format(
                STATUS_RED,
                STATUS_END,
                STATUS_SPLIT_LINE,
                datetime.strftime(datetime.now(), "%b %d %H:%M:%S"),
                "TEST TASK ERROR",
                x=align,
            )
        )
    print(
        "\r\n{0}{2}[ {3} | TEST TASK: {4:<{x}} | FINISH ]{2}{1}\n".format(
            STATUS_BLUE,
            STATUS_END,
            STATUS_SPLIT_LINE,
            datetime.strftime(datetime.now(), "%b %d %H:%M:%S"),
            command,
            x=ALIGN_CMD,
        )
    )

    if CURRENT_STEP != WHOLE_STEP:
        statusColor = STATUS_YELLOW
    else:
        statusColor = STATUS_GREEN
    print(
        "{0}{2}[ {3} | {4:<{x}} ]{2}{1}\n".format(
            statusColor,
            STATUS_END,
            STATUS_SPLIT_LINE,
            datetime.strftime(datetime.now(), "%b %d %H:%M:%S"),
            "TEST TASK COMPLETION: {:>2} / {:>2}".format(str(CURRENT_STEP), str(WHOLE_STEP)),
            x=align,
        )
    )
    sys.stdout = STDOUT_DEFAULT
    drawProgressBar(int(CURRENT_STEP / WHOLE_STEP * 100))
    sys.stdout = STDOUT_LOG


def parseArgs():
    parser = argparse.ArgumentParser(description="test script")
    parser.add_argument(
        "-b",
        "--build",
        nargs="?",
        choices=["debug", "release"],
        const="debug",
        help="test with build.sh",
    )
    parser.add_argument(
        "-v", "--valgrind", action="store_true", default=False, help="test with valgrind"
    )
    args = parser.parse_args()

    if args.build:
        if os.path.isfile(BUILD_CMD):
            if args.build == "debug":
                buildProject(BUILD_CMD + " 2>&1")
            elif args.build == "release":
                buildProject(BUILD_CMD + " --release 2>&1")
        else:
            printAbort("There is no shell script build.sh.")
    if args.valgrind:
        cmd = executeCommand("command -v valgrind 2>&1", output=False)
        out = cmd.stdout.read()
        if out.find("valgrind") != -1:
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


def analyzeTestLog():
    refresh = open(TEMP_LOG, "rt")
    inputContent = refresh.read()
    outputContent = re.sub(STATUS_ESC_REGEX, "", inputContent)
    refresh = open(TEMP_LOG, "w")
    refresh.write(outputContent)


def testOptionType1():
    for each in OPTION_TYPE_1:
        runTestTask(RUN_CMD + " " + each)
    runTestTask(RUN_CMD + " " + " ".join(OPTION_TYPE_1))


def testOptimum():
    for each in OPTIMUM:
        runTestTask(RUN_CMD + " " + OPTION_TYPE_1[0] + " " + each)
    runTestTask(RUN_CMD + " " + OPTION_TYPE_1[0] + " " + " ".join(OPTIMUM))


def testIntegral():
    for each in INTEGRAL:
        runTestTask(RUN_CMD + " " + OPTION_TYPE_1[1] + " " + each)
    runTestTask(RUN_CMD + " " + OPTION_TYPE_1[1] + " " + " ".join(INTEGRAL))


def testSort():
    for each in SORT:
        runTestTask(RUN_CMD + " " + OPTION_TYPE_1[2] + " " + each)
    runTestTask(RUN_CMD + " " + OPTION_TYPE_1[2] + " " + " ".join(SORT))


def testOptionType2():
    runTestTask(RUN_CMD)
    for each in OPTION_TYPE_2:
        runTestTask(RUN_CMD + " " + each)


if __name__ == "__main__":
    prepareTest()

    testOptionType1()
    testOptimum()
    testIntegral()
    testSort()
    testOptionType2()

    completeTest()
    analyzeTestLog()
