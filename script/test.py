#!/usr/bin/env python3

import argparse
import os
import re
import sys
from datetime import datetime
import common

BIN_CMD = "foo"
BIN_DIR = "./build/bin/"
LIB_LIST = ["libutility.so", "libalgorithm.so"]
LIB_DIR = "./build/lib/"
OPTION_UTIL_TYPE = ["--help", "--version", "--console"]
CONSOLE_COMMAND = ["help", "quit", "run ./script/batch.txt", "log"]
OPTION_ALG_TYPE = ["--optimum", "--integral", "--sort"]
OPTIMUM_METHOD = ["fib", "gra", "ann", "par", "gen"]
INTEGRAL_METHOD = ["tra", "sim", "rom", "gau", "mon"]
SORT_METHOD = ["bub", "sel", "ins", "she", "mer", "qui", "hea", "cou", "buc", "rad"]
PASS_STEP = 0
COMPLETE_STEP = 0
WHOLE_STEP = (
    +len(OPTION_UTIL_TYPE)
    + (len(CONSOLE_COMMAND) + 1)
    + len(OPTION_ALG_TYPE)
    + (len(OPTIMUM_METHOD) + 1)
    + (len(INTEGRAL_METHOD) + 1)
    + (len(SORT_METHOD) + 1)
)
TEMP_LOG = "./temp/foo_test.log"
TEMP_PATH = "./temp"

ARG_COMMAND_DEPEND = "command -v"
ARG_CHECK_MEMORY_STATE = False
ARG_CHECK_MEMORY_CMD = "valgrind"
ARG_CHECK_MEMORY_SUM = "ERROR SUMMARY: 0 errors from 0 contexts (suppressed: 0 from 0)"
ARG_CHECK_COVERAGE_STATE = False
ARG_CHECK_COVERAGE_CMD = "LLVM_PROFILE_FILE"
ARG_CHECK_COVERAGE_PROFDATA_CMD = "llvm-profdata-12 merge -sparse"
ARG_CHECK_COVERAGE_COV_CMD = "llvm-cov-12 report"
ARG_BUILD_SHELL = "./script/build.sh"
ARG_BUILD_COMPILE_FINISH = "Built target foo"

PRINT_STATUS_RED = "\033[0;31;40m"
PRINT_STATUS_GREEN = "\033[0;32;40m"
PRINT_STATUS_YELLOW = "\033[0;33;40m"
PRINT_STATUS_BLUE = "\033[0;36;40m"
PRINT_STATUS_END = "\033[0m"
PRINT_STATUS_ESC_REGEX = r"(\033\[0.*?m)"
PRINT_STATUS_SPLIT_LINE = "=========="
PRINT_ALIGN_MAX = 30
PRINT_ALIGN_CMD_LEN = 10

STDOUT_DEFAULT = sys.stdout
STDOUT_LOG = sys.stdout


def printAbort(message):
    print(f"Python script test.py: {message}")
    sys.exit(-1)


def buildProject(command):
    stdout, stderr, errcode = common.executeCommand(command)
    if stderr or errcode != 0:
        printAbort(f"Failed to execute shell script {ARG_BUILD_SHELL} in test.")
    else:
        print(stdout)
        if stdout.find(ARG_BUILD_COMPILE_FINISH) == -1:
            printAbort(f"Failed to build project by shell script {ARG_BUILD_SHELL}.")


