#ifndef __BOOTLOADER_H__
#define __BOOTLOADER_H__

#define BOOT_ADDR 0x3800

inline void bootloader(void);
inline void sw_reset(void);

#endif // __BOOTLOADER_H__
