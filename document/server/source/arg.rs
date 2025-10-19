use crate::{exec_name, fatal, util};
use std::net::{IpAddr, Ipv4Addr};
use std::path::Path;

#[derive(Debug)]
pub struct Args {
    pub root_dirs: Vec<String>,
    pub host: IpAddr,
    pub port: u16,
}

pub struct Parser {
    options: getopts::Options,
}
impl Parser {
    pub fn new() -> Self {
        let mut options = getopts::Options::new();

        options.optflag("h", "help", "show help and exit");
        options.optmulti("r", "root-dir", "set root directory", "DIR");
        options.optopt("H", "host", "set host IP address", "IP");
        options.optopt("p", "port", "set starting port", "NUM");
        Parser { options }
    }

    pub fn parse(&self, args: &[String]) -> getopts::Matches {
        match self.options.parse(&args[1..]) {
            Ok(m) => m,
            Err(e) => fatal!("{}.", e),
        }
    }

    pub fn usage(&self) {
        println!(
            "usage: {} [-h] {{-r {{DIR}}}} [{{-r {{DIR}}}} ...] [-H {{IP}}] [-p {{NUM}}]",
            exec_name!()
        );
        println!();
        println!("archive server");
        println!();
        println!("options:");
        println!("  -h, --help            show this help message and exit");
        println!("  -r {{DIR}}, --root-dir {{DIR}}");
        println!("                        set root directory");
        println!("  -H {{IP}}, --host {{IP}}  set host address");
        println!("  -p {{NUM}}, --port {{NUM}}");
        println!("                        set starting port");
    }
}

pub fn parse_args() -> Args {
    let parser = Parser::new();
    let args: Vec<_> = std::env::args().collect();
    let matches = parser.parse(&args);

    if !matches.free.is_empty() {
        fatal!("Unknown argument.");
    }
    if matches.opt_present("h") || args.len() == 1 {
        parser.usage();
        std::process::exit(0);
    }

    let root_dirs = matches.opt_strs("r");
    if root_dirs.is_empty() {
        fatal!("At least one root directory must be specified.");
    }
    for root_dir in &root_dirs {
        if !Path::new(&root_dir).exists() {
            fatal!("Illegal root directory: {}.", root_dir);
        }
    }
    let host = match matches.opt_str("H") {
        Some(h) => h.parse().unwrap_or_else(|_| fatal!("Invalid IP address.")),
        _none => IpAddr::V4(Ipv4Addr::LOCALHOST),
    };
    let port = match matches.opt_str("p") {
        Some(p) => match p.parse() {
            Ok(p) if (0..=65535).contains(&p) => p,
            _ => fatal!("Invalid port number."),
        },
        _none => 61503,
    };
    if port as usize + root_dirs.len() - 1 > 65535 {
        fatal!("Not enough available port numbers.");
    }
    Args { root_dirs, port, host }
}