def runTestTask(command, enter=""):
    global PASS_STEP, COMPLETE_STEP
    fullCommand = f"{BIN_DIR}{command}"
    if ARG_CHECK_MEMORY_STATE:
        fullCommand = f"{ARG_CHECK_MEMORY_CMD} {fullCommand}"
    if ARG_CHECK_COVERAGE_STATE:
        fullCommand = f"{ARG_CHECK_COVERAGE_CMD}\
=\"{TEMP_PATH}/foo_{str(COMPLETE_STEP + 1)}.profraw\" {fullCommand}"
    align = max(len(command) + (PRINT_ALIGN_MAX - PRINT_ALIGN_CMD_LEN), PRINT_ALIGN_MAX)
    print(
        "\r\n{0}{2}[ {3} | TEST CASE: {4:<{x}} | START  ]{2}{1}\n".format(
            PRINT_STATUS_BLUE,
            PRINT_STATUS_END,
            PRINT_STATUS_SPLIT_LINE,
            datetime.strftime(datetime.now(), "%b %d %H:%M:%S"),
            command,
            x=PRINT_ALIGN_CMD_LEN,
        )
    )

    stdout, stderr, errcode = common.executeCommand(fullCommand, enter)
    if stderr or errcode != 0:
        print(f"stderr: {stderr}\nerrcode: {errcode}")
        print(
            "{0}{2}[ {3} | {4:<{x}} ]{2}{1}".format(
                PRINT_STATUS_RED,
                PRINT_STATUS_END,
                PRINT_STATUS_SPLIT_LINE,
                datetime.strftime(datetime.now(), "%b %d %H:%M:%S"),
                "TEST CASE FAILURE",
                x=align,
            )
        )
    else:
        print(stdout)
        PASS_STEP += 1
        if ARG_CHECK_MEMORY_STATE:
            if stdout.find(ARG_CHECK_MEMORY_SUM) == -1:
                PASS_STEP -= 1
                print(
                    "{0}{2}[ {3} | {4:<{x}} ]{2}{1}".format(
                        PRINT_STATUS_RED,
                        PRINT_STATUS_END,
                        PRINT_STATUS_SPLIT_LINE,
                        datetime.strftime(datetime.now(), "%b %d %H:%M:%S"),
                        "TEST CASE FAILURE",
                        x=align,
                    )
                )

    COMPLETE_STEP += 1
    print(
        "\r\n{0}{2}[ {3} | TEST CASE: {4:<{x}} | FINISH ]{2}{1}\n".format(
            PRINT_STATUS_BLUE,
            PRINT_STATUS_END,
            PRINT_STATUS_SPLIT_LINE,
            datetime.strftime(datetime.now(), "%b %d %H:%M:%S"),
            command,
            x=PRINT_ALIGN_CMD_LEN,
        )
    )

    if PASS_STEP != WHOLE_STEP:
        statusColor = PRINT_STATUS_YELLOW
    else:
        statusColor = PRINT_STATUS_GREEN
    print(
        "{0}{2}[ {3} | {4:<{x}} ]{2}{1}\n".format(
            statusColor,
            PRINT_STATUS_END,
            PRINT_STATUS_SPLIT_LINE,
            datetime.strftime(datetime.now(), "%b %d %H:%M:%S"),
            "TEST CASE SUCCESS: {:>2} / {:>2}".format(str(PASS_STEP), str(WHOLE_STEP)),
            x=align,
        )
    )
    sys.stdout = STDOUT_DEFAULT
    common.drawProgressBar(int(COMPLETE_STEP / WHOLE_STEP * 100))
    sys.stdout = STDOUT_LOG


def parseArgs():
    parser = argparse.ArgumentParser(description="test script")
    parser.add_argument(
        "-c",
        "--check",
        choices=["coverage", "memory"],
        nargs="+",
        help="test with check: coverage check or memory check",
    )
    parser.add_argument(
        "-b",
        "--build",
        choices=["debug", "release"],
        nargs="?",
        const="debug",
        help="test with build: debug version or release version",
    )
    args = parser.parse_args()

    if args.check:
        if "coverage" in args.check:
            stdout, _, _ = common.executeCommand(
                f"{ARG_COMMAND_DEPEND} llvm-profdata-12 llvm-cov-12 2>&1"
            )
            if stdout.find("llvm-profdata-12") != -1 and stdout.find("llvm-cov-12") != -1:
                os.environ["FOO_ENV"] = "CODE_COVERAGE"
                global ARG_CHECK_COVERAGE_STATE
                ARG_CHECK_COVERAGE_STATE = True
            else:
                printAbort("There is no llvm-profdata or llvm-cov program. Please check it.")

        if "memory" in args.check:
            stdout, _, _ = common.executeCommand(f"{ARG_COMMAND_DEPEND} valgrind 2>&1")
            if stdout.find("valgrind") != -1:
                global ARG_CHECK_MEMORY_STATE
                ARG_CHECK_MEMORY_STATE = True
            else:
                printAbort("There is no valgrind program. Please check it.")

    if args.build:
        if os.path.isfile(ARG_BUILD_SHELL):
            if args.build == "debug":
                buildProject(f"{ARG_BUILD_SHELL} 2>&1")
            elif args.build == "release":
                buildProject(f"{ARG_BUILD_SHELL} --release 2>&1")
        else:
            printAbort("There is no shell script build.sh in script folder.")


