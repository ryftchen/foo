#!/usr/bin/env python3

try:
    import argparse
    import os
    import re
    import sys
    from datetime import datetime
    import common
except ImportError as err:
    raise ImportError(err)

BIN_CMD = "foo"
BIN_DIR = "./build/bin/"
LIB_LIST = ["libutility.so", "libalgorithm.so"]
LIB_DIR = "./build/lib/"
OPTION_UTIL_TYPE = ["--help", "--version", "--console"]
UTIL_CONSOLE_COMMAND = ["help", "quit", "run ./script/batch.txt", "log"]
OPTION_ALGO_TYPE = ["--algorithm", ("optimum", "integral", "sort")]
ALGO_OPTIMUM_METHOD = ["fib", "gra", "ann", "par", "gen"]
ALGO_INTEGRAL_METHOD = ["tra", "sim", "rom", "gau", "mon"]
ALGO_SORT_METHOD = ["bub", "sel", "ins", "she", "mer", "qui", "hea", "cou", "buc", "rad"]
PASS_STEP = 0
COMPLETE_STEP = 0
WHOLE_STEP = (
    (1 + len(OPTION_UTIL_TYPE))
    + len(UTIL_CONSOLE_COMMAND)
    + len(OPTION_ALGO_TYPE[1])
    + (len(ALGO_OPTIMUM_METHOD) + 1)
    + (len(ALGO_INTEGRAL_METHOD) + 1)
    + (len(ALGO_SORT_METHOD) + 1)
)
TEMP_LOG = "./temp/foo_test.log"
TEMP_PATH = "./temp"

ARG_CHECK_MEMORY_STATE = False
ARG_CHECK_MEMORY_SUM = "ERROR SUMMARY: 0 errors from 0 contexts (suppressed: 0 from 0)"
ARG_CHECK_COVERAGE_STATE = False
ARG_BUILD_SHELL = "./script/build.sh"
ARG_BUILD_COMPILE_FINISH = "Built target foo"
ENV_COVERAGE = "CODE_COVERAGE"

PRINT_STATUS_RED = "\033[0;31;40m"
PRINT_STATUS_GREEN = "\033[0;32;40m"
PRINT_STATUS_YELLOW = "\033[0;33;40m"
PRINT_STATUS_BLUE = "\033[0;36;40m"
PRINT_STATUS_END = "\033[0m"
PRINT_STATUS_ESC_REGEX = r"((\033.*?m)|(\007))"
PRINT_STATUS_COLUMN_LENGTH = 10
PRINT_ALIGN_MAX = 30
PRINT_ALIGN_CMD_LEN = 10

STDOUT_DEFAULT = sys.stdout
STDOUT_LOG = sys.stdout


def printException(message):
    print(f"\r\nPython script test.py: {message}")
    sys.exit(-1)


def printStatus(color, context):
    print(
        "{0}{2}[ {3} | {4} ]{2}{1}".format(
            color,
            PRINT_STATUS_END,
            f"{'=' * PRINT_STATUS_COLUMN_LENGTH}",
            datetime.strftime(datetime.now(), "%b %d %H:%M:%S"),
            context,
        )
    )


def buildProject(command):
    stdout, stderr, errcode = common.executeCommand(command)
    if stderr or errcode != 0:
        printException(f"Failed to run shell script {ARG_BUILD_SHELL} in test.")
    else:
        print(stdout)
        if ARG_BUILD_COMPILE_FINISH not in stdout:
            printException(f"Failed to build project by shell script {ARG_BUILD_SHELL}.")


def runTestTask(command, enter=""):
    global PASS_STEP, COMPLETE_STEP
    fullCommand = f"{BIN_DIR}{command}"
    if ARG_CHECK_MEMORY_STATE:
        fullCommand = f"valgrind {fullCommand}"
    if ARG_CHECK_COVERAGE_STATE:
        fullCommand = (
            f"LLVM_PROFILE_FILE=\"{TEMP_PATH}/foo_{str(COMPLETE_STEP + 1)}.profraw\" {fullCommand}"
        )
    align = max(len(command) + (PRINT_ALIGN_MAX - PRINT_ALIGN_CMD_LEN), PRINT_ALIGN_MAX)
    printStatus(
        PRINT_STATUS_BLUE, "TEST CASE: {0:<{x}} | START ".format(command, x=PRINT_ALIGN_CMD_LEN)
    )

    stdout, stderr, errcode = common.executeCommand(fullCommand, enter)
    if stderr or errcode != 0:
        print(f"stderr: {stderr}\nerrcode: {errcode}")
        printStatus(PRINT_STATUS_RED, "{0:<{x}}".format("TEST CASE FAILURE", x=align))
    else:
        print(stdout)
        PASS_STEP += 1
        if ARG_CHECK_MEMORY_STATE and (ARG_CHECK_MEMORY_SUM not in stdout):
            PASS_STEP -= 1
            printStatus(PRINT_STATUS_RED, "{0:<{x}}".format("TEST CASE FAILURE", x=align))

    COMPLETE_STEP += 1
    printStatus(
        PRINT_STATUS_BLUE, "TEST CASE: {0:<{x}} | FINISH".format(command, x=PRINT_ALIGN_CMD_LEN)
    )

    if PASS_STEP != WHOLE_STEP:
        statusColor = PRINT_STATUS_YELLOW
    else:
        statusColor = PRINT_STATUS_GREEN
    printStatus(
        statusColor,
        "{0:<{x}}".format(
            "TEST CASE SUCCESS: {:>2} / {:>2}".format(str(PASS_STEP), str(WHOLE_STEP)), x=align
        ),
    )
    print("\n")

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
        help="test with check: coverage / memory",
    )
    parser.add_argument(
        "-b",
        "--build",
        choices=["debug", "release"],
        nargs="?",
        const="debug",
        help="test with build: debug / release",
    )
    args = parser.parse_args()

    if args.check:
        if "coverage" in args.check:
            stdout, _, _ = common.executeCommand("command -v llvm-profdata-12 llvm-cov-12 2>&1")
            if stdout.find("llvm-profdata-12") != -1 and stdout.find("llvm-cov-12") != -1:
                os.environ["FOO_ENV"] = ENV_COVERAGE
                global ARG_CHECK_COVERAGE_STATE
                ARG_CHECK_COVERAGE_STATE = True
            else:
                printException("No llvm-profdata or llvm-cov program. Please check it.")

        if "memory" in args.check:
            stdout, _, _ = common.executeCommand("command -v valgrind 2>&1")
            if stdout.find("valgrind") != -1:
                global ARG_CHECK_MEMORY_STATE
                ARG_CHECK_MEMORY_STATE = True
            else:
                printException("No valgrind program. Please check it.")

    if args.build:
        if os.path.isfile(ARG_BUILD_SHELL):
            if args.build == "debug":
                buildProject(f"{ARG_BUILD_SHELL} 2>&1")
            elif args.build == "release":
                buildProject(f"{ARG_BUILD_SHELL} --release 2>&1")
        else:
            printException("No shell script build.sh in script folder.")


