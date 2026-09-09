.code16
.globl start
start:
movb $1,    %al # write mode
movb $0x13, %ah # function code (write string)
movb $0,    %bh # page number
movb $7,    %bl # color
mov $0x0c,  %cx # number of characters
movb $10,    %dh # row
movb $0,    %dl # col
mov $hello, %bp # string
int $0x10       # video+text output

stop:
jmp stop

hello:
.string "Hello World!"

.org 0x1fe
.word 0xAA55
