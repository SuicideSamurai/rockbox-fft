/***************************************************************************
 *             __________               __   ___.
 *   Open      \______   \ ____   ____ |  | _\_ |__   _______  ___
 *   Source     |       _//  _ \_/ ___\|  |/ /| __ \ /  _ \  \/  /
 *   Jukebox    |    |   (  <_> )  \___|    < | \_\ (  <_> > <  <
 *   Firmware   |____|_  /\____/ \___  >__|_ \|___  /\____/__/\_ \
 *                     \/            \/     \/    \/            \/
 * $Id$
 *
 * Copyright (C) 2002 Bj�rn Stenberg
 *
 * All files in this archive are subject to the GNU General Public License.
 * See the file COPYING in the source tree root for full license agreement.
 *
 * This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
 * KIND, either express or implied.
 *
 ****************************************************************************/
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <atoi.h>
#include <timefuncs.h>
#include "button.h"
#include "lcd.h"
#include "dir.h"
#include "file.h"
#include "kernel.h"
#include "sprintf.h"
#include "screens.h"
#include "misc.h"
#include "mas.h"
#include "plugin.h"
#include "lang.h"
#include "keyboard.h"
#include "mpeg.h"

#ifdef HAVE_LCD_BITMAP
#include "widgets.h"
#endif

#ifdef SIMULATOR
  #include <debug.h>
  #ifdef WIN32
    #include "plugin-win32.h"
    #define PREFIX(_x_) _x_
  #else
    #include <dlfcn.h>
    #define PREFIX(_x_) x11_ ## _x_
  #endif
#else
#define PREFIX(_x_) _x_
#endif

#define PLUGIN_BUFFER_SIZE 0x8000

#ifdef SIMULATOR
static unsigned char pluginbuf[PLUGIN_BUFFER_SIZE];
#else
extern unsigned char pluginbuf[];
#endif

static bool plugin_loaded = false;
static int  plugin_size = 0;

static int plugin_test(int api_version, int model, int memsize);

static struct plugin_api rockbox_api = {
    PLUGIN_API_VERSION,

    plugin_test,
    
    /* lcd */
    lcd_clear_display,
    lcd_puts,
    lcd_puts_scroll,
    lcd_stop_scroll,
#ifdef HAVE_LCD_CHARCELLS
    lcd_define_pattern,
    lcd_get_locked_pattern,
    lcd_unlock_pattern,
    lcd_putc,
#else
    lcd_putsxy,
    lcd_bitmap,
    lcd_drawline,
    lcd_clearline,
    lcd_drawpixel,
    lcd_clearpixel,
    lcd_setfont,
    lcd_clearrect,
    lcd_fillrect,
    lcd_drawrect,
    lcd_invertrect,
    lcd_getstringsize,
    lcd_update,
    lcd_update_rect,
    progressbar,
    slidebar,
    scrollbar,
#ifndef SIMULATOR
    lcd_roll,
#endif
#endif

    /* button */
    button_get,
    button_get_w_tmo,

    /* file */
    (open_func)PREFIX(open),
    PREFIX(close),
    (read_func)read,
    lseek,
    (creat_func)PREFIX(creat),
    (write_func)write,
    remove,
    rename,
    ftruncate,
    PREFIX(filesize),
    fprintf,
    read_line,

    /* dir */
    PREFIX(opendir),
    PREFIX(closedir),
    PREFIX(readdir),

    /* kernel */
    PREFIX(sleep),
    usb_screen,
    &current_tick,

    /* strings and memory */
    snprintf,
    strcpy,
    strlen,
    memset,
    memcpy,

    /* sound */
#ifndef SIMULATOR
#ifdef HAVE_MAS3587F
    mas_codec_readreg,
#endif
#endif
    
    /* misc */
    srand,
    rand,
    splash,
    (qsort_func)qsort,
    kbd_input,
    mpeg_current_track,
    atoi,
    get_time,
    plugin_get_buffer,

    /* new stuff at the end, sort into place next time the API gets incompatible */

#ifndef HAVE_LCD_CHARCELLS
    &lcd_framebuffer[0][0],
    lcd_blit,
#endif
    yield,
};

