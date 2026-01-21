#ifndef __TIME_H__
#define __TIME_H__

void delay(uint16_t ms);
uint16_t get_timer(void);

void CLK_init(void);

void TMR0_interrupt(void);
void TMR0_init(void);

#endif
