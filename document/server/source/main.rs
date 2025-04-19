mod arg;
mod srv;
mod util;

use std::net::SocketAddr;

async fn run(args: arg::Args) {
    let root_dirs = &args.root_dirs;
    let mut srv_group = vec![];
    let mut hint = String::new();

    for (offset, root_dir) in root_dirs.iter().enumerate() {
        let addr = SocketAddr::new(args.host, args.port + offset as u16);

        srv_group.push(async move {
            srv::run_service(addr, root_dir, None).await;
        });
        hint += format!("=> http://{}/ for directory {}\n", addr, abs_path!(root_dir)).as_str();
    }
    print!("The archive server starts listening ...\n{}", hint);

    let _ret = futures_util::future::join_all(srv_group).await;
}

#[tokio::main]
async fn main() -> Result<(), Box<dyn std::error::Error + Send + Sync>> {
    let args = arg::parse_args();

    run(args).await;

    Ok(())
}
