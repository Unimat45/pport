use core::arch::asm;

const PORT: u16 = 0x378;

trait PortWrite {
    /// Writes a `Self` value to the given port.
    ///
    /// ## Safety
    ///
    /// This function is unsafe because the I/O port could have side effects that violate memory
    /// safety.
    unsafe fn write_to_port(port: u16, value: Self);
}

trait PortRead {
    /// Reads a `Self` value from the given port.
    ///
    /// ## Safety
    ///
    /// This function is unsafe because the I/O port could have side effects that violate memory
    /// safety.
    unsafe fn read_from_port(port: u16) -> Self;
}

impl PortRead for u8 {
	#[inline]
    unsafe fn read_from_port(port: u16) -> Self {
		let value: u8;
		unsafe {
			asm!("in al, dx", out("al") value, in("dx") port, options(nomem, nostack, preserves_flags));
		}
		return value;
	}
}

impl PortWrite for u8 {
	#[inline]
    unsafe fn write_to_port(port: u16, value: Self) {
		unsafe {
			asm!("out dx, al", in("dx") port, in("al") value, options(nomem, nostack, preserves_flags));
		}
	}
}

pub fn inb() -> u8 {
	unsafe { u8::read_from_port(PORT) }
}

pub fn outb(value: u8) {
	unsafe { u8::write_to_port(PORT, value) }
}
