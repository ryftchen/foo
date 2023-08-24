#!/usr/bin/env python3

try:
    import argparse
    import fcntl
    import fnmatch
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

STDOUT = sys.stdout


class Task:
    app_bin_cmd = "foo"
    app_bin_dir = "./build/bin"
    tst_bin_cmd = "foo_test"
    tst_bin_dir = "./test/build/bin"
    lib_list = ["libfoo_util.so", "libfoo_algo.so", "libfoo_ds.so", "libfoo_dp.so", "libfoo_num.so"]
    lib_dir = "./build/lib"
    basic_task_dict = {
        "--help": [],
        "--version": [],
        "--console": [
            r"'help'",
            r"'quit'",
            r"'batch " + os.path.abspath("./script/console_batch.txt") + r"'",
            r"'refresh'",
            r"'reconnect'",
            r"'log'",
            r"'stat'",
        ],
    }
    regular_task_dict = {
        "app-algo": {
            "--help": [],
            "--match": ["rab", "knu", "boy", "hor", "sun"],
            "--notation": ["pre", "pos"],
            "--optimal": ["gra", "ann", "par", "gen"],
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
    options = {"tst": False, "chk_cov": False, "chk_mem": False}
    build_script = "./script/build.sh"
    cache_dir = "./.cache"
    log_file = f"{cache_dir}/foo_run.log"
    report_file = f"{cache_dir}/foo_run.report"

    def __init__(self):
        self.pass_steps = 0
        self.complete_steps = 0
        self.total_steps = 0
        self.repeat_count = 1

        if not os.path.exists(self.cache_dir):
            os.mkdir(self.cache_dir)
        self.logger = common.Log(self.log_file)
        self.progress_bar = common.ProgressBar()
        self.task_queue = queue.Queue()

    def run(self):
        script_path = os.path.split(os.path.realpath(__file__))[0]
        if not fnmatch.fnmatch(script_path, "*foo/script"):
            Output.exit_with_error("Illegal path to current script.")
        os.chdir(os.path.dirname(script_path))

        self.parse_arguments()
        self.prepare()

        if not self.options["tst"]:
            thread_list = []
            producer = threading.Thread(target=self.generate_tasks(), args=())
            producer.start()
            thread_list.append(producer)
            consumer = threading.Thread(target=self.perform_tasks(), args=())
            consumer.start()
            thread_list.append(consumer)
            for thread in thread_list:
                thread.join()
        else:
            self.total_steps = self.repeat_count
            while self.repeat_count:
                self.run_task(self.tst_bin_cmd)
                self.repeat_count -= 1

        self.complete()
        self.format_run_log()
        self.summarize_run_log()

    def stop(self, message=""):
        try:
            if self.options["chk_cov"]:
                common.execute_command(f"rm -rf {self.cache_dir}/*.profraw {self.cache_dir}/*.profdata")
            if self.options["chk_mem"]:
                common.execute_command(f"rm -rf {self.cache_dir}/*.xml")
            sys.stdout = STDOUT
            self.progress_bar.destroy_progress_bar()
            del self.logger
            self.format_run_log()
        except Exception:  # pylint: disable=broad-except
            pass
        finally:
            if message:
                Output.exit_with_error(message)
            sys.exit(1)

    def parse_arguments(self):
        def check_positive(value):
            value = int(value)
            if value <= 0:
                raise argparse.ArgumentTypeError(f"\"{value}\" is an invalid positive int value.")
            return value

        parser = argparse.ArgumentParser(description="run script", formatter_class=argparse.RawTextHelpFormatter)
        parser.add_argument("-t", "--test", action="store_true", default=False, help="run unit test only")
        parser.add_argument(
            "-r", "--repeat", nargs="?", const=1, type=check_positive, help="run repeatedly", metavar="times"
        )
        parser.add_argument(
            "-c",
            "--check",
            nargs="+",
            choices=["cov", "mem"],
            help="run with check\n- cov    coverage\n- mem    memory",
        )
        parser.add_argument(
            "-b",
            "--build",
            nargs="?",
            const="dbg",
            choices=["dbg", "rls"],
            help="run after build\n- dbg    debug\n- rls    release",
        )

        args = parser.parse_args()
        if args.test:
            self.options["tst"] = True

        if args.repeat is not None:
            self.repeat_count = args.repeat

        if args.check is not None:
            if self.options["tst"]:
                Output.exit_with_error("No support for the --check option during testing.")
            if "cov" in args.check:
                stdout, _, _ = common.execute_command("command -v llvm-profdata-12 llvm-cov-12 2>&1")
                if stdout.find("llvm-profdata-12") != -1 and stdout.find("llvm-cov-12") != -1:
                    os.environ["FOO_CHK_COV"] = "on"
                    self.options["chk_cov"] = True
                    common.execute_command(f"rm -rf {self.cache_dir}/coverage")
                else:
                    Output.exit_with_error("No llvm-profdata or llvm-cov program. Please check it.")

            if "mem" in args.check:
                stdout, _, _ = common.execute_command("command -v valgrind valgrind-ci 2>&1")
                if stdout.find("valgrind") != -1 and stdout.find("valgrind-ci") != -1:
                    os.environ["FOO_CHK_MEM"] = "on"
                    self.options["chk_mem"] = True
                    common.execute_command(f"rm -rf {self.cache_dir}/memory")
                else:
                    Output.exit_with_error("No valgrind or valgrind-ci program. Please check it.")

        if args.build is not None:
            if os.path.isfile(self.build_script):
                build_cmd = self.build_script
                if self.options["tst"]:
                    build_cmd += " --test"
                if args.build == "dbg":
                    self.build_executable(f"{build_cmd} 2>&1")
                elif args.build == "rls":
                    self.build_executable(f"{build_cmd} --release 2>&1")
            else:
                Output.exit_with_error(f"No shell script {self.build_script} file.")

    def build_executable(self, build_cmd):
        stdout, stderr, return_code = common.execute_command(build_cmd)
        if stderr or return_code != 0:
            print(f"\r\n[STDOUT]\n{stdout}\n[STDERR]\n{stderr}\n[RETURN CODE]\n{return_code}")
            Output.exit_with_error(f"Failed to run shell script {self.build_script} file.")
        else:
            print(stdout)
            if "FAILED:" in stdout:
                Output.exit_with_error(f"Failed to build the executable by shell script {self.build_script} file.")

    def prepare(self):
        if not self.options["tst"] and not os.path.isfile(f"{self.app_bin_dir}/{self.app_bin_cmd}"):
            Output.exit_with_error("No executable file. Please use the --build option to build it.")
        if self.options["tst"] and not os.path.isfile(f"{self.tst_bin_dir}/{self.tst_bin_cmd}"):
            Output.exit_with_error("No executable file for testing. Please use the --build option to build it.")

        if not os.path.exists(self.cache_dir):
            os.makedirs(self.cache_dir)
        self.progress_bar.setup_progress_bar()
        sys.stdout = self.logger

        self.total_steps = len(self.basic_task_dict["--console"])
        self.total_steps += len(self.basic_task_dict.keys())
        for task_category_list in self.basic_task_dict.values():
            self.total_steps += len(task_category_list)
        for sub_cli_map in self.regular_task_dict.values():
            self.total_steps += 1 + len(sub_cli_map.keys())
            for task_category_list in sub_cli_map.values():
                if task_category_list:
                    self.total_steps += len(task_category_list) + 1
        self.total_steps *= self.repeat_count

    def complete(self):
        if self.options["chk_mem"]:
            common.execute_command(f"rm -rf {self.cache_dir}/*.xml")

        if self.options["chk_cov"]:
            self.check_coverage()

        sys.stdout = STDOUT
        self.progress_bar.destroy_progress_bar()
        del self.logger

    def generate_tasks(self):
        while self.repeat_count:
            for target_task in self.basic_task_dict["--console"]:
                target_task = target_task.replace("'", "")
                self.task_queue.put((self.app_bin_cmd, f"{target_task}\nquit"))

            for task_category, task_category_list in self.basic_task_dict.items():
                self.task_queue.put((f"{self.app_bin_cmd} {task_category}", ""))
                for target_task in task_category_list:
                    self.task_queue.put((f"{self.app_bin_cmd} {task_category} {target_task}", ""))

            for sub_cli, sub_cli_map in self.regular_task_dict.items():
                self.task_queue.put((f"{self.app_bin_cmd} {sub_cli}", ""))
                for task_category, target_task_list in sub_cli_map.items():
                    self.task_queue.put((f"{self.app_bin_cmd} {sub_cli} {task_category}", ""))
                    for target_task in target_task_list:
                        self.task_queue.put((f"{self.app_bin_cmd} {sub_cli} {task_category} {target_task}", ""))
                    if target_task_list:
                        self.task_queue.put(
                            (f"{self.app_bin_cmd} {sub_cli} {task_category} {' '.join(target_task_list)}", "")
                        )
            self.repeat_count -= 1

    def perform_tasks(self):
        while not self.task_queue.empty():
            command, enter = self.task_queue.get()
            self.run_task(command, enter)

    def run_task(self, command, enter=""):
        if not self.options["tst"]:
            full_cmd = f"{self.app_bin_dir}/{command}"
        else:
            full_cmd = f"{self.tst_bin_dir}/{command}"
        if self.options["chk_mem"]:
            full_cmd = f"valgrind --tool=memcheck --xml=yes \
--xml-file={self.cache_dir}/foo_chk_mem_{str(self.complete_steps + 1)}.xml {full_cmd}"
        if self.options["chk_cov"]:
            full_cmd = f"LLVM_PROFILE_FILE=\
\"{os.environ['HOME']}/.foo/foo_chk_cov_{str(self.complete_steps + 1)}.profraw\" {full_cmd}"
        align_len = max(
            len(command) + Output.stat_cont_len_excl_cmd,
            Output.stat_min_cont_len,
            len(str(self.total_steps)) * 2 + len(" / ") + Output.stat_cont_len_excl_cmd,
        )
        Output.refresh_status(
            Output.color["blue"], f"CASE: {f'{command}':<{align_len - Output.stat_cont_len_excl_cmd}} | START "
        )

        stdout, stderr, return_code = common.execute_command(full_cmd, enter)
        if len(stdout.strip()) == 0 or stderr or return_code != 0:
            print(f"\r\n[STDOUT]\n{stdout}\n[STDERR]\n{stderr}\n[RETURN CODE]\n{return_code}")
            Output.refresh_status(
                Output.color["red"], f"{f'STAT: FAILURE NO.{str(self.complete_steps + 1)}':<{align_len}}"
            )
        else:
            stdout = stdout.replace("\t", "    ")
            print(stdout)
            self.pass_steps += 1
            if ("[ERR]" in stdout or "[WRN]" in stdout) and ("log" not in command and "log" not in enter):
                self.pass_steps -= 1
                Output.refresh_status(
                    Output.color["red"], f"{f'STAT: FAILURE NO.{str(self.complete_steps + 1)}':<{align_len}}"
                )
            elif self.options["chk_mem"]:
                self.check_memory(align_len)

        self.complete_steps += 1
        Output.refresh_status(
            Output.color["blue"], f"CASE: {f'{command}':<{align_len - Output.stat_cont_len_excl_cmd}} | FINISH"
        )

        if self.pass_steps != self.total_steps:
            status_color = Output.color["yellow"]
        else:
            status_color = Output.color["green"]
        Output.refresh_status(
            status_color,
            f"""\
{f"STAT: SUCCESS {f'{str(self.pass_steps)}':>{len(str(self.total_steps))}} / {str(self.total_steps)}":<{align_len}}""",
        )
        print("\n")

        sys.stdout = STDOUT
        self.progress_bar.draw_progress_bar(int(self.complete_steps / self.total_steps * 100))
        sys.stdout = self.logger

    def check_coverage(self):
        common.execute_command(f"mv {os.environ['HOME']}/.foo/foo_chk_cov_*.profraw {self.cache_dir}/")
        common.execute_command(
            f"llvm-profdata-12 merge -sparse {self.cache_dir}/foo_chk_cov_*.profraw \
-o {self.cache_dir}/foo_chk_cov.profdata"
        )
        common.execute_command(
            f"llvm-cov-12 show -instr-profile={self.cache_dir}/foo_chk_cov.profdata -show-branches=percent \
-show-expansions -show-regions -show-line-counts-or-regions -format=html -output-dir={self.cache_dir}/coverage \
-Xdemangler=c++filt -object={self.app_bin_dir}/{self.app_bin_cmd} \
{' '.join([f'-object={self.lib_dir}/{lib}' for lib in self.lib_list])} 2>&1"
        )
        stdout, _, _ = common.execute_command(
            f"llvm-cov-12 report -instr-profile={self.cache_dir}/foo_chk_cov.profdata \
-object={self.app_bin_dir}/{self.app_bin_cmd} {' '.join([f'-object={self.lib_dir}/{lib}' for lib in self.lib_list])} \
2>&1"
        )
        common.execute_command(f"rm -rf {self.cache_dir}/*.profraw {self.cache_dir}/*.profdata")
        print(f"\r\n[CHECK COVERAGE]\n{stdout}")
        if "error" in stdout:
            print("Please rebuild the executable file with the --check option.")

    def check_memory(self, align_len):
        inst_num = 0
        xml_filename = f"{self.cache_dir}/foo_chk_mem_{str(self.complete_steps + 1)}"
        with open(f"{xml_filename}.xml", "rt", encoding="utf-8") as mem_xml:
            inst_num = mem_xml.read().count("</valgrindoutput>")
        stdout = ""
        if inst_num == 1:
            stdout, _, _ = common.execute_command(f"valgrind-ci {xml_filename}.xml --summary")
        elif inst_num == 2:
            common.execute_command(
                f"cp {xml_filename}.xml {xml_filename}_inst_1.xml && \
mv {xml_filename}.xml {xml_filename}_inst_2.xml"
            )
            common.execute_command(
                f"a=$(sed -n '/<\\/status>/!d;=;Q' {xml_filename}_inst_1.xml) ; \
b=$(sed -n '/<\\/valgrindoutput>/!d;=;Q' {xml_filename}_inst_1.xml) ; \
sed -i $(($a + 1)),$(($b))d {xml_filename}_inst_1.xml"
            )
            common.execute_command(f"sed -i '/<\\/valgrindoutput>/q' {xml_filename}_inst_2.xml")
            stdout, _, _ = common.execute_command(
                f"valgrind-ci {xml_filename}_inst_1.xml --summary && \
valgrind-ci {xml_filename}_inst_2.xml --summary"
            )

        if "errors" in stdout:
            stdout = stdout.replace("\t", "    ")
            print(f"\r\n[CHECK MEMORY]\n{stdout}")
            if inst_num == 1:
                common.execute_command(
                    f"valgrind-ci {xml_filename}.xml --source-dir=./ \
--output-dir={self.cache_dir}/memory/case_{str(self.complete_steps + 1)}"
                )
            elif inst_num == 2:
                common.execute_command(
                    f"valgrind-ci {xml_filename}_inst_1.xml --source-dir=./ \
--output-dir={self.cache_dir}/memory/case_{str(self.complete_steps + 1)}_inst_1"
                )
                common.execute_command(
                    f"valgrind-ci {xml_filename}_inst_2.xml --source-dir=./ \
--output-dir={self.cache_dir}/memory/case_{str(self.complete_steps + 1)}_inst_2"
                )
            self.pass_steps -= 1
            Output.refresh_status(
                Output.color["red"], f"{f'STAT: FAILURE NO.{str(self.complete_steps + 1)}':<{align_len}}"
            )
        elif inst_num == 0 or inst_num > 2:
            self.pass_steps -= 1
            print("\r\n[CHECK MEMORY]\nUnsupported valgrind output xml file content.")
            Output.refresh_status(
                Output.color["red"], f"{f'STAT: FAILURE NO.{str(self.complete_steps + 1)}':<{align_len}}"
            )

    def format_run_log(self):
        run_log = ""
        with open(self.log_file, "rt", encoding="utf-8") as run_log:
            fcntl.flock(run_log.fileno(), fcntl.LOCK_EX)
            old_content = run_log.read()
            fcntl.flock(run_log.fileno(), fcntl.LOCK_UN)
        new_content = re.sub(Output.color_esc_regex, "", old_content)
        with open(self.log_file, "w", encoding="utf-8") as run_log:
            fcntl.flock(run_log.fileno(), fcntl.LOCK_EX)
            run_log.write(new_content)
            fcntl.flock(run_log.fileno(), fcntl.LOCK_UN)

    def summarize_run_log(self):
        def analyze_for_report(readlines, start_indices, finish_indices, tags):
            fail_res = {}
            cov_per = {}
            mem_err = {}
            for start_index, finish_index in zip(start_indices, finish_indices):
                if "| STAT: FAILURE" in readlines[finish_index - 1]:
                    fail_line = readlines[finish_index - 1]
                    number = fail_line[fail_line.find("NO.") : fail_line.find("]")].strip()
                    cmd_line = readlines[start_index]
                    case_task = (
                        number + ": " + cmd_line[cmd_line.find(self.app_bin_cmd) : cmd_line.find("| START")].strip()
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
                            elif len(ut_case) != 0 and f"[  FAILED  ] {ut_case}" in line:
                                ut_run_indices.append(ut_run_index)
                                ut_fail_indices.append(index)
                                ut_case = ""
                        ut_fail_res = ""
                        for ut_run_index, ut_fail_index in zip(ut_run_indices, ut_fail_indices):
                            ut_fail_res += "".join(readlines[ut_run_index : ut_fail_index + 1])
                        fail_res[case_task] = ut_fail_res
                        continue

                    last_index = finish_index
                    if tags["chk_mem"]:
                        for index, line in enumerate(readlines[start_index + 1 : finish_index]):
                            if "[CHECK MEMORY]" in line:
                                index += start_index + 1
                                mem_err[case_task] = "".join(readlines[index + 1 : finish_index - 1])
                                last_index = index
                    fail_res[case_task] = "".join(readlines[start_index + 1 : last_index - 1])

            if tags["chk_cov"]:
                category = ["Regions", "Functions", "Lines", "Branches"]
                for line in readlines:
                    if re.search(r"(^TOTAL(\s+\d+\s+\d+\s+\d+\.\d+%){4}$)", line):
                        matches = re.findall(r"(\d+\.\d+%)", line)
                        percentages = [str(float(s.strip("%"))) for s in matches]
                        if len(percentages) == 4:
                            for index, per in enumerate(percentages):
                                cov_per[category[index]] = str(per) + "%"
                        break
                if len(cov_per) == 0:
                    for cat in category:
                        cov_per[cat] = "-"

            return fail_res, cov_per, mem_err

        tags = {"tst": False, "chk_cov": False, "chk_mem": False}
        readlines = []
        with open(self.log_file, "rt", encoding="utf-8") as run_log:
            fcntl.flock(run_log.fileno(), fcntl.LOCK_EX)
            readlines = run_log.readlines()
            run_log.seek(0)
            content = run_log.read()
            if self.tst_bin_cmd in content:
                tags["tst"] = True
            if "[CHECK COVERAGE]" in content:
                tags["chk_cov"] = True
            if "[CHECK MEMORY]" in content:
                tags["chk_mem"] = True
            fcntl.flock(run_log.fileno(), fcntl.LOCK_UN)
        if (
            (tags["tst"] != self.options["tst"])
            or (tags["chk_cov"] != self.options["chk_cov"])
            or (tags["chk_mem"] and not self.options["chk_mem"])
        ):
            Output.exit_with_error(f"Run options do not match the actual contents of the run log {self.log_file} file.")
        if tags["tst"] and (tags["chk_cov"] or tags["chk_mem"]):
            Output.exit_with_error(f"The run log {self.log_file} file is complex. Please retry.")

        start_indices = []
        finish_indices = []
        for index, line in enumerate(readlines):
            if "| START" in line:
                start_indices.append(index)
            elif "| FINISH" in line:
                finish_indices.append(index)
        if len(start_indices) != len(finish_indices) or len(start_indices) != self.total_steps:
            Output.exit_with_error(f"The run log {self.log_file} file is incomplete. Please retry.")

        fail_res, cov_per, mem_err = analyze_for_report(readlines, start_indices, finish_indices, tags)
        with open(self.report_file, "w", encoding="utf-8") as run_report:
            fcntl.flock(run_report.fileno(), fcntl.LOCK_EX)
            run_stat = {"Passed": str(self.total_steps - len(fail_res)), "Failed": str(len(fail_res))}
            run_stat_rep = "REPORT FOR RUN STATISTICS"
            if tags["tst"]:
                run_stat_rep += " (UNIT TEST)"
            run_stat_rep += ":\n" + Output().format_as_table(run_stat, "STATUS", "RUN STATISTICS") + "\n\n"
            fail_res_rep = ""
            if fail_res:
                fail_res_rep = (
                    "\r\nREPORT FOR FAILURE RESULT:\n"
                    + Output().format_as_table(fail_res, "CASE", "FAILURE RESULT")
                    + "\n\n"
                )
            cov_per_rep = ""
            if cov_per:
                cov_per_rep = (
                    "\r\nREPORT FOR COVERAGE PERCENT:\n"
                    + Output().format_as_table(cov_per, "CATEGORY", "COVERAGE PERCENT")
                    + "\n\n"
                )
            mem_err_rep = ""
            if mem_err:
                mem_err_rep = (
                    "\r\nREPORT FOR MEMORY ERROR:\n"
                    + Output().format_as_table(mem_err, "CASE", "MEMORY ERROR")
                    + "\n\n"
                )
            run_report.write(run_stat_rep + fail_res_rep + cov_per_rep + mem_err_rep)
            fcntl.flock(run_report.fileno(), fcntl.LOCK_UN)

        if fail_res:
            sys.exit(1)


class Output:
    color = {"red": "\033[0;31;40m", "green": "\033[0;32;40m", "yellow": "\033[0;33;40m", "blue": "\033[0;34;40m"}
    color_bg = "\033[49m"
    color_bold = "\033[1m"
    color_off = "\033[0m"
    color_esc_regex = r"((\033\[.*?(m|s|u|A|J))|(\007|\017))"
    stat_min_cont_len = 55
    stat_cont_len_excl_cmd = 15
    tbl_min_key_width = 20
    tbl_min_value_width = 80

    @classmethod
    def exit_with_error(cls, message):
        print(f"\r\n{os.path.basename(__file__)}: {message}")
        sys.exit(1)

    @classmethod
    def refresh_status(cls, color_fg, content):
        print(
            f"""{color_fg}{cls.color_bold}{cls.color_bg}[ {datetime.strftime(datetime.now(), "%b %d %H:%M:%S")} \
| {content} ]{cls.color_off}"""
        )

    @classmethod
    def format_as_table(cls, data: dict, key_name="", value_name=""):
        rows = []
        rows.append("=" * (cls.tbl_min_key_width + 2 + cls.tbl_min_value_width + 1))
        rows.append(f"{key_name.ljust(cls.tbl_min_key_width)} | {value_name.ljust(cls.tbl_min_value_width)}")
        rows.append("=" * (cls.tbl_min_key_width + 2 + cls.tbl_min_value_width + 1))

        for key, value in data.items():
            key_lines = []
            for line in key.split("\n"):
                line_len = len(line)
                for index in range(0, line_len, cls.tbl_min_key_width):
                    key_lines.append(line[index : index + cls.tbl_min_key_width].ljust(cls.tbl_min_key_width))

            value_lines = []
            for line in value.split("\n"):
                line_len = len(line)
                for index in range(0, line_len, cls.tbl_min_value_width):
                    value_lines.append(line[index : index + cls.tbl_min_value_width].ljust(cls.tbl_min_value_width))

            line_count = max(len(key_lines), len(value_lines))
            for index in range(line_count):
                new_line = ""
                if index < len(key_lines):
                    new_line = f"{key_lines[index]} | "
                else:
                    new_line = f"{' ' * cls.tbl_min_key_width} | "
                if index < len(value_lines):
                    new_line += value_lines[index]
                else:
                    new_line += " " * cls.tbl_min_value_width
                rows.append(new_line)

            rows.append("-" * (cls.tbl_min_key_width + 2 + cls.tbl_min_value_width + 1))
        return "\n".join(rows)


if __name__ == "__main__":
    try:
        os.environ["TERM"] = "linux"
        os.environ["TERMINFO"] = "/etc/terminfo"
        task = Task()
        task.run()
    except Exception:  # pylint: disable=broad-except
        task.stop(traceback.format_exc())
    except KeyboardInterrupt:
        task.stop()
