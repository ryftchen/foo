use crate::{die, exec_name, util};
use getopts::Options;
use std::path::Path;

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
            die!("{}.", f);
        }
    };

    if !matches.free.is_empty() {
        die!("Unknown argument.");
    }
    if matches.opt_present("h") && matches.opt_present("r") {
        die!("Excessive arguments.");
    }

    if matches.opt_present("h") || args.len() == 1 {
        show_help();
    }

    if !matches.opt_present("r") {
        die!("Require to specify root directory.");
    }
    let root_dir = matches.opt_str("r").unwrap_or_else(|| ".".to_string());
    if !Path::new(&root_dir).exists() {
        die!("Illegal root directory.");
    }

    Args { root_dir }
}

fn show_help() {
    println!("usage: {} [-h] [-r [DIR]]", exec_name!());
    println!();
    println!("document server");
    println!();
    println!("options:");
    println!("  -h, --help            show help and exit");
    println!("  -r [DIR], --root-dir [DIR]");
    println!("                        set root directory");

    std::process::exit(0);
}
