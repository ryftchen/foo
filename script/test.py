#!/usr/bin/env python3

try:
    import argparse
    import os
    import queue
    import re
    import sys
    import threading
    from datetime import datetime
    import common
except ImportError as err:
    raise ImportError(err) from err

os.environ["TERM"] = "linux"
os.environ["TERMINFO"] = "/etc/terminfo"
STDOUT = sys.stdout


class Output:
    colorRed = "\033[0;31;40m"
    colorGreen = "\033[0;32;40m"
    colorYellow = "\033[0;33;40m"
    colorBlue = "\033[0;34;40m"
    colorForBackground = "\033[49m"
    colorOff = "\033[0m"
    colorEscapeRegex = r"((\033.*?m)|(\007))"
    columnLength = 10
    alignMaxLen = 30
    alignCmdLen = 10

    @classmethod
    def printException(cls, message):
        print(f"\r\nPython script test.py: {message}")
        sys.exit(-1)

    @classmethod
    def printStatus(cls, colorForForeground, content):
        print(
            f"""{colorForForeground}{cls.colorForBackground}{f"{'=' * cls.columnLength}"}\
[ {datetime.strftime(datetime.now(), "%b %d %H:%M:%S")} | {content} ]{f"{'=' * cls.columnLength}"}{cls.colorOff}"""
        )


