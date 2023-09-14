use std::io::Error as IoError;
use std::net::SocketAddr;
use std::path::Path;

use futures_util::future::join;
use http::response::Builder as ResponseBuilder;
use http::{header, StatusCode};
use hyper::server::conn::http1;
use hyper::service::service_fn;
use hyper::{Request, Response};
use hyper_staticfile::{Body, Static};
use hyper_util::rt::TokioIo;
use tokio::net::TcpListener;

async fn request_handling<B>(req: Request<B>, static_: Static, sub_dir: &str) -> Result<Response<Body>, IoError> {
    if req.uri().path() == "/" {
        let res = ResponseBuilder::new()
            .status(StatusCode::MOVED_PERMANENTLY)
            .header(header::LOCATION, format!("/{sub_dir}/"))
            .body(Body::Empty)
            .expect("Unable to build response.");
        Ok(res)
    } else {
        static_.clone().serve(req).await
    }
}

#[tokio::main]
async fn main() -> Result<(), Box<dyn std::error::Error + Send + Sync>> {
    let doc_root = "document";
    let addr1: SocketAddr = ([127, 0, 0, 1], 61503).into();
    let addr2: SocketAddr = ([127, 0, 0, 1], 61504).into();

    let srv1 = async move {
        let static_ = Static::new(Path::new(format!("./{doc_root}").as_str()));
        let doxygen_dir = "doxygen";
        let listener = TcpListener::bind(addr1)
            .await
            .unwrap_or_else(|_| panic!("Failed to create TCP listener for {doxygen_dir} online."));
        loop {
            let (stream, _) = listener
                .accept()
                .await
                .unwrap_or_else(|_| panic!("Failed to accept TCP connection {doxygen_dir} online."));

            let static_ = static_.clone();
            tokio::spawn(async move {
                if let Err(err) = http1::Builder::new()
                    .serve_connection(
                        TokioIo::new(stream),
                        service_fn(move |req| request_handling(req, static_.clone(), doxygen_dir)),
                    )
                    .await
                {
                    eprintln!("Error serving connection {doxygen_dir} online: {:?}.", err);
                }
            });
        }
    };

    let srv2 = async move {
        let static_ = Static::new(Path::new(format!("./{doc_root}").as_str()));
        let browser_dir = "browser";
        let listener = TcpListener::bind(addr2)
            .await
            .unwrap_or_else(|_| panic!("Failed to create TCP listener for {browser_dir} online."));
        loop {
            let (stream, _) = listener
                .accept()
                .await
                .unwrap_or_else(|_| panic!("Failed to accept TCP connection for {browser_dir} online."));

            let static_ = static_.clone();
            tokio::spawn(async move {
                if let Err(err) = http1::Builder::new()
                    .serve_connection(
                        TokioIo::new(stream),
                        service_fn(move |req| request_handling(req, static_.clone(), browser_dir)),
                    )
                    .await
                {
                    eprintln!("Error serving connection for {browser_dir} online: {:?}.", err);
                }
            });
        }
    };

    println!("The document servers start to listen...\n=> doxygen online: http://{addr1}/\n=> browser online: http://{addr2}/");
    let _ret = join(srv1, srv2).await;

    Ok(())
}
