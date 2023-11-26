mod udp_client;

use std::error::Error;

use clap::{Parser, Subcommand};
use udp_client::udp_send;

use crate::udp_client::Utf8Len;

trait ToState {
    fn to_state(&self) -> String;
}

impl ToState for bool {
    fn to_state(&self) -> String {
        if *self { "On".into() } else { "Off".into() }
    }
}

#[derive(Debug, Parser)]
struct Argv {
    #[arg(long, default_value_t = false)]
    status: bool,

    #[arg(long, default_value_t = false)]
    show: bool,

    #[arg(long, default_value_t = false)]
    reboot: bool,

    #[arg(short = 'n', long)]
    on: Option<u8>,

    #[arg(short = 'f', long)]
    off: Option<u8>,

    #[arg(short, long)]
    toggle: Option<u8>,

    #[command(subcommand)]
    label: Option<ELabel>
}

#[derive(Debug, Subcommand)]
enum ELabel {
    Label {
        label: String,
        pin: u8
    }
}

#[tokio::main]
async fn main() -> Result<(), Box<dyn Error>> {
    let argv = Argv::parse();

    if argv.status || argv.status {
        let res = udp_send("SHOW").await;

        if res.is_err() {
            println!("{}", res.err().unwrap());
        }
        else {   
            let (pins, longest) = res.unwrap();
            
            for p in pins {
                println!("{}{}: {}", p.label, " ".repeat(longest - p.label.utf8_strlen()), p.state.to_state());
            }
        }
    }

    if argv.reboot {
        let res = udp_send("REBOOT").await;

        if res.is_err() {
            println!("{}", res.err().unwrap());
        }
        else {   
            let (pins, longest) = res.unwrap();
            
            for p in pins {
                println!("{}{}: {}", p.label, " ".repeat(longest - p.label.utf8_strlen()), p.state.to_state());
            }
        }
    }

    if argv.on.is_some() {
        let res = udp_send(&format!("SET PIN {} HIGH", argv.on.unwrap())).await;

        if res.is_err() {
            println!("{}", res.err().unwrap());
        }
        else {   
            let (pins, _) = res?;
            let pin = &pins[0];
            println!("{}: {}", pin.label, pin.state.to_state());
        }
    }

    if argv.off.is_some() {
        let res = udp_send(&format!("SET PIN {} LOW", argv.off.unwrap())).await;

        if res.is_err() {
            println!("{}", res.err().unwrap());
        }
        else {   
            let (pins, _) = res?;
            let pin = &pins[0];
            println!("{}: {}", pin.label, pin.state.to_state());
        }
    }

    if argv.toggle.is_some() {
        let res = udp_send(&format!("TOGGLE PIN {}", argv.toggle.unwrap())).await;

        if res.is_err() {
            println!("{}", res.err().unwrap());
        }
        else {   
            let (pins, _) = res?;
            let pin = &pins[0];
            println!("{}: {}", pin.label, pin.state.to_state());
        }
    }

    if argv.label.is_some() {
        match argv.label.unwrap() {
            ELabel::Label { label, pin } => {
                let res = udp_send(&format!("LABEL PIN {} {}", pin, label)).await;

                if res.is_err() {
                    println!("{}", res.err().unwrap());
                }
                else {   
                    let (pins, _) = res?;
                    let pin = &pins[0];
                    println!("{}: {}", pin.label, pin.state.to_state());
                }
            },
        }
    }

    Ok(())
}
