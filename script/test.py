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
    libList = ["libutility.so", "libalgorithm.so", "libdesign_pattern.so", "libnumeric.so"]
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
            "search": ["bin", "int", "fib"],
            "sort": ["bub", "sel", "ins", "she", "mer", "qui", "hea", "cou", "buc", "rad"],
        },
        "--design-pattern": {
            "behavioral": ["cha", "com", "int", "ite", "med", "mem", "obs", "sta", "str", "tem", "vis"],
            "creational": ["abs", "bui", "fac", "pro", "sin"],
            "structural": ["ada", "bri", "com", "dec", "fac", "fly", "pro"],
        },
        "--numeric": {
            "arithmetic": ["add", "sub", "mul", "div"],
            "divisor": ["euc", "ste"],
            "integral": ["tra", "sim", "rom", "gau", "mon"],
            "optimal": ["gra", "ann", "par", "gen"],
            "sieve": ["era", "eul"],
        },
    }
    isCheckCoverage = False
    envCoverage = "CODE_COVERAGE"
    isCheckMemory = False
    memorySummaryContent = "ERROR SUMMARY: 0 errors from 0 contexts (suppressed: 0 from 0)"
    buildFile = "./script/build.sh"
    logFile = "./temporary/foo_test.log"
    tempDir = "./temporary"
    passStep = 0
    completeStep = 0
    basicTaskDict["--help"] = [
        f"{taskCategory} {taskType}"
        for taskCategory, taskCategoryMaps in generalTaskDict.items()
        for taskType in taskCategoryMaps.keys()
    ]
    totalStep = 1 + len(basicTaskDict.keys())
    for taskCategoryLists in basicTaskDict.values():
        totalStep += len(taskCategoryLists)
    for taskCategoryMaps in generalTaskDict.values():
        totalStep += len(taskCategoryMaps.keys())
        for taskMethods in taskCategoryMaps.values():
            totalStep += len(taskMethods) + 1

    def __init__(self):
        if not os.path.exists(self.tempDir):
            os.mkdir(self.tempDir)
        self.log = common.Log(self.logFile)
        self.progressBar = common.ProgressBar()
        self.taskQueue = queue.Queue()

    def run(self):
        self.prepareTest()

        threadList = []
        generater = threading.Thread(target=self.generateTestTask(), args=())
        generater.start()
        threadList.append(generater)
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
                else:
                    Output.printException("No llvm-profdata or llvm-cov program. Please check it.")

            if "mem" in args.check:
                stdout, _, _ = common.executeCommand("command -v valgrind 2>&1")
                if stdout.find("valgrind") != -1:
                    self.isCheckMemory = True
                else:
                    Output.printException("No valgrind program. Please check it.")

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

    def analyzeLog(self):
        refresh = open(self.logFile, "rt")
        inputContent = refresh.read()
        outputContent = re.sub(Output.regexColorEsc, "", inputContent)
        refresh = open(self.logFile, "w")
        refresh.write(outputContent)

    def generateBasicTask(self):
        for taskCategory, taskCategoryLists in self.basicTaskDict.items():
            self.taskQueue.put(f"{self.binCmd} {taskCategory}")
            for option in taskCategoryLists:
                self.taskQueue.put(f"{self.binCmd} {taskCategory} {option}")

    def generateGeneralTask(self):
        for taskCategory, taskCategoryMaps in self.generalTaskDict.items():
            for taskType, taskMethods in taskCategoryMaps.items():
                self.taskQueue.put(f"{self.binCmd} {taskCategory} {taskType}")
                for method in taskMethods:
                    self.taskQueue.put(f"{self.binCmd} {taskCategory} {taskType} {method}")
                self.taskQueue.put(f"{self.binCmd} {taskCategory} {taskType} {' '.join(taskMethods)}")


if __name__ == "__main__":
    Test().run()
