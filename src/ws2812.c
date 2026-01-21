#include "ws2812.h"

// LED buffer
#ifndef WS2812_COUNT
#define WS2812_COUNT 1
#endif

__xdata uint8_t led_buffer[WS2812_COUNT * 3];

void ws2812_set_color(uint8_t index, uint8_t r, uint8_t g, uint8_t b) {
    if (index >= WS2812_COUNT) return;
    led_buffer[index * 3 + 0] = g; // WS2812 expects GRB
    led_buffer[index * 3 + 1] = r;
    led_buffer[index * 3 + 2] = b;
}

// Internal ASM worker function
// len passed in DPL
void ws2812_asm_show(uint8_t len) __naked {
    __asm
        mov r7, dpl         ; Save length
        mov dptr, #_led_buffer
        
    00001$:
        movx a, @dptr       ; Load byte from __xdata
        inc dptr
        mov r3, #8          ; 8 bits per byte
        
    00002$:
        rlc a               ; Rotate Left through Carry. MSB -> Carry
        jc 00003$           ; Jump if Carry (1)
        
    ; Send 0
    ; T0H: ~350ns (Target 400ns)
        setb _WS2812_PIN
        nop
        nop
        nop
        nop
        nop
        nop                 ; 6 NOPs + setb(2) = 8 cycles = 333ns
        
    ; T0L: ~800ns (Target 850ns)
        clr _WS2812_PIN
        nop
        nop
        nop
        nop
        nop
        nop
        nop
        nop
        nop
        nop
        nop
        nop
        nop                 ; 13 NOPs + clr(2) + sjmp(3) + overhead = long enough
        sjmp 00004$
        
    00003$: ; Send 1
    ; T1H: ~700ns (Target 800ns)
        setb _WS2812_PIN
        nop
        nop
        nop
        nop
        nop
        nop
        nop
        nop
        nop
        nop
        nop
        nop
        nop
        nop
        nop                 ; 15 NOPs + setb(2) = 17 cycles = 708ns
        
    ; T1L: ~500ns (Target 450ns)
        clr _WS2812_PIN
        nop
        nop
        nop
        nop
        nop                 ; 5 NOPs
        
    00004$:
        djnz r3, 00002$     ; Loop bits
        djnz r7, 00001$     ; Loop bytes
        ret
    __endasm;
}

void ws2812_show(void) {
    EA = 0;
    ws2812_asm_show(WS2812_COUNT * 3);
    EA = 1;
}