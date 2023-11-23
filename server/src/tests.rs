#[cfg(test)]

use crate::{command::{token_command, Command, Instruction, Pins}, pin::PinState};

#[test]
fn t_token_command_1() {
	let cmd = "SHOW";
	let expected = Command {
		instruction: Instruction::Show,
		label: String::new(),
		pin: Pins::NoPin,
		state: PinState::Off
	};
	
	let result = token_command(cmd);
	
	assert!(result.is_ok_and(|r| r.eq(&expected)));
}

#[test]
fn t_token_command_2() {	
	let cmd = "SHOW PIN 2";
	let expected = Command {
		instruction: Instruction::Show,
		label: String::new(),
		pin: Pins::Pin2,
		state: PinState::Off
	};
	
	let result = token_command(cmd);

	
	assert!(result.is_ok_and(|r| r.pin.eq(&expected.pin)));
}

#[test]
fn t_token_command_3() {
	let cmd = "SET PIN 3 HIGH";
	let expected = Command {
		instruction: Instruction::Set,
		label: String::new(),
		pin: Pins::Pin3,
		state: PinState::On
	};

	let result = token_command(cmd);

	assert!(result.is_ok_and(|r| r.eq(&expected)));
}

#[test]
fn t_token_command_4() {
	let cmd = "REBOOT";
	let expected = Command {
		instruction: Instruction::Reboot,
		label: String::new(),
		pin: Pins::NoPin,
		state: PinState::Off
	};

	let result = token_command(cmd);

	assert!(result.is_ok_and(|r| r.eq(&expected)));
}

#[test]
fn t_token_command_5() {
	let cmd = "TOGGLE PIN 4";
	let expected = Command {
		instruction: Instruction::Toggle,
		label: String::new(),
		pin: Pins::Pin4,
		state: PinState::Off
	};

	let result = token_command(cmd);

	assert!(result.is_ok_and(|r| r.eq(&expected)));
}

#[test]
fn t_token_command_6() {
	let cmd = "LABEL PIN 5 Test";
	let expected = Command {
		instruction: Instruction::Label,
		label: "Test".into(),
		pin: Pins::Pin5,
		state: PinState::Off
	};

	let result = token_command(cmd);

	assert!(result.is_ok_and(|r| r.eq(&expected)));
}

#[test]
fn t_token_command_7() {
	let cmd = "DFGHNBERTHFR";

	let result = token_command(cmd);

	assert!(result.is_err());
}


