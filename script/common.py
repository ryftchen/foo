#!/usr/bin/env python3

try:
    import curses
    import fcntl
    import signal
    import sys
    import subprocess
    import time
except ImportError as err:
    raise ImportError(err) from err


def executeCommand(command, setInput="", setTimeout=300):
    try:
        process = subprocess.run(
            command,
            shell=True,
            executable="/bin/bash",
            universal_newlines=True,
            capture_output=True,
            check=True,
            encoding="utf-8",
            input=setInput,
            timeout=setTimeout,
        )
        return process.stdout.strip(), process.stderr.strip(), process.returncode
    except subprocess.CalledProcessError as error:
        return error.stdout.strip(), error.stderr.strip(), error.returncode
    except subprocess.TimeoutExpired as error:
        return "", error, 124


class Log:
    def __init__(self, filename, stream=sys.stdout):
        self.terminal = stream
        self.log = open(filename, "w", encoding="utf-8")  # pylint: disable=consider-using-with
        fcntl.flock(self.log, fcntl.LOCK_EX | fcntl.LOCK_NB)

    def write(self, message):
        self.terminal.write(message)
        try:
            self.log.write(message)
        except IOError:
            fcntl.flock(self.log, fcntl.LOCK_UN)
            self.log.close()

    def flush(self):
        pass

    def __del__(self):
        fcntl.flock(self.log, fcntl.LOCK_UN)
        self.log.close()


class ProgressBar:
    saveCursor = "\033[s"
    restoreCursor = "\033[u"
    moveUpCursor = "\033[1A"
    foreColor = "\033[30m"
    backColor = "\033[42m"
    defaultForeColor = "\033[39m"
    defaultBackColor = "\033[49m"
    currentLines = 0
    placeholderLength = 20
    setTrap = False
    defaultSignal = None

    def setupProgressBar(self):
        curses.setupterm()

        self.trapDueToInterrupt()

        self.currentLines = self.tputLines()
        lines = self.currentLines - 1
        self.printProgress("\n")

        self.printProgress(self.saveCursor)
        self.printProgress(f"\033[0;{str(lines)}r")

        self.printProgress(self.restoreCursor)
        self.printProgress(self.moveUpCursor)
        self.drawProgressBar(0)

    def drawProgressBar(self, percentage):
        lines = self.tputLines()
        if lines != self.currentLines:
            self.setupProgressBar()

        self.printProgress(self.saveCursor)
        self.printProgress(f"\033[{str(lines)};0f")

        self.tput()
        self.printBar(percentage)
        self.printProgress(self.restoreCursor)
        time.sleep(0.01)

    def destroyProgressBar(self):
        lines = self.tputLines()
        self.printProgress(self.saveCursor)
        self.printProgress(f"\033[0;{str(lines)}r")

        self.printProgress(self.restoreCursor)
        self.printProgress(self.moveUpCursor)

        self.clearProgressBar()
        self.printProgress("\n\n")
        if self.setTrap:
            signal.signal(signal.SIGINT, self.defaultSignal)

    def clearProgressBar(self):
        lines = self.tputLines()
        self.printProgress(self.saveCursor)
        self.printProgress(f"\033[{str(lines)};0f")

        self.tput()
        self.printProgress(self.restoreCursor)

    def trapDueToInterrupt(self):
        self.setTrap = True
        self.defaultSignal = signal.getsignal(signal.SIGINT)
        signal.signal(signal.SIGINT, self.clearDueToInterrupt)

    def clearDueToInterrupt(self, sign, frame):
        _ = (sign, frame)
        self.destroyProgressBar()
        raise KeyboardInterrupt

    def printBar(self, percentage):
        cols = self.tputCols()
        barSize = cols - self.placeholderLength
        color = f"{self.foreColor}{self.backColor}"
        defaultColor = f"{self.defaultForeColor}{self.defaultBackColor}"

        completeSize = int((barSize * percentage) / 100)
        remainderSize = barSize - completeSize
        progressBar = f"[{color}{'#' * int(completeSize)}{defaultColor}{'.' * int(remainderSize)}]"
        self.printProgress(f" Progress {percentage:>3}% {progressBar}\r")

    @staticmethod
    def printProgress(text):
        print(text, end="")

    @staticmethod
    def tputLines():
        stdout, _, _ = executeCommand("tput lines")
        return int(stdout)

    @staticmethod
    def tputCols():
        stdout, _, _ = executeCommand("tput cols")
        return int(stdout)

    @staticmethod
    def tput():
        print(curses.tparm(curses.tigetstr("el")).decode(), end="")
