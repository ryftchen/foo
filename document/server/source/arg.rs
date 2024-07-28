use crate::{die, exec_name, util};
use getopts::Options;
use std::net::{IpAddr, Ipv4Addr};
use std::path::Path;

#[derive(Debug)]
pub struct Args {
    pub root_dirs: Vec<String>,
    pub host: IpAddr,
    pub port: u16,
}

pub fn parse_args() -> Args {
    let mut opts = Options::new();
    opts.optflag("h", "help", "show help and exit");
    opts.optmulti("r", "root-dir", "set root directory", "DIR");
    opts.optopt("H", "host", "set host IP address", "IP");
    opts.optopt("p", "port", "set starting port", "NUM");
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
    if matches.opt_present("h") || args.len() == 1 {
        show_help();
        std::process::exit(0);
    }

    let root_dirs: Vec<String> = matches.opt_strs("r");
    if root_dirs.is_empty() {
        die!("At least one root directory must be specified.");
    }
    for root_dir in &root_dirs {
        if !Path::new(&root_dir).exists() {
            die!("Illegal root directory: {}.", root_dir);
        }
    }

    let host: IpAddr = match matches.opt_str("H") {
        Some(h) => h.parse().unwrap_or_else(|_| die!("Invalid IP address.")),
        _none => IpAddr::V4(Ipv4Addr::LOCALHOST),
    };

    let port: u16 = match matches.opt_str("p") {
        Some(p) => match p.parse() {
            Ok(p) if (0..=65535).contains(&p) => p,
            _ => die!("Invalid port number."),
        },
        _none => 61503,
    };
    let port_with_offset = port as usize + root_dirs.len();
    if port_with_offset > 65535 {
        die!("Not enough available port numbers.");
    }

    Args { root_dirs, port, host }
}

fn show_help() {
    println!("usage: {} [-h] [-r [DIR]] {{-H [IP]}} {{-p [NUM]}}", exec_name!());
    println!();
    println!("archive server");
    println!();
    println!("options:");
    println!("  -h, --help            show help and exit");
    println!("  -r [DIR], --root-dir [DIR]");
    println!("                        set root directory");
    println!("  -H [IP], --host [IP]  set host address");
    println!("  -p [NUM], --port [NUM]");
    println!("                        set starting port");
}
