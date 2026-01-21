use anyhow::{Context, Result};
use clap::Parser;
use hidapi::HidApi;
use std::{thread, time::Duration};

#[derive(Parser)]
#[command(author, version, about, long_about = None)]
struct Cli {
    /// Run a test animation loop
    #[arg(short, long)]
    test: bool,

    /// LED Index (0-based)
    #[arg(short, long, default_value_t = 0)]
    index: u8,

    /// Red component (0-255)
    #[arg(short, long, default_value_t = 0)]
    red: u8,

    /// Green component (0-255)
    #[arg(short, long, default_value_t = 0)]
    green: u8,

    /// Blue component (0-255)
    #[arg(short, long, default_value_t = 0)]
    blue: u8,
}

const VID: u16 = 0xCAFE;
const PID: u16 = 0xBABE;
const LED_COUNT: u8 = 6;

fn main() -> Result<()> {
    let cli = Cli::parse();
    let api = HidApi::new().context("Failed to initialize HID API")?;

    let device = loop {
        match api.open(VID, PID) {
            Ok(dev) => break dev,
            Err(_) => {
                println!("Waiting for device {:04x}:{:04x}...", VID, PID);
                thread::sleep(Duration::from_secs(1));
            }
        }
    };

    if cli.test {
        println!("Starting LED test loop. Press Ctrl+C to stop.");
        loop {
            // Red Chase
            run_chase(&device, 255, 0, 0)?;
            thread::sleep(Duration::from_millis(500));
            // Green Chase
            run_chase(&device, 0, 255, 0)?;
            thread::sleep(Duration::from_millis(500));
            // Blue Chase
            run_chase(&device, 0, 0, 255)?;
            thread::sleep(Duration::from_millis(500));
            // All White
            set_all(&device, 50, 50, 50)?;
            thread::sleep(Duration::from_millis(500));

            // All Off
            set_all(&device, 0, 0, 0)?;
            thread::sleep(Duration::from_millis(500));
        }
    } else {
        send_color(&device, cli.index, cli.red, cli.green, cli.blue)?;
        println!(
            "Sent LED update: Index={}, R={}, G={}, B={}",
            cli.index, cli.red, cli.green, cli.blue
        );
    }

    Ok(())
}

fn send_color(device: &hidapi::HidDevice, index: u8, r: u8, g: u8, b: u8) -> Result<()> {
    let mut buf = [0u8; 64];
    buf[0] = 0x00; // Report ID
    buf[1] = 0x01; // Command: Set LED
    buf[2] = index;
    buf[3] = r;
    buf[4] = g;
    buf[5] = b;

    device.write(&buf).context("Failed to write to device")?;
    Ok(())
}

fn run_chase(device: &hidapi::HidDevice, r: u8, g: u8, b: u8) -> Result<()> {
    println!("Starting chase: R={}, G={}, B={}", r, g, b);
    for i in 0..LED_COUNT {
        send_color(device, i, r, g, b)?;
        thread::sleep(Duration::from_millis(100));
        send_color(device, i, 0, 0, 0)?; // Turn off for chase effect
        thread::sleep(Duration::from_millis(20)); // Prevent packet flood
    }
    Ok(())
}

fn set_all(device: &hidapi::HidDevice, r: u8, g: u8, b: u8) -> Result<()> {
    for i in 0..LED_COUNT {
        send_color(device, i, r, g, b)?;
        thread::sleep(Duration::from_millis(10));
    }
    Ok(())
}
