use std::ffi::OsStr;
use std::path::Path;

#[macro_export]
macro_rules! die {
    ($($msg:tt)*) => {
        panic!("{}: {}", util::try_to_get_exec_name().unwrap(), format!($($msg)*))
    };
}

#[macro_export]
macro_rules! exec_name {
    () => {
        util::try_to_get_exec_name().unwrap()
    };
}

#[macro_export]
macro_rules! abs_path {
    ($path:expr) => {
        util::try_to_get_abs_path($path).unwrap()
    };
}

#[inline(always)]
pub fn try_to_get_exec_name() -> Option<String> {
    std::env::current_exe()
        .ok()
        .and_then(|pb| pb.file_name().map(|s| s.to_os_string()))
        .and_then(|s| s.into_string().ok())
}

#[inline(always)]
pub fn try_to_get_abs_path(path: &str) -> Option<String> {
    shellexpand::full(path)
        .ok()
        .and_then(|x| Path::new(OsStr::new(x.as_ref())).canonicalize().ok())
        .and_then(|p| p.into_os_string().into_string().ok())
}
