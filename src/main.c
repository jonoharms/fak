#include "ch55x.h"
#include "keyboard.h"
#include "time.h"
#include "ws2812.h"
#include "bootloader.h"

SBIT(BootKey, 0x90, 1); // P1.1

#ifdef SPLIT_SIDE_CENTRAL
#include "usb.h"

void USB_interrupt(void);
void USB_ISR(void) __interrupt(INT_NO_USB) {
    USB_interrupt();
}
#endif

#ifdef RAW_HID_ENABLE
__bit raw_hid_has_new_data = 0;

void raw_hid_task(void) {
    if (!raw_hid_has_new_data) return;

    raw_hid_has_new_data = 0;

    // Command: 0x01 (set LED color)
    // Data: [LED_INDEX (1 byte), R (1 byte), G (1 byte), B (1 byte)]
    if (raw_hid_rx_buf[0] == 0x01) {
        uint8_t led_idx = raw_hid_rx_buf[1];
        uint8_t r = raw_hid_rx_buf[2];
        uint8_t g = raw_hid_rx_buf[3];
        uint8_t b = raw_hid_rx_buf[4];

        ws2812_set_color(led_idx, r, g, b);
        ws2812_show();
    }
}
#endif

void TMR0_interrupt(void);
void TMR0_ISR(void) __interrupt(INT_NO_TMR0) {
    TMR0_interrupt();
}

#if defined(SPLIT_ENABLE) && !defined(SPLIT_SOFT_SERIAL_PIN)
#ifdef SPLIT_SIDE_PERIPHERAL
void UART0_interrupt(void);
void UART0_ISR(void) __interrupt(INT_NO_UART0) {
    UART0_interrupt();
}
#endif

static void UART0_init(void) {
    // UART0 @ Timer1, 750k bps
    SM0 = 0;
    SM1 = 1;
    PCON |= SMOD;

    TMOD = TMOD & ~bT1_GATE & ~bT1_CT & ~MASK_T1_MOD | bT1_M1;
    T2MOD |= bTMR_CLK | bT1_CLK;
#if CH55X == 2
    TH1 = 254;
#elif CH55X == 9
    TH1 = 255;
#endif
    TR1 = 1;
    TI = 1;
}
#endif

static void main(void) {
    CLK_init();

    // Check if bootloader key (P1.1) is pressed (low)
    if (BootKey == 0) {
        bootloader();
    }

#if defined(SPLIT_ENABLE) && !defined(SPLIT_SOFT_SERIAL_PIN)
    UART0_init();
#endif
#ifdef SPLIT_SIDE_CENTRAL
    TMR0_init();
    USB_init();
#endif
    keyboard_init();

#ifdef UART0_ALT
    PIN_FUNC |= bUART0_PIN_X;
#endif
#ifdef UART1_ALT
    PIN_FUNC |= bUART1_PIN_X;
#endif

#if CH55X == 9
    // This is to get same behavior as CH552 to drive pins high immediately
    P0_DIR = 0xFF;
    P1_DIR = 0xFF;
    P2_DIR = 0xFF;
    P3_DIR = 0xFF;
#endif

#ifdef WS2812_COUNT
    // Configure P3.4 (LED) as Push-Pull
    // P3_MOD_OC &= ~(1<<4);
    // P3_DIR_PU |= (1<<4);
    // However, P3_MOD_OC/DIR_PU are SFRs.
    P3_MOD_OC &= 0xEF;
    P3_DIR_PU |= 0x10;
    WS2812_PIN = 0;

    // LED Chase on startup
    for (uint8_t i = 0; i < WS2812_COUNT; i++) {
        ws2812_set_color(i, 20, 0, 20);
        ws2812_show();
        delay(1000 / WS2812_COUNT);
        ws2812_set_color(i, 0, 0, 0);
    }
    ws2812_show();
#endif

    EA = 1;

    while (1) {
        keyboard_scan();
#ifdef RAW_HID_ENABLE
        raw_hid_task();
#endif
    }
}
