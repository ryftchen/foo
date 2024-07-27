use crate::{die, util};
use http::response::Builder as ResponseBuilder;
use hyper::server::conn::http1;
use hyper_staticfile::{Body, Static};
use hyper_util::rt::TokioIo;
use std::path::Path;
use tokio::net::TcpListener;

pub async fn do_service(addr: std::net::SocketAddr, root_dir: &str, sub_dir: &'static str) {
    let serving = Static::new(Path::new(root_dir));
    let listener = TcpListener::bind(addr)
        .await
        .unwrap_or_else(|_| die!("Could not create TCP listener for {} online.", sub_dir));
    loop {
        let (stream, _) = listener
            .accept()
            .await
            .unwrap_or_else(|_| die!("Could not accept TCP connection for {} online.", sub_dir));

        let serving = serving.clone();
        tokio::spawn(async move {
            if let Err(err) = http1::Builder::new()
                .serve_connection(
                    TokioIo::new(stream),
                    hyper::service::service_fn(move |req| handle_request(req, serving.clone(), sub_dir)),
                )
                .await
            {
                die!("Error serving connection for {} online: {:?}.", sub_dir, err);
            }
        });
    }
}

async fn handle_request<B>(
    req: hyper::Request<B>,
    serving: Static,
    sub_dir: &'static str,
) -> Result<hyper::Response<Body>, std::io::Error> {
    if req.uri().path() == "/" {
        let res = ResponseBuilder::new()
            .status(http::StatusCode::MOVED_PERMANENTLY)
            .header(http::header::LOCATION, format!("/{}/", sub_dir))
            .body(Body::Empty)
            .unwrap_or_else(|_| die!("Unable to build response for {} online.", sub_dir));
        Ok(res)
    } else {
        serving.clone().serve(req).await
    }
}
