#!/usr/bin/env python3

try:
    import argparse
    import ast
    import curses
    import errno
    import fcntl
    import fnmatch
    import importlib
    import os
    import pathlib
    import pty
    import queue
    import re
    import select
    import shutil
    import signal
    import subprocess
    import sys
    import threading
    import time
    import traceback
    import typing
    from datetime import datetime
except ImportError as err:
    raise ImportError(err) from err

STDOUT = sys.stdout


class Task:
    _app_bin_cmd = "foo"
    _app_bin_path = "./build/bin"
    _tst_bin_cmd = "foo_test"
    _tst_bin_path = "./test/build/bin"
    _lib_list = ["libfoo_util.so", "libfoo_algo.so", "libfoo_ds.so", "libfoo_dp.so", "libfoo_num.so"]
    _lib_path = "./build/lib"
    _script_path = "./script"
    _build_script = f"{_script_path}/build.sh"
    _run_dict = f"{_script_path}/.run_dict"
    _run_console_batch = f"{_script_path}/.run_console_batch"
    _app_native_task_dict = {
        "--help": [],
        "--version": [],
        "--dump": [],
        "--console": [
            r"usage",
            r"quit",
            r"trace",
            r"clean",
            f"batch {_run_console_batch}",
            r"refresh",
            r"reconnect",
            r"depend",
            r'''execute "ps -eww -o size,pid,user,command --sort -size | awk '{ hr=\$1/1024; printf(\"%6.2fMB \", hr) } { for (x=4; x<=NF; x++) { printf(\"%s \", \$x) } print \"\" }' | cut -d '' -f 2 | sed '1d'"''',  # pylint: disable=line-too-long
            r"journal",
            r"monitor 0",
            r"profile",
        ],
    }
    _app_extra_task_dict = {
        "app-algo": {
            "--help": [],
            "--match": ["rab", "knu", "boy", "hor", "sun"],
            "--notation": ["pre", "pos"],
            "--optimal": ["gra", "tab", "ann", "par", "ant", "gen"],
            "--search": ["bin", "int", "fib"],
            "--sort": ["bub", "sel", "ins", "she", "mer", "qui", "hea", "cou", "buc", "rad"],
        },
        "app-dp": {
            "--help": [],
            "--behavioral": ["cha", "com", "int", "ite", "med", "mem", "obs", "sta", "str", "tem", "vis"],
            "--creational": ["abs", "bui", "fac", "pro", "sin"],
            "--structural": ["ada", "bri", "com", "dec", "fac", "fly", "pro"],
        },
        "app-ds": {"--help": [], "--linear": ["lin", "sta", "que"], "--tree": ["bin", "ade", "spl"]},
        "app-num": {
            "--help": [],
            "--arithmetic": ["add", "sub", "mul", "div"],
            "--divisor": ["euc", "ste"],
            "--integral": ["tra", "sim", "rom", "gau", "mon"],
            "--prime": ["era", "eul"],
        },
    }
    _tst_task_filt = []
    _marked_options = {"tst": False, "chk": {"cov": False, "mem": False}}
    _analyze_only = False
    _report_path = "./report"
    _run_log_file = f"{_report_path}/foo_run.log"
    _run_report_file = f"{_report_path}/foo_run.report"
    _suspicious_trait = [" \033[0;31;40m\033[1m\033[49m[ERR]\033[0m ", " \033[0;33;40m\033[1m\033[49m[WRN]\033[0m "]
    _stat_default_len = 55
    _stat_at_least_len = 15
    _esc_color = {"red": "\033[0;31;40m", "green": "\033[0;32;40m", "yellow": "\033[0;33;40m", "blue": "\033[0;34;40m"}
    _tbl_key_width = 15
    _tbl_value_width = 60

    def __init__(self):
        self._args = None
        self._passed_steps = 0
        self._complete_steps = 0
        self._total_steps = 0
        self._repeat_count = 1
        self._duration = 0

        os.environ["TERM"] = "linux"
        os.environ["TERMINFO"] = "/etc/terminfo"
        env = os.getenv("FOO_ENV")
        if env is not None:
            if env != "foo_dev":
                TermUtil.exit_with_error("The environment variable FOO_ENV must be foo_dev.")
        else:
            TermUtil.exit_with_error("Please export the environment variable FOO_ENV.")
        self_path = os.path.split(os.path.realpath(__file__))[0]
        if not fnmatch.fnmatch(self_path, "*foo/script"):
            TermUtil.exit_with_error("Illegal path to current script.")
        os.chdir(os.path.dirname(self_path))
        sys.path.append("./")
        if not os.path.exists(self._report_path):
            os.mkdir(self._report_path)
        proc_path = os.path.expanduser("~/.foo")
        if os.path.exists(proc_path):
            execute_command(f"rm -rf {proc_path}")

        self._stream_logger = sys.stdout
        self._progress_bar = ProgressBar()
        self._task_queue = queue.Queue()

    def run(self):
        self._parse_arguments()
        self._apply_arguments()

        self._prepare()
        start_time = datetime.now()

        if not self._marked_options["tst"]:
            thread_list = []
            producer = threading.Thread(target=self._generate_tasks(), args=())
            producer.start()
            thread_list.append(producer)
            consumer = threading.Thread(target=self._perform_tasks(), args=())
            consumer.start()
            thread_list.append(consumer)
            for thread in thread_list:
                thread.join()
        else:
            self._total_steps = self._repeat_count
            command = self._tst_bin_cmd
            if self._tst_task_filt:
                command = f"{self._tst_bin_cmd} {' '.join(self._tst_task_filt)}"
            while self._repeat_count:
                self._run_single_task(command)
                self._repeat_count -= 1

        self._duration = (datetime.now() - start_time).total_seconds()
        self._complete()
        self._format_run_log()
        self._summarize_run_log()

    def stop(self, message: str = ""):
        try:
            if self._marked_options["chk"]["cov"]:
                execute_command(f"rm -rf {self._report_path}/dca/chk_cov/{{*.profraw,*.profdata}}")
            if self._marked_options["chk"]["mem"]:
                execute_command(f"rm -rf {self._report_path}/dca/chk_mem/*.xml")
            sys.stdout = STDOUT
            self._progress_bar.destroy_progress_bar()
            del self._stream_logger
            self._format_run_log()
        except Exception:  # pylint: disable=broad-except
            pass
        finally:
            if message:
                TermUtil.exit_with_error(message)
            sys.exit(1)

    def _parse_arguments(self):
        def _check_positive_integer(value: int) -> int:
            value = int(value)
            if value > 0:
                return value
            raise argparse.ArgumentTypeError("Must be a positive integer.")

        parser = argparse.ArgumentParser(description="run script", formatter_class=argparse.RawTextHelpFormatter)
        parser.add_argument("-t", "--test", action="store_true", default=False, help="run unit test only")
        parser.add_argument(
            "-r", "--repeat", nargs="?", const=1, type=_check_positive_integer, help="run repeatedly", metavar="TIMES"
        )
        parser.add_argument(
            "-c",
            "--check",
            nargs="+",
            choices=["cov", "mem"],
            help="run with check\n- cov    coverage (force to use clang)\n- mem    memory",
        )
        parser.add_argument(
            "-b",
            "--build",
            nargs="?",
            const="dbg",
            choices=["dbg", "rls"],
            help="build before run\n- dbg    debug\n- rls    release",
        )
        parser.add_argument(
            "-s",
            "--sanitizer",
            choices=["asan", "tsan", "ubsan"],
            required=False,
            help="build with sanitizer (force to use clang)\n\
- asan   address sanitizer\n- tsan   thread sanitizer\n- ubsan  undefined behavior sanitizer",
        )
        parser.add_argument("-a", "--analyze", action="store_true", default=False, help="analyze run log only")
        parser.add_argument("-d", "--dump", action="store_true", default=False, help="dump run task dictionary only")

        self._args = parser.parse_args()

    def _apply_arguments(self):
        if self._args.analyze:
            if len(sys.argv) > 2:
                TermUtil.exit_with_error("No other arguments are supported during analyzing.")
            self._analyze_only = True
            self._summarize_run_log()
            sys.exit(0)

        if self._args.dump:
            self._dump_run_dict()
            sys.exit(0)

        if self._args.test:
            self._marked_options["tst"] = True

        if self._args.repeat is not None:
            self._repeat_count = self._args.repeat

        if self._args.check is not None:
            if self._args.sanitizer is not None:
                TermUtil.exit_with_error(
                    "It is not possible to use the --check option and the --sanitizer option at the same time."
                )
            if "cov" in self._args.check:
                if self._args.build is None:
                    TermUtil.exit_with_error(
                        "Checking coverage requires recompiling with instrumentation. Please add the --build option."
                    )
                if "mem" in self._args.check:
                    TermUtil.exit_with_error(
                        "Checking coverage and memory at the same time can lead to inaccurate results."
                    )
                self._initialize_for_check_coverage()

            if "mem" in self._args.check:
                self._initialize_for_check_memory()

        if self._args.sanitizer is not None:
            if self._args.build is None:
                TermUtil.exit_with_error(
                    "The runtime sanitizer requires recompiling with instrumentation. Please add the --build option."
                )
            os.environ["FOO_BLD_SAN"] = self._args.sanitizer

        if self._args.build is not None:
            if os.path.isfile(self._build_script):
                build_cmd = self._build_script
                if self._marked_options["tst"]:
                    build_cmd += " --test"
                if self._args.build == "rls":
                    build_cmd += " --release"
                return_code = TermUtil.execute_in_pty(build_cmd)
                if return_code:
                    TermUtil.exit_with_error(f"Failed to run shell script {self._build_script}.")
            else:
                TermUtil.exit_with_error(f"No shell script {self._build_script} file.")

    def _load_run_dict(self):
        if os.path.isfile(self._run_dict):
            loader = importlib.machinery.SourceFileLoader("run_dict", self._run_dict)
            spec = importlib.util.spec_from_loader(loader.name, loader)
            module = importlib.util.module_from_spec(spec)
            loader.exec_module(module)
            self._app_native_task_dict = module.APP_NATIVE_TASK_DICT
            self._app_extra_task_dict = module.APP_EXTRA_TASK_DICT
            self._tst_task_filt = module.TST_TASK_FILT

    def _dump_run_dict(self):
        orig_content = "#!/usr/bin/env python3\n\n"
        orig_content += f"APP_NATIVE_TASK_DICT = {ast.literal_eval(str(self._app_native_task_dict))}\n"
        orig_content += f"APP_EXTRA_TASK_DICT = {ast.literal_eval(str(self._app_extra_task_dict))}\n"
        orig_content += f"TST_TASK_FILT = {ast.literal_eval(str(self._tst_task_filt))}\n"

        with open(self._run_dict, "wt", encoding="utf-8") as run_dict_content:
            fcntl.flock(run_dict_content.fileno(), fcntl.LOCK_EX)
            try:
                black = importlib.import_module("black")
                formatted_content = black.format_str(orig_content, mode=black.FileMode(line_length=120))
                run_dict_content.write(formatted_content)
                print(formatted_content)
            except ImportError:
                run_dict_content.write(orig_content)
                print(orig_content)
            fcntl.flock(run_dict_content.fileno(), fcntl.LOCK_UN)

    def _prepare(self):
        if not self._marked_options["tst"] and not os.path.isfile(f"{self._app_bin_path}/{self._app_bin_cmd}"):
            TermUtil.exit_with_error("No executable file. Please use the --build option to build it.")
        if self._marked_options["tst"] and not os.path.isfile(f"{self._tst_bin_path}/{self._tst_bin_cmd}"):
            TermUtil.exit_with_error("No executable file for testing. Please use the --build option to build it.")

        if not os.path.exists(self._report_path):
            os.makedirs(self._report_path)
        if not os.path.isfile(self._run_console_batch):
            pathlib.Path(self._run_console_batch).write_text("# console option\n\nusage\nquit\n", "utf-8")

        self._stream_logger = StreamLogger(self._run_log_file)
        self._progress_bar.setup_progress_bar()
        sys.stdout = self._stream_logger

        self._load_run_dict()
        console_mode_arg = "--console"
        if console_mode_arg in self._app_native_task_dict:
            self._total_steps += len(self._app_native_task_dict[console_mode_arg])
        self._total_steps += len(self._app_native_task_dict.keys())
        for task_category_list in self._app_native_task_dict.values():
            self._total_steps += len(task_category_list)
        for sub_cli_map in self._app_extra_task_dict.values():
            self._total_steps += 1 + len(sub_cli_map.keys())
            for task_category_list in sub_cli_map.values():
                if task_category_list:
                    self._total_steps += len(task_category_list) + 1
        self._total_steps *= self._repeat_count

    def _complete(self):
        if self._marked_options["chk"]["cov"]:
            self._check_coverage_handling()
        if self._marked_options["chk"]["mem"]:
            self._check_memory_handling()

        sys.stdout = STDOUT
        self._progress_bar.destroy_progress_bar()
        del self._stream_logger

    def _generate_tasks(self):
        console_mode_arg = "--console"
        while self._repeat_count:
            if console_mode_arg in self._app_native_task_dict:
                for console_cmd in self._app_native_task_dict[console_mode_arg]:
                    self._task_queue.put((self._app_bin_cmd, f"{console_cmd}\nquit"))

            for category, option_list in self._app_native_task_dict.items():
                self._task_queue.put((f"{self._app_bin_cmd} {category}", ""))
                for option in option_list:
                    option = option.replace("\\", "\\\\\\").replace('"', '\\"', 1)
                    option = '\\"'.join(option.rsplit('"', 1))
                    self._task_queue.put((f"{self._app_bin_cmd} {category} \"{option}\"", ""))

            for sub_cli, category_map in self._app_extra_task_dict.items():
                self._task_queue.put((f"{self._app_bin_cmd} {sub_cli}", ""))
                for category, choice_list in category_map.items():
                    self._task_queue.put((f"{self._app_bin_cmd} {sub_cli} {category}", ""))
                    for choice in choice_list:
                        self._task_queue.put((f"{self._app_bin_cmd} {sub_cli} {category} {choice}", ""))
                    if choice_list:
                        self._task_queue.put((f"{self._app_bin_cmd} {sub_cli} {category} {' '.join(choice_list)}", ""))
            self._repeat_count -= 1

    def _perform_tasks(self):
        while not self._task_queue.empty():
            command, enter = self._task_queue.get()
            self._run_single_task(command, enter)

    def _run_single_task(self, command: str, enter: str = ""):
        full_cmd = (
            f"{self._app_bin_path}/{command}"
            if not self._marked_options["tst"]
            else f"{self._tst_bin_path}/{command} --gtest_color=yes"
        )
        if self._marked_options["chk"]["mem"]:
            full_cmd = f"valgrind --tool=memcheck --xml=yes \
--xml-file={self._report_path}/dca/chk_mem/foo_chk_mem_{str(self._complete_steps + 1)}.xml {full_cmd}"
        if self._marked_options["chk"]["cov"]:
            full_cmd = f"LLVM_PROFILE_FILE=\
\"{self._report_path}/dca/chk_cov/foo_chk_cov_{str(self._complete_steps + 1)}.profraw\" {full_cmd}"
        if enter:
            command += " > " + enter.replace("\nquit", "")
        align_len = max(
            len(command) + self._stat_at_least_len,
            self._stat_default_len,
            len(str(self._total_steps)) * 2 + len(" / ") + self._stat_at_least_len,
        )
        self._hint_with_highlight(
            self._esc_color["blue"], f"CASE: {f'{command}':<{align_len - self._stat_at_least_len}} # START "
        )

        stdout, stderr, return_code = execute_command(full_cmd, enter)
        if not stdout or stderr or return_code:
            print(f"\n[STDOUT]\n{stdout}\n[STDERR]\n{stderr}\n[RETURN CODE]\n{return_code}")
            self._hint_with_highlight(
                self._esc_color["red"], f"{f'STAT: FAILURE NO.{str(self._complete_steps + 1)}':<{align_len}}"
            )
        else:
            stdout = stdout.expandtabs()
            print(stdout)
            self._passed_steps += 1
            if any(sus in stdout for sus in self._suspicious_trait):
                self._passed_steps -= 1
                self._hint_with_highlight(
                    self._esc_color["red"], f"{f'STAT: FAILURE NO.{str(self._complete_steps + 1)}':<{align_len}}"
                )
            elif self._marked_options["chk"]["mem"]:
                self._convert_valgrind_output(command, align_len)

        self._complete_steps += 1
        self._hint_with_highlight(
            self._esc_color["blue"], f"CASE: {f'{command}':<{align_len - self._stat_at_least_len}} # FINISH"
        )

        stat = "SUCCESS" if self._passed_steps == self._complete_steps else "PARTIAL"
        stat_color = (
            self._esc_color["yellow"]
            if stat != "SUCCESS" or self._complete_steps != self._total_steps
            else self._esc_color["green"]
        )
        self._hint_with_highlight(
            stat_color,
            f"""{f"STAT: {stat} \
{f'{str(self._passed_steps)}':>{len(str(self._total_steps))}} / {str(self._total_steps)}":<{align_len}}""",
        )

        print("\n")

        sys.stdout = STDOUT
        self._progress_bar.draw_progress_bar(int(self._complete_steps / self._total_steps * 100))
        sys.stdout = self._stream_logger

    def _hint_with_highlight(self, color: str, hint: str):
        print(f"""{color}\033[1m\033[49m[ {datetime.strftime(datetime.now(), "%b %d %H:%M:%S")} # {hint} ]\033[0m""")

    def _initialize_for_check_coverage(self):
        stdout, _, _ = execute_command("command -v llvm-profdata-16 llvm-cov-16 2>&1")
        if stdout.find("llvm-profdata-16") != -1 and stdout.find("llvm-cov-16") != -1:
            os.environ["FOO_BLD_COV"] = "llvm-cov"
            self._marked_options["chk"]["cov"] = True
            execute_command(f"rm -rf {self._report_path}/dca/chk_cov && mkdir -p {self._report_path}/dca/chk_cov")
        else:
            TermUtil.exit_with_error("No llvm-profdata or llvm-cov program. Please check it.")

    def _check_coverage_handling(self):
        folder_path = f"{self._report_path}/dca/chk_cov"
        execute_command(
            f"llvm-profdata-16 merge -sparse {folder_path}/foo_chk_cov_*.profraw -o {folder_path}/foo_chk_cov.profdata"
        )
        if not self._marked_options["tst"]:
            execute_command(
                f"llvm-cov-16 show -instr-profile={folder_path}/foo_chk_cov.profdata -show-branches=percent \
-show-expansions -show-regions -show-line-counts-or-regions -format=html -output-dir={folder_path} -Xdemangler=c++filt \
-object={self._app_bin_path}/{self._app_bin_cmd} \
{' '.join([f'-object={self._lib_path}/{lib}' for lib in self._lib_list])} 2>&1"
            )
        else:
            execute_command(
                f"llvm-cov-16 show -instr-profile={folder_path}/foo_chk_cov.profdata -show-branches=percent \
-show-expansions -show-regions -show-line-counts-or-regions -format=html -output-dir={folder_path} -Xdemangler=c++filt \
-object={self._tst_bin_path}/{self._tst_bin_cmd} 2>&1"
            )
        stdout, _, _ = (
            execute_command(
                f"llvm-cov-16 report -instr-profile={folder_path}/foo_chk_cov.profdata \
-object={self._app_bin_path}/{self._app_bin_cmd} \
{' '.join([f'-object={self._lib_path}/{lib}' for lib in self._lib_list])} 2>&1"
            )
            if not self._marked_options["tst"]
            else execute_command(
                f"llvm-cov-16 report -instr-profile={folder_path}/foo_chk_cov.profdata \
-object={self._tst_bin_path}/{self._tst_bin_cmd} 2>&1"
            )
        )
        execute_command(f"rm -rf {folder_path}/{{*.profraw,*.profdata}}")
        print(f"\n[CHECK COVERAGE]\n{stdout}")
        if "error" in stdout:
            print("Please further check the compilation parameters related to instrumentation.")

    def _initialize_for_check_memory(self):
        stdout, _, _ = execute_command("command -v valgrind valgrind-ci 2>&1")
        if stdout.find("valgrind") != -1 and stdout.find("valgrind-ci") != -1:
            self._marked_options["chk"]["mem"] = True
            execute_command(f"rm -rf {self._report_path}/dca/chk_mem && mkdir -p {self._report_path}/dca/chk_mem")
        else:
            TermUtil.exit_with_error("No valgrind (including valgrind-ci) program. Please check it.")

    def _check_memory_handling(self):
        execute_command(f"rm -rf {self._report_path}/dca/chk_mem/*.xml")
        folder_path = f"{self._report_path}/dca/chk_mem/memory"
        if not os.path.exists(folder_path):
            os.mkdir(folder_path)
        case_folders = [folder.path for folder in os.scandir(folder_path) if folder.is_dir()]
        case_folders_with_ctime = [(folder, os.path.getctime(folder)) for folder in case_folders]
        case_folders_with_ctime.sort(key=lambda pair: pair[1])
        sorted_case_folders = [os.path.basename(folder[0]) for folder in case_folders_with_ctime]
        case_names = [
            pathlib.Path(f"{folder[0]}/case_name")
            .read_text("utf-8")
            .replace(f"{self._app_bin_cmd} > ", f"{self._app_bin_cmd} &gt; ")
            for folder in case_folders_with_ctime
        ]
        stdout, _, _ = execute_command("pip3 show ValgrindCI")
        pkg_loc = ""
        for line in stdout.splitlines():
            if line.startswith("Location:"):
                pkg_loc = line.split(":", 1)[1].strip()
                break
        pkg_loc += "/ValgrindCI/data"
        if (
            not os.path.isfile(f"{pkg_loc}/index.html")
            or not os.path.isfile(f"{pkg_loc}/valgrind.css")
            or not os.path.isfile(f"{pkg_loc}/valgrind.js")
        ):
            print("\n[CHECK MEMORY]\nMissing source files prevent indexing.")
        execute_command(f"cp -rf {pkg_loc}/{{index.html,valgrind.css,valgrind.js}} {self._report_path}/dca/chk_mem/")

        with open(f"{self._report_path}/dca/chk_mem/index.html", "rt", encoding="utf-8") as index_content:
            fcntl.flock(index_content.fileno(), fcntl.LOCK_EX)
            old_content = index_content.read()
            fcntl.flock(index_content.fileno(), fcntl.LOCK_UN)
        multi_inst_count = sum(1 for sub in sorted_case_folders if "_inst_" in sub)
        fail_case_num = len(sorted_case_folders) - multi_inst_count + int(multi_inst_count / 2)
        exec_date = datetime.strftime(datetime.now(), "%Y-%m-%d")
        old_content = (
            old_content.replace(
                r"<p><b>{{ num_errors }}</b> errors</p>", f"<p><b>{fail_case_num}</b> failure cases on {exec_date}</p>"
            )
            .replace(r"<th>Source file name</th>", "<th>Case index</th>")
            .replace(r"<th>Valgrind errors</th>", "<th>Case name</th>")
        )

        new_body = ""
        for subfolder, case_name in zip(sorted_case_folders, case_names):
            new_body += "                <tr>\n"
            new_body += f"                    <td> <a href=\"memory/{subfolder}/index.html\">{subfolder}</a></td>\n"
            new_body += f"                    <td style=\"text-align: left;\"> {case_name} </td>\n"
            new_body += "                </tr>\n"
        new_content = re.sub(
            r"(                {% for item in source_list %}.*?{% endfor %}\n)", new_body, old_content, flags=re.DOTALL
        )
        with open(f"{self._report_path}/dca/chk_mem/index.html", "wt", encoding="utf-8") as index_content:
            fcntl.flock(index_content.fileno(), fcntl.LOCK_EX)
            index_content.write(new_content)
            fcntl.flock(index_content.fileno(), fcntl.LOCK_UN)

    def _convert_valgrind_output(self, command: str, align_len: int):
        xml_filename = f"{self._report_path}/dca/chk_mem/foo_chk_mem_{str(self._complete_steps + 1)}"
        with open(f"{xml_filename}.xml", "rt", encoding="utf-8") as mem_xml:
            inst_num = mem_xml.read().count("</valgrindoutput>")
        stdout = ""
        stderr = ""
        if inst_num == 1:
            stdout, stderr, _ = execute_command(f"valgrind-ci {xml_filename}.xml --summary")
        elif inst_num == 2:
            execute_command(
                f"cp {xml_filename}.xml {xml_filename}_inst_1.xml && mv {xml_filename}.xml {xml_filename}_inst_2.xml"
            )
            execute_command(
                f"a=$(sed -n '/<\\/status>/!d;=;Q' {xml_filename}_inst_1.xml); \
b=$(sed -n '/<\\/valgrindoutput>/!d;=;Q' {xml_filename}_inst_1.xml); \
sed -i $(($a + 1)),$(($b))d {xml_filename}_inst_1.xml"
            )
            execute_command(f"sed -i '/<\\/valgrindoutput>/q' {xml_filename}_inst_2.xml")
            stdout, stderr, _ = execute_command(
                f"valgrind-ci {xml_filename}_inst_1.xml --summary && valgrind-ci {xml_filename}_inst_2.xml --summary"
            )

        if "errors" in stdout:
            stdout = stdout.expandtabs()
            print(f"\n[CHECK MEMORY]\n{stdout}")
            case_path = f"{self._report_path}/dca/chk_mem/memory/case_{str(self._complete_steps + 1)}"
            if inst_num == 1:
                execute_command(f"valgrind-ci {xml_filename}.xml --source-dir=./ --output-dir={case_path}")
                pathlib.Path(f"{case_path}/case_name").write_text(command, "utf-8")
            elif inst_num == 2:
                execute_command(
                    f"valgrind-ci {xml_filename}_inst_1.xml --source-dir=./ --output-dir={case_path}_inst_1"
                )
                pathlib.Path(f"{case_path}_inst_1/case_name").write_text(command, "utf-8")
                execute_command(
                    f"valgrind-ci {xml_filename}_inst_2.xml --source-dir=./ --output-dir={case_path}_inst_2"
                )
                pathlib.Path(f"{case_path}_inst_2/case_name").write_text(command, "utf-8")
            self._passed_steps -= 1
            self._hint_with_highlight(
                self._esc_color["red"], f"{f'STAT: FAILURE NO.{str(self._complete_steps + 1)}':<{align_len}}"
            )
        elif inst_num not in (1, 2) or stderr:
            self._passed_steps -= 1
            print("\n[CHECK MEMORY]\nUnsupported valgrind output xml file content.")
            self._hint_with_highlight(
                self._esc_color["red"], f"{f'STAT: FAILURE NO.{str(self._complete_steps + 1)}':<{align_len}}"
            )

    def _format_run_log(self):
        with open(self._run_log_file, "rt", encoding="utf-8") as run_log:
            fcntl.flock(run_log.fileno(), fcntl.LOCK_EX)
            old_content = run_log.read()
            fcntl.flock(run_log.fileno(), fcntl.LOCK_UN)
        new_content = re.compile(r"\x1B(?:\[[0-?]*[ -/]*[@-~]|[PX^_].*?\x1B\\|.)").sub("", old_content)
        with open(self._run_log_file, "wt", encoding="utf-8") as run_log:
            fcntl.flock(run_log.fileno(), fcntl.LOCK_EX)
            run_log.write(new_content)
            fcntl.flock(run_log.fileno(), fcntl.LOCK_UN)

    def _summarize_run_log(self):
        tags = {"tst": False, "chk": {"cov": False, "mem": False}}
        with open(self._run_log_file, "rt", encoding="utf-8") as run_log:
            fcntl.flock(run_log.fileno(), fcntl.LOCK_EX)
            readlines = run_log.readlines()
            run_log.seek(0)
            content = run_log.read()
            if self._tst_bin_cmd in content:
                tags["tst"] = True
            if "[CHECK COVERAGE]" in content:
                tags["chk"]["cov"] = True
            if "[CHECK MEMORY]" in content:
                tags["chk"]["mem"] = True
            fcntl.flock(run_log.fileno(), fcntl.LOCK_UN)
        if not self._analyze_only and (
            tags["tst"] != self._marked_options["tst"]
            or tags["chk"]["cov"] != self._marked_options["chk"]["cov"]
            or (tags["chk"]["mem"] and not self._marked_options["chk"]["mem"])
        ):
            TermUtil.exit_with_error(
                f"Run options do not match the actual contents of the run log {self.log_file} file."
            )

        start_indices = []
        finish_indices = []
        for index, line in enumerate(readlines):
            if "# START" in line:
                start_indices.append(index)
            elif "# FINISH" in line:
                finish_indices.append(index)
        if (
            not start_indices
            or not finish_indices
            or len(start_indices) != len(finish_indices)
            or (not self._analyze_only and len(finish_indices) != self._total_steps)
        ):
            TermUtil.exit_with_error(f"The run log {self._run_log_file} file is incomplete. Please retry.")

        dur_time, fail_res, cov_per, mem_leak = self._analyze_for_report(readlines, start_indices, finish_indices, tags)
        with open(self._run_report_file, "wt", encoding="utf-8") as run_report:
            fcntl.flock(run_report.fileno(), fcntl.LOCK_EX)
            run_stat = {
                "Summary": "Unstable" if fail_res else "Stable",
                "Passed": str(len(finish_indices) - len(fail_res)),
                "Failed": str(len(fail_res)),
                "Duration": f"{self._duration} s" if not self._analyze_only else f"{dur_time} s",
            }
            hint = " (UNIT TEST)" if tags["tst"] else ""
            run_stat_rep = (
                "REPORT FOR RUN STATISTICS:\n"
                + self._format_as_table(run_stat, "ENTRY", f"RUN STATISTICS{hint}")
                + "\n"
            )
            fail_res_rep = (
                ("\nREPORT FOR FAILURE RESULT:\n" + self._format_as_table(fail_res, "CASE", "FAILURE RESULT") + "\n")
                if fail_res
                else ""
            )
            cov_per_rep = (
                (
                    "\nREPORT FOR COVERAGE PERCENT:\n"
                    + self._format_as_table(cov_per, "ENTRY", "COVERAGE PERCENT")
                    + "\n"
                )
                if cov_per
                else ""
            )
            mem_err_rep = (
                ("\nREPORT FOR MEMORY LEAK:\n" + self._format_as_table(mem_leak, "CASE", "MEMORY LEAK") + "\n")
                if mem_leak
                else ""
            )
            summary = run_stat_rep + fail_res_rep + cov_per_rep + mem_err_rep
            run_report.write(summary)
            if self._analyze_only:
                print(summary)
            fcntl.flock(run_report.fileno(), fcntl.LOCK_UN)

        if fail_res:
            sys.exit(1)

    def _analyze_for_report(
        self,
        readlines: list[str],
        start_indices: list[int],
        finish_indices: list[int],
        tags: dict[str, bool | dict[str, bool]],
    ) -> tuple[dict[str, str], dict[str, str], dict[str, str]]:
        begin_date_match = re.search(r"(\b[a-zA-Z]{3} \d{2} \d{2}:\d{2}:\d{2}\b)", readlines[start_indices[0]])
        end_date_match = re.search(r"(\b[a-zA-Z]{3} \d{2} \d{2}:\d{2}:\d{2}\b)", readlines[finish_indices[-1]])
        dur_time = (
            (
                datetime.strptime(end_date_match.group(), "%b %d %H:%M:%S")
                - datetime.strptime(begin_date_match.group(), "%b %d %H:%M:%S")
            ).total_seconds()
            if begin_date_match and end_date_match
            else 0.0
        )

        fail_res = {}
        cov_per = {}
        mem_leak = {}
        for start_index, finish_index in zip(start_indices, finish_indices):
            if "# STAT: FAILURE" in readlines[finish_index - 1]:
                fail_line = readlines[finish_index - 1]
                number = fail_line[fail_line.find("NO.") : fail_line.find("]")].strip()
                cmd_line = readlines[start_index]
                case_task = (
                    number
                    + ": "
                    + cmd_line[
                        cmd_line.find(self._app_bin_cmd if not tags["tst"] else self._tst_bin_cmd) : cmd_line.find(
                            "# START"
                        )
                    ].strip()
                )

                if tags["tst"] and "FAILED TEST" in "".join(readlines[start_index + 1 : finish_index]):
                    ut_case = ""
                    ut_run_index = -1
                    ut_run_indices = []
                    ut_fail_indices = []
                    for index, line in enumerate(readlines[start_index + 1 : finish_index]):
                        index += start_index + 1
                        if "[ RUN      ]" in line:
                            ut_case = line[line.find("]") + 2 : line.find("\n")]
                            ut_run_index = index
                        elif ut_case and f"[  FAILED  ] {ut_case}" in line:
                            ut_run_indices.append(ut_run_index)
                            ut_fail_indices.append(index)
                            ut_case = ""
                    ut_fail_res = ""
                    for ut_run_index, ut_fail_index in zip(ut_run_indices, ut_fail_indices):
                        ut_fail_res += "".join(readlines[ut_run_index : ut_fail_index + 1])
                    fail_res[case_task] = ut_fail_res
                    continue

                last_index = finish_index
                if tags["chk"]["mem"]:
                    for index, line in enumerate(readlines[start_index + 1 : finish_index]):
                        if "[CHECK MEMORY]" in line:
                            index += start_index + 1
                            mem_leak[case_task] = "".join(readlines[index + 1 : finish_index - 1])
                            last_index = index
                fail_res[case_task] = "".join(readlines[start_index + 1 : last_index - 1])

        if tags["chk"]["cov"]:
            category = ["Regions", "Functions", "Lines", "Branches"]
            for line in readlines:
                if re.search(r"(^TOTAL(\s+\d+\s+\d+\s+\d+\.\d+%){4}$)", line):
                    matches = re.findall(r"(\d+\.\d+%)", line)
                    percentages = [float(s.strip("%")) for s in matches]
                    if len(percentages) == 4:
                        for index, per in enumerate(percentages):
                            cov_per[category[index]] = f"{per:.2f}%"
                    break
            if not cov_per:
                for cat in category:
                    cov_per[cat] = "-"

        return dur_time, fail_res, cov_per, mem_leak

    def _format_as_table(self, data: dict[str, str], key_title: str, value_title: str) -> str:
        rows = []
        rows.append("=" * (self._tbl_key_width + 2 + self._tbl_value_width + 1))
        rows.append(f"{key_title.ljust(self._tbl_key_width)} | {value_title.ljust(self._tbl_value_width)}")
        rows.append("=" * (self._tbl_key_width + 2 + self._tbl_value_width + 1))

        for key, value in data.items():
            key_lines = []
            for line in key.split("\n"):
                line_len = len(line)
                for index in range(0, line_len, self._tbl_key_width):
                    key_lines.append(line[index : index + self._tbl_key_width].ljust(self._tbl_key_width))

            value_lines = []
            for line in value.split("\n"):
                line_len = len(line)
                for index in range(0, line_len, self._tbl_value_width):
                    value_lines.append(line[index : index + self._tbl_value_width].ljust(self._tbl_value_width))

            line_count = max(len(key_lines), len(value_lines))
            for index in range(line_count):
                new_line = ""
                if index < len(key_lines):
                    new_line = f"{key_lines[index]} | "
                else:
                    new_line = f"{' ' * self._tbl_key_width} | "
                if index < len(value_lines):
                    new_line += value_lines[index]
                else:
                    new_line += " " * self._tbl_value_width
                rows.append(new_line)

            rows.append("-" * (self._tbl_key_width + 2 + self._tbl_value_width + 1))

        return "\n".join(rows)