class Test:
    binCmd = "foo"
    binDir = "./build/bin/"
    libList = ["libutility.so", "libalgorithm.so", "libdata_structure.so", "libdesign_pattern.so", "libnumeric.so"]
    libDir = "./build/lib/"
    basicTaskDict = {
        "--console": [r"'help'", r"'quit'", r"'run ./script/console_batch.txt'", r"'log'"],
        "--help": "",
        "--version": "",
    }
    generalTaskDict = {
        "--algorithm": {
            "match": ["rab", "knu", "boy", "hor", "sun"],
            "notation": ["pre", "pos"],
            "optimal": ["gra", "ann", "par", "gen"],
            "search": ["bin", "int", "fib"],
            "sort": ["bub", "sel", "ins", "she", "mer", "qui", "hea", "cou", "buc", "rad"],
        },
        "--data-structure": {"linear": ["lin", "sta", "que"], "tree": ["bin", "ade", "spl"]},
        "--design-pattern": {
            "behavioral": ["cha", "com", "int", "ite", "med", "mem", "obs", "sta", "str", "tem", "vis"],
            "creational": ["abs", "bui", "fac", "pro", "sin"],
            "structural": ["ada", "bri", "com", "dec", "fac", "fly", "pro"],
        },
        "--numeric": {
            "arithmetic": ["add", "sub", "mul", "div"],
            "divisor": ["euc", "ste"],
            "integral": ["tra", "sim", "rom", "gau", "mon"],
            "prime": ["era", "eul"],
        },
    }
    isCheckCoverage = False
    isCheckMemory = False
    envCoverage = "FOO_COV"
    buildFile = "./script/build.sh"
    logFile = "./temporary/foo_test.log"
    tempDir = "./temporary"
    passStep = 0
    completeStep = 0
    basicTaskDict["--help"] = [
        f"{taskCategory} {taskType}"
        for taskCategory, taskCategoryMap in generalTaskDict.items()
        for taskType in taskCategoryMap.keys()
    ]
    totalStep = 1 + len(basicTaskDict.keys())
    for taskCategoryList in basicTaskDict.values():
        totalStep += len(taskCategoryList)
    for taskCategoryMap in generalTaskDict.values():
        totalStep += len(taskCategoryMap.keys())
        for targetTaskList in taskCategoryMap.values():
            totalStep += len(targetTaskList) + 1

    def __init__(self):
        if not os.path.exists(self.tempDir):
            os.mkdir(self.tempDir)
        self.log = common.Log(self.logFile)
        self.progressBar = common.ProgressBar()
        self.taskQueue = queue.Queue()

    def run(self):
        self.prepareTest()

        threadList = []
        generator = threading.Thread(target=self.generateTestTask(), args=())
        generator.start()
        threadList.append(generator)
        performer = threading.Thread(target=self.performTestTask(), args=())
        performer.start()
        threadList.append(performer)
        for thread in threadList:
            thread.join()

        self.completeTest()
        self.analyzeLog()

    def generateTestTask(self):
        self.generateBasicTask()
        self.generateGeneralTask()

    def performTestTask(self):
        self.runTask(self.binCmd, "quit")
        while self.completeStep < self.totalStep:
            cmd = self.taskQueue.get()
            self.runTask(cmd)

    def runTask(self, command, enter=""):
        fullCommand = f"{self.binDir}{command}"
        if self.isCheckMemory:
            fullCommand = f"valgrind --xml=yes --xml-file={self.tempDir}/foo_mem_{str(self.completeStep + 1)}.xml \
{fullCommand}"
        if self.isCheckCoverage:
            fullCommand = f"LLVM_PROFILE_FILE=\"{self.tempDir}/foo_cov_{str(self.completeStep + 1)}.profraw\" \
{fullCommand}"
        align = max(len(command) + (Output.alignMaxLen - Output.alignCmdLen), Output.alignMaxLen)
        Output.printStatus(Output.colorBlue, f"TEST CASE: {f'{command}':<{Output.alignCmdLen}} | START ")

        stdout, stderr, errcode = common.executeCommand(fullCommand, enter)
        if stderr or errcode != 0:
            print(f"STDERR:\n{stderr}\nERRCODE:\n{errcode}")
            Output.printStatus(Output.colorRed, f"{f'TEST CASE: NO.{str(self.completeStep + 1)} FAILURE':<{align}}")
        else:
            print(stdout)
            self.passStep += 1
            if self.isCheckMemory:
                stdout, _, _ = common.executeCommand(
                    f"valgrind-ci {self.tempDir}/foo_mem_{str(self.completeStep + 1)}.xml --summary"
                )
                if "error" in stdout:
                    print(f"\r\nCHECK MEMORY:\n{stdout}")
                    common.executeCommand(
                        f"valgrind-ci {self.tempDir}/foo_mem_{str(self.completeStep + 1)}.xml --source-dir=./ \
--output-dir={self.tempDir}/memory/test_case_{str(self.completeStep + 1)}"
                    )
                    self.passStep -= 1
                    Output.printStatus(
                        Output.colorRed, f"{f'TEST CASE: NO.{str(self.completeStep + 1)} FAILURE':<{align}}"
                    )

        self.completeStep += 1
        Output.printStatus(Output.colorBlue, f"TEST CASE: {f'{command}':<{Output.alignCmdLen}} | FINISH")

        if self.passStep != self.totalStep:
            statusColor = Output.colorYellow
        else:
            statusColor = Output.colorGreen
        Output.printStatus(
            statusColor,
            f"""\
{f"TEST CASE: {f'{str(self.passStep)}':>{len(str(self.totalStep))}} / {str(self.totalStep)} SUCCESS":<{align}}""",
        )
        print("\n")

        sys.stdout = STDOUT
        self.progressBar.drawProgressBar(int(self.completeStep / self.totalStep * 100))
        sys.stdout = self.log

    def parseArgs(self):
        parser = argparse.ArgumentParser(description="test script")
        parser.add_argument(
            "-c", "--check", choices=["cov", "mem"], nargs="+", help="test with check: coverage / memory"
        )
        parser.add_argument(
            "-b", "--build", choices=["dbg", "rls"], nargs="?", const="dbg", help="test with build: debug / release"
        )
        args = parser.parse_args()

        if args.check:
            if "cov" in args.check:
                stdout, _, _ = common.executeCommand("command -v llvm-profdata-12 llvm-cov-12 2>&1")
                if stdout.find("llvm-profdata-12") != -1 and stdout.find("llvm-cov-12") != -1:
                    os.environ["FOO_ENV"] = self.envCoverage
                    self.isCheckCoverage = True
                    common.executeCommand(f"rm -rf {self.tempDir}/coverage/*")
                else:
                    Output.printException("No llvm-profdata or llvm-cov program. Please check it.")

            if "mem" in args.check:
                stdout, _, _ = common.executeCommand("command -v valgrind valgrind-ci 2>&1")
                if stdout.find("valgrind") != -1 and stdout.find("valgrind-ci") != -1:
                    self.isCheckMemory = True
                    common.executeCommand(f"rm -rf {self.tempDir}/memory/*")
                else:
                    Output.printException("No valgrind or valgrind-ci program. Please check it.")

        if args.build:
            if os.path.isfile(self.buildFile):
                if args.build == "dbg":
                    self.buildProject(f"{self.buildFile} 2>&1")
                elif args.build == "rls":
                    self.buildProject(f"{self.buildFile} --release 2>&1")
            else:
                Output.printException("No shell script build.sh in script folder.")

    def buildProject(self, command):
        stdout, stderr, errcode = common.executeCommand(command)
        if stderr or errcode != 0:
            Output.printException(f"Failed to run shell script {self.buildFile} in test.")
        else:
            print(stdout)
            if "FAILED:" in stdout:
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
        if self.isCheckMemory:
            common.executeCommand(f"rm -rf {self.tempDir}/*.xml")

        if self.isCheckCoverage:
            common.executeCommand(
                f"llvm-profdata-12 merge -sparse {self.tempDir}/foo_cov_*.profraw -o {self.tempDir}/foo_cov.profdata"
            )
            common.executeCommand(
                f"llvm-cov-12 show -instr-profile={self.tempDir}/foo_cov.profdata -show-branches=percent \
-show-expansions -show-regions -show-line-counts-or-regions -format=html -output-dir={self.tempDir}/coverage \
-Xdemangler=c++filt -object={self.binDir}{self.binCmd} "
                + ' '.join([f"-object={self.libDir}{lib}" for lib in self.libList])
                + " 2>&1"
            )
            stdout, _, _ = common.executeCommand(
                f"llvm-cov-12 report -instr-profile={self.tempDir}/foo_cov.profdata -object={self.binDir}{self.binCmd} "
                + ' '.join([f"-object={self.libDir}{lib}" for lib in self.libList])
                + " 2>&1"
            )
            common.executeCommand(f"rm -rf {self.tempDir}/*.profraw {self.tempDir}/*.profdata")
            print(f"\r\nCHECK COVERAGE:\n{stdout}")
            if "error" in stdout:
                Output.printException("Please rebuild the executable file before use --check option.")

        sys.stdout = STDOUT
        self.progressBar.destroyProgressBar()
        del self.log

    def analyzeLog(self):
        refresh = ""
        with open(self.logFile, "rt", encoding="utf-8") as refresh:
            inputContent = refresh.read()
        outputContent = re.sub(Output.colorEscapeRegex, "", inputContent)
        with open(self.logFile, "w", encoding="utf-8") as refresh:
            refresh.write(outputContent)

    def generateBasicTask(self):
        for taskCategory, taskCategoryList in self.basicTaskDict.items():
            self.taskQueue.put(f"{self.binCmd} {taskCategory}")
            for option in taskCategoryList:
                self.taskQueue.put(f"{self.binCmd} {taskCategory} {option}")

    def generateGeneralTask(self):
        for taskCategory, taskCategoryMap in self.generalTaskDict.items():
            for taskType, targetTaskList in taskCategoryMap.items():
                self.taskQueue.put(f"{self.binCmd} {taskCategory} {taskType}")
                for target in targetTaskList:
                    self.taskQueue.put(f"{self.binCmd} {taskCategory} {taskType} {target}")
                self.taskQueue.put(f"{self.binCmd} {taskCategory} {taskType} {' '.join(targetTaskList)}")


if __name__ == "__main__":
    Test().run()
