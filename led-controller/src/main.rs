use clap::Parser;
use hidapi::HidApi;
use anyhow::{Result, Context};

#[derive(Parser)]
#[command(author, version, about, long_about = None)]
struct Cli {
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

fn main() -> Result<()> {
    let cli = Cli::parse();

    let api = HidApi::new().context("Failed to initialize HID API")?;

    let device = api.open(VID, PID).context(format!("Failed to open device {:04x}:{:04x}. Is it connected and permissions set?", VID, PID))?;

    // Report format: [Command, Index, R, G, B]
    // Padded to 64 bytes (USB_EP4_SIZE)
    let mut buf = [0u8; 64];
    
    // On some platforms/backends, the first byte of the buffer sent to write() is the Report ID.
    // If the device does not use Report IDs (or uses ID 0), this byte should be 0.
    // However, the C code expects `raw_hid_rx_buf[0] == 0x01`.
    // 
    // Let's check src/usb.c. 
    // USB_HID_RAW_REPORT_DESCR uses a Usage Page (Vendor Defined). It doesn't seem to explicit define Report IDs.
    // If no report IDs are defined, data starts at byte 0.
    // 
    // HIDAPI write() convention:
    // "The first byte of the data buffer passed to write() must contain the Report ID. For devices which only support a single report, this must be set to 0x0."
    // 
    // If we send [0x00, 0x01, idx, r, g, b ...], the device receives [0x01, idx, r, g, b ...] because the first byte is consumed as Report ID by the OS/driver stack in many cases, BUT `raw_hid_rx_buf` in firmware is filled from the USB packet.
    // 
    // Let's try prepending 0x00 as Report ID.
    
    buf[0] = 0x00; // Report ID (fake)
    buf[1] = 0x01; // Command (Set LED)
    buf[2] = cli.index;
    buf[3] = cli.red;
    buf[4] = cli.green;
    buf[5] = cli.blue;

    println!("Sending LED update: Index={}, R={}, G={}, B={}", cli.index, cli.red, cli.green, cli.blue);

    let res = device.write(&buf).context("Failed to write to device")?;
    
    println!("Wrote {} bytes", res);

    Ok(())
}