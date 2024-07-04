#!/usr/bin/env python3

try:
    import curses
    import fcntl
    import signal
    import sys
    import shutil
    import subprocess
    import time
except ImportError as err:
    raise ImportError(err) from err


def execute_command(command, set_input="", set_timeout=300):
    try:
        process = subprocess.run(
            command,
            executable="/bin/bash",
            shell=True,
            universal_newlines=True,
            encoding="utf-8",
            input=set_input,
            capture_output=True,
            timeout=set_timeout,
            check=True,
        )
        return process.stdout.strip(), process.stderr.strip(), process.returncode
    except subprocess.CalledProcessError as error:
        return error.stdout.strip(), error.stderr.strip(), error.returncode
    except subprocess.TimeoutExpired as error:
        return "", error, 124


class Log:
    def __init__(self, filename, mode="wt", stream=sys.stdout):
        self.terminal = stream
        self.log = open(filename, mode, encoding="utf-8")  # pylint: disable=consider-using-with
        fcntl.flock(self.log.fileno(), fcntl.LOCK_EX | fcntl.LOCK_NB)

    def write(self, message):
        self.terminal.write(message)
        try:
            self.log.write(message)
        except IOError:
            fcntl.flock(self.log.fileno(), fcntl.LOCK_UN)
            self.log.close()

    def flush(self):
        pass

    def __del__(self):
        fcntl.flock(self.log.fileno(), fcntl.LOCK_UN)
        self.log.close()


class ProgressBar:
    save_cursor = "\033[s"
    restore_cursor = "\033[u"
    move_up_cursor = "\033[1A"
    fore_color = "\033[30m"
    back_color = "\033[42m"
    default_fore_color = "\033[39m"
    default_back_color = "\033[49m"
    placeholder_length = 20

    def __init__(self):
        self.current_lines = 0
        self.set_trap = False
        self.default_signal = None

    def setup_progress_bar(self):
        curses.setupterm()

        self.trap_due_to_interrupt()

        self.current_lines = self.tput_lines()
        lines = self.current_lines - 1
        self.print_progress("\n")

        self.print_progress(self.save_cursor)
        self.print_progress(f"\033[0;{str(lines)}r")

        self.print_progress(self.restore_cursor)
        self.print_progress(self.move_up_cursor)
        self.draw_progress_bar(0)

    def draw_progress_bar(self, percentage):
        lines = self.tput_lines()
        if lines != self.current_lines:
            self.setup_progress_bar()

        self.print_progress(self.save_cursor)
        self.print_progress(f"\033[{str(lines)};0f")

        self.tput()
        self.print_bar(percentage)
        self.print_progress(self.restore_cursor)
        time.sleep(0.01)

    def destroy_progress_bar(self):
        lines = self.tput_lines()
        self.print_progress(self.save_cursor)
        self.print_progress(f"\033[0;{str(lines)}r")

        self.print_progress(self.restore_cursor)
        self.print_progress(self.move_up_cursor)

        self.clear_progress_bar()
        self.print_progress("\n\n")
        if self.set_trap:
            signal.signal(signal.SIGINT, self.default_signal)

    def clear_progress_bar(self):
        lines = self.tput_lines()
        self.print_progress(self.save_cursor)
        self.print_progress(f"\033[{str(lines)};0f")

        self.tput()
        self.print_progress(self.restore_cursor)

    def trap_due_to_interrupt(self):
        self.set_trap = True
        self.default_signal = signal.getsignal(signal.SIGINT)
        signal.signal(signal.SIGINT, self.clear_due_to_interrupt)

    def clear_due_to_interrupt(self, sign, frame):
        _ = (sign, frame)
        self.destroy_progress_bar()
        raise KeyboardInterrupt

    def print_bar(self, percentage):
        cols = self.tput_cols()
        bar_size = cols - self.placeholder_length
        color = f"{self.fore_color}{self.back_color}"
        default_color = f"{self.default_fore_color}{self.default_back_color}"

        complete_size = int((bar_size * percentage) / 100)
        remainder_size = bar_size - complete_size
        progress_bar = f"[{color}{'#' * int(complete_size)}{default_color}{'.' * int(remainder_size)}]"
        self.print_progress(f" Progress {percentage:>3}% {progress_bar}\r")

    @staticmethod
    def print_progress(text):
        print(text, end="")

    @staticmethod
    def tput_lines():
        _, lines = shutil.get_terminal_size()
        return int(lines)

    @staticmethod
    def tput_cols():
        cols, _ = shutil.get_terminal_size()
        return int(cols)

    @staticmethod
    def tput():
        print(curses.tparm(curses.tigetstr("el")).decode(), end="")
