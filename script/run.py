#!/usr/bin/env python3

try:
    import argparse
    import os
    import queue
    import re
    import sys
    import threading
    import traceback
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
    colorEscapeRegex = r"((\033\[.*?(m|s|u|A))|(\007|\017))"
    columnLength = 10
    alignMinLen = 30
    alignExclCmdLen = 20

    @classmethod
    def exception(cls, message):
        print(f"\r\nrun.py: {message}")
        sys.exit(-1)

    @classmethod
    def status(cls, colorForForeground, content):
        print(
            f"""{colorForForeground}{cls.colorForBackground}{f"{'=' * cls.columnLength}"}\
[ {datetime.strftime(datetime.now(), "%b %d %H:%M:%S")} | {content} ]{f"{'=' * cls.columnLength}"}{cls.colorOff}"""
        )


class Task:
    binCmd = "foo"
    binDir = "./build/bin"
    testBinCmd = "foo_test"
    testBinDir = "./test/build/bin"
    libList = ["libutility.so", "libalgorithm.so", "libdata_structure.so", "libdesign_pattern.so", "libnumeric.so"]
    libDir = "./build/lib"
    basicTaskDict = {
        "--console": [r"'help'", r"'quit'", r"'batch ./script/console_batch.txt'", r"'log'"],
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
    isUnitTest = False
    buildFile = "./script/build.sh"
    logFile = "./temporary/foo_run.log"
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
        self.prepareTask()

        if not self.isUnitTest:
            threadList = []
            producer = threading.Thread(target=self.generateTasks(), args=())
            producer.start()
            threadList.append(producer)
            consumer = threading.Thread(target=self.performTasks(), args=())
            consumer.start()
            threadList.append(consumer)
            for thread in threadList:
                thread.join()
        else:
            while self.completeStep < self.totalStep:
                self.runTask(self.testBinCmd)

        self.completeTask()
        self.formatRunLog()

    def stop(self, message=""):
        try:
            if self.isCheckMemory:
                common.executeCommand(f"rm -rf {self.tempDir}/*.xml")
            if self.isCheckCoverage:
                common.executeCommand(f"rm -rf {self.tempDir}/*.profraw {self.tempDir}/*.profdata")
            sys.stdout = STDOUT
            self.progressBar.destroyProgressBar()
            del self.log
            self.formatRunLog()
        except Exception:  # pylint: disable=broad-except
            pass
        finally:
            if message:
                Output.exception(message)

    def generateTasks(self):
        self.generateBasicTasks()
        self.generateGeneralTasks()

    def performTasks(self):
        self.runTask(self.binCmd, "quit")
        while self.completeStep < self.totalStep:
            cmd = self.taskQueue.get()
            self.runTask(cmd)

    def runTask(self, command, enter=""):
        if not self.isUnitTest:
            fullCommand = f"{self.binDir}/{command}"
        else:
            fullCommand = f"{self.testBinDir}/{command}"
        if self.isCheckMemory:
            fullCommand = f"valgrind --tool=memcheck --xml=yes \
--xml-file={self.tempDir}/foo_chk_mem_{str(self.completeStep + 1)}.xml {fullCommand}"
        if self.isCheckCoverage:
            fullCommand = f"LLVM_PROFILE_FILE=\"{self.tempDir}/foo_chk_cov_{str(self.completeStep + 1)}.profraw\" \
{fullCommand}"
        align = max(
            len(command) + Output.alignExclCmdLen,
            Output.alignMinLen,
            len(str(self.totalStep)) * 2 + len(" / ") + Output.alignExclCmdLen,
        )
        Output.status(Output.colorBlue, f"CASE TASK: {f'{command}':<{align - Output.alignExclCmdLen}} | START ")

        stdout, stderr, returncode = common.executeCommand(fullCommand, enter)
        if stderr or returncode != 0:
            print(f"<STDOUT>\n{stdout}\n<STDERR>\n{stderr}\n<RETURN CODE>\n{returncode}")
            Output.status(Output.colorRed, f"{f'CASE TASK: FAILURE NO.{str(self.completeStep + 1)}':<{align}}")
        else:
            print(stdout)
            self.passStep += 1
            if self.isCheckMemory:
                stdout, _, _ = common.executeCommand(
                    f"valgrind-ci {self.tempDir}/foo_chk_mem_{str(self.completeStep + 1)}.xml --summary"
                )
                if "error" in stdout:
                    print(f"\r\n<CHECK MEMORY>\n{stdout}")
                    common.executeCommand(
                        f"valgrind-ci {self.tempDir}/foo_chk_mem_{str(self.completeStep + 1)}.xml --source-dir=./ \
--output-dir={self.tempDir}/memory/case_task_{str(self.completeStep + 1)}"
                    )
                    self.passStep -= 1
                    Output.status(Output.colorRed, f"{f'CASE TASK: FAILURE NO.{str(self.completeStep + 1)}':<{align}}")

        self.completeStep += 1
        Output.status(Output.colorBlue, f"CASE TASK: {f'{command}':<{align - Output.alignExclCmdLen}} | FINISH")

        if self.passStep != self.totalStep:
            statusColor = Output.colorYellow
        else:
            statusColor = Output.colorGreen
        Output.status(
            statusColor,
            f"""\
{f"CASE TASK: SUCCESS {f'{str(self.passStep)}':>{len(str(self.totalStep))}} / {str(self.totalStep)}":<{align}}""",
        )
        print("\n")

        sys.stdout = STDOUT
        self.progressBar.drawProgressBar(int(self.completeStep / self.totalStep * 100))
        sys.stdout = self.log

    def parseArguments(self):
        parser = argparse.ArgumentParser(description="run script")
        parser.add_argument("-t", "--test", nargs="?", type=int, const=1, help="run unit test only")
        parser.add_argument(
            "-c", "--check", choices=["cov", "mem"], nargs="+", help="run with check: coverage and memory"
        )
        parser.add_argument(
            "-b", "--build", choices=["dbg", "rls"], nargs="?", const="dbg", help="run with build: debug or release"
        )
        args = parser.parse_args()

        if args.check:
            if args.test:
                Output.exception("No support for --check during testing.")
            if "cov" in args.check:
                stdout, _, _ = common.executeCommand("command -v llvm-profdata-12 llvm-cov-12 2>&1")
                if stdout.find("llvm-profdata-12") != -1 and stdout.find("llvm-cov-12") != -1:
                    os.environ["FOO_CHK_COV"] = "on"
                    self.isCheckCoverage = True
                    common.executeCommand(f"rm -rf {self.tempDir}/coverage")
                else:
                    Output.exception("No llvm-profdata or llvm-cov program. Please check it.")

            if "mem" in args.check:
                stdout, _, _ = common.executeCommand("command -v valgrind valgrind-ci 2>&1")
                if stdout.find("valgrind") != -1 and stdout.find("valgrind-ci") != -1:
                    self.isCheckMemory = True
                    common.executeCommand(f"rm -rf {self.tempDir}/memory")
                else:
                    Output.exception("No valgrind or valgrind-ci program. Please check it.")

        if args.build:
            if os.path.isfile(self.buildFile):
                buildCmd = self.buildFile
                if args.test:
                    buildCmd += " --test"
                if args.build == "dbg":
                    self.buildTarget(f"{buildCmd} 2>&1")
                elif args.build == "rls":
                    self.buildTarget(f"{buildCmd} --release 2>&1")
            else:
                Output.exception("No shell script build.sh in script folder.")

        if args.test:
            self.isUnitTest = True
            self.totalStep = args.test

    def buildTarget(self, buildCmd):
        stdout, stderr, returncode = common.executeCommand(buildCmd)
        if stderr or returncode != 0:
            print(f"<STDOUT>\n{stdout}\n<STDERR>\n{stderr}\n<RETURN CODE>\n{returncode}")
            Output.exception(f"Failed to run shell script {self.buildFile}.")
        else:
            print(stdout)
            if "FAILED:" in stdout:
                Output.exception(f"Failed to build target by shell script {self.buildFile}.")

    def prepareTask(self):
        filePath = os.path.split(os.path.realpath(__file__))[0]
        os.chdir(filePath.replace(filePath[filePath.index("script") :], ''))

        self.parseArguments()
        if not self.isUnitTest and not os.path.isfile(f"{self.binDir}/{self.binCmd}"):
            Output.exception("No executable file. Please build it.")
        if self.isUnitTest and not os.path.isfile(f"{self.testBinDir}/{self.testBinCmd}"):
            Output.exception("No executable file for testing. Please build it.")
        if not os.path.exists(self.tempDir):
            os.makedirs(self.tempDir)

        self.progressBar.setupProgressBar()
        sys.stdout = self.log

    def completeTask(self):
        if self.isCheckMemory:
            common.executeCommand(f"rm -rf {self.tempDir}/*.xml")

        if self.isCheckCoverage:
            common.executeCommand(
                f"llvm-profdata-12 merge -sparse {self.tempDir}/foo_chk_cov_*.profraw \
-o {self.tempDir}/foo_chk_cov.profdata"
            )
            common.executeCommand(
                f"llvm-cov-12 show -instr-profile={self.tempDir}/foo_chk_cov.profdata -show-branches=percent \
-show-expansions -show-regions -show-line-counts-or-regions -format=html -output-dir={self.tempDir}/coverage \
-Xdemangler=c++filt -object={self.binDir}/{self.binCmd} "
                + ' '.join([f"-object={self.libDir}/{lib}" for lib in self.libList])
                + " 2>&1"
            )
            stdout, _, _ = common.executeCommand(
                f"llvm-cov-12 report -instr-profile={self.tempDir}/foo_chk_cov.profdata \
-object={self.binDir}/{self.binCmd} "
                + ' '.join([f"-object={self.libDir}/{lib}" for lib in self.libList])
                + " 2>&1"
            )
            common.executeCommand(f"rm -rf {self.tempDir}/*.profraw {self.tempDir}/*.profdata")
            print(f"\r\n<CHECK COVERAGE>\n{stdout}")
            if "error" in stdout:
                print("Please rebuild the executable file and use the --check option.")

        sys.stdout = STDOUT
        self.progressBar.destroyProgressBar()
        del self.log

    def formatRunLog(self):
        refresh = ""
        with open(self.logFile, "rt", encoding="utf-8") as refresh:
            inputContent = refresh.read()
        outputContent = re.sub(Output.colorEscapeRegex, "", inputContent)
        with open(self.logFile, "w", encoding="utf-8") as refresh:
            refresh.write(outputContent)

    def generateBasicTasks(self):
        for taskCategory, taskCategoryList in self.basicTaskDict.items():
            self.taskQueue.put(f"{self.binCmd} {taskCategory}")
            for option in taskCategoryList:
                self.taskQueue.put(f"{self.binCmd} {taskCategory} {option}")

    def generateGeneralTasks(self):
        for taskCategory, taskCategoryMap in self.generalTaskDict.items():
            for taskType, targetTaskList in taskCategoryMap.items():
                self.taskQueue.put(f"{self.binCmd} {taskCategory} {taskType}")
                for target in targetTaskList:
                    self.taskQueue.put(f"{self.binCmd} {taskCategory} {taskType} {target}")
                self.taskQueue.put(f"{self.binCmd} {taskCategory} {taskType} {' '.join(targetTaskList)}")


if __name__ == "__main__":
    try:
        task = Task()
        task.run()
    except Exception:  # pylint: disable=broad-except
        task.stop(traceback.format_exc())
    except KeyboardInterrupt:
        task.stop()
