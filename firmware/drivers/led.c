/***************************************************************************
 *             __________               __   ___.
 *   Open      \______   \ ____   ____ |  | _\_ |__   _______  ___
 *   Source     |       _//  _ \_/ ___\|  |/ /| __ \ /  _ \  \/  /
 *   Jukebox    |    |   (  <_> )  \___|    < | \_\ (  <_> > <  <
 *   Firmware   |____|_  /\____/ \___  >__|_ \|___  /\____/__/\_ \
 *                     \/            \/     \/    \/            \/
 * $Id$
 *
 * Copyright (C) 2002 by Alan Korr
 *
 * All files in this archive are subject to the GNU General Public License.
 * See the file COPYING in the source tree root for full license agreement.
 *
 * This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
 * KIND, either express or implied.
 *
 ****************************************************************************/
#include "config.h"
#include <stdbool.h>
#include "cpu.h"
#include "led.h"
#include "system.h"
#include "kernel.h"

static bool current;

#ifdef HAVE_LED

static bool xor;

void led(bool on)
{
    current = on;
    if ( on ^ xor )
#ifdef GMINI_ARCH
        P2 |= 1;
    else
        P2 &= ~1;
#else
    {
        or_b(0x40, &PBDRL);
    }
    else
    {
        and_b(~0x40, &PBDRL);
    }
#endif
}

void invert_led(bool on)
{
    if ( on )
    {
        xor = 1;
    }
    else
    {
        xor = 0;
    }
    led(current);
}

#else /* no LED, just status update */

static long delay; 

void led(bool on)
{
	if (current && !on) /* switching off */
	{
		delay = current_tick + HZ/2; /* delay the "off" status a bit */
	}
    current = on;
}

void invert_led(bool on)
{
    (void)on; /* no invert feature */
}

bool led_read(void) /* read by status bar update */
{
	return (current || TIME_BEFORE(current_tick, delay));
}

#endif // #ifdef HAVE_LED
