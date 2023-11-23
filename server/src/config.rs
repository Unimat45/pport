use crate::{pin::{Pin, SerializeP}, io::inb};
use serde::{Deserialize, Serialize};
use tokio::{fs::File, io::{self, AsyncWriteExt, AsyncReadExt}};

const CONFIG_FILE: &str = "/etc/pport_conf.yaml";

#[derive(Debug, Serialize, Deserialize)]
struct AppConfig {
	pins: Vec<Pin>
}

pub async fn load_parallel_from_file() -> io::Result<Vec<Pin>> {
	let file = File::open(CONFIG_FILE).await;

	if file.is_err() {
		let mut result: Vec<Pin> = Vec::new();
		let current_value = inb();

		for i in 0..8u8 {
			result.push(Pin::new((current_value & (1 << i)).try_into().expect("Err decoding"), format!("Pin {}", i + 2)));
		}

		write_to_file(&result).await?;

		return Ok(result);
	}
	else {
		let mut f = file?;

		let mut data = String::new();
		f.read_to_string(&mut data).await?;

		let cfg: AppConfig = serde_yaml::from_str(&data).expect("Read error");

		return Ok(cfg.pins);
	}
}

pub async fn write_to_file(parallel: &Vec<Pin>) -> io::Result<()> {
	let cfg = AppConfig {
		pins: parallel.clone().to_vec()
	};

	let mut f = File::create(CONFIG_FILE).await?;

	f.write_all(serde_yaml::to_string(&cfg).unwrap().as_bytes()).await?;

	Ok(())
}

