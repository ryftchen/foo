use crate::util;
use getopts::Options;

macro_rules! args_err {
    ($msg:expr) => {
        eprintln!("{}: {}", util::get_exec_name().unwrap(), $msg);
        std::process::exit(1);
    };
}

#[derive(Debug)]
pub struct Args {
    pub root_dir: String,
}

pub fn parse_args() -> Args {
    let mut opts = Options::new();
    opts.optflag("h", "help", "show help and exit");
    opts.optopt("r", "root-dir", "set root directory", "DIR");
    let args: Vec<_> = std::env::args().collect();
    let matches = match opts.parse(&args[1..]) {
        Ok(m) => m,
        Err(f) => {
            args_err!(format!("{}.", f));
        }
    };

    if args.len() == 1 || matches.opt_present("h") {
        if matches.opt_present("r") {
            args_err!("Excess arguments.");
        }
        show_help();
    }

    let root_dir = matches.opt_str("r").unwrap_or_else(|| ".".to_string());

    Args { root_dir }
}

fn show_help() {
    println!("usage: {} [-h] [-r DIR]", util::get_exec_name().unwrap());
    println!();
    println!("document server");
    println!();
    println!("options:");
    println!("  -h, --help        show help and exit");
    println!("  -r, --root-dir    set root directory");

    std::process::exit(0);
}
