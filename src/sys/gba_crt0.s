// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2022

    .section    .gba_crt0, "ax"
    .global     entrypoint
    .cpu        arm7tdmi

    .arm

entrypoint:
    b       header_end

    .fill   156, 1, 0   // Nintendo Logo
    .fill   12, 1, 0    // Game Title
    .fill   4, 1, 0     // Game Code
    .byte   0x30, 0x30  // Maker Code ("00")
    .byte   0x96        // Fixed Value (must be 0x96)
    .byte   0x00        // Main unit code
    .byte   0x00        // Device Type
    .fill   7, 1, 0     // Reserved Area
    .byte   0x00        // Software version
    .byte   0x00        // Complement check (header checksum)
    .byte   0x00, 0x00  // Reserved Area

header_end:
    b       start_vector

    // Multiboot Header Entries
    .byte   0           // Boot mode
    .byte   0           // Slave ID Number
    .fill   26, 1, 0    // Not used
    .word   0           // JOYBUS entrypoint

    .align

start_vector:

    // Disable interrupts
    mov     r0, #0x04000000
    mov     r1, #0
    str     r1, [r0, #0x0208] // IME

    // Setup IRQ mode stack
    mov     r0, #0x12
    msr     cpsr, r0
    ldr     sp, =__STACK_IRQ_END__

    // Setup system mode stack
    mov     r0, #0x1F
    msr     cpsr, r0
    ldr     sp, =__STACK_USR_END__

    // Switch to Thumb mode
    add     r0, pc, #1
    bx      r0

    .thumb

    // Clear IWRAM
    mov     r0, #0
    ldr     r1, =#0x03000000
    ldr     r2, =#(32 * 1024 / 4)
    bl      BlockFill

    // Copy data section from ROM to RAM
    ldr     r0, =__DATA_LMA__
    ldr     r1, =__DATA_START__
    ldr     r2, =__DATA_SIZE__
    lsr     r2,#2
    bl      BlockCopy

    // Copy IWRAM data from ROM to RAM
    ldr     r0, =__IWRAM_LMA__
    ldr     r1, =__IWRAM_START__
    ldr     r2, =__IWRAM_SIZE__
    lsr     r2,#2
    bl      BlockCopy

    // Clear EWRAM
    mov     r0, #0
    ldr     r1, =#0x02000000
    ldr     r2, =#(256 * 1024 / 4)
    bl      BlockFill

    // Copy EWRAM data from ROM to RAM
    ldr     r0, =__EWRAM_LMA__
    ldr     r1, =__EWRAM_START__
    ldr     r2, =__EWRAM_SIZE__
    lsr     r2,#2
    bl      BlockCopy

    // Initialize Heap
    ldr     r0, =__HEAP_START__
    ldr     r1, =__HEAP_SIZE__
    sub     r1, #4
    neg     r1, r1
    str     r1, [r0]

    // Call main()
    mov     r0, #0 // int argc
    mov     r1, #0 // char *argv[]
    ldr     r2, =main
    bl      go
go:
    bx      r2
    // If main() returns, reboot the GBA using SoftReset

    .global Reset
Reset:
    swi     #0x00

    .global BlockFill
    .global BlockCopy
// r0: src
// r1: dst
// r2: numWords
BlockFill:
    ldr     r3,=#(1 << 24)
    orr     r2,r3
BlockCopy:
    swi     #0x0C
    bx      lr

    .global Halt
Halt:
    swi     #0x02
    bx      lr

    .global Stop
Stop:
    swi     #0x03
    bx      lr

    .global IntrWait
// r0: clear IF?
// r1: interrupts to wait for
IntrWait:
    swi     #0x04
    bx      lr

    .global VBlankIntrWait
VBlankIntrWait:
    swi     #0x05
    bx      lr

    .global Sqrt
// r0: u32
Sqrt:
    swi     #0x08
    bx       lr

    .global Atan
// r0: Tan, 16bit (1bit sign, 1bit integral part, 14bit decimal part)
// return: "-PI/2<THETA/<PI/2" in a range of C000h-4000h.
Atan:
    swi     #0x09
    bx      lr

    .global Atan2
// r0: x, 16bit (1bit sign, 1bit integral part, 14bit decimal part)
// r1: y, same
Atan2:
    swi     #0x0A
    bx      lr

    .global Diff8bitUnFilter
Diff8bitUnFilter:
    swi     #0x16
    bx      lr

    .global Diff16bitUnFilter
Diff16bitUnFilter:
    swi     #0x18
    bx      lr

    .global HuffUnComp
HuffUnComp:
    swi     #0x13
    bx      lr

    .global LZ77UnComp
LZ77UnComp:
    swi     #0x11
    bx      lr

    .global RLUnComp
RLUnComp:
    swi     #0x14
    bx      lr


// set C division and modulo operators to call BIOS
    .global __aeabi_idiv
    .global __aeabi_uidiv
    .global __aeabi_idivmod
    .global __aeabi_uidivmod
__aeabi_idiv:
__aeabi_idivmod:
    swi     #0x06
    bx      lr
__aeabi_uidiv:
__aeabi_uidivmod:
    swi     #0x06
    mov     r0,r3
    bx      lr

    .align
    .pool
    .end
