use std::ffi::OsStr;
use std::path::Path;

#[macro_export]
macro_rules! trace {
    ($($msg:tt)*) => {{
        eprintln!("[ {} ] {}", chrono::Local::now().format("%Y-%m-%d %H:%M:%S%.6f"), format!($($msg)*));
    }};
}

#[macro_export]
macro_rules! fatal {
    ($($msg:tt)*) => {
        panic!("{}: {}", util::try_get_exec_name().unwrap(), format!($($msg)*))
    };
}

#[macro_export]
macro_rules! exec_name {
    () => {
        util::try_get_exec_name().unwrap()
    };
}

#[macro_export]
macro_rules! abs_path {
    ($path:expr) => {
        util::try_get_abs_path($path).unwrap()
    };
}

#[inline(always)]
pub fn try_get_exec_name() -> Option<String> {
    std::env::current_exe()
        .ok()
        .and_then(|path| path.file_name().map(|name| name.to_os_string()))
        .and_then(|name| name.into_string().ok())
}

#[inline(always)]
pub fn try_get_abs_path(path: &str) -> Option<String> {
    shellexpand::full(path)
        .ok()
        .and_then(|expanded| Path::new(OsStr::new(expanded.as_ref())).canonicalize().ok())
        .and_then(|path| path.into_os_string().into_string().ok())
}
