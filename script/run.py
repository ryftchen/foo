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
    color = {"red": "\033[0;31;40m", "green": "\033[0;32;40m", "yellow": "\033[0;33;40m", "blue": "\033[0;34;40m"}
    colorBack = "\033[49m"
    colorOff = "\033[0m"
    colorEscapeRegex = r"((\033\[.*?(m|s|u|A))|(\007|\017))"
    columnLength = 10
    minAlignLen = 30
    exclCmdAlignLen = 20
    minKeyWidth = 20
    minValueWidth = 80

    @classmethod
    def abort(cls, message):
        print(f"\r\nrun.py: {message}")
        sys.exit(-1)

    @classmethod
    def status(cls, colorFore, content):
        print(
            f"""{colorFore}{cls.colorBack}{f"{'=' * cls.columnLength}"}\
[ {datetime.strftime(datetime.now(), "%b %d %H:%M:%S")} | {content} ]{f"{'=' * cls.columnLength}"}{cls.colorOff}"""
        )

    @classmethod
    def formatAsTable(cls, data: dict, keyName="", valueName=""):
        rows = []
        rows.append("=" * (cls.minKeyWidth + 2 + cls.minValueWidth + 1))
        rows.append(f"{keyName.ljust(cls.minKeyWidth)} | {valueName.ljust(cls.minValueWidth)}")
        rows.append("=" * (cls.minKeyWidth + 2 + cls.minValueWidth + 1))

        for key, value in data.items():
            keyLines = []
            for line in key.split("\n"):
                lineLen = len(line)
                for index in range(0, lineLen, cls.minKeyWidth):
                    keyLines.append(line[index : index + cls.minKeyWidth].ljust(cls.minKeyWidth))

            valueLines = []
            for line in value.split("\n"):
                lineLen = len(line)
                for index in range(0, lineLen, cls.minValueWidth):
                    valueLines.append(line[index : index + cls.minValueWidth].ljust(cls.minValueWidth))

            lineCount = max(len(keyLines), len(valueLines))
            for index in range(lineCount):
                newLine = ""
                if index < len(keyLines):
                    newLine = f"{keyLines[index]} | "
                else:
                    newLine = f"{' ' * cls.minKeyWidth} | "
                if index < len(valueLines):
                    newLine += valueLines[index]
                else:
                    newLine += " " * cls.minValueWidth
                rows.append(newLine)

            rows.append("-" * (cls.minKeyWidth + 2 + cls.minValueWidth + 1))
        return "\n".join(rows)


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
    options = {"tst": False, "chk_cov": False, "chk_mem": False}
    buildScript = "./script/build.sh"
    tempDir = "./temporary"
    logFile = f"{tempDir}/foo_run.log"
    reportFile = f"{tempDir}/foo_run.report"
    passSteps = 0
    completeSteps = 0
    basicTaskDict["--help"] = [
        f"{taskCategory} {taskType}"
        for taskCategory, taskCategoryMap in generalTaskDict.items()
        for taskType in taskCategoryMap.keys()
    ]
    totalSteps = 1 + len(basicTaskDict.keys())
    for taskCategoryList in basicTaskDict.values():
        totalSteps += len(taskCategoryList)
    for taskCategoryMap in generalTaskDict.values():
        totalSteps += len(taskCategoryMap.keys())
        for targetTaskList in taskCategoryMap.values():
            totalSteps += len(targetTaskList) + 1

    def __init__(self):
        if not os.path.exists(self.tempDir):
            os.mkdir(self.tempDir)
        self.log = common.Log(self.logFile)
        self.progressBar = common.ProgressBar()
        self.taskQueue = queue.Queue()

    def run(self):
        filePath = os.path.split(os.path.realpath(__file__))[0]
        os.chdir(filePath.replace(filePath[filePath.index("script") :], ""))

        self.parseArguments()
        self.prepare()

        if not self.options["tst"]:
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
            while self.completeSteps < self.totalSteps:
                self.runTask(self.testBinCmd)

        self.complete()
        self.formatRunLog()
        self.summarizeRunLog()

    def stop(self, message=""):
        try:
            if self.options["chk_cov"]:
                common.executeCommand(f"rm -rf {self.tempDir}/*.profraw {self.tempDir}/*.profdata")
            if self.options["chk_mem"]:
                common.executeCommand(f"rm -rf {self.tempDir}/*.xml")
            sys.stdout = STDOUT
            self.progressBar.destroyProgressBar()
            del self.log
            self.formatRunLog()
        except Exception:  # pylint: disable=broad-except
            pass
        finally:
            if message:
                Output.abort(message)
            sys.exit(-1)

    def parseArguments(self):
        def checkPositive(value):
            value = int(value)
            if value <= 0:
                raise argparse.ArgumentTypeError(f"\"{value}\" is an invalid positive int value")
            return value

        parser = argparse.ArgumentParser(description="run script")
        parser.add_argument(
            "-t", "--test", nargs="?", const=1, type=checkPositive, help="run unit test only", metavar="times"
        )
        parser.add_argument(
            "-c", "--check", nargs="+", choices=["cov", "mem"], help="run with check: cov - coverage, mem - memory"
        )
        parser.add_argument(
            "-b",
            "--build",
            nargs="?",
            const="dbg",
            choices=["dbg", "rls"],
            help="run after build: dbg - debug, rls - release",
        )
        args = parser.parse_args()

        if args.check is not None:
            if args.test is not None:
                Output.abort("No support for the --check option during testing.")
            if "cov" in args.check:
                stdout, _, _ = common.executeCommand("command -v llvm-profdata-12 llvm-cov-12 2>&1")
                if stdout.find("llvm-profdata-12") != -1 and stdout.find("llvm-cov-12") != -1:
                    os.environ["FOO_CHK_COV"] = "on"
                    self.options["chk_cov"] = True
                    common.executeCommand(f"rm -rf {self.tempDir}/coverage")
                else:
                    Output.abort("No llvm-profdata or llvm-cov program. Please check it.")

            if "mem" in args.check:
                stdout, _, _ = common.executeCommand("command -v valgrind valgrind-ci 2>&1")
                if stdout.find("valgrind") != -1 and stdout.find("valgrind-ci") != -1:
                    os.environ["FOO_CHK_MEM"] = "on"
                    self.options["chk_mem"] = True
                    common.executeCommand(f"rm -rf {self.tempDir}/memory")
                else:
                    Output.abort("No valgrind or valgrind-ci program. Please check it.")

        if args.build is not None:
            if os.path.isfile(self.buildScript):
                buildCmd = self.buildScript
                if args.test is not None:
                    buildCmd += " --test"
                if args.build == "dbg":
                    self.buildExecutable(f"{buildCmd} 2>&1")
                elif args.build == "rls":
                    self.buildExecutable(f"{buildCmd} --release 2>&1")
            else:
                Output.abort("No shell script build.sh in script folder.")

        if args.test is not None:
            self.options["tst"] = True
            self.totalSteps = args.test

    def buildExecutable(self, buildCmd):
        stdout, stderr, returncode = common.executeCommand(buildCmd)
        if stderr or returncode != 0:
            print(f"<STDOUT>\n{stdout}\n<STDERR>\n{stderr}\n<RETURN CODE>\n{returncode}")
            Output.abort(f"Failed to run shell script {self.buildScript}.")
        else:
            print(stdout)
            if "FAILED:" in stdout:
                Output.abort(f"Failed to build the executable by shell script {self.buildScript}.")

    def prepare(self):
        if not self.options["tst"] and not os.path.isfile(f"{self.binDir}/{self.binCmd}"):
            Output.abort("No executable file. Please use the --build option to build it.")
        if self.options["tst"] and not os.path.isfile(f"{self.testBinDir}/{self.testBinCmd}"):
            Output.abort("No executable file for testing. Please use the --build option to build it.")
        common.executeCommand("ulimit -s unlimited")
        common.executeCommand("echo 'core.%s.%e.%p' | tee /proc/sys/kernel/core_pattern")

        if not os.path.exists(self.tempDir):
            os.makedirs(self.tempDir)
        self.progressBar.setupProgressBar()
        sys.stdout = self.log

    def complete(self):
        if self.options["chk_mem"]:
            common.executeCommand(f"rm -rf {self.tempDir}/*.xml")

        if self.options["chk_cov"]:
            common.executeCommand(
                f"llvm-profdata-12 merge -sparse {self.tempDir}/foo_chk_cov_*.profraw \
-o {self.tempDir}/foo_chk_cov.profdata"
            )
            common.executeCommand(
                f"llvm-cov-12 show -instr-profile={self.tempDir}/foo_chk_cov.profdata -show-branches=percent \
-show-expansions -show-regions -show-line-counts-or-regions -format=html -output-dir={self.tempDir}/coverage \
-Xdemangler=c++filt -object={self.binDir}/{self.binCmd} \
{' '.join([f'-object={self.libDir}/{lib}' for lib in self.libList])} 2>&1"
            )
            stdout, _, _ = common.executeCommand(
                f"llvm-cov-12 report -instr-profile={self.tempDir}/foo_chk_cov.profdata \
-object={self.binDir}/{self.binCmd} {' '.join([f'-object={self.libDir}/{lib}' for lib in self.libList])} 2>&1"
            )
            common.executeCommand(f"rm -rf {self.tempDir}/*.profraw {self.tempDir}/*.profdata")
            print(f"\r\n<CHECK COVERAGE>\n{stdout}")
            if "error" in stdout:
                print("Please rebuild the executable file with the --check option.")

        sys.stdout = STDOUT
        self.progressBar.destroyProgressBar()
        del self.log

    def generateTasks(self):
        for taskCategory, taskCategoryList in self.basicTaskDict.items():
            self.taskQueue.put(f"{self.binCmd} {taskCategory}")
            for option in taskCategoryList:
                self.taskQueue.put(f"{self.binCmd} {taskCategory} {option}")

        for taskCategory, taskCategoryMap in self.generalTaskDict.items():
            for taskType, targetTaskList in taskCategoryMap.items():
                self.taskQueue.put(f"{self.binCmd} {taskCategory} {taskType}")
                for target in targetTaskList:
                    self.taskQueue.put(f"{self.binCmd} {taskCategory} {taskType} {target}")
                self.taskQueue.put(f"{self.binCmd} {taskCategory} {taskType} {' '.join(targetTaskList)}")

    def performTasks(self):
        self.runTask(self.binCmd, "quit")
        while self.completeSteps < self.totalSteps:
            cmd = self.taskQueue.get()
            self.runTask(cmd)

    def runTask(self, command, enter=""):
        if not self.options["tst"]:
            fullCommand = f"{self.binDir}/{command}"
        else:
            fullCommand = f"{self.testBinDir}/{command}"
        if self.options["chk_mem"]:
            fullCommand = f"valgrind --tool=memcheck --xml=yes \
--xml-file={self.tempDir}/foo_chk_mem_{str(self.completeSteps + 1)}.xml {fullCommand}"
        if self.options["chk_cov"]:
            fullCommand = f"LLVM_PROFILE_FILE=\"{self.tempDir}/foo_chk_cov_{str(self.completeSteps + 1)}.profraw\" \
{fullCommand}"
        align = max(
            len(command) + Output.exclCmdAlignLen,
            Output.minAlignLen,
            len(str(self.totalSteps)) * 2 + len(" / ") + Output.exclCmdAlignLen,
        )
        Output.status(Output.color["blue"], f"CASE TASK: {f'{command}':<{align - Output.exclCmdAlignLen}} | START ")

        stdout, stderr, returncode = common.executeCommand(fullCommand, enter)
        if len(stdout.strip()) == 0 or stderr or returncode != 0:
            print(f"\r\n<STDOUT>\n{stdout}\n<STDERR>\n{stderr}\n<RETURN CODE>\n{returncode}")
            Output.status(Output.color["red"], f"{f'CASE TASK: FAILURE NO.{str(self.completeSteps + 1)}':<{align}}")
        else:
            print(stdout)
            self.passSteps += 1
            if self.options["chk_mem"]:
                stdout, _, _ = common.executeCommand(
                    f"valgrind-ci {self.tempDir}/foo_chk_mem_{str(self.completeSteps + 1)}.xml --summary"
                )
                if "error" in stdout:
                    stdout = stdout.replace("\t", "    ")
                    print(f"\r\n<CHECK MEMORY>\n{stdout}")
                    common.executeCommand(
                        f"valgrind-ci {self.tempDir}/foo_chk_mem_{str(self.completeSteps + 1)}.xml --source-dir=./ \
--output-dir={self.tempDir}/memory/case_task_{str(self.completeSteps + 1)}"
                    )
                    self.passSteps -= 1
                    Output.status(
                        Output.color["red"], f"{f'CASE TASK: FAILURE NO.{str(self.completeSteps + 1)}':<{align}}"
                    )

        self.completeSteps += 1
        Output.status(Output.color["blue"], f"CASE TASK: {f'{command}':<{align - Output.exclCmdAlignLen}} | FINISH")

        if self.passSteps != self.totalSteps:
            statusColor = Output.color["yellow"]
        else:
            statusColor = Output.color["green"]
        Output.status(
            statusColor,
            f"""\
{f"CASE TASK: SUCCESS {f'{str(self.passSteps)}':>{len(str(self.totalSteps))}} / {str(self.totalSteps)}":<{align}}""",
        )
        print("\n")

        sys.stdout = STDOUT
        self.progressBar.drawProgressBar(int(self.completeSteps / self.totalSteps * 100))
        sys.stdout = self.log

    def formatRunLog(self):
        refresh = ""
        with open(self.logFile, "rt", encoding="utf-8") as refresh:
            inputContent = refresh.read()
        outputContent = re.sub(Output.colorEscapeRegex, "", inputContent)
        with open(self.logFile, "w", encoding="utf-8") as refresh:
            refresh.write(outputContent)

    def summarizeRunLog(self):
        def analyzeForReport(readlines, startIndices, finishIndices, tags):
            failRes = {}
            covPer = {}
            memErr = {}
            for startIndex, finishIndex in zip(startIndices, finishIndices):
                if "| CASE TASK: FAILURE" in readlines[finishIndex - 1]:
                    failLine = readlines[finishIndex - 1]
                    number = failLine[failLine.find("NO.") : failLine.find("]")].strip()
                    cmdLine = readlines[startIndex]
                    caseTask = number + ": " + cmdLine[cmdLine.find(self.binCmd) : cmdLine.find("| START")].strip()

                    if tags["tst"] and "FAILED TEST" in "".join(readlines[startIndex + 1 : finishIndex]):
                        utCase = ""
                        utRunIndex = -1
                        utRunIndices = []
                        utFailIndices = []
                        for index, line in enumerate(readlines[startIndex + 1 : finishIndex]):
                            index += startIndex + 1
                            if "[ RUN      ]" in line:
                                utCase = line[line.find("]") + 2 : line.find("\n")]
                                utRunIndex = index
                            elif len(utCase) != 0 and f"[  FAILED  ] {utCase}" in line:
                                utRunIndices.append(utRunIndex)
                                utFailIndices.append(index)
                                utCase = ""
                        utFailRes = ""
                        for utRunIndex, utFailIndex in zip(utRunIndices, utFailIndices):
                            utFailRes += "".join(readlines[utRunIndex : utFailIndex + 1])
                        failRes[caseTask] = utFailRes
                        continue

                    lastIndex = finishIndex
                    if tags["chk_mem"]:
                        for index, line in enumerate(readlines[startIndex + 1 : finishIndex]):
                            if "<CHECK MEMORY>" in line:
                                index += startIndex + 1
                                memErr[caseTask] = "".join(readlines[index + 1 : finishIndex - 1])
                                lastIndex = index
                    failRes[caseTask] = "".join(readlines[startIndex + 1 : lastIndex - 1])

            if tags["chk_cov"]:
                category = ["Regions", "Functions", "Lines", "Branches"]
                for line in readlines:
                    if re.search(r"(^TOTAL(\s+\d+\s+\d+\s+\d+\.\d+%){4}$)", line):
                        matches = re.findall(r"(\d+\.\d+%)", line)
                        percentages = [str(float(s.strip("%"))) for s in matches]
                        if len(percentages) == 4:
                            for index, per in enumerate(percentages):
                                covPer[category[index]] = str(per) + "%"
                        break
                if len(covPer) == 0:
                    for cat in category:
                        covPer[cat] = "-"

            return failRes, covPer, memErr

        tags = {"tst": False, "chk_cov": False, "chk_mem": False}
        readlines = []
        with open(self.logFile, "rt", encoding="utf-8") as runLog:
            readlines = runLog.readlines()
            runLog.seek(0)
            content = runLog.read()
            if self.testBinCmd in content:
                tags["tst"] = True
            if "<CHECK COVERAGE>" in content:
                tags["chk_cov"] = True
            if "<CHECK MEMORY>" in content:
                tags["chk_mem"] = True
        if (
            (tags["tst"] != self.options["tst"])
            or (tags["chk_cov"] != self.options["chk_cov"])
            or (tags["chk_mem"] and not self.options["chk_mem"])
        ):
            Output.abort("Run options do not match the actual contents of the run log file.")
        if tags["tst"] and (tags["chk_cov"] or tags["chk_mem"]):
            Output.abort(f"The run log file {self.logFile} is complex. Please retry.")

        startIndices = []
        finishIndices = []
        for index, line in enumerate(readlines):
            if "| START" in line:
                startIndices.append(index)
            elif "| FINISH" in line:
                finishIndices.append(index)
        if len(startIndices) != len(finishIndices) or len(startIndices) != self.totalSteps:
            Output.abort(f"The run log file {self.logFile} is incomplete. Please retry.")

        failRes, covPer, memErr = analyzeForReport(readlines, startIndices, finishIndices, tags)
        with open(self.reportFile, "w", encoding="utf-8") as runReport:
            runStat = {"Passed": str(self.totalSteps - len(failRes)), "Failed": str(len(failRes))}
            runStatRep = "REPORT FOR RUN STATISTICS"
            if tags["tst"]:
                runStatRep += " (UNIT TEST)"
            runStatRep += ":\n" + Output().formatAsTable(runStat, "STATUS", "RUN STATISTICS") + "\n\n"
            failResRep = ""
            if failRes:
                failResRep = (
                    "\r\nREPORT FOR FAILURE RESULT:\n"
                    + Output().formatAsTable(failRes, "CASE TASK", "FAILURE RESULT")
                    + "\n\n"
                )
            covPerRep = ""
            if covPer:
                covPerRep = (
                    "\r\nREPORT FOR COVERAGE PERCENT:\n"
                    + Output().formatAsTable(covPer, "CATEGORY", "COVERAGE PERCENT")
                    + "\n\n"
                )
            memErrRep = ""
            if memErr:
                memErrRep = (
                    "\r\nREPORT FOR MEMORY ERROR:\n"
                    + Output().formatAsTable(memErr, "CASE TASK", "MEMORY ERROR")
                    + "\n\n"
                )
            runReport.write(runStatRep + failResRep + covPerRep + memErrRep)

        if failRes:
            sys.exit(1)


if __name__ == "__main__":
    try:
        task = Task()
        task.run()
    except Exception:  # pylint: disable=broad-except
        task.stop(traceback.format_exc())
    except KeyboardInterrupt:
        task.stop()