def prepareTest():
    filePath = os.path.split(os.path.realpath(__file__))[0]
    os.chdir(filePath.replace(filePath[filePath.index("script") :], ''))

    parseArgs()
    if not os.path.isfile(f"{BIN_DIR}{BIN_CMD}"):
        printException("No executable file. Please build it.")
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
            f"llvm-profdata-12 merge -sparse {TEMP_PATH}/foo_*.profraw -o {TEMP_PATH}/foo.profdata"
        )
        stdout, _, _ = common.executeCommand(
            f"llvm-cov-12 report -instr-profile={TEMP_PATH}/foo.profdata \
-object={BIN_DIR}{BIN_CMD} "
            + ' '.join([f"-object={LIB_DIR}{lib}" for lib in LIB_LIST])
            + " 2>&1"
        )
        common.executeCommand(f"rm -rf {TEMP_PATH}/*.profraw")
        print(stdout)
        if "error" in stdout:
            printException("Please rebuild the executable file before use --check option.")


def analyzeTestLog():
    refresh = open(TEMP_LOG, "rt")
    inputContent = refresh.read()
    outputContent = re.sub(PRINT_STATUS_ESC_REGEX, "", inputContent)
    refresh = open(TEMP_LOG, "w")
    refresh.write(outputContent)


def testUtilTypeOption():
    runTestTask(BIN_CMD, UTIL_CONSOLE_COMMAND[1])
    for each in OPTION_UTIL_TYPE:
        runTestTask(f"{BIN_CMD} {each}")
    testConsoleCommand()


def testConsoleCommand():
    for each in UTIL_CONSOLE_COMMAND:
        runTestTask(f"{BIN_CMD} {OPTION_UTIL_TYPE[2]} \"{each}\"")


def testAlgoTypeOption():
    for each in OPTION_ALGO_TYPE[1]:
        runTestTask(f"{BIN_CMD} {OPTION_ALGO_TYPE[0]} {each}")
    testOptimumMethod()
    testIntegralMethod()
    testSortMethod()


def testOptimumMethod():
    for each in ALGO_OPTIMUM_METHOD:
        runTestTask(f"{BIN_CMD} {OPTION_ALGO_TYPE[0]} {OPTION_ALGO_TYPE[1][0]} {each}")
    runTestTask(
        f"{BIN_CMD} {OPTION_ALGO_TYPE[0]} {OPTION_ALGO_TYPE[1][0]} {' '.join(ALGO_OPTIMUM_METHOD)}"
    )


def testIntegralMethod():
    for each in ALGO_INTEGRAL_METHOD:
        runTestTask(f"{BIN_CMD} {OPTION_ALGO_TYPE[0]} {OPTION_ALGO_TYPE[1][1]} {each}")
    runTestTask(
        f"{BIN_CMD} {OPTION_ALGO_TYPE[0]} {OPTION_ALGO_TYPE[1][1]} {' '.join(ALGO_INTEGRAL_METHOD)}"
    )


def testSortMethod():
    for each in ALGO_SORT_METHOD:
        runTestTask(f"{BIN_CMD} {OPTION_ALGO_TYPE[0]} {OPTION_ALGO_TYPE[1][2]} {each}")
    runTestTask(
        f"{BIN_CMD} {OPTION_ALGO_TYPE[0]} {OPTION_ALGO_TYPE[1][2]} {' '.join(ALGO_SORT_METHOD)}"
    )


if __name__ == "__main__":
    prepareTest()

    testUtilTypeOption()
    testAlgoTypeOption()

    completeTest()
    analyzeTestLog()