int plugin_load(char* plugin, void* parameter)
{
    enum plugin_status (*plugin_start)(struct plugin_api* api, void* param);
    int rc;
    char buf[64];
#ifdef SIMULATOR
    void* pd;
    char path[256];
#else
    int fd;
#endif

#ifdef HAVE_LCD_BITMAP
    int xm,ym;
    lcd_clear_display();
    xm = lcd_getxmargin();
    ym = lcd_getymargin();
    lcd_setmargins(0,0);
    lcd_update();
#else
    lcd_clear_display();
#endif
#ifdef SIMULATOR
#ifdef WIN32
    snprintf(path, sizeof path, "%s", plugin);
#else
    snprintf(path, sizeof path, "archos%s", plugin);
#endif
    pd = dlopen(path, RTLD_NOW);
    if (!pd) {
        snprintf(buf, sizeof buf, "Can't open %s", plugin);
        splash(HZ*2, true, buf);
        DEBUGF("dlopen(%s): %s\n",path,dlerror());
        dlclose(pd);
        return -1;
    }

    plugin_start = dlsym(pd, "plugin_start");
    if (!plugin_start) {
        plugin_start = dlsym(pd, "_plugin_start");
        if (!plugin_start) {
            splash(HZ*2, true, "Can't find entry point");
            dlclose(pd);
            return -1;
        }
    }
#else
    fd = open(plugin, O_RDONLY);
    if (fd < 0) {
        snprintf(buf, sizeof buf, str(LANG_PLUGIN_CANT_OPEN), plugin);
        splash(HZ*2, true, buf);
        return fd;
    }

    plugin_start = (void*)&pluginbuf;
    plugin_size = read(fd, plugin_start, PLUGIN_BUFFER_SIZE);
    close(fd);
    if (plugin_size < 0) {
        /* read error */
        snprintf(buf, sizeof buf, str(LANG_READ_FAILED), plugin);
        splash(HZ*2, true, buf);
        return -1;
    }
    if (plugin_size == 0) {
        /* loaded a 0-byte plugin, implying it's not for this model */
        splash(HZ*2, true, str(LANG_PLUGIN_WRONG_MODEL));
        return -1;
    }
#endif

    plugin_loaded = true;
    rc = plugin_start(&rockbox_api, parameter);
    plugin_loaded = false;

    switch (rc) {
        case PLUGIN_OK:
            break;

        case PLUGIN_USB_CONNECTED:
            return PLUGIN_USB_CONNECTED;

        case PLUGIN_WRONG_API_VERSION:
            splash(HZ*2, true, str(LANG_PLUGIN_WRONG_VERSION));
            break;

        case PLUGIN_WRONG_MODEL:
            splash(HZ*2, true, str(LANG_PLUGIN_WRONG_MODEL));
            break;

        default:
            splash(HZ*2, true, str(LANG_PLUGIN_ERROR));
            break;
    }

#ifdef SIMULATOR
    dlclose(pd);
#endif

#ifdef HAVE_LCD_BITMAP
    /* restore margins */
    lcd_setmargins(xm,ym);
#endif

    return PLUGIN_OK;
}

/* Returns a pointer to the portion of the plugin buffer that is not already
   being used.  If no plugin is loaded, returns the entire plugin buffer */
void* plugin_get_buffer(int* buffer_size)
{
    int buffer_pos;

    if (plugin_loaded)
    {
        if (plugin_size >= PLUGIN_BUFFER_SIZE)
            return NULL;
        
        *buffer_size = PLUGIN_BUFFER_SIZE-plugin_size;
        buffer_pos = plugin_size;
    }
    else
    {
        *buffer_size = PLUGIN_BUFFER_SIZE;
        buffer_pos = 0;
    }

    return &pluginbuf[buffer_pos];
}

static int plugin_test(int api_version, int model, int memsize)
{
    if (api_version < PLUGIN_MIN_API_VERSION ||
        api_version > PLUGIN_API_VERSION)
        return PLUGIN_WRONG_API_VERSION;

    if (model != MODEL)
        return PLUGIN_WRONG_MODEL;

    if (memsize != MEM)
        return PLUGIN_WRONG_MODEL;
    
    return PLUGIN_OK;
}
