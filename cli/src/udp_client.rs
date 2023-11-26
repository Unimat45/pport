use std::error::Error;

use tokio::net::UdpSocket;

const HOST: &str = "10.0.0.106:5663";

#[derive(Debug)]
pub struct Pin {
	pub state: bool,
	pub label: String
}

pub trait Utf8Len {
	fn utf8_strlen(&self) -> usize;
}

impl Utf8Len for String {
	fn utf8_strlen(&self) -> usize {
		let mut len = 0;
		let mut i = 0;
		let s = self.as_bytes();
	
		while i < s.len() {
			if (s[i] & 0x80) == 0 || (s[i] & 0xE0) == 0xC0 {
				len += 1;
				i += 1;
			}
			else if (s[i] & 0xF0) == 0xE0 {
				len += 1;
				i += 2;
			}
			else if (s[i] & 0xF8) == 0xF0 {
				len += 1;
				i += 3;
			}
			else {
				i += 1;
			}
		}
	
		return len;
	}
}


fn count_til_zero(buf: &[u8]) -> usize {
	let mut r = 0usize;

	for &b in buf {
		if b == 0 {
			return r;
		}

		r += 1;
	}

	return 0;
}

fn deserialize_pins(buf: &[u8]) -> (Vec<Pin>, usize) {
	let mut pins = Vec::<Pin>::new();
	let mut longest_label = 0;

	if buf[0] == 1 {
		let mut last_len = 1;

		for _ in 0..8 {
			let next_len = count_til_zero(&buf[last_len + 1..]);
			let label = String::from_utf8_lossy(&buf[last_len + 1 .. last_len + 1 + next_len]).to_string();
			let str_len = label.utf8_strlen();

			if str_len > longest_label {
				longest_label = str_len;
			}

			pins.push(Pin {
				state: buf[last_len] == 1,
				label
			});

			last_len += 2 + next_len;
		}
	}
	else {
		longest_label = buf.len() - 2;

		pins.push(Pin {
			state: buf[1] == 1,
			label: String::from_utf8_lossy(&buf[2..]).to_string()
		});
	}

	return (pins, longest_label);
}

pub async fn udp_send(cmd: &str) -> Result<(Vec<Pin>, usize), Box<dyn Error>> {
    let sock = UdpSocket::bind("0.0.0.0:0").await?;

    sock.send_to(cmd.as_bytes(), HOST).await?;

    let mut buf = [0u8; 2104];

    let (len, _) = sock.recv_from(&mut buf).await?;

	if String::from_utf8_lossy(&buf[0..5]).starts_with("ERROR") {
		return Err(String::from_utf8_lossy(&buf).into());
	}

    Ok(deserialize_pins(&buf[0..len]))
}