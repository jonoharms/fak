# /// script
# dependencies = [
#   "hid==1.0.8",
# ]
# ///

from led_controller import LedController
import sys
import time

def main():
    try:
        controller = LedController()
        print("Controller initialized successfully.")
        
        print("Running chase animation...")
        controller.chase_animation(loops=5)
        
    except Exception as e:
        print(f"Error: {e}")
        print("Ensure 'uv pip install hidapi' is run and the device is connected.")
        sys.exit(1)

if __name__ == "__main__":
    main()