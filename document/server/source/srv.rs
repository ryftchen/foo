use crate::{die, util};
use http::response::Builder as ResponseBuilder;
use hyper::server::conn::http1;
use hyper_staticfile::{Body, Static};
use hyper_util::rt::TokioIo;
use std::path::Path;
use tokio::net::TcpListener;

pub async fn do_service(addr: std::net::SocketAddr, root_dir: &str, sub_dir: Option<&'static str>) {
    let serving = Static::new(Path::new(root_dir));
    let listener = TcpListener::bind(addr)
        .await
        .unwrap_or_else(|_| die!("Could not create TCP listener for {} directory.", root_dir));

    loop {
        let (stream, _) = listener
            .accept()
            .await
            .unwrap_or_else(|_| die!("Could not accept TCP connection for {} directory.", root_dir));
        let serving = serving.clone();

        tokio::spawn(async move {
            if let Err(err) = http1::Builder::new()
                .serve_connection(
                    TokioIo::new(stream),
                    hyper::service::service_fn(move |req| handle_request(req, serving.clone(), sub_dir)),
                )
                .await
            {
                die!(
                    "Connection serving error for {} redirection: {:?}.",
                    sub_dir.unwrap_or("default"),
                    err
                );
            }
        });
    }
}

async fn handle_request<B>(
    req: hyper::Request<B>,
    serving: Static,
    redirect: Option<&'static str>,
) -> Result<hyper::Response<Body>, std::io::Error> {
    if redirect.is_none() {
        serving.clone().serve(req).await
    } else if req.uri().path() == "/" {
        let res = ResponseBuilder::new()
            .status(http::StatusCode::MOVED_PERMANENTLY)
            .header(http::header::LOCATION, format!("/{}/", redirect.unwrap()))
            .body(Body::Empty)
            .unwrap_or_else(|_| die!("Unable to build response for {} redirection.", redirect.unwrap()));
        Ok(res)
    } else {
        serving.clone().serve(req).await
    }
}
