#!/usr/bin/env python3

import argparse
import os
import re
import sys
from datetime import datetime
import common

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


def printAbort(message):
    print(f"Python script test.py: {message}")
    sys.exit(-1)


def buildProject(command):
    cmd = common.executeCommand(command, output=False)
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
    cmd = common.executeCommand(fullCommand, output=False)
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
    common.drawProgressBar(int(CURRENT_STEP / WHOLE_STEP * 100))
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
            cmd = common.executeCommand(f"{COMMAND_DESCRIPTION_CMD} valgrind 2>&1", output=False)
            out = cmd.stdout.read()
            if out.find("valgrind") != -1:
                global CHECK_SET_VALGRIND
                CHECK_SET_VALGRIND = True
            else:
                printAbort("There is no valgrind program. Please check it.")
        if "coverage" in args.check:
            cmd1 = common.executeCommand(
                f"{COMMAND_DESCRIPTION_CMD} llvm-profdata-12 2>&1", output=False
            )
            out1 = cmd1.stdout.read()
            cmd2 = common.executeCommand(
                f"{COMMAND_DESCRIPTION_CMD} llvm-cov-12 2>&1", output=False
            )
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
    filePath = os.path.split(os.path.realpath(__file__))[0]
    os.chdir(filePath.replace(filePath[filePath.index("script") :], ''))

    parseArgs()
    if not os.path.isfile(f"{RUN_DIR}{RUN_CMD}"):
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
    sys.stdout = STDOUT_LOG
    sys.stdout.uninit()

    if CHECK_SET_COVERAGE:
        common.executeCommand(
            f"{CHECK_COVERAGE_PROFDATA_CMD} {TEMP_PATH}/foo_*.profraw -o {TEMP_PATH}/foo.profdata"
        )
        cmd = common.executeCommand(
            f"{CHECK_COVERAGE_COV_CMD} {RUN_DIR}{RUN_CMD} -instr-profile={TEMP_PATH}/foo.profdata \
2>&1",
            output=False,
        )
        common.executeCommand(f"rm -rf {TEMP_PATH}/*.profraw")
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
