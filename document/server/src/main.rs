mod arg;
mod srv;
mod util;

async fn run(args: arg::Args) {
    let root_dir = &args.root_dir;
    let folder_vec: Vec<&str> = vec!["document/doxygen", "document/browser"];
    let addr_vec: Vec<std::net::SocketAddr> = vec![([127, 0, 0, 1], 61503).into(), ([127, 0, 0, 1], 61504).into()];
    let mut srv_vec = vec![];
    let mut prompt: String = "".to_string();

    for (folder, addr) in std::iter::zip(&folder_vec, &addr_vec) {
        srv_vec.push(async move {
            srv::do_service(*addr, root_dir, folder).await;
        });
        prompt += format!("=> {} online: http://{}/\n", folder, addr).as_str();
    }

    print!(
        "\r\nThe document server starts listening under the {} directory...\n\
        {}\n",
        abs_path!(root_dir),
        prompt
    );
    let _ret = futures_util::future::join_all(srv_vec).await;
}

#[tokio::main]
async fn main() -> Result<(), Box<dyn std::error::Error + Send + Sync>> {
    let args = arg::parse_args();

    run(args).await;

    Ok(())
}
