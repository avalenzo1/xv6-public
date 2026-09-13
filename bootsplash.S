#include "mmu.h"
#include "asm.h"

.code16
.global start
start:

movb $0x03, %ah # get cursor row and col
movb $0,    %bh # page number
int $0x10 # video+text api call

# dh is row
# dl is col

movb $1,    %al # write mode
movb $0x13, %ah # function code (write string)
movb $0,    %bh # page number
movb $7,    %bl # color
mov $0x10,  %cx # number of characters
mov $hello, %bp # string
int $0x10       # video+text output

# Wait for key press
movb $0, %ah
int $0x16

movb $0, %ah
movb $0x13, %al
int $0x10

# Change to 32-bit protected mode so we can run C code
cli
lgdt    gdtdesc
movl    %cr0, %eax
orl     $CR0_PE, %eax
movl    %eax, %cr0

ljmp    $(SEG_KCODE<<3), $start32
.code32  # Tell assembler to generate 32-bit code now.
start32:

movw    $(SEG_KDATA<<3), %ax    # Our data segment selector
movw    %ax, %ds                # -> DS: Data Segment
movw    %ax, %ss                # -> SS: Stack Segment

mov     $0x1000, %esp
call splashmain

loop:
    jmp loop

hello: .string "xv6 @ UIC rocks!"

# Bootstrap GDT
.p2align 2                                # force 4 byte alignment
gdt:
  SEG_NULLASM                             # null seg
  SEG_ASM(STA_X|STA_R, 0x0, 0xffffffff)   # code seg
  SEG_ASM(STA_W, 0x0, 0xffffffff)         # data seg

gdtdesc:
  .word   (gdtdesc - gdt - 1)             # sizeof(gdt) - 1
  .long   gdt                             # address gdt