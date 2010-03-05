/****************************************************************************
 *             __________               __   ___.
 *   Open      \______   \ ____   ____ |  | _\_ |__   _______  ___
 *   Source     |       _//  _ \_/ ___\|  |/ /| __ \ /  _ \  \/  /
 *   Jukebox    |    |   (  <_> )  \___|    < | \_\ (  <_> > <  <
 *   Firmware   |____|_  /\____/ \___  >__|_ \|___  /\____/__/\_ \
 *                     \/            \/     \/    \/            \/
 *
 * Copyright (C) 2007 Michael Giacomelli
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

#include "wmadec.h"
#include "wmafixed.h"
#include <codecs.h>

fixed64 IntTo64(int x){
    fixed64 res = 0;
    unsigned char *p = (unsigned char *)&res;

#ifdef ROCKBOX_BIG_ENDIAN
    p[5] = x & 0xff;
    p[4] = (x & 0xff00)>>8;
    p[3] = (x & 0xff0000)>>16;
    p[2] = (x & 0xff000000)>>24;
#else
    p[2] = x & 0xff;
    p[3] = (x & 0xff00)>>8;
    p[4] = (x & 0xff0000)>>16;
    p[5] = (x & 0xff000000)>>24;
#endif
    return res;
}

int IntFrom64(fixed64 x)
{
    int res = 0;
    unsigned char *p = (unsigned char *)&x;

#ifdef ROCKBOX_BIG_ENDIAN
    res = p[5] | (p[4]<<8) | (p[3]<<16) | (p[2]<<24);
#else
    res = p[2] | (p[3]<<8) | (p[4]<<16) | (p[5]<<24);
#endif
    return res;
}

fixed32 Fixed32From64(fixed64 x)
{
  return x & 0xFFFFFFFF;
}

fixed64 Fixed32To64(fixed32 x)
{
  return (fixed64)x;
}


/*
 * Helper functions for wma_window.
 *
 *
 */

#ifdef CPU_ARM
inline void vector_fmul_add_add(fixed32 *dst, const fixed32 *data,
                         const fixed32 *window, int n)
{
    /* Block sizes are always power of two */
    asm volatile (
        "0:"
        "ldmia %[d]!, {r0, r1};"
        "ldmia %[w]!, {r4, r5};"
        /* consume the first data and window value so we can use those
         * registers again */
        "smull r8, r9, r0, r4;"
        "ldmia %[dst], {r0, r4};"
        "add   r0, r0, r9, lsl #1;"  /* *dst=*dst+(r9<<1)*/
        "smull r8, r9, r1, r5;"
        "add   r1, r4, r9, lsl #1;"
        "stmia %[dst]!, {r0, r1};"
        "subs  %[n], %[n], #2;"
        "bne   0b;"
        : [d] "+r" (data), [w] "+r" (window), [dst] "+r" (dst), [n] "+r" (n)
        : : "r0", "r1", "r4", "r5", "r8", "r9", "memory", "cc");
}

inline void vector_fmul_reverse(fixed32 *dst, const fixed32 *src0, const fixed32 *src1,
                         int len)
{
    /* Block sizes are always power of two */
    asm volatile (
        "add   %[s1], %[s1], %[n], lsl #2;"
        "0:"
        "ldmia %[s0]!, {r0, r1};"
        "ldmdb %[s1]!, {r4, r5};"
        "smull r8, r9, r0, r5;"
        "mov   r0, r9, lsl #1;"
        "smull r8, r9, r1, r4;"
        "mov   r1, r9, lsl #1;"
        "stmia %[dst]!, {r0, r1};"
        "subs  %[n], %[n], #2;"
        "bne   0b;"
        : [s0] "+r" (src0), [s1] "+r" (src1), [dst] "+r" (dst), [n] "+r" (len)
        : : "r0", "r1", "r4", "r5", "r8", "r9", "memory", "cc");
}

#elif defined(CPU_COLDFIRE)

