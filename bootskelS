.code16
.globl start
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
mov $0x0c,  %cx # number of characters
mov $hello, %bp # string
int $0x10       # video+text output

stop:
jmp stop

hello:
.string "Hello World!"

.org 0x1fe
.word 0xAA55