class TermUtil:
    @staticmethod
    def exit_with_error(message: str):
        print(f"{os.path.basename(__file__)}: {message}", file=sys.stderr)
        sys.exit(1)

    @staticmethod
    def execute_in_pty(command: str):
        master_fd, slave_fd = pty.openpty()
        with subprocess.Popen(
            command,
            executable="/bin/bash",
            stdin=slave_fd,
            stdout=slave_fd,
            stderr=slave_fd,
            close_fds=True,
            shell=True,
            universal_newlines=False,
            encoding="utf-8",
        ) as process:
            os.close(slave_fd)
            buffer = b""
            while True:
                rlist, _, _ = select.select([master_fd], [], [], 0.1)
                if master_fd in rlist:
                    try:
                        chunk = os.read(master_fd, 1024)
                    except OSError as error:
                        if error.errno == errno.EIO:
                            break
                        raise
                    if not chunk:
                        break
                    buffer += chunk
                    lines = buffer.split(b"\n")
                    buffer = lines.pop()
                    for raw in lines:
                        print(raw.decode("utf-8", "ignore"))

                if process.poll() is not None:
                    continue

            return process.wait()


class StreamLogger:
    def __init__(self, filename: str, mode: str = "wt", stream: typing.TextIO = sys.stdout):
        self._terminal = stream
        self._log = open(filename, mode, encoding="utf-8")  # pylint: disable=consider-using-with
        fcntl.flock(self._log.fileno(), fcntl.LOCK_EX | fcntl.LOCK_NB)

    def __del__(self):
        fcntl.flock(self._log.fileno(), fcntl.LOCK_UN)
        self._log.close()

    def write(self, content: str):
        try:
            self._terminal.write(content)
            self._log.write(content)
        except IOError:
            pass

    def flush(self):
        try:
            self._terminal.flush()
            self._log.flush()
        except IOError:
            pass


