#!/usr/bin/env python3

try:
    import argparse
    import ast
    import fcntl
    import fnmatch
    import importlib
    import os
    import queue
    import re
    import subprocess
    import sys
    import threading
    import traceback
    from datetime import datetime
    from pathlib import Path
    import common
except ImportError as err:
    raise ImportError(err) from err

STDOUT = sys.stdout


class Task:
    app_bin_cmd = "foo"
    app_bin_path = "./build/bin"
    tst_bin_cmd = "foo_test"
    tst_bin_path = "./test/build/bin"
    lib_list = ["libfoo_util.so", "libfoo_algo.so", "libfoo_ds.so", "libfoo_dp.so", "libfoo_num.so"]
    lib_path = "./build/lib"
    script_path = "./script"
    build_script = f"{script_path}/build.sh"
    run_dict_file = f"{script_path}/.run_dict"
    console_file = f"{script_path}/.console_batch"
    app_native_task_dict = {
        "--help": [],
        "--version": [],
        "--dump": [],
        "--console": [
            r"usage",
            r"quit",
            r"trace",
            r"clean",
            f"batch {console_file}",
            r"refresh",
            r"reconnect",
            r"depend",
            r'''execute "ps -eww -o size,pid,user,command --sort -size | awk '{ hr=\$1/1024 ; printf(\"%6.2fMB \", hr) } { for (x=4 ; x<=NF ; x++) { printf(\"%s \", \$x) } print \"\" }' | cut -d '' -f 2 | sed '1d'"''',  # pylint: disable=line-too-long
            r"journal",
            r"monitor 0",
            r"profile",
        ],
    }
    app_extra_task_dict = {
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
    tst_task_filt = []
    stored_options = {"tst": False, "chk": {"cov": False, "mem": False}}
    analyze_only = False
    report_path = "./report"
    log_file = f"{report_path}/foo_run.log"
    report_file = f"{report_path}/foo_run.report"
    suspicious_output = [" \033[0;31;40m\033[1m\033[49m[ERR]\033[0m ", " \033[0;33;40m\033[1m\033[49m[WRN]\033[0m "]

    def __init__(self):
        self.passed_steps = 0
        self.complete_steps = 0
        self.total_steps = 0
        self.repeat_count = 1
        self.duration = 0

        os.environ["TERM"] = "linux"
        os.environ["TERMINFO"] = "/etc/terminfo"
        script_path = os.path.split(os.path.realpath(__file__))[0]
        if not fnmatch.fnmatch(script_path, "*foo/script"):
            Output.exit_with_error("Illegal path to current script.")
        os.chdir(os.path.dirname(script_path))
        sys.path.append(".")
        if not os.path.exists(self.report_path):
            os.mkdir(self.report_path)
        proc_path = os.path.expanduser("~/.foo")
        if os.path.exists(proc_path):
            common.execute_command(f"rm -rf {proc_path}")

        self.logger = sys.stdout
        self.progress_bar = common.ProgressBar()
        self.task_queue = queue.Queue()

    def parse_arguments(self):
        def check_positive_integer(value):
            value = int(value)
            if value > 0:
                return value
            raise argparse.ArgumentTypeError("Must be a positive integer.")

        parser = argparse.ArgumentParser(description="run script", formatter_class=argparse.RawTextHelpFormatter)
        parser.add_argument("-t", "--test", action="store_true", default=False, help="run unit test only")
        parser.add_argument(
            "-r", "--repeat", nargs="?", const=1, type=check_positive_integer, help="run repeatedly", metavar="TIMES"
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

        self.apply_arguments(parser.parse_args())

    def apply_arguments(self, args):
        if args.analyze:
            if len(sys.argv) > 2:
                Output.exit_with_error("No other arguments are supported during analyzing.")
            self.analyze_only = True
            self.summarize_run_log()
            sys.exit(0)

        if args.dump:
            self.dump_run_dict()
            sys.exit(0)

        if args.test:
            self.stored_options["tst"] = True

        if args.repeat is not None:
            self.repeat_count = args.repeat

        if args.check is not None:
            if args.sanitizer is not None:
                Output.exit_with_error(
                    "It is not possible to use the --check option and the --sanitizer option at the same time."
                )
            if "cov" in args.check:
                if args.build is None:
                    Output.exit_with_error(
                        "Checking coverage requires recompiling with instrumentation. Please add the --build option."
                    )
                if "mem" in args.check:
                    Output.exit_with_error(
                        "Checking coverage and memory at the same time can lead to inaccurate results."
                    )
                self.initialize_for_check_coverage()

            if "mem" in args.check:
                self.initialize_for_check_memory()

        if args.sanitizer is not None:
            if args.build is None:
                Output.exit_with_error(
                    "The runtime sanitizer requires recompiling with instrumentation. Please add the --build option."
                )
            os.environ["FOO_BLD_SAN"] = args.sanitizer

        if args.build is not None:
            if os.path.isfile(self.build_script):
                build_cmd = self.build_script
                if self.stored_options["tst"]:
                    build_cmd += " --test"
                if args.build == "dbg":
                    self.build_executable(f"{build_cmd} 2>&1")
                elif args.build == "rls":
                    self.build_executable(f"{build_cmd} --release 2>&1")
            else:
                Output.exit_with_error(f"No shell script {self.build_script} file.")

    def run(self):
        self.parse_arguments()
        self.prepare()
        start_time = datetime.now()

        if not self.stored_options["tst"]:
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
            command = self.tst_bin_cmd
            if len(self.tst_task_filt) != 0:
                command = f"{self.tst_bin_cmd} {' '.join(self.tst_task_filt)}"
            while self.repeat_count:
                self.run_single_task(command)
                self.repeat_count -= 1

        self.duration = (datetime.now() - start_time).total_seconds()
        self.complete()
        self.format_run_log()
        self.summarize_run_log()

    def stop(self, message=""):
        try:
            if self.stored_options["chk"]["cov"]:
                common.execute_command(f"rm -rf {self.report_path}/dca/chk_cov/{{*.profraw,*.profdata}}")
            if self.stored_options["chk"]["mem"]:
                common.execute_command(f"rm -rf {self.report_path}/dca/chk_mem/*.xml")
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

    def load_run_dict(self):
        if os.path.isfile(self.run_dict_file):
            loader = importlib.machinery.SourceFileLoader("run_dict", self.run_dict_file)
            spec = importlib.util.spec_from_loader(loader.name, loader)
            module = importlib.util.module_from_spec(spec)
            loader.exec_module(module)
            self.app_native_task_dict = module.app_native_task_dict
            self.app_extra_task_dict = module.app_extra_task_dict
            self.tst_task_filt = module.tst_task_filt

    def dump_run_dict(self):
        orig_content = "#!/usr/bin/env python3\n\n"
        orig_content += f"app_native_task_dict = {ast.literal_eval(str(self.app_native_task_dict))}\n"
        orig_content += f"app_extra_task_dict = {ast.literal_eval(str(self.app_extra_task_dict))}\n"
        orig_content += f"tst_task_filt = {ast.literal_eval(str(self.tst_task_filt))}\n"

        with open(self.run_dict_file, "wt", encoding="utf-8") as run_dict_content:
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

    def build_executable(self, build_cmd):
        with subprocess.Popen(
            build_cmd,
            executable="/bin/bash",
            stdout=subprocess.PIPE,
            shell=True,
            universal_newlines=True,
            encoding="utf-8",
        ) as process:
            while True:
                output = process.stdout.readline()
                if len(output) == 0 and process.poll() is not None:
                    break
                if output:
                    print(output.strip())
            return_code = process.poll()
            if return_code != 0:
                Output.exit_with_error(f"Failed to run shell script {self.build_script} file.")

    def prepare(self):
        if not self.stored_options["tst"] and not os.path.isfile(f"{self.app_bin_path}/{self.app_bin_cmd}"):
            Output.exit_with_error("No executable file. Please use the --build option to build it.")
        if self.stored_options["tst"] and not os.path.isfile(f"{self.tst_bin_path}/{self.tst_bin_cmd}"):
            Output.exit_with_error("No executable file for testing. Please use the --build option to build it.")

        if not os.path.exists(self.report_path):
            os.makedirs(self.report_path)
        if not os.path.isfile(self.console_file):
            Path(self.console_file).write_text("# console option\n\nusage\nquit\n", encoding="utf-8")

        self.logger = common.Log(self.log_file)
        self.progress_bar.setup_progress_bar()
        sys.stdout = self.logger

        self.load_run_dict()
        console_mode_arg = "--console"
        if console_mode_arg in self.app_native_task_dict:
            self.total_steps += len(self.app_native_task_dict[console_mode_arg])
        self.total_steps += len(self.app_native_task_dict.keys())
        for task_category_list in self.app_native_task_dict.values():
            self.total_steps += len(task_category_list)
        for sub_cli_map in self.app_extra_task_dict.values():
            self.total_steps += 1 + len(sub_cli_map.keys())
            for task_category_list in sub_cli_map.values():
                if task_category_list:
                    self.total_steps += len(task_category_list) + 1
        self.total_steps *= self.repeat_count

    def complete(self):
        if self.stored_options["chk"]["cov"]:
            self.check_coverage_handling()
        if self.stored_options["chk"]["mem"]:
            self.check_memory_handling()

        sys.stdout = STDOUT
        self.progress_bar.destroy_progress_bar()
        del self.logger

    def generate_tasks(self):
        console_mode_arg = "--console"
        while self.repeat_count:
            if console_mode_arg in self.app_native_task_dict:
                for console_cmd in self.app_native_task_dict[console_mode_arg]:
                    self.task_queue.put((self.app_bin_cmd, f"{console_cmd}\nquit"))

            for category, option_list in self.app_native_task_dict.items():
                self.task_queue.put((f"{self.app_bin_cmd} {category}", ""))
                for option in option_list:
                    option = option.replace("\\", "\\\\\\").replace('"', '\\"', 1)
                    option = '\\"'.join(option.rsplit('"', 1))
                    self.task_queue.put((f"{self.app_bin_cmd} {category} \"{option}\"", ""))

            for sub_cli, category_map in self.app_extra_task_dict.items():
                self.task_queue.put((f"{self.app_bin_cmd} {sub_cli}", ""))
                for category, choice_list in category_map.items():
                    self.task_queue.put((f"{self.app_bin_cmd} {sub_cli} {category}", ""))
                    for choice in choice_list:
                        self.task_queue.put((f"{self.app_bin_cmd} {sub_cli} {category} {choice}", ""))
                    if choice_list:
                        self.task_queue.put((f"{self.app_bin_cmd} {sub_cli} {category} {' '.join(choice_list)}", ""))
            self.repeat_count -= 1

    def perform_tasks(self):
        while not self.task_queue.empty():
            command, enter = self.task_queue.get()
            self.run_single_task(command, enter)

    def run_single_task(self, command, enter=""):
        full_cmd = (
            f"{self.app_bin_path}/{command}"
            if not self.stored_options["tst"]
            else f"{self.tst_bin_path}/{command} --gtest_color=yes"
        )
        if self.stored_options["chk"]["mem"]:
            full_cmd = f"valgrind --tool=memcheck --xml=yes \
--xml-file={self.report_path}/dca/chk_mem/foo_chk_mem_{str(self.complete_steps + 1)}.xml {full_cmd}"
        if self.stored_options["chk"]["cov"]:
            full_cmd = f"LLVM_PROFILE_FILE=\
\"{self.report_path}/dca/chk_cov/foo_chk_cov_{str(self.complete_steps + 1)}.profraw\" {full_cmd}"
        if len(enter) != 0:
            command += " > " + enter.replace("\nquit", "")
        align_len = max(
            len(command) + Output.stat_cont_len_excl_cmd,
            Output.stat_min_cont_len,
            len(str(self.total_steps)) * 2 + len(" / ") + Output.stat_cont_len_excl_cmd,
        )
        Output.refresh_status(
            Output.esc_color["blue"], f"CASE: {f'{command}':<{align_len - Output.stat_cont_len_excl_cmd}} # START "
        )

        stdout, stderr, return_code = common.execute_command(full_cmd, enter)
        if len(stdout.strip()) == 0 or stderr or return_code != 0:
            print(f"\n[STDOUT]\n{stdout}\n[STDERR]\n{stderr}\n[RETURN CODE]\n{return_code}")
            Output.refresh_status(
                Output.esc_color["red"], f"{f'STAT: FAILURE NO.{str(self.complete_steps + 1)}':<{align_len}}"
            )
        else:
            stdout = stdout.expandtabs()
            print(stdout)
            self.passed_steps += 1
            if any(sub in stdout for sub in self.suspicious_output):
                self.passed_steps -= 1
                Output.refresh_status(
                    Output.esc_color["red"], f"{f'STAT: FAILURE NO.{str(self.complete_steps + 1)}':<{align_len}}"
                )
            elif self.stored_options["chk"]["mem"]:
                self.convert_valgrind_output(command, align_len)

        self.complete_steps += 1
        Output.refresh_status(
            Output.esc_color["blue"], f"CASE: {f'{command}':<{align_len - Output.stat_cont_len_excl_cmd}} # FINISH"
        )

        stat = "SUCCESS" if self.passed_steps == self.complete_steps else "PARTIAL"
        stat_color = (
            Output.esc_color["yellow"]
            if stat != "SUCCESS" or self.complete_steps != self.total_steps
            else Output.esc_color["green"]
        )
        Output.refresh_status(
            stat_color,
            f"""\
{f"STAT: {stat} {f'{str(self.passed_steps)}':>{len(str(self.total_steps))}} / {str(self.total_steps)}":<{align_len}}""",
        )

        print("\n")

        sys.stdout = STDOUT
        self.progress_bar.draw_progress_bar(int(self.complete_steps / self.total_steps * 100))
        sys.stdout = self.logger

    def initialize_for_check_coverage(self):
        stdout, _, _ = common.execute_command("command -v llvm-profdata-16 llvm-cov-16 2>&1")
        if stdout.find("llvm-profdata-16") != -1 and stdout.find("llvm-cov-16") != -1:
            os.environ["FOO_BLD_COV"] = "llvm-cov"
            self.stored_options["chk"]["cov"] = True
            common.execute_command(f"rm -rf {self.report_path}/dca/chk_cov")
            common.execute_command(f"mkdir -p {self.report_path}/dca/chk_cov")
        else:
            Output.exit_with_error("No llvm-profdata or llvm-cov program. Please check it.")

    def check_coverage_handling(self):
        folder_path = f"{self.report_path}/dca/chk_cov"
        common.execute_command(
            f"llvm-profdata-16 merge -sparse {folder_path}/foo_chk_cov_*.profraw -o {folder_path}/foo_chk_cov.profdata"
        )
        if not self.stored_options["tst"]:
            common.execute_command(
                f"llvm-cov-16 show -instr-profile={folder_path}/foo_chk_cov.profdata -show-branches=percent \
-show-expansions -show-regions -show-line-counts-or-regions -format=html -output-dir={folder_path} -Xdemangler=c++filt \
-object={self.app_bin_path}/{self.app_bin_cmd} {' '.join([f'-object={self.lib_path}/{lib}' for lib in self.lib_list])} \
2>&1"
            )
        else:
            common.execute_command(
                f"llvm-cov-16 show -instr-profile={folder_path}/foo_chk_cov.profdata -show-branches=percent \
-show-expansions -show-regions -show-line-counts-or-regions -format=html -output-dir={folder_path} -Xdemangler=c++filt \
-object={self.tst_bin_path}/{self.tst_bin_cmd} 2>&1"
            )
        stdout, _, _ = (
            common.execute_command(
                f"llvm-cov-16 report -instr-profile={folder_path}/foo_chk_cov.profdata \
-object={self.app_bin_path}/{self.app_bin_cmd} {' '.join([f'-object={self.lib_path}/{lib}' for lib in self.lib_list])} \
2>&1"
            )
            if not self.stored_options["tst"]
            else common.execute_command(
                f"llvm-cov-16 report -instr-profile={folder_path}/foo_chk_cov.profdata \
-object={self.tst_bin_path}/{self.tst_bin_cmd} 2>&1"
            )
        )
        common.execute_command(f"rm -rf {folder_path}/{{*.profraw,*.profdata}}")
        print(f"\n[CHECK COVERAGE]\n{stdout}")
        if "error" in stdout:
            print("Please further check the compilation parameters related to instrumentation.")

    def initialize_for_check_memory(self):
        stdout, _, _ = common.execute_command("command -v valgrind valgrind-ci 2>&1")
        if stdout.find("valgrind") != -1 and stdout.find("valgrind-ci") != -1:
            self.stored_options["chk"]["mem"] = True
            common.execute_command(f"rm -rf {self.report_path}/dca/chk_mem")
            common.execute_command(f"mkdir -p {self.report_path}/dca/chk_mem")
        else:
            Output.exit_with_error("No valgrind (including valgrind-ci) program. Please check it.")

    def check_memory_handling(self):
        common.execute_command(f"rm -rf {self.report_path}/dca/chk_mem/*.xml")
        folder_path = f"{self.report_path}/dca/chk_mem/memory"
        if not os.path.exists(folder_path):
            os.mkdir(folder_path)
        case_folders = [folder.path for folder in os.scandir(folder_path) if folder.is_dir()]
        case_folders_with_ctime = [(folder, os.path.getctime(folder)) for folder in case_folders]
        case_folders_with_ctime.sort(key=lambda pair: pair[1])
        sorted_case_folders = [os.path.basename(folder[0]) for folder in case_folders_with_ctime]
        case_names = [
            Path(f"{folder[0]}/case_name")
            .read_text(encoding="utf-8")
            .replace(f"{self.app_bin_cmd} > ", f"{self.app_bin_cmd} &gt; ")
            for folder in case_folders_with_ctime
        ]
        stdout, _, _ = common.execute_command("pip3 show ValgrindCI")
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
        common.execute_command(
            f"cp -rf {pkg_loc}/{{index.html,valgrind.css,valgrind.js}} {self.report_path}/dca/chk_mem/"
        )

        index_content = ""
        with open(f"{self.report_path}/dca/chk_mem/index.html", "rt", encoding="utf-8") as index_content:
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
        with open(f"{self.report_path}/dca/chk_mem/index.html", "wt", encoding="utf-8") as index_content:
            fcntl.flock(index_content.fileno(), fcntl.LOCK_EX)
            index_content.write(new_content)
            fcntl.flock(index_content.fileno(), fcntl.LOCK_UN)

    def convert_valgrind_output(self, command, align_len):
        inst_num = 0
        xml_filename = f"{self.report_path}/dca/chk_mem/foo_chk_mem_{str(self.complete_steps + 1)}"
        with open(f"{xml_filename}.xml", "rt", encoding="utf-8") as mem_xml:
            inst_num = mem_xml.read().count("</valgrindoutput>")
        stdout = ""
        stderr = ""
        if inst_num == 1:
            stdout, stderr, _ = common.execute_command(f"valgrind-ci {xml_filename}.xml --summary")
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
            stdout, stderr, _ = common.execute_command(
                f"valgrind-ci {xml_filename}_inst_1.xml --summary && \
valgrind-ci {xml_filename}_inst_2.xml --summary"
            )

        if "errors" in stdout:
            stdout = stdout.expandtabs()
            print(f"\n[CHECK MEMORY]\n{stdout}")
            case_path = f"{self.report_path}/dca/chk_mem/memory/case_{str(self.complete_steps + 1)}"
            if inst_num == 1:
                common.execute_command(f"valgrind-ci {xml_filename}.xml --source-dir=./ --output-dir={case_path}")
                Path(f"{case_path}/case_name").write_text(command, encoding="utf-8")
            elif inst_num == 2:
                common.execute_command(
                    f"valgrind-ci {xml_filename}_inst_1.xml --source-dir=./ --output-dir={case_path}_inst_1"
                )
                Path(f"{case_path}_inst_1/case_name").write_text(command, encoding="utf-8")
                common.execute_command(
                    f"valgrind-ci {xml_filename}_inst_2.xml --source-dir=./ --output-dir={case_path}_inst_2"
                )
                Path(f"{case_path}_inst_2/case_name").write_text(command, encoding="utf-8")
            self.passed_steps -= 1
            Output.refresh_status(
                Output.esc_color["red"], f"{f'STAT: FAILURE NO.{str(self.complete_steps + 1)}':<{align_len}}"
            )
        elif inst_num == 0 or inst_num > 2 or len(stderr) != 0:
            self.passed_steps -= 1
            print("\n[CHECK MEMORY]\nUnsupported valgrind output xml file content.")
            Output.refresh_status(
                Output.esc_color["red"], f"{f'STAT: FAILURE NO.{str(self.complete_steps + 1)}':<{align_len}}"
            )

    def format_run_log(self):
        run_log = ""
        with open(self.log_file, "rt", encoding="utf-8") as run_log:
            fcntl.flock(run_log.fileno(), fcntl.LOCK_EX)
            old_content = run_log.read()
            fcntl.flock(run_log.fileno(), fcntl.LOCK_UN)
        new_content = Output.ansi_escape.sub("", old_content)
        with open(self.log_file, "wt", encoding="utf-8") as run_log:
            fcntl.flock(run_log.fileno(), fcntl.LOCK_EX)
            run_log.write(new_content)
            fcntl.flock(run_log.fileno(), fcntl.LOCK_UN)

    def summarize_run_log(self):
        tags = {"tst": False, "chk": {"cov": False, "mem": False}}
        readlines = []
        with open(self.log_file, "rt", encoding="utf-8") as run_log:
            fcntl.flock(run_log.fileno(), fcntl.LOCK_EX)
            readlines = run_log.readlines()
            run_log.seek(0)
            content = run_log.read()
            if self.tst_bin_cmd in content:
                tags["tst"] = True
            if "[CHECK COVERAGE]" in content:
                tags["chk"]["cov"] = True
            if "[CHECK MEMORY]" in content:
                tags["chk"]["mem"] = True
            fcntl.flock(run_log.fileno(), fcntl.LOCK_UN)
        if not self.analyze_only and (
            tags["tst"] != self.stored_options["tst"]
            or tags["chk"]["cov"] != self.stored_options["chk"]["cov"]
            or (tags["chk"]["mem"] and not self.stored_options["chk"]["mem"])
        ):
            Output.exit_with_error(f"Run options do not match the actual contents of the run log {self.log_file} file.")

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
            or (not self.analyze_only and len(finish_indices) != self.total_steps)
        ):
            Output.exit_with_error(f"The run log {self.log_file} file is incomplete. Please retry.")

        dur_time, fail_res, cov_per, mem_err = self.analyze_for_report(readlines, start_indices, finish_indices, tags)
        with open(self.report_file, "wt", encoding="utf-8") as run_report:
            fcntl.flock(run_report.fileno(), fcntl.LOCK_EX)
            run_stat = {
                "Passed": str(len(finish_indices) - len(fail_res)),
                "Failed": str(len(fail_res)),
                "Duration": f"{self.duration} s" if not self.analyze_only else f"{dur_time} s",
            }
            prompt = " (UNIT TEST)" if tags["tst"] else ""
            run_stat_rep = (
                "REPORT FOR RUN STATISTICS:\n"
                + Output().format_as_table(run_stat, "STATUS", f"RUN STATISTICS{prompt}")
                + "\n"
            )
            fail_res_rep = (
                ("\nREPORT FOR FAILURE RESULT:\n" + Output().format_as_table(fail_res, "CASE", "FAILURE RESULT") + "\n")
                if fail_res
                else ""
            )
            cov_per_rep = (
                (
                    "\nREPORT FOR COVERAGE PERCENT:\n"
                    + Output().format_as_table(cov_per, "CATEGORY", "COVERAGE PERCENT")
                    + "\n"
                )
                if cov_per
                else ""
            )
            mem_err_rep = (
                ("\nREPORT FOR MEMORY ERROR:\n" + Output().format_as_table(mem_err, "CASE", "MEMORY ERROR") + "\n")
                if mem_err
                else ""
            )
            summary = run_stat_rep + fail_res_rep + cov_per_rep + mem_err_rep
            run_report.write(summary)
            if self.analyze_only:
                print(summary)
            fcntl.flock(run_report.fileno(), fcntl.LOCK_UN)

        if fail_res:
            sys.exit(1)

    def analyze_for_report(self, readlines, start_indices, finish_indices, tags):
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
        mem_err = {}
        for start_index, finish_index in zip(start_indices, finish_indices):
            if "# STAT: FAILURE" in readlines[finish_index - 1]:
                fail_line = readlines[finish_index - 1]
                number = fail_line[fail_line.find("NO.") : fail_line.find("]")].strip()
                cmd_line = readlines[start_index]
                case_task = number + ": " + cmd_line[cmd_line.find(self.app_bin_cmd) : cmd_line.find("# START")].strip()

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
                if tags["chk"]["mem"]:
                    for index, line in enumerate(readlines[start_index + 1 : finish_index]):
                        if "[CHECK MEMORY]" in line:
                            index += start_index + 1
                            mem_err[case_task] = "".join(readlines[index + 1 : finish_index - 1])
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
            if len(cov_per) == 0:
                for cat in category:
                    cov_per[cat] = "-"

        return dur_time, fail_res, cov_per, mem_err


class Output:
    esc_color = {"red": "\033[0;31;40m", "green": "\033[0;32;40m", "yellow": "\033[0;33;40m", "blue": "\033[0;34;40m"}
    esc_bg_color = "\033[49m"
    esc_font_bold = "\033[1m"
    esc_off = "\033[0m"
    ansi_escape = re.compile(r"\x1B(?:\[[0-?]*[ -/]*[@-~]|[PX^_].*?\x1B\\|.)")
    stat_min_cont_len = 55
    stat_cont_len_excl_cmd = 15
    tbl_min_key_width = 15
    tbl_min_value_width = 60

    @classmethod
    def exit_with_error(cls, message):
        print(f"{os.path.basename(__file__)}: {message}", file=sys.stderr)
        sys.exit(1)

    @classmethod
    def refresh_status(cls, esc_fg_color, content):
        print(
            f"""{esc_fg_color}{cls.esc_font_bold}{cls.esc_bg_color}\
[ {datetime.strftime(datetime.now(), "%b %d %H:%M:%S")} # {content} ]{cls.esc_off}"""
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


def main():
    task = None
    try:
        task = Task()
        task.run()
    except Exception:  # pylint: disable=broad-except
        if task is not None:
            task.stop(traceback.format_exc())
        else:
            raise
    except KeyboardInterrupt:
        if task is not None:
            task.stop()


if __name__ == "__main__":
    try:
        main()
    except Exception:  # pylint: disable=broad-except
        Output.exit_with_error(traceback.format_exc())
