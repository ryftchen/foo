use crate::trace;
use http::response::Builder as ResponseBuilder;
use hyper::server::conn::http1;
use hyper_staticfile::{Body, Static};
use hyper_util::rt::TokioIo;
use std::io::{Error, ErrorKind};
use std::path::Path;
use tokio::net::TcpListener;

pub async fn run_service(addr: std::net::SocketAddr, root_dir: &str, redirect_to: Option<&'static str>) {
    let serving = Static::new(Path::new(root_dir));
    let listener = match TcpListener::bind(addr).await {
        Ok(l) => l,
        Err(e) => {
            trace!(
                "Could not create TCP listener on {} for {} directory: {:?}.",
                addr,
                root_dir,
                e
            );
            return;
        }
    };

    loop {
        let (stream, _) = match listener.accept().await {
            Ok(s) => s,
            Err(e) => {
                trace!(
                    "Could not accept TCP connection on {} for {} directory: {:?}.",
                    addr,
                    root_dir,
                    e
                );
                continue;
            }
        };
        let serving = serving.clone();

        tokio::spawn(async move {
            if let Err(err) = http1::Builder::new()
                .keep_alive(true)
                .serve_connection(
                    TokioIo::new(stream),
                    hyper::service::service_fn(move |req| handle_request(req, serving.clone(), redirect_to)),
                )
                .await
            {
                trace!("Serve connection ({}) error: {:?}.", addr, err);
            }
        });
    }
}

async fn handle_request<B>(
    req: hyper::Request<B>,
    serving: Static,
    redirect_to: Option<&'static str>,
) -> Result<hyper::Response<Body>, Error> {
    if let Some(redirect) = redirect_to {
        if req.uri().path() == "/" {
            let resp = ResponseBuilder::new()
                .status(http::StatusCode::MOVED_PERMANENTLY)
                .header(http::header::LOCATION, format!("/{}/", redirect))
                .body(Body::Empty)
                .map_err(|err| {
                    trace!("Unable to build response for {} redirection: {:?}.", redirect, err);
                    Error::new(ErrorKind::Other, err)
                })?;
            return Ok(resp);
        }
    }
    serving.clone().serve(req).await
}