class ProgressBar:
    _save_cursor = "\033[s"
    _restore_cursor = "\033[u"
    _move_up_cursor = "\033[1A"
    _fore_color = "\033[30m"
    _back_color = "\033[42m"
    _default_fore_color = "\033[39m"
    _default_back_color = "\033[49m"
    _placeholder_length = 20

    def __init__(self):
        self._current_lines = 0
        self._set_trap = False
        self._default_signal = None

    def setup_progress_bar(self):
        curses.setupterm()
        self._trap_due_to_interrupt()

        self._current_lines = self._tput_lines()
        lines = self._current_lines - 1
        self._print_progress("\n")

        self._print_progress(self._save_cursor)
        self._print_progress(f"\033[0;{str(lines)}r")

        self._print_progress(self._restore_cursor)
        self._print_progress(self._move_up_cursor)
        self.draw_progress_bar(0)

    def draw_progress_bar(self, percentage: int):
        lines = self._tput_lines()
        if lines != self._current_lines:
            self.setup_progress_bar()

        self._print_progress(self._save_cursor)
        self._print_progress(f"\033[{str(lines)};0f")

        self._tput()
        self._print_bar(percentage)
        self._print_progress(self._restore_cursor)
        time.sleep(0.01)

    def destroy_progress_bar(self):
        lines = self._tput_lines()
        self._print_progress(self._save_cursor)
        self._print_progress(f"\033[0;{str(lines)}r")

        self._print_progress(self._restore_cursor)
        self._print_progress(self._move_up_cursor)

        self._clear_progress_bar()
        self._print_progress("\n\n")
        if self._set_trap:
            signal.signal(signal.SIGINT, self._default_signal)

    def _trap_due_to_interrupt(self):
        self._set_trap = True
        self._default_signal = signal.getsignal(signal.SIGINT)
        signal.signal(signal.SIGINT, self._clear_due_to_interrupt)

    def _clear_due_to_interrupt(self, *args, **kwargs):  # pylint: disable=unused-argument
        self.destroy_progress_bar()
        raise KeyboardInterrupt

    @classmethod
    def _clear_progress_bar(cls):
        lines = cls._tput_lines()
        cls._print_progress(cls._save_cursor)
        cls._print_progress(f"\033[{str(lines)};0f")

        cls._tput()
        cls._print_progress(cls._restore_cursor)

    @classmethod
    def _print_bar(cls, percentage: int):
        cols = cls._tput_cols()
        bar_size = cols - cls._placeholder_length
        color = f"{cls._fore_color}{cls._back_color}"
        default_color = f"{cls._default_fore_color}{cls._default_back_color}"

        completed_size = int((bar_size * percentage) / 100)
        remaining_size = bar_size - completed_size
        progress_bar = f"[{color}{'#' * int(completed_size)}{default_color}{'.' * int(remaining_size)}]"
        cls._print_progress(f" Progress {percentage:>3}% {progress_bar}\r")

    @staticmethod
    def _print_progress(text: str):
        print(text, end="")

    @staticmethod
    def _tput_lines() -> int:
        _, lines = shutil.get_terminal_size()
        return int(lines)

    @staticmethod
    def _tput_cols() -> int:
        cols, _ = shutil.get_terminal_size()
        return int(cols)

    @staticmethod
    def _tput():
        print(curses.tparm(curses.tigetstr("el")).decode(), end="")


def execute_command(
    command: str, input: str = "", timeout: int = 300  # pylint: disable=redefined-builtin
) -> tuple[str, str, int]:
    try:
        process = subprocess.run(
            command,
            executable="/bin/bash",
            shell=True,
            universal_newlines=True,
            capture_output=True,
            check=True,
            encoding="utf-8",
            input=input,
            timeout=timeout,
        )
        return process.stdout.strip(), process.stderr.strip(), process.returncode
    except subprocess.CalledProcessError as error:
        return error.stdout.strip(), error.stderr.strip(), error.returncode
    except subprocess.TimeoutExpired as error:
        return "", str(error), 124


def main():
    task = None
    try:
        task = Task()
        task.run()
    except Exception:  # pylint: disable=broad-except
        if task is not None:
            task.stop(f"Raised exception.\n{traceback.format_exc()}")
        else:
            raise
    except KeyboardInterrupt:
        if task is not None:
            task.stop()


if __name__ == "__main__":
    try:
        main()
    except Exception:  # pylint: disable=broad-except
        TermUtil.exit_with_error(f"Raised exception.\n{traceback.format_exc()}")
