use std::error::Error;

use crate::{pin::{PinState, Pin, SerializeP}, config::{write_to_file, load_parallel_from_file}, io::{outb, inb}};

#[derive(Debug, PartialEq, Eq)]
pub enum Instruction {
	NoInst = 0xFF,
	Set = 0,
	Show,
	Reboot,
	Toggle,
	Label
}

#[derive(Debug, PartialEq, Eq)]
pub enum Pins {
	NoPin = 0,
	Pin2 = 2,
	Pin3,
	Pin4,
	Pin5,
	Pin6,
	Pin7,
	Pin8,
	Pin9,
	All = 0xFF
}

impl TryFrom<i32> for Pins {
	type Error = ();

	fn try_from(value: i32) -> Result<Self, Self::Error> {
		match value {
			x if x == Pins::NoPin as i32 => Ok(Pins::NoPin),
			x if x == Pins::Pin2 as i32 => Ok(Pins::Pin2),
			x if x == Pins::Pin3 as i32 => Ok(Pins::Pin3),
			x if x == Pins::Pin4 as i32 => Ok(Pins::Pin4),
			x if x == Pins::Pin5 as i32 => Ok(Pins::Pin5),
			x if x == Pins::Pin6 as i32 => Ok(Pins::Pin6),
			x if x == Pins::Pin7 as i32 => Ok(Pins::Pin7),
			x if x == Pins::Pin8 as i32 => Ok(Pins::Pin8),
			x if x == Pins::Pin9 as i32 => Ok(Pins::Pin9),
			x if x == Pins::All as i32 => Ok(Pins::All),
			_ => Err(())
		}
	}
}

#[derive(Debug)]
pub struct Command {
	pub instruction: Instruction,
	pub pin: Pins,
	pub state: PinState,
	pub label: String
}

impl PartialEq for Command {
	fn eq(&self, other: &Self) -> bool {
		self.instruction == other.instruction &&
			self.pin == other.pin &&
			self.state == other.state &&
			self.label == other.label
	}
}
impl Eq for Command {}

fn parallel_to_mem(parallel: &Vec<Pin>) -> Vec<u8> {
	let mut r: Vec<u8> = Vec::new();

	r.push(1);

	for p in parallel {
		r.extend_from_slice(&p.as_mem()[1..]);
	}

	return r;
}

pub fn token_command(cmd: &str) -> Result<Command, Box<dyn Error>> {
	let tokens: Vec<&str> = cmd.split(" ").collect();

	let mut c = Command {
		instruction: Instruction::NoInst,
		pin: Pins::NoPin,
		state: PinState::Off,
		label: String::new()
	};

	match tokens[0].to_ascii_uppercase().as_str() {
		"SET" => c.instruction = Instruction::Set,
		"SHOW" => c.instruction = Instruction::Show,
		"REBOOT" => c.instruction = Instruction::Reboot,
		"TOGGLE" => c.instruction = Instruction::Toggle,
		"LABEL" => c.instruction = Instruction::Label,
		_ => { return Err("ERROR: Invalid Instruction".into()) }
	}

	if tokens.len() < 2 {
		return Ok(c);
	}

	if !tokens[1].eq_ignore_ascii_case("PIN") {
		return Err(format!("ERROR: Syntax error near '{}'", tokens[0]).into());
	}

	if tokens[2].eq_ignore_ascii_case("ALL") {
		c.pin = Pins::All;
	}
	else if let Ok(i) = tokens[2].parse::<i32>() {
		if i >= 2 && i <= 9 {
			c.pin = i.try_into().unwrap();
		}
		else {
			return Err(format!("ERROR: Syntax error near '{}'", tokens[1]).into());
		}
	}
	else {
		return Err(format!("ERROR: Syntax error near '{}'", tokens[1]).into());
	}

	if tokens.len() < 4 {
		return Ok(c);
	}

	match tokens[3].to_ascii_uppercase().as_str() {
		"HIGH" => c.state = PinState::On,
		"ON" => c.state = PinState::On,
		"LOW" => c.state = PinState::Off,
		"OFF" => c.state = PinState::Off,
		_ => {
			c.label.push_str(&tokens[3]);
			c.label.push(' ');
		}
	}

	if tokens.len() > 4 {
		for token in &tokens[4..] {
			c.label.push_str(&token);
			c.label.push(' ');
		}
	}

	if c.label.len() > 0 {
		c.label.remove(c.label.len() - 1);
	}

	Ok(c)
}

pub async fn parse_command(parallel: &mut Vec<Pin>, cmd: Command) -> Result<Vec<u8>, Box<dyn Error>> {
	match cmd.instruction {
		Instruction::Show => {
			match cmd.pin {
				Pins::All => Ok(parallel_to_mem(parallel)),
				Pins::NoPin => Ok(parallel_to_mem(parallel)),
				_ => Ok(parallel[cmd.pin as usize - 2].as_mem())
			}
		},
		Instruction::Set => {
			match cmd.pin {
				Pins::NoPin => Err("ERROR: Invalid pin number".into()),
				Pins::All => {
					for p in parallel.into_iter() {
						p.set_state(cmd.state);
					}

					outb(0xFF);

					write_to_file(parallel).await?;
					Ok(parallel_to_mem(parallel))
				},
				_ => {
					let current_value = inb();

					let pin_nbr = cmd.pin as usize - 2;

					parallel[pin_nbr].set_state(cmd.state);

					match cmd.state {
						PinState::On => outb(current_value | (1 << pin_nbr)),
						PinState::Off => outb(current_value & (0xFF - (1 << pin_nbr)))
					}

					write_to_file(parallel).await?;

					Ok(parallel[pin_nbr].as_mem())
				}
			}
		},
		Instruction::Reboot => {
			let p = load_parallel_from_file().await?;

			parallel.clear();
			parallel.extend(p);

			Ok(parallel_to_mem(parallel))
		},
		Instruction::Toggle => {
			match cmd.pin {
				Pins::All => Err("ERROR: Invalid pin number".into()),
				Pins::NoPin => Err("ERROR: Invalid pin number".into()),
				_ => {
					let current_value = inb();
					
					let pin_nbr = cmd.pin as usize - 2;

					parallel[pin_nbr].toggle_state();

					match cmd.state {
						PinState::On => outb(current_value | (1 << pin_nbr)),
						PinState::Off => outb(current_value & (0xFF - (1 << pin_nbr)))
					}

					write_to_file(parallel).await?;

					Ok(parallel[pin_nbr].as_mem())
				}
			}
		},
		Instruction::Label => {
			match cmd.pin {
				Pins::All => Err("ERROR: Invalid pin number".into()),
				Pins::NoPin => Err("ERROR: Invalid pin number".into()),
				_ => {
					let pin_nbr = cmd.pin as usize - 2;

					parallel[pin_nbr].set_label(&cmd.label);

					write_to_file(parallel).await?;

					Ok(parallel[pin_nbr].as_mem())
				}
			}
		},
		Instruction::NoInst => Err("ERROR: Invalid instruction".into())
	}
}