def prepareTest():
    filePath = os.path.split(os.path.realpath(__file__))[0]
    os.chdir(filePath.replace(filePath[filePath.index("script") :], ''))

    parseArgs()
    if not os.path.isfile(f"{BIN_DIR}{BIN_CMD}"):
        printAbort("There is no executable file. Please build it.")
    if not os.path.exists(TEMP_PATH):
        os.makedirs(TEMP_PATH)

    common.setupProgressBar()
    sys.stdout = common.Log(TEMP_LOG, sys.stdout)
    global STDOUT_LOG
    STDOUT_LOG = sys.stdout


def completeTest():
    sys.stdout = STDOUT_DEFAULT
    common.destroyProgressBar()
    global STDOUT_LOG
    del STDOUT_LOG

    if ARG_CHECK_COVERAGE_STATE:
        common.executeCommand(
            f"{ARG_CHECK_COVERAGE_PROFDATA_CMD} {TEMP_PATH}/foo_*.profraw \
-o {TEMP_PATH}/foo.profdata"
        )
        stdout, _, _ = common.executeCommand(
            f"{ARG_CHECK_COVERAGE_COV_CMD} -instr-profile={TEMP_PATH}/foo.profdata \
-object={BIN_DIR}{BIN_CMD} "
            + ' '.join([f"-object={LIB_DIR}{lib}" for lib in LIB_LIST])
            + " 2>&1"
        )
        common.executeCommand(f"rm -rf {TEMP_PATH}/*.profraw")
        print(stdout)
        if stdout.find("error") != -1:
            printAbort("Please rebuild the executable file before use the coverage option.")


def analyzeTestLog():
    refresh = open(TEMP_LOG, "rt")
    inputContent = refresh.read()
    outputContent = re.sub(PRINT_STATUS_ESC_REGEX, "", inputContent)
    refresh = open(TEMP_LOG, "w")
    refresh.write(outputContent)


def testUtilTypeOption():
    for each in OPTION_UTIL_TYPE:
        runTestTask(f"{BIN_CMD} {each}")


def testConsoleCommand():
    runTestTask(BIN_CMD, CONSOLE_COMMAND[1])
    for each in CONSOLE_COMMAND:
        runTestTask(f"{BIN_CMD} {OPTION_UTIL_TYPE[2]} \"{each}\"")


def testAlgTypeOption():
    for each in OPTION_ALG_TYPE:
        runTestTask(f"{BIN_CMD} {each}")


def testOptimumMethod():
    for each in OPTIMUM_METHOD:
        runTestTask(f"{BIN_CMD} {OPTION_ALG_TYPE[0]} {each}")
    runTestTask(f"{BIN_CMD} {OPTION_ALG_TYPE[0]} {' '.join(OPTIMUM_METHOD)}")


def testIntegralMethod():
    for each in INTEGRAL_METHOD:
        runTestTask(f"{BIN_CMD} {OPTION_ALG_TYPE[1]} {each}")
    runTestTask(f"{BIN_CMD} {OPTION_ALG_TYPE[1]} {' '.join(INTEGRAL_METHOD)}")


def testSortMethod():
    for each in SORT_METHOD:
        runTestTask(f"{BIN_CMD} {OPTION_ALG_TYPE[2]} {each}")
    runTestTask(f"{BIN_CMD} {OPTION_ALG_TYPE[2]} {' '.join(SORT_METHOD)}")


if __name__ == "__main__":
    prepareTest()

    testUtilTypeOption()
    testConsoleCommand()

    testAlgTypeOption()
    testOptimumMethod()
    testIntegralMethod()
    testSortMethod()

    completeTest()
    analyzeTestLog()
