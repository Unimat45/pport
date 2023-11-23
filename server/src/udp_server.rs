use std::io;
use tokio::net::UdpSocket;
use crate::{pin::Pin, command::{token_command, parse_command}, config::load_parallel_from_file};

pub async fn start_server(parallel: &mut Vec<Pin>) -> Result<(), io::Error> {
	let r = load_parallel_from_file().await?;

	parallel.clear();
	parallel.extend(r);

	let sock = UdpSocket::bind("0.0.0.0:5663").await?;

	let mut buf = [0u8; 1024];

	loop {
		let (len, addr) = sock.recv_from(&mut buf).await?;

		let msg = String::from_utf8_lossy(&buf[0..len]).to_string();

		if msg.trim().eq_ignore_ascii_case("STOP") {
			sock.send_to("STOP".as_bytes(), addr).await?;
			
			break;
		}

		let res_cmd = token_command(&msg);

		if res_cmd.is_err() {
			sock.send_to(res_cmd.unwrap_err().to_string().as_bytes(), addr).await?;
			continue;
		}

		let cmd = res_cmd.unwrap();

		let res_result = parse_command(parallel, cmd).await;

		if res_result.is_err() {
			sock.send_to(res_result.unwrap_err().to_string().as_bytes(), addr).await?;
		}
		else {
			let result = res_result.unwrap();
			sock.send_to(&result, addr).await?;
		}
	}

	Ok(())
}