inline void vector_fmul_add_add(fixed32 *dst, const fixed32 *data,
                         const fixed32 *window, int n)
{
    /* Block sizes are always power of two. Smallest block is always way bigger
     * than four too.*/
    asm volatile (
        "0:"
        "movem.l (%[d]), %%d0-%%d3;"
        "movem.l (%[w]), %%d4-%%d5/%%a0-%%a1;"
        "mac.l %%d0, %%d4, %%acc0;"
        "mac.l %%d1, %%d5, %%acc1;"
        "mac.l %%d2, %%a0, %%acc2;"
        "mac.l %%d3, %%a1, %%acc3;"
        "lea.l (16, %[d]), %[d];"
        "lea.l (16, %[w]), %[w];"
        "movclr.l %%acc0, %%d0;"
        "movclr.l %%acc1, %%d1;"
        "movclr.l %%acc2, %%d2;"
        "movclr.l %%acc3, %%d3;"
        "movem.l (%[dst]), %%d4-%%d5/%%a0-%%a1;"
        "add.l %%d4, %%d0;"
        "add.l %%d5, %%d1;"
        "add.l %%a0, %%d2;"
        "add.l %%a1, %%d3;"
        "movem.l %%d0-%%d3, (%[dst]);"
        "lea.l (16, %[dst]), %[dst];"
        "subq.l #4, %[n];"
        "jne 0b;"
        : [d] "+a" (data), [w] "+a" (window), [dst] "+a" (dst), [n] "+d" (n)
        : : "d0", "d1", "d2", "d3", "d4", "d5", "a0", "a1", "memory", "cc");
}

inline void vector_fmul_reverse(fixed32 *dst, const fixed32 *src0, const fixed32 *src1,
                         int len)
{
    /* Block sizes are always power of two. Smallest block is always way bigger
     * than four too.*/
    asm volatile (
        "lea.l (-16, %[s1], %[n]*4), %[s1];"
        "0:"
        "movem.l (%[s0]), %%d0-%%d3;"
        "movem.l (%[s1]), %%d4-%%d5/%%a0-%%a1;"
        "mac.l %%d0, %%a1, %%acc0;"
        "mac.l %%d1, %%a0, %%acc1;"
        "mac.l %%d2, %%d5, %%acc2;"
        "mac.l %%d3, %%d4, %%acc3;"
        "lea.l (16, %[s0]), %[s0];"
        "lea.l (-16, %[s1]), %[s1];"
        "movclr.l %%acc0, %%d0;"
        "movclr.l %%acc1, %%d1;"
        "movclr.l %%acc2, %%d2;"
        "movclr.l %%acc3, %%d3;"
        "movem.l %%d0-%%d3, (%[dst]);"
        "lea.l (16, %[dst]), %[dst];"
        "subq.l #4, %[n];"
        "jne 0b;"
        : [s0] "+a" (src0), [s1] "+a" (src1), [dst] "+a" (dst), [n] "+d" (len)
        : : "d0", "d1", "d2", "d3", "d4", "d5", "a0", "a1", "memory", "cc");
}

#else

inline void vector_fmul_add_add(fixed32 *dst, const fixed32 *src0, const fixed32 *src1, int len){
    int i;
    for(i=0; i<len; i++)
        dst[i] = fixmul32b(src0[i], src1[i]) + dst[i];
}

inline void vector_fmul_reverse(fixed32 *dst, const fixed32 *src0, const fixed32 *src1, int len){
    int i;
    src1 += len-1;
    for(i=0; i<len; i++)
        dst[i] = fixmul32b(src0[i], src1[-i]);
}

#endif

/*
    Not performance senstitive code here

*/

fixed32 fixdiv32(fixed32 x, fixed32 y)
{
    fixed64 temp;

    if(x == 0)
        return 0;
    if(y == 0)
        return 0x7fffffff;
    temp = x;
    temp <<= PRECISION;
    return (fixed32)(temp / y);
}

