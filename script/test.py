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

STDOUT = sys.stdout


class Output:
    colorRed = "\033[0;31;40m"
    colorGreen = "\033[0;32;40m"
    colorYellow = "\033[0;33;40m"
    colorBlue = "\033[0;36;40m"
    colorEnd = "\033[0m"
    regexColorEsc = r"((\033.*?m)|(\007))"
    columnLength = 10
    alignMaxLen = 30
    alignCmdLen = 10

    @classmethod
    def printException(cls, message):
        print(f"\r\nPython script test.py: {message}")
        sys.exit(-1)

    @classmethod
    def printStatus(cls, color, content):
        print(
            "{0}{2}[ {3} | {4} ]{2}{1}".format(
                color,
                cls.colorEnd,
                f"{'=' * cls.columnLength}",
                datetime.strftime(datetime.now(), "%b %d %H:%M:%S"),
                content,
            )
        )


class Test:
    binCmd = "foo"
    binDir = "./build/bin/"
    libList = ["libutility.so", "libalgorithm.so"]
    libDir = "./build/lib/"
    basicTaskType = ["--help", "--version", "--console"]
    basicConsoleCmd = ["help", "quit", "run ./script/batch.txt", "log"]
    algoTaskType = ["--algorithm", ("optimum", "integral", "sort")]
    algoOptimumMethod = ["fib", "gra", "ann", "par", "gen"]
    algoIntegralMethod = ["tra", "sim", "rom", "gau", "mon"]
    algoSortMethod = ["bub", "sel", "ins", "she", "mer", "qui", "hea", "cou", "buc", "rad"]
    passStep = 0
    completeStep = 0
    totalStep = (
        (1 + len(basicTaskType))
        + len(basicConsoleCmd)
        + len(algoTaskType[1])
        + (len(algoOptimumMethod) + 1)
        + (len(algoIntegralMethod) + 1)
        + (len(algoSortMethod) + 1)
    )
    isCheckCoverage = False
    envCoverage = "CODE_COVERAGE"
    isCheckMemory = False
    memorySummaryContent = "ERROR SUMMARY: 0 errors from 0 contexts (suppressed: 0 from 0)"
    buildFile = "./script/build.sh"
    buildSummaryContent = "Built target foo"
    logFile = "./temp/foo_test.log"
    tempDir = "./temp"

    def __init__(self):
        self.log = common.Log(self.logFile)
        self.progressBar = common.ProgressBar()

    def run(self):
        self.prepareTest()

        self.testBasicTask()
        self.testAlgoTask()

        self.completeTest()
        self.analyzeTestLog()

    def runTestTask(self, command, enter=""):
        fullCommand = f"{self.binDir}{command}"
        if self.isCheckMemory:
            fullCommand = f"valgrind {fullCommand}"
        if self.isCheckCoverage:
            fullCommand = f"LLVM_PROFILE_FILE=\"{self.tempDir}/foo_{str(self.completeStep + 1)}.profraw\" {fullCommand}"
        align = max(len(command) + (Output.alignMaxLen - Output.alignCmdLen), Output.alignMaxLen)
        Output.printStatus(Output.colorBlue, "TEST CASE: {0:<{x}} | START ".format(command, x=Output.alignCmdLen))

        stdout, stderr, errcode = common.executeCommand(fullCommand, enter)
        if stderr or errcode != 0:
            print(f"stderr: {stderr}\nerrcode: {errcode}")
            Output.printStatus(Output.colorRed, "{0:<{x}}".format("TEST CASE FAILURE", x=align))
        else:
            print(stdout)
            self.passStep += 1
            if self.isCheckMemory and (self.memorySummaryContent not in stdout):
                self.passStep -= 1
                Output.printStatus(Output.colorRed, "{0:<{x}}".format("TEST CASE FAILURE", x=align))

        self.completeStep += 1
        Output.printStatus(Output.colorBlue, "TEST CASE: {0:<{x}} | FINISH".format(command, x=Output.alignCmdLen))

        if self.passStep != self.totalStep:
            statusColor = Output.colorYellow
        else:
            statusColor = Output.colorGreen
        Output.printStatus(
            statusColor,
            "{0:<{x}}".format(
                "TEST CASE SUCCESS: {:>2} / {:>2}".format(str(self.passStep), str(self.totalStep)), x=align
            ),
        )
        print("\n")

        global STDOUT
        sys.stdout = STDOUT
        self.progressBar.drawProgressBar(int(self.completeStep / self.totalStep * 100))
        sys.stdout = self.log

    def parseArgs(self):
        parser = argparse.ArgumentParser(description="test script")
        parser.add_argument(
            "-c", "--check", choices=["coverage", "memory"], nargs="+", help="test with check: coverage / memory"
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
                    os.environ["FOO_ENV"] = self.envCoverage
                    self.isCheckCoverage = True
                else:
                    Output.printException("No llvm-profdata or llvm-cov program. Please check it.")

            if "memory" in args.check:
                stdout, _, _ = common.executeCommand("command -v valgrind 2>&1")
                if stdout.find("valgrind") != -1:
                    self.isCheckMemory = True
                else:
                    Output.printException("No valgrind program. Please check it.")

        if args.build:
            if os.path.isfile(self.buildFile):
                if args.build == "debug":
                    self.buildProject(f"{self.buildFile} 2>&1")
                elif args.build == "release":
                    self.buildProject(f"{self.buildFile} --release 2>&1")
            else:
                Output.printException("No shell script build.sh in script folder.")

    def buildProject(self, command):
        stdout, stderr, errcode = common.executeCommand(command)
        if stderr or errcode != 0:
            Output.printException(f"Failed to run shell script {self.buildFile} in test.")
        else:
            print(stdout)
            if self.buildSummaryContent not in stdout:
                Output.printException(f"Failed to build project by shell script {self.buildFile}.")

    def prepareTest(self):
        filePath = os.path.split(os.path.realpath(__file__))[0]
        os.chdir(filePath.replace(filePath[filePath.index("script") :], ''))

        self.parseArgs()
        if not os.path.isfile(f"{self.binDir}{self.binCmd}"):
            Output.printException("No executable file. Please build it.")
        if not os.path.exists(self.tempDir):
            os.makedirs(self.tempDir)

        self.progressBar.setupProgressBar()
        sys.stdout = self.log

    def completeTest(self):
        global STDOUT
        sys.stdout = STDOUT
        self.progressBar.destroyProgressBar()
        del self.log

        if self.isCheckCoverage:
            common.executeCommand(
                f"llvm-profdata-12 merge -sparse {self.tempDir}/foo_*.profraw -o {self.tempDir}/foo.profdata"
            )
            stdout, _, _ = common.executeCommand(
                f"llvm-cov-12 report -instr-profile={self.tempDir}/foo.profdata -object={self.binDir}{self.binCmd} "
                + ' '.join([f"-object={self.libDir}{lib}" for lib in self.libList])
                + " 2>&1"
            )
            common.executeCommand(f"rm -rf {self.tempDir}/*.profraw")
            print(stdout)
            if "error" in stdout:
                Output.printException("Please rebuild the executable file before use --check option.")

    def analyzeTestLog(self):
        refresh = open(self.logFile, "rt")
        inputContent = refresh.read()
        outputContent = re.sub(Output.regexColorEsc, "", inputContent)
        refresh = open(self.logFile, "w")
        refresh.write(outputContent)

    def testBasicTask(self):
        self.runTestTask(self.binCmd, self.basicConsoleCmd[1])
        for each in self.basicTaskType:
            self.runTestTask(f"{self.binCmd} {each}")
        self.testConsoleCommand()

    def testConsoleCommand(self):
        for each in self.basicConsoleCmd:
            self.runTestTask(f"{self.binCmd} {self.basicTaskType[2]} \"{each}\"")

    def testAlgoTask(self):
        self.testOptimumMethod()
        self.testIntegralMethod()
        self.testSortMethod()

    def testOptimumMethod(self):
        self.runTestTask(f"{self.binCmd} {self.algoTaskType[0]} {self.algoTaskType[1][0]}")
        for each in self.algoOptimumMethod:
            self.runTestTask(f"{self.binCmd} {self.algoTaskType[0]} {self.algoTaskType[1][0]} {each}")
        self.runTestTask(
            f"{self.binCmd} {self.algoTaskType[0]} {self.algoTaskType[1][0]} {' '.join(self.algoOptimumMethod)}"
        )

    def testIntegralMethod(self):
        self.runTestTask(f"{self.binCmd} {self.algoTaskType[0]} {self.algoTaskType[1][1]}")
        for each in self.algoIntegralMethod:
            self.runTestTask(f"{self.binCmd} {self.algoTaskType[0]} {self.algoTaskType[1][1]} {each}")
        self.runTestTask(
            f"{self.binCmd} {self.algoTaskType[0]} {self.algoTaskType[1][1]} {' '.join(self.algoIntegralMethod)}"
        )

    def testSortMethod(self):
        self.runTestTask(f"{self.binCmd} {self.algoTaskType[0]} {self.algoTaskType[1][2]}")
        for each in self.algoSortMethod:
            self.runTestTask(f"{self.binCmd} {self.algoTaskType[0]} {self.algoTaskType[1][2]} {each}")
        self.runTestTask(
            f"{self.binCmd} {self.algoTaskType[0]} {self.algoTaskType[1][2]} {' '.join(self.algoSortMethod)}"
        )


if __name__ == "__main__":
    Test().run()
