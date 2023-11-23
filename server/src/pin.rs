use serde::{Serialize, Deserialize};

#[derive(Debug, Copy, Clone, Serialize, Deserialize, PartialEq, Eq)]
pub enum PinState {
	Off = 0,
	On
}

impl TryFrom<u8> for PinState {
	type Error = ();

	fn try_from(value: u8) -> Result<Self, Self::Error> {
		match value {
			x if x > 0 => Ok(PinState::On),
			x if x == 0 => Ok(PinState::Off),
			_ => Err(())
		}
	}
}

#[derive(Debug, Serialize, Deserialize, Clone)]
pub struct Pin {
	state: PinState,
	label: String
}

pub trait SerializeP {
	fn new(state: PinState, label: String) -> Self;
	fn as_mem(&self) -> Vec<u8>;
	fn set_state(&mut self, state: PinState);
	fn toggle_state(&mut self);
	fn set_label(&mut self, label: &str);
}

impl SerializeP for Pin {
    fn as_mem(&self) -> Vec<u8> {
		let mut r: Vec<u8> = Vec::new();

		r.push(0);
		r.push(self.state as u8);
		r.extend_from_slice(&self.label.as_bytes());
		r.push(0);

		return r;
    }

	fn set_state(&mut self, state: PinState) {
		self.state = state;
	}

	fn toggle_state(&mut self) {
		self.state = match self.state {
			PinState::On => PinState::Off,
			PinState::Off => PinState::On
		};
	}

	fn set_label(&mut self, label: &str) {
		self.label = label.to_string();
	}

    fn new(state: PinState, label: String) -> Self {
        Pin { state: state, label }
    }
}

