mod arg;
mod srv;
mod util;

async fn run(args: arg::Args) {
    let root_dirs = &args.root_dirs;
    let mut srv_vec = vec![];
    let mut prompt = String::new();

    for (i, root_dir) in root_dirs.iter().enumerate() {
        let addr = std::net::SocketAddr::new(args.host, args.port + i as u16);
        srv_vec.push(async move {
            srv::do_service(addr, root_dir, None).await;
        });
        prompt += format!("=> http://{}/ for directory {}\n", addr, abs_path!(root_dir)).as_str();
    }
    print!("The archive server starts listening ...\n{}", prompt);

    let _ret = futures::future::join_all(srv_vec).await;
}

#[tokio::main]
async fn main() -> Result<(), Box<dyn std::error::Error + Send + Sync>> {
    let args = arg::parse_args();

    run(args).await;

    Ok(())
}
