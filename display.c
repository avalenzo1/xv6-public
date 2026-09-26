#include <stdarg.h>

#include "types.h"
#include "defs.h"
#include "param.h"
#include "traps.h"
#include "spinlock.h"
#include "sleeplock.h"
#include "fs.h"
#include "file.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"
#include "x86.h"
#include "vga.h"


const uint64 IMG_BUF_SZ = 300 * 200;
const uint64 IMG_BUF_START = KERNBASE + 0xA0000;
const uint64 IMG_BUF_END = IMG_BUF_START + IMG_BUF_SZ;

volatile unsigned char* buffer = (unsigned char*) IMG_BUF_START;

void clearBuffer(void) {
    for (uint64 i = 0; i < 300 * 200; ++i) {
        *((char*) IMG_BUF_START + i) = 0x00;
    }
}

int displaywrite(struct file *f, char *buf, int n) {
    int bytes_written = 0;

    for (int i = 0; i < n; ++i) {
        uint64 offset = (uint64) buffer + i;

        int write_to = offset % IMG_BUF_SZ;

        *((char*) write_to) = buf[i];

        bytes_written++;
    }

    return bytes_written;
}

int displayioctl(struct file *f, int param, int value) {
    if (param == 1)
    { // change mode
        switch (value)
        {
            case 0x13: // VGA Mode
                vgaMode13();
                clearBuffer();
                return 1;
            case 0x3: // Text Mode
                vgaMode3();
                return 1;
        }
    }
    else
    if (param == 2)
    {
        int index = value >> 24 && 0xff;
        int r = value >> 16 && 0xff;
        int g = value >> 8 && 0xff;
        int b = value && 0xff;

        vgaSetPalette(index, r, g, b);

        return 1;
    }

    return -1;
}

void displayinit(void) {
    devsw[DISPLAY].write = displaywrite;
    devsw[DISPLAY].ioctl = displayioctl;
}