fixed64 fixdiv64(fixed64 x, fixed64 y)
{
    fixed64 temp;

    if(x == 0)
        return 0;
    if(y == 0)
        return 0x07ffffffffffffffLL;
    temp = x;
    temp <<= PRECISION64;
    return (fixed64)(temp / y);
}

 fixed32 fixsqrt32(fixed32 x)
{

    unsigned long r = 0, s, v = (unsigned long)x;

#define STEP(k) s = r + (1 << k * 2); r >>= 1; \
    if (s <= v) { v -= s; r |= (1 << k * 2); }

    STEP(15);
    STEP(14);
    STEP(13);
    STEP(12);
    STEP(11);
    STEP(10);
    STEP(9);
    STEP(8);
    STEP(7);
    STEP(6);
    STEP(5);
    STEP(4);
    STEP(3);
    STEP(2);
    STEP(1);
    STEP(0);

    return (fixed32)(r << (PRECISION / 2));
}


     static const long cordic_circular_gain = 0xb2458939; /* 0.607252929 */  
     
 /* Table of values of atan(2^-i) in 0.32 format fractions of pi where pi = 0xffffffff / 2 */    
 static const unsigned long atan_table[] = {     
     0x1fffffff, /* +0.785398163 (or pi/4) */    
     0x12e4051d, /* +0.463647609 */  
     0x09fb385b, /* +0.244978663 */  
     0x051111d4, /* +0.124354995 */  
     0x028b0d43, /* +0.062418810 */  
     0x0145d7e1, /* +0.031239833 */  
     0x00a2f61e, /* +0.015623729 */  
     0x00517c55, /* +0.007812341 */  
     0x0028be53, /* +0.003906230 */  
     0x00145f2e, /* +0.001953123 */  
     0x000a2f98, /* +0.000976562 */  
     0x000517cc, /* +0.000488281 */  
     0x00028be6, /* +0.000244141 */  
     0x000145f3, /* +0.000122070 */  
     0x0000a2f9, /* +0.000061035 */  
     0x0000517c, /* +0.000030518 */  
     0x000028be, /* +0.000015259 */  
     0x0000145f, /* +0.000007629 */  
     0x00000a2f, /* +0.000003815 */  
     0x00000517, /* +0.000001907 */  
     0x0000028b, /* +0.000000954 */  
     0x00000145, /* +0.000000477 */  
     0x000000a2, /* +0.000000238 */  
     0x00000051, /* +0.000000119 */  
     0x00000028, /* +0.000000060 */  
     0x00000014, /* +0.000000030 */  
     0x0000000a, /* +0.000000015 */  
     0x00000005, /* +0.000000007 */  
     0x00000002, /* +0.000000004 */  
     0x00000001, /* +0.000000002 */  
     0x00000000, /* +0.000000001 */  
     0x00000000, /* +0.000000000 */  
 };  
     
 /**     
  * Implements sin and cos using CORDIC rotation.    
  *  
  * @param phase has range from 0 to 0xffffffff, representing 0 and  
  *        2*pi respectively.    
  * @param cos return address for cos    
  * @return sin of phase, value is a signed value from LONG_MIN to LONG_MAX,     
  *         representing -1 and 1 respectively.  
  *  
  *        Gives at least 24 bits precision (last 2-8 bits or so are probably off)   
  */     
     
 long fsincos(unsigned long phase, fixed32 *cos)     
 {   
     int32_t x, x1, y, y1;   
     unsigned long z, z1;    
     int i;  
     
     /* Setup initial vector */  
     x = cordic_circular_gain;   
     y = 0;  
     z = phase;  
     
     /* The phase has to be somewhere between 0..pi for this to work right */    
     if (z < 0xffffffff / 4) {   
         /* z in first quadrant, z += pi/2 to correct */     
         x = -x;     
         z += 0xffffffff / 4;    
     } else if (z < 3 * (0xffffffff / 4)) {  
         /* z in third quadrant, z -= pi/2 to correct */     
         z -= 0xffffffff / 4;    
     } else {    
         /* z in fourth quadrant, z -= 3pi/2 to correct */   
         x = -x;     
         z -= 3 * (0xffffffff / 4);  
     }   
     
     /* Each iteration adds roughly 1-bit of extra precision */  
     for (i = 0; i < 31; i++) {  
         x1 = x >> i;    
         y1 = y >> i;    
         z1 = atan_table[i];     
     
         /* Decided which direction to rotate vector. Pivot point is pi/2 */     
         if (z >= 0xffffffff / 4) {  
             x -= y1;    
             y += x1;    
             z -= z1;    
         } else {    
             x += y1;    
             y -= x1;    
             z += z1;    
         }   
     }   
     
     if (cos)    
         *cos = x;   
     
     return y;   
 }
 