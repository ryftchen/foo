mod arg;
mod srv;
mod util;

use std::net::SocketAddr;
use tokio::signal::unix::{signal, SignalKind};
use tokio::sync::broadcast;

async fn run(args: arg::Args, mut sig_rx: broadcast::Receiver<()>) {
    let root_dirs = &args.root_dirs;
    let mut srv_group = vec![];
    let mut hint = String::new();

    for (offset, root_dir) in root_dirs.iter().enumerate() {
        let addr = SocketAddr::new(args.host, args.port + offset as u16);

        srv_group.push(async move {
            srv::run_service(addr, root_dir, None, args.no_cache).await;
        });
        hint += format!("=> http://{}/ for directory {}\n", addr, abs_path!(root_dir)).as_str();
    }

    print!("The archive server starts listening ...\n{}", hint);
    tokio::spawn(async move {
        while sig_rx.recv().await.is_ok() {
            print!("The archive server has started listening ...\n{}", hint);
        }
    });
    let _ret = futures_util::future::join_all(srv_group).await;
}

fn listen_signal(sig_tx: broadcast::Sender<()>) {
    tokio::spawn(async move {
        let mut stream = signal(SignalKind::user_defined1()).unwrap();
        while stream.recv().await.is_some() {
            let _ = sig_tx.send(());
        }
    });
}

#[tokio::main]
async fn main() -> Result<(), Box<dyn std::error::Error + Send + Sync>> {
    let args = arg::parse_args();
    let (sig_tx, sig_rx) = broadcast::channel(16);

    listen_signal(sig_tx.clone());
    run(args, sig_rx).await;
    Ok(())
}
