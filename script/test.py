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
    len(OPTION_TYPE_1)
    + (len(OPTIMUM) + 1)
    + (len(INTEGRAL) + 1)
    + (len(SORT) + 1)
    + len(OPTION_TYPE_2)
    + 1
)
TEMP_LOG = "./temp/foo_test.log"
TEMP_PATH = "./temp"
COMMAND_DESCRIPTION_CMD = "command -v"
CHECK_SET_VALGRIND = False
CHECK_VALGRIND_CMD = "valgrind --tool=memcheck --show-reachable=yes --leak-check=full \
--leak-resolution=high --log-fd=1"
CHECK_VALGRIND_INFO = "ERROR SUMMARY: 0 errors from 0 contexts (suppressed: 0 from 0)"
CHECK_SET_COVERAGE = False
CHECK_COVERAGE_CMD = "LLVM_PROFILE_FILE"
CHECK_COVERAGE_PROFDATA_CMD = "llvm-profdata-12 merge -sparse"
CHECK_COVERAGE_COV_CMD = "llvm-cov-12 report"
BUILD_SHELL = "./script/build.sh"
BUILD_COMPILE_START = "Configuring done"
BUILD_COMPILE_FINISH = "Built target"
STATUS_RED = "\033[0;31;40m"
STATUS_GREEN = "\033[0;32;40m"
STATUS_YELLOW = "\033[0;33;40m"
STATUS_BLUE = "\033[0;36;40m"
STATUS_END = "\033[0m"
STATUS_ESC_REGEX = r"(\033\[0.*?m)"
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
            printAbort(f"Failed to write log file {TEMP_LOG}.")

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
    print(f"Python script test.py: {message}")
    sys.exit(-1)


def buildProject(command):
    cmd = executeCommand(command, output=False)
    out = cmd.stdout.read()
    print(out)
    if cmd.returncode != 0:
        printAbort(f"Failed to execute shell script {BUILD_SHELL}")
    elif out.find(BUILD_COMPILE_START) != -1 and out.find(BUILD_COMPILE_FINISH) == -1:
        printAbort(f"Failed to build project by shell script {BUILD_SHELL}.")


def runTestTask(command):
    global CURRENT_STEP
    fullCommand = f"{RUN_DIR}{command}"
    if CHECK_SET_VALGRIND:
        fullCommand = f"{CHECK_VALGRIND_CMD} {fullCommand}"
    if CHECK_SET_COVERAGE:
        fullCommand = f"{CHECK_COVERAGE_CMD}=\"{TEMP_PATH}/foo_{str(CURRENT_STEP + 1)}.profraw\" \
{fullCommand}"
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
        CURRENT_STEP += 1
        if CHECK_SET_VALGRIND:
            if out.find(CHECK_VALGRIND_INFO) == -1:
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
        "-c", "--check", choices=["valgrind", "coverage"], nargs="+", help="test with check"
    )
    parser.add_argument(
        "-b",
        "--build",
        choices=["debug", "release"],
        nargs="?",
        const="debug",
        help="test with build",
    )
    args = parser.parse_args()

    if args.check:
        if "valgrind" in args.check:
            cmd = executeCommand(f"{COMMAND_DESCRIPTION_CMD} valgrind 2>&1", output=False)
            out = cmd.stdout.read()
            if out.find("valgrind") != -1:
                global CHECK_SET_VALGRIND
                CHECK_SET_VALGRIND = True
            else:
                printAbort("There is no valgrind program. Please check it.")
        if "coverage" in args.check:
            cmd1 = executeCommand(f"{COMMAND_DESCRIPTION_CMD} llvm-profdata-12 2>&1", output=False)
            out1 = cmd1.stdout.read()
            cmd2 = executeCommand(f"{COMMAND_DESCRIPTION_CMD} llvm-cov-12 2>&1", output=False)
            out2 = cmd2.stdout.read()
            if out1.find("llvm-profdata-12") != -1 and out2.find("llvm-cov-12") != -1:
                os.environ["FOO_ENV"] = "CODE_COVERAGE"
                global CHECK_SET_COVERAGE
                CHECK_SET_COVERAGE = True
            else:
                printAbort("There is no llvm-profdata or llvm-cov program. Please check it.")
    if args.build:
        if os.path.isfile(BUILD_SHELL):
            if args.build == "debug":
                buildProject(f"{BUILD_SHELL} 2>&1")
            elif args.build == "release":
                buildProject(f"{BUILD_SHELL} --release 2>&1")
        else:
            printAbort("There is no shell script build.sh.")


def prepareTest():
    os.chdir(os.path.split(os.path.split(os.path.realpath(__file__))[0])[0])
    parseArgs()
    if not os.path.isfile(f"{RUN_DIR}{RUN_CMD}"):
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

    if CHECK_SET_COVERAGE:
        executeCommand(
            f"{CHECK_COVERAGE_PROFDATA_CMD} {TEMP_PATH}/foo_*.profraw -o {TEMP_PATH}/foo.profdata"
        )
        cmd = executeCommand(
            f"{CHECK_COVERAGE_COV_CMD} {RUN_DIR}{RUN_CMD} -instr-profile={TEMP_PATH}/foo.profdata \
2>&1",
            output=False,
        )
        executeCommand(f"rm -rf {TEMP_PATH}/*.profraw")
        out = cmd.stdout.read()
        print(out)
        if out.find("error") != -1:
            printAbort("Please rebuild the executable file before use the coverage option.")


def analyzeTestLog():
    refresh = open(TEMP_LOG, "rt")
    inputContent = refresh.read()
    outputContent = re.sub(STATUS_ESC_REGEX, "", inputContent)
    refresh = open(TEMP_LOG, "w")
    refresh.write(outputContent)


def testOptionType1():
    runTestTask(RUN_CMD)
    for each in OPTION_TYPE_1:
        runTestTask(f"{RUN_CMD} {each}")


def testOptimum():
    for each in OPTIMUM:
        runTestTask(f"{RUN_CMD} {OPTION_TYPE_1[0]} {each}")
    runTestTask(f"{RUN_CMD} {OPTION_TYPE_1[0]} {' '.join(OPTIMUM)}")


def testIntegral():
    for each in INTEGRAL:
        runTestTask(f"{RUN_CMD} {OPTION_TYPE_1[1]} {each}")
    runTestTask(f"{RUN_CMD} {OPTION_TYPE_1[1]} {' '.join(INTEGRAL)}")


def testSort():
    for each in SORT:
        runTestTask(f"{RUN_CMD} {OPTION_TYPE_1[2]} {each}")
    runTestTask(f"{RUN_CMD} {OPTION_TYPE_1[2]} {' '.join(SORT)}")


def testOptionType2():
    for each in OPTION_TYPE_2:
        runTestTask(f"{RUN_CMD} {each}")


if __name__ == "__main__":
    prepareTest()

    testOptionType1()
    testOptimum()
    testIntegral()
    testSort()
    testOptionType2()

    completeTest()
    analyzeTestLog()
