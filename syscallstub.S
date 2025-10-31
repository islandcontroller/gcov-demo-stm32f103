.syntax unified
.cpu cortex-m3
.fpu softvfp
.thumb

.macro def_fn fn
    .globl \fn
    .type \fn, %function
    \fn:
.endm

.section  .text
.balign 4
def_fn _open
def_fn _read
def_fn _write
def_fn _lseek
def_fn _fstat
def_fn _close
def_fn _kill
    ldr     r0,    =-1
    blx     lr

def_fn _getpid
def_fn _isatty
    ldr     r0,    =0
    blx     lr
