mod io;
mod pin;
mod tests;
mod config;
mod command;
mod udp_server;

use libc;
use pin::Pin;
use std::error;
use udp_server::start_server;

const PORT: u64 = 0x378;

#[tokio::main]
async fn main() -> Result<(), Box<dyn error::Error>> {
    if !cfg!(unix) {
        return Err("You must be on a unix machine to run this program".into());
    }

    let mut parallel: Vec<Pin> = Vec::new();

    unsafe {
        if libc::ioperm(PORT, 1, 1) != 0 {
            return Err("Access Denied".into());
        }
    }

    println!("Server started on port 5663");
    start_server(&mut parallel).await?;

    Ok(())
}
