import hid
import time
import sys

VID = 0xCAFE
PID = 0xBABE

def test_leds():
    try:
        # Try to open the device
        # Note: 'hid' library usage might vary slightly depending on backend/version
        # This uses the 'hid' package API which is common.
        h = hid.device()
        h.open(VID, PID)
        h.set_nonblocking(1)
        print(f"Opened device {hex(VID)}:{hex(PID)}")
    except Exception as ex:
        print(f"Error: {ex}")
        print("Ensure the device is connected and you have permissions.")
        print("You may need to run: uv run --with hidapi python test_leds.py")
        sys.exit(1)

    led_count = 6
    
    print("Starting LED test loop. Press Ctrl+C to stop.")

    try:
        while True:
            # Sequence 1: Cycle individual LEDs
            print("Cycling LEDs...")
            for i in range(led_count):
                # Set LED i to Blue
                # Report: [ReportID=0, Cmd=1, Index, R, G, B, ...]
                data = [0x00, 0x01, i, 0, 0, 255] + [0] * 58
                h.write(data)
                time.sleep(0.1)
                
                # Turn off (optional, to make it a chase)
                data = [0x00, 0x01, i, 0, 0, 0] + [0] * 58
                h.write(data)

            # Sequence 2: All Red
            print("All Red")
            for i in range(led_count):
                data = [0x00, 0x01, i, 255, 0, 0] + [0] * 58
                h.write(data)
            time.sleep(0.5)

            # Sequence 3: All Green
            print("All Green")
            for i in range(led_count):
                data = [0x00, 0x01, i, 0, 255, 0] + [0] * 58
                h.write(data)
            time.sleep(0.5)
            
            # Sequence 4: All Off
            print("All Off")
            for i in range(led_count):
                data = [0x00, 0x01, i, 0, 0, 0] + [0] * 58
                h.write(data)
            time.sleep(0.5)

    except KeyboardInterrupt:
        print("\nExiting...")
        h.close()

if __name__ == "__main__":
    test_leds()
