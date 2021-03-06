/***************************************************************************
 *             __________               __   ___.
 *   Open      \______   \ ____   ____ |  | _\_ |__   _______  ___
 *   Source     |       _//  _ \_/ ___\|  |/ /| __ \ /  _ \  \/  /
 *   Jukebox    |    |   (  <_> )  \___|    < | \_\ (  <_> > <  <
 *   Firmware   |____|_  /\____/ \___  >__|_ \|___  /\____/__/\_ \
 *                     \/            \/     \/    \/            \/
 * $Id$
 *
 * Copyright (C) 2002 by Linus Nielsen Feltzing
 *
 * Arm bootloader and startup code based on startup.s from the iPodLinux loader
 *
 * Copyright (c) 2003, Daniel Palffy (dpalffy (at) rainstorm.org)
 * Copyright (c) 2005, Bernard Leach <leachbj@bouncycastle.org>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
 * KIND, either express or implied.
 *
 ****************************************************************************/
#include "config.h"
#include "cpu.h"

    .section .init.text,"ax",%progbits

    .global    start
start:
    msr    cpsr, #0xd3 /* enter supervisor mode, disable IRQ/FIQ */

#ifndef CREATIVE_ZVx
    /* Copy exception handler code to address 0 */
    ldr    r2, =_vectorsstart
    ldr    r3, =_vectorsend
    ldr    r4, =_vectorscopy
1:
    cmp    r3, r2
    ldrhi  r5, [r4], #4
    strhi  r5, [r2], #4
    bhi    1b
#endif

    /* Disable data and instruction cache, high vectors (at 0xffff0000 instead of 0x00000000) */
    mrc    p15, 0, r0, c1, c0, 0
    /* clear bits 13, 9:8 (--VI --RS) */
    bic    r0, r0, #0x00003300
    /* clear bits 7, 2:0 (B--- -C-M) */
    bic    r0, r0, #0x00000085
    /* make sure bit 2 (A) Align is set */
    orr    r0, r0, #0x00000002
    mcr    p15, 0, r0, c1, c0, 0

#if !defined(STUB)
    /* Zero out IBSS */
    ldr    r2, =_iedata
    ldr    r3, =_iend
    mov    r4, #0
1:
    cmp    r3, r2
    strhi  r4, [r2], #4
    bhi    1b

#ifndef CREATIVE_ZVx
    /* Copy the IRAM */
    ldr    r2, =_iramcopy
    ldr    r3, =_iramstart
    ldr    r4, =_iramend
1:
    cmp    r4, r3
    ldrhi  r5, [r2], #4
    strhi  r5, [r3], #4
    bhi    1b
#endif
#endif /* !STUB */


    /* Initialise bss section to zero */
    ldr     r2, =_edata
    ldr     r3, =_end
    mov     r4, #0
bsszero:
    cmp     r3, r2
    strhi   r4, [r2], #4
    bhi     bsszero

    /* Set up some stack and munge it with 0xdeadbeef */
    ldr     sp, =stackend
    mov     r3, sp
    ldr     r2, =stackbegin
    ldr     r4, =0xdeadbeef
stackmunge:
    cmp     r3, r2
    strhi   r4, [r2], #4
    bhi     stackmunge

    /* Set up stack for IRQ mode */
    msr    cpsr_c, #0xd2 /* IRQ disabled, FIQ enabled */
    ldr    sp, =irq_stack
    /* Set up stack for FIQ mode */
    msr    cpsr_c, #0xd1 /* IRQ/FIQ disabled */
    ldr    sp, =fiq_stack

    /* Let abort and undefined modes use IRQ stack */
    msr    cpsr_c, #0xd7 /* IRQ/FIQ disabled */
    ldr    sp, =irq_stack
    msr    cpsr_c, #0xdb /* IRQ/FIQ disabled */
    ldr    sp, =irq_stack

    /* Switch to supervisor mode (no IRQ) */
    msr    cpsr_c, #0xd3
    ldr    sp, =stackend

start_loc:
     bl main
    /* main() should never return */

/* Exception handlers. Will be copied to address 0 after memory remapping */
    .section .vectors,"aw"
    b   start
    b   undef_instr_handler
    b   software_int_handler
    b   prefetch_abort_handler
    b   data_abort_handler
    b   reserved_handler
    b   irq_handler
    b   fiq_handler

    .text

/* All illegal exceptions call into UIE with exception address as first
   parameter. This is calculated differently depending on which exception
   we're in. Second parameter is exception number, used for a string lookup
   in UIE.
 */
undef_instr_handler:
    sub    r0, lr, #4
    mov    r1, #0
    b      UIE

/* We run supervisor mode most of the time, and should never see a software
   exception being thrown. Perhaps make it illegal and call UIE?
 */
software_int_handler:
reserved_handler:
    movs   pc, lr

prefetch_abort_handler:
    sub    r0, lr, #4
    mov    r1, #1
    b      UIE

data_abort_handler:
    sub    r0, lr, #8 
    mov    r1, #2
    b      UIE

#ifdef STUB
UIE:
    b UIE
#endif

