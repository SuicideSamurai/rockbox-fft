/***************************************************************************
*             __________               __   ___.
 *   Open      \______   \ ____   ____ |  | _\_ |__   _______  ___
 *   Source     |       _//  _ \_/ ___\|  |/ /| __ \ /  _ \  \/  /
 *   Jukebox    |    |   (  <_> )  \___|    < | \_\ (  <_> > <  <
 *   Firmware   |____|_  /\____/ \___  >__|_ \|___  /\____/__/\_ \
 *                     \/            \/     \/    \/            \/
 * $Id$
 *
 * Copyright (C) 2009 Delyan Kratunov
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
#include "plugin.h"

#include "lib/helper.h"
#include "lib/xlcd.h"
#include "math.h"

PLUGIN_HEADER

#if CONFIG_KEYPAD == ARCHOS_AV300_PAD
#   define FFT_PREV_GRAPH     BUTTON_LEFT
#   define FFT_NEXT_GRAPH    BUTTON_RIGHT
#   define FFT_ORIENTATION  BUTTON_F3
#   define FFT_WINDOW     BUTTON_F1
#   define FFT_SCALE       BUTTON_UP
#   define FFT_COLOR     BUTTON_DOWN
#   define FFT_QUIT     BUTTON_OFF

#elif (CONFIG_KEYPAD == IRIVER_H100_PAD) || \
      (CONFIG_KEYPAD == IRIVER_H300_PAD)
#   define FFT_PREV_GRAPH     BUTTON_LEFT
#   define FFT_NEXT_GRAPH    BUTTON_RIGHT
#   define FFT_ORIENTATION  BUTTON_REC
#   define FFT_WINDOW     BUTTON_SELECT
#   define FFT_SCALE       BUTTON_UP
#   define FFT_COLOR     BUTTON_DOWN
#   define FFT_QUIT     BUTTON_OFF

#elif (CONFIG_KEYPAD == IPOD_4G_PAD) || \
      (CONFIG_KEYPAD == IPOD_3G_PAD) || \
      (CONFIG_KEYPAD == IPOD_1G2G_PAD)
#   define MINESWP_SCROLLWHEEL
#   define FFT_PREV_GRAPH       BUTTON_LEFT
#   define FFT_NEXT_GRAPH      BUTTON_RIGHT
#   define FFT_ORIENTATION  (BUTTON_SELECT | BUTTON_LEFT)
#   define FFT_WINDOW     (BUTTON_SELECT | BUTTON_RIGHT)
#   define FFT_SCALE         BUTTON_MENU
#   define FFT_COLOR       BUTTON_PLAY
#   define FFT_QUIT       (BUTTON_SELECT | BUTTON_MENU)

#elif (CONFIG_KEYPAD == IAUDIO_X5M5_PAD)
#   define FFT_PREV_GRAPH     BUTTON_LEFT
#   define FFT_NEXT_GRAPH    BUTTON_RIGHT
#   define FFT_ORIENTATION  BUTTON_SELECT
#   define FFT_WINDOW     BUTTON_PLAY
#   define FFT_SCALE       BUTTON_UP
#   define FFT_COLOR     BUTTON_DOWN
#   define FFT_QUIT     BUTTON_POWER

#elif (CONFIG_KEYPAD == GIGABEAT_PAD)
#   define FFT_PREV_GRAPH     BUTTON_LEFT
#   define FFT_NEXT_GRAPH    BUTTON_RIGHT
#   define FFT_SCALE       BUTTON_UP
#   define FFT_ORIENTATION  BUTTON_SELECT
#   define FFT_WINDOW        BUTTON_A
#   define FFT_COLOR     BUTTON_DOWN
#   define FFT_QUIT     BUTTON_POWER

#elif (CONFIG_KEYPAD == SANSA_E200_PAD)
#   define FFT_PREV_GRAPH     BUTTON_LEFT
#   define FFT_NEXT_GRAPH    BUTTON_RIGHT
#   define FFT_ORIENTATION  BUTTON_SELECT
#   define FFT_WINDOW        BUTTON_REC
#   define FFT_SCALE       BUTTON_UP
#   define FFT_COLOR     BUTTON_DOWN
#   define FFT_QUIT     BUTTON_POWER

#elif (CONFIG_KEYPAD == SANSA_FUZE_PAD)
#   define FFT_PREV_GRAPH     BUTTON_LEFT
#   define FFT_NEXT_GRAPH    BUTTON_RIGHT
#   define FFT_ORIENTATION  (BUTTON_SELECT | BUTTON_LEFT)
#   define FFT_WINDOW     (BUTTON_SELECT | BUTTON_RIGHT)
#   define FFT_SCALE       BUTTON_UP
#   define FFT_COLOR     BUTTON_DOWN
#   define FFT_QUIT     BUTTON_POWER

#elif (CONFIG_KEYPAD == SANSA_C200_PAD) || \
(CONFIG_KEYPAD == SANSA_M200_PAD)
#   define FFT_PREV_GRAPH       BUTTON_LEFT
#   define FFT_NEXT_GRAPH      BUTTON_RIGHT
#   define FFT_ORIENTATION  BUTTON_UP
#   define FFT_WINDOW        BUTTON_REC
#   define FFT_SCALE         BUTTON_SELECT
#   define FFT_COLOR       BUTTON_DOWN
#   define FFT_QUIT       BUTTON_POWER
#elif (CONFIG_KEYPAD == SANSA_CLIP_PAD)
#   define FFT_PREV_GRAPH       BUTTON_LEFT
#   define FFT_NEXT_GRAPH      BUTTON_RIGHT
#   define FFT_ORIENTATION  BUTTON_UP
#   define FFT_WINDOW        BUTTON_HOME
#   define FFT_SCALE         BUTTON_SELECT
#   define FFT_COLOR       BUTTON_DOWN
#   define FFT_QUIT       BUTTON_POWER

#elif (CONFIG_KEYPAD == IRIVER_H10_PAD)
#   define FFT_PREV_GRAPH       BUTTON_LEFT
#   define FFT_NEXT_GRAPH       BUTTON_RIGHT
#   define FFT_ORIENTATION      BUTTON_FF
#   define FFT_WINDOW       BUTTON_SCROLL_UP
#   define FFT_SCALE            BUTTON_REW
#   define FFT_COLOR            BUTTON_PLAY
#   define FFT_QUIT             BUTTON_POWER

#elif (CONFIG_KEYPAD == GIGABEAT_S_PAD)
#   define FFT_PREV_GRAPH       BUTTON_LEFT
#   define FFT_NEXT_GRAPH       BUTTON_RIGHT
#   define FFT_ORIENTATION      BUTTON_MENU
#   define FFT_WINDOW     BUTTON_PREV
#   define FFT_SCALE            BUTTON_UP
#   define FFT_COLOR            BUTTON_DOWN
#   define FFT_QUIT             BUTTON_BACK

#elif (CONFIG_KEYPAD == MROBE100_PAD)
#   define FFT_PREV_GRAPH       BUTTON_LEFT
#   define FFT_NEXT_GRAPH       BUTTON_RIGHT
#   define FFT_ORIENTATION      BUTTON_PLAY
#   define FFT_WINDOW     BUTTON_SELECT
#   define FFT_SCALE            BUTTON_UP
#   define FFT_COLOR            BUTTON_DOWN
#   define FFT_QUIT             BUTTON_POWER

#elif CONFIG_KEYPAD == IAUDIO_M3_PAD
#   define FFT_PREV_GRAPH       BUTTON_RC_REW
#   define FFT_NEXT_GRAPH       BUTTON_RC_FF
#   define FFT_ORIENTATION      BUTTON_RC_MODE
#   define FFT_WINDOW        BUTTON_RC_PLAY
#   define FFT_SCALE            BUTTON_RC_VOL_UP
#   define FFT_COLOR            BUTTON_RC_VOL_DOWN
#   define FFT_QUIT             BUTTON_RC_REC

#elif (CONFIG_KEYPAD == COWOND2_PAD)
#   define FFT_QUIT             BUTTON_POWER
#   define FFT_PREV_GRAPH       BUTTON_PLUS
#   define FFT_NEXT_GRAPH       BUTTON_MINUS

#elif CONFIG_KEYPAD == CREATIVEZVM_PAD
#   define FFT_PREV_GRAPH       BUTTON_LEFT
#   define FFT_NEXT_GRAPH       BUTTON_RIGHT
#   define FFT_ORIENTATION      BUTTON_MENU
#   define FFT_WINDOW     BUTTON_SELECT
#   define FFT_SCALE            BUTTON_UP
#   define FFT_COLOR            BUTTON_DOWN
#   define FFT_QUIT             BUTTON_BACK

#elif CONFIG_KEYPAD == PHILIPS_HDD1630_PAD
#   define FFT_PREV_GRAPH       BUTTON_LEFT
#   define FFT_NEXT_GRAPH       BUTTON_RIGHT
#   define FFT_ORIENTATION      BUTTON_SELECT
#   define FFT_WINDOW     BUTTON_MENU
#   define FFT_SCALE            BUTTON_UP
#   define FFT_COLOR            BUTTON_DOWN
#   define FFT_QUIT             BUTTON_POWER

#else
#error No keymap defined!
#endif

#include "pluginbitmaps/fft_gradient_horizontal.h"
#include "pluginbitmaps/fft_gradient_vertical.h"
#include "pluginbitmaps/fft_colors.h"

#include "kiss_fft.h"
#include "_kiss_fft_guts.h" /* sizeof(struct kiss_fft_state) */

#define FFT_SIZE (1024)
#define ARRAYSIZE_IN FFT_SIZE
#define ARRAYSIZE_OUT FFT_SIZE
#define ARRAYSIZE_PLOT FFT_SIZE/2
#define BUFSIZE (sizeof(struct kiss_fft_state)+sizeof(kiss_fft_cpx)*(FFT_SIZE-1))
#define FFT_ALLOC kiss_fft_alloc
#define FFT_FFT   kiss_fft
#define FFT_CFG   kiss_fft_cfg

/****************************** Globals ****************************/

static kiss_fft_cpx input[ARRAYSIZE_IN];
static kiss_fft_cpx output[ARRAYSIZE_OUT];
static int32_t plot[ARRAYSIZE_PLOT];
static char buffer[BUFSIZE];

const unsigned char* modes_text[] = { "Lines", "Bars", "Spectrogram" };
const unsigned char* scales_text[] = { "Linear scale", "Logarithmic scale" };
const unsigned char* window_text[] = { "Hamming window", "Hann window" };

#ifdef HAVE_LCD_COLOR
#   define MODES_COUNT 3
#else
#   define MODES_COUNT 2
#endif
#define REFRESH_RATE 8

struct {
    bool logarithmic;
    bool orientation_vertical;
    int window_func;
#ifdef HAVE_LCD_COLOR
    bool colored;
    struct {
        int column;
        int row;
    } spectrogram;
#endif
    struct {
        bool orientation;
        bool mode;
        bool scale;
    } changed;
} graph_settings;

#define COLORS BMPWIDTH_fft_colors

static long next_update = 0;
/************************* End of globals *************************/

/************************* Math functions *************************/
#define QLOG_MAX 286286
#define QLIN_MAX 1534588906
#define QLN_10 float_q16(2.302585093)
#define LIN_MAX (QLIN_MAX >> 16)

/* Returns logarithmically scaled values in S15.16 format */
inline int32_t get_log_value(int32_t value)
{
    return Q16_DIV(flog(value), QLN_10);
}

/* Apply window function to input
 * 0 - Hamming window
 * 1 - Hann window */
#define WINDOW_COUNT 2
void apply_window_func(char mode)
{
    switch(mode)
    {
        case 0: /* Hamming window */
        {
            const int32_t hamming_a = float_q(0.53836, 15), hamming_b =
                    float_q(0.46164, 15);
            size_t i;
            for (i = 0; i < ARRAYSIZE_IN; ++i)
            {
                int32_t cos;
                (void) fsincos(Q16_DIV(i << 16, (ARRAYSIZE_IN - 1) << 16) << 16,
                               &cos);
                cos >>= 16;

                /* value *= v(hamming_a - hamming_b * cos( 2 * pi * i/(ArraySize - 1) ) ) */
                input[i].r = Q15_MUL(input[i].r << 15,
                                   (hamming_a - Q15_MUL(cos, hamming_b))) >> 15;
                input[i].i = Q15_MUL(input[i].i << 15,
                                   (hamming_a - Q15_MUL(cos, hamming_b))) >> 15;
            }
            break;
        }
        case 1: /* Hann window */
        {
            size_t i;
            for (i = 0; i < ARRAYSIZE_IN; ++i)
            {
                int32_t factor;
                (void) fsincos(Q16_DIV(i << 16, (ARRAYSIZE_IN - 1) << 16) << 16,
                               &factor);
                /* s16.15; cos( 2* pi * i/(ArraySize - 1))*/
                factor >>= 16;
                /* 0.5 * cos( 2* pi * i/(ArraySize - 1))*/
                factor = Q15_MUL( (1 << 14), factor);
                /* 0.5 - 0.5 * cos( 2* pi * i/(ArraySize - 1)))*/
                factor = (1 << 14) - factor;

                input[i].r = Q15_MUL(input[i].r << 15, factor) >> 15;
                input[i].i = Q15_MUL(input[i].i << 15, factor) >> 15;
            }
            break;
        }
    }
}

/* Calculates the magnitudes from complex numbers and returns the maximum */
int32_t calc_magnitudes(bool logarithmic)
{
    int64_t tmp;
    size_t i;

  //  static int32_t max_ = 0;
  //  if(graph_settings.changed.scale)
  //      max_ = 0;

    int32_t max = -2147483647;
    //const int32_t scale_bits = 0;

    /* Calculate the magnitude, discarding the phase.
     * The sum of the squares can easily overflow the 15-bit (s15.16)
     * requirement for fsqrt, so we scale the data down */
    for (i = 0; i < ARRAYSIZE_PLOT; ++i)
    {
        tmp = output[i].r * output[i].r + output[i].i * output[i].i;
        tmp <<= 16;//tmp <<=(16 - scale_bits);

        tmp = fsqrt64(tmp, 16);
        //if (scale_bits > 0)
        //    tmp = Q16_MUL(tmp, 1 << (16 + scale_bits/2));

        if (logarithmic)
            tmp = get_log_value(tmp & 0x7FFFFFFF);

        plot[i] = tmp;

        if (plot[i] > max)
            max = plot[i];
    }
   //if(max > max_)
  //      max_ = max, DEBUGF("%s: %s max: %i\n", __func__, logarithmic ? "log" : "lin", max_);
    return max;
}
/************************ End of math functions ***********************/

/********************* Plotting functions (modes) *********************/
void draw_lines_vertical(void);
void draw_lines_horizontal(void);
void draw_bars_vertical(void);
void draw_bars_horizontal(void);
void draw_spectrogram_vertical(void);
void draw_spectrogram_horizontal(void);

void draw(char mode, const unsigned char* message)
{
    static uint32_t show_message = 0;
    static unsigned char* last_message = 0;

    static char last_mode = 0;
    static bool last_orientation = true, last_scale = true;

    if (message != 0)
    {
        last_message = (unsigned char*) message;
        show_message = 5;
    }

    if(last_mode != mode)
    {
        last_mode = mode;
        graph_settings.changed.mode = true;
    }
    if(last_scale != graph_settings.logarithmic)
    {
        last_scale = graph_settings.logarithmic;
        graph_settings.changed.scale = true;
    }
    if(last_orientation != graph_settings.orientation_vertical)
    {
        last_orientation = graph_settings.orientation_vertical;
        graph_settings.changed.orientation = true;
    }

    switch (mode)
    {
        default:
        case 0: {
            rb->lcd_clear_display();

            if (graph_settings.orientation_vertical)
                draw_lines_vertical();
            else
                draw_lines_horizontal();
            break;
        }
        case 1: {
            rb->lcd_clear_display();

            if(graph_settings.orientation_vertical)
                draw_bars_vertical();
            else
                draw_bars_horizontal();

            break;
        }
#   ifdef HAVE_LCD_COLOR
        case 2: {
            if(graph_settings.orientation_vertical)
                draw_spectrogram_vertical();
            else
                draw_spectrogram_horizontal();
            break;
        }
#   endif
    }

    if (show_message > 0)
    {
        int x, y;
        rb->lcd_getstringsize(last_message, &x, &y);
        x += 6; /* 3 px of horizontal padding and */
        y += 4; /* 2 px of vertical padding */

#   ifdef HAVE_LCD_COLOR
        if(mode == 2)
        {
            if (graph_settings.orientation_vertical)
            {
                if(graph_settings.spectrogram.column > LCD_WIDTH-x-2)
                {
                    xlcd_scroll_left(graph_settings.spectrogram.column -
                                     (LCD_WIDTH - x - 1));
                    graph_settings.spectrogram.column = LCD_WIDTH - x - 2;
                }
            }
        }
#   endif

#if LCD_DEPTH > 2
        rb->lcd_set_foreground(LCD_DARKGRAY);
        rb->lcd_fillrect(LCD_WIDTH-1-x, 0, LCD_WIDTH-1, y);

        rb->lcd_set_foreground(LCD_DEFAULT_FG);
        rb->lcd_set_background(LCD_DARKGRAY);
#endif
        rb->lcd_putsxy(LCD_WIDTH-1-x+3, 2, last_message);
#if LCD_DEPTH > 2
        rb->lcd_set_background(LCD_DEFAULT_BG);
#endif

        show_message--;

#   ifdef HAVE_LCD_COLOR
        if(show_message == 0 && mode == 2)
        {
            if(graph_settings.orientation_vertical)
            {
                rb->lcd_set_drawmode(DRMODE_SOLID | DRMODE_INVERSEVID);
                rb->lcd_fillrect(LCD_WIDTH-2-x, 0, LCD_WIDTH-1, y);
                rb->lcd_set_drawmode(DRMODE_SOLID);
            }
            else
            {
                xlcd_scroll_up(y);
                graph_settings.spectrogram.row -= y;
                if(graph_settings.spectrogram.row < 0)
                    graph_settings.spectrogram.row = 0;
            }
        }
#   endif
    }
    rb->lcd_update();

    graph_settings.changed.mode =  false;
    graph_settings.changed.orientation = false;
    graph_settings.changed.scale = false;

    /* we still have time in our time slot, so we sleep() */
    if (*rb->current_tick < next_update)
        rb->sleep(next_update - *rb->current_tick);

    /* end of next time slot */
    next_update = *rb->current_tick + HZ / REFRESH_RATE;
}

void draw_lines_vertical(void)
{
    static int32_t max = 0, vfactor = 0, vfactor_count = 0;
    static const int32_t hfactor =
            Q16_DIV(LCD_WIDTH << 16, (ARRAYSIZE_PLOT) << 16),
            bins_per_pixel = (ARRAYSIZE_PLOT) / LCD_WIDTH;
    static bool old_scale = true;

    if (old_scale != graph_settings.logarithmic)
        old_scale = graph_settings.logarithmic, max = 0; /* reset the graph on scaling mode change */

    int32_t new_max = calc_magnitudes(graph_settings.logarithmic);

    if (new_max > max)
    {
        max = new_max;
        vfactor = Q16_DIV(LCD_HEIGHT << 16, max); /* s15.16 */
        vfactor_count = Q16_DIV(vfactor, bins_per_pixel << 16); /* s15.16 */
    }

    if (new_max == 0 || max == 0) /* nothing to draw */
        return;

#ifdef HAVE_LCD_COLOR
    if(graph_settings.colored)
    {
        rb->lcd_bitmap(fft_gradient_vertical, 0, 0,
                       BMPWIDTH_fft_gradient_vertical, BMPHEIGHT_fft_gradient_vertical);
        /* Erase the lines with the background color */
        rb->lcd_set_foreground(rb->lcd_get_background());
    }
#endif

    /* take the average of neighboring bins
     * if we have to scale the graph horizontally */
    int64_t bins_avg = 0;
    bool draw = true;
    int32_t i;
    for (i = 0; i < ARRAYSIZE_PLOT; ++i)
    {
        int32_t x = 0, y = 0;

        x = Q16_MUL(hfactor, i << 16) >> 16;
        //x = (x + (1 << 15)) >> 16;

        if (hfactor < 65536) /* hfactor < 0, graph compression */
        {
            draw = false;
            bins_avg += plot[i];

            /* fix the division by zero warning:
             * bins_per_pixel is zero when the graph is expanding;
             * execution won't even reach this point - this is a dummy constant
             */
            const int32_t div = bins_per_pixel > 0 ? bins_per_pixel : 1;
            if ((i + 1) % div == 0)
            {
                y = Q16_MUL(vfactor_count, bins_avg) >> 16;

                bins_avg = 0;
                draw = true;
            }
        }
        else
        {
            y = Q16_MUL(vfactor, plot[i]) >> 16;
            draw = true;
        }

        if (draw)
        {
#       ifdef HAVE_LCD_COLOR
            if(graph_settings.colored)
                rb->lcd_vline(x, 0, LCD_HEIGHT-y-1);
            else
#       endif
            rb->lcd_vline(x, LCD_HEIGHT-1, LCD_HEIGHT-y-1);
        }
    }
#   ifdef HAVE_LCD_COLOR
    rb->lcd_set_foreground(LCD_DEFAULT_FG);
#   endif
}

void draw_lines_horizontal(void)
{
    static int max = 0;

    static const int32_t vfactor =
            Q16_DIV(LCD_HEIGHT << 16, (ARRAYSIZE_PLOT) << 16),
            bins_per_pixel = (ARRAYSIZE_PLOT) / LCD_HEIGHT;

    if (graph_settings.changed.scale)
        max = 0; /* reset the graph on scaling mode change */

    int32_t new_max = calc_magnitudes(graph_settings.logarithmic);

    if (new_max > max)
        max = new_max;

    if (new_max == 0 || max == 0) /* nothing to draw */
        return;

    int32_t hfactor;

    hfactor = Q16_DIV((LCD_WIDTH - 1) << 16, max); /* s15.16 */

#ifdef HAVE_LCD_COLOR
    if(graph_settings.colored)
    {
        rb->lcd_bitmap(fft_gradient_horizontal, 0, 0,
                       BMPWIDTH_fft_gradient_horizontal, BMPHEIGHT_fft_gradient_horizontal);
        /* Erase the lines with the background color */
        rb->lcd_set_foreground(rb->lcd_get_background());
    }
#endif

    /* take the average of neighboring bins
     * if we have to scale the graph horizontally */
    int64_t bins_avg = 0;
    bool draw = true;
    int32_t i;
    for (i = 0; i < ARRAYSIZE_PLOT; ++i)
    {
        int32_t x = 0, y = 0;

        y = Q16_MUL(vfactor, i << 16) + (1 << 15);
        y >>= 16;

        if (vfactor < 65536) /* vfactor < 0, graph compression */
        {
            draw = false;
            bins_avg += plot[i];

            /* fix the division by zero warning:
             * bins_per_pixel is zero when the graph is expanding;
             * execution won't even reach this point - this is a dummy constant
             */
            const int32_t div = bins_per_pixel > 0 ? bins_per_pixel : 1;
            if ((i + 1) % div == 0)
            {
                bins_avg = Q16_DIV(bins_avg, div << 16);
                x = Q16_MUL(hfactor, bins_avg) >> 16;

                bins_avg = 0;
                draw = true;
            }
        }
        else
        {
            y = Q16_MUL(hfactor, plot[i]) >> 16;
            draw = true;
        }

        if (draw)
        {
#       ifdef HAVE_LCD_COLOR
            if(graph_settings.colored)
            {
                rb->lcd_hline(LCD_WIDTH-1, x, y);
            }
            else
#       endif
            rb->lcd_hline(0, x, y);
        }
    }
#   ifdef HAVE_LCD_COLOR
    rb->lcd_set_foreground(LCD_DEFAULT_FG);
#   endif
}

void draw_bars_vertical(void)
{
    static const unsigned int bars = 14, border = 3, items = ARRAYSIZE_PLOT
            / bars, width = (LCD_WIDTH - ((bars - 1) * border)) / bars;

    calc_magnitudes(graph_settings.logarithmic);

    rb->lcd_set_foreground(LCD_DEFAULT_FG);
    int64_t bars_values[bars], bars_max = 0, avg = 0;
    unsigned int i, bars_idx = 0;
    for (i = 0; i < ARRAYSIZE_PLOT; ++i)
    {
        avg += plot[i];
        if ((i + 1) % items == 0)
        {
            /* Calculate the average value and keep the fractional part
             * for some added precision */
            avg = Q16_DIV(avg, items << 16); /* s15.16 */
            bars_values[bars_idx] = avg;

            if (bars_values[bars_idx] > bars_max)
                bars_max = bars_values[bars_idx];

            bars_idx++;
            avg = 0;
        }
    }

    if(bars_max == 0) /* nothing to draw */
        return;

    /* Give the graph some headroom */
    bars_max = Q16_MUL(bars_max, float_q16(1.1));

    int64_t vfactor = Q16_DIV(LCD_HEIGHT << 16, bars_max);

    for (i = 0; i < bars; ++i)
    {
        int x = (i) * (border + width);
        int y;
        y = Q16_MUL(vfactor, bars_values[i]) + (1 << 15);
        y >>= 16;

        rb->lcd_fillrect(x, LCD_HEIGHT - y, width, y);
    }
}

void draw_bars_horizontal(void)
{
    static const unsigned int bars = 14, border = 3, items = ARRAYSIZE_PLOT
            / bars, height = (LCD_HEIGHT - ((bars - 1) * border)) / bars;

    calc_magnitudes(graph_settings.logarithmic);

    rb->lcd_set_foreground(LCD_DEFAULT_FG);
    int64_t bars_values[bars], bars_max = 0, avg = 0;
    unsigned int i, bars_idx = 0;
    for (i = 0; i < ARRAYSIZE_PLOT; ++i)
    {
        avg += plot[i];
        if ((i + 1) % items == 0)
        {
            /* Calculate the average value and keep the fractional part
             * for some added precision */
            avg = Q16_DIV(avg, items << 16); /* s15.16 */
            bars_values[bars_idx] = avg;

            if (bars_values[bars_idx] > bars_max)
                bars_max = bars_values[bars_idx];

            bars_idx++;
            avg = 0;
        }
    }

    if(bars_max == 0) /* nothing to draw */
        return;

    /* Give the graph some headroom */
    bars_max = Q16_MUL(bars_max, float_q16(1.1));

    int64_t hfactor = Q16_DIV(LCD_WIDTH << 16, bars_max);

    for (i = 0; i < bars; ++i)
    {
        int y = (i) * (border + height);
        int x;
        x = Q16_MUL(hfactor, bars_values[i]) + (1 << 15);
        x >>= 16;

        rb->lcd_fillrect(0, y, x, height);
    }
}

#ifdef HAVE_LCD_COLOR
void draw_spectrogram_vertical(void)
{
    const int32_t scale_factor =
            ( Q16_DIV(ARRAYSIZE_PLOT << 16, LCD_HEIGHT << 16) + (1<<15) ) >> 16,
        remaining_div =
            ( Q16_DIV((scale_factor*LCD_HEIGHT) << 16,
                      (ARRAYSIZE_PLOT-scale_factor*LCD_HEIGHT) << 16)
             + (1<<15) ) >> 16,
        colors_per_val_log = Q16_DIV((COLORS-1) << 16, QLOG_MAX),
        colors_per_val_lin = Q16_DIV((COLORS-1) << 16, QLIN_MAX);

    calc_magnitudes(graph_settings.logarithmic);
    if(graph_settings.changed.mode || graph_settings.changed.orientation)
    {
        graph_settings.spectrogram.column = 0;
        rb->lcd_clear_display();
    }

    int i, y = LCD_HEIGHT-1, count = 0, rem_count = 0;
    uint64_t avg = 0;
    for(i = 0; i < ARRAYSIZE_PLOT; ++i)
    {
        if(plot[i] < 0)
            avg += 1 << 16;
        else
            avg += plot[i];
        ++count;
        ++rem_count;

        /* Kinda hacky - due to the rounding in scale_factor, we try to
         * uniformly interweave the extra values in our calculations */
        if(remaining_div > 0 && rem_count == remaining_div &&
                (i+1) < ARRAYSIZE_PLOT)
        {
            ++i;
            if(plot[i] < 0)
                avg += 1 << 16;
            else
                avg += plot[i];
            rem_count = 0;
        }

        if(count >= scale_factor)
        {
            if(rem_count == 0) /* if we just added an extra value */
                ++count;

            int32_t color;

            avg = Q16_DIV(avg, count << 16);
            if(graph_settings.logarithmic)
                color = Q16_MUL(avg, colors_per_val_log) >> 16;
            else
                color = Q16_MUL(avg, colors_per_val_lin) >> 16;

            if(color >= COLORS) /* TODO: investigate why we get these cases */
                color = COLORS-1;
            else if (color < 0)
                color = 0;

            rb->lcd_set_foreground(fft_colors[color]);
            rb->lcd_drawpixel(graph_settings.spectrogram.column, y);

            y--;

            avg = 0;
            count = 0;
        }
        if(y < 0)
            break;
    }
    if(graph_settings.spectrogram.column != LCD_WIDTH - 1)
        graph_settings.spectrogram.column++;
    else
        xlcd_scroll_left(1);
}
void draw_spectrogram_horizontal(void)
{
    const int32_t scale_factor =
            ( Q16_DIV(ARRAYSIZE_PLOT << 16, LCD_WIDTH << 16)) >> 16,
        remaining_div =
            ( Q16_DIV((scale_factor*LCD_WIDTH) << 16,
                      (ARRAYSIZE_PLOT-scale_factor*LCD_WIDTH) << 16)
             + (1<<15) ) >> 16,
         colors_per_val_log = Q16_DIV((COLORS-1) << 16, QLOG_MAX),
         colors_per_val_lin = Q16_DIV((COLORS-1) << 16, QLIN_MAX);

    calc_magnitudes(graph_settings.logarithmic);
    if(graph_settings.changed.mode || graph_settings.changed.orientation)
    {
        graph_settings.spectrogram.row = 0;
        rb->lcd_clear_display();
    }

    int i, x = 0, count = 0, rem_count = 0;
    int64_t avg = 0;
    for(i = 0; i < ARRAYSIZE_PLOT; ++i)
    {
        avg += plot[i];
        ++count;
        ++rem_count;

        /* Kinda hacky - due to the rounding in scale_factor, we try to
         * uniformly interweave the extra values in our calculations */
        if(remaining_div > 0 && rem_count == remaining_div &&
                i < (ARRAYSIZE_PLOT-1))
        {
            ++i;
            avg += plot[i];
            rem_count = 0;
        }

        if(count >= scale_factor)
        {
            if(rem_count == 0) /* if we just added an extra value */
                ++count;

            int32_t color;

            avg = Q16_DIV(avg, count << 16);
            if(graph_settings.logarithmic)
                color = Q16_MUL(avg, colors_per_val_log) >> 16;
            else
                color = Q16_MUL(avg, colors_per_val_lin) >> 16;

            rb->lcd_set_foreground(fft_colors[color]);
            rb->lcd_drawpixel(x, graph_settings.spectrogram.row);

            x++;

            avg = 0;
            count = 0;
        }
        if(x >= LCD_WIDTH)
            break;
    }
    if(graph_settings.spectrogram.row != LCD_HEIGHT-1)
        graph_settings.spectrogram.row++;
    else
        xlcd_scroll_up(1);
}
#endif
/********************* End of plotting functions (modes) *********************/

enum plugin_status plugin_start(const void* parameter)
{
    (void) parameter;
    if ((rb->audio_status() & AUDIO_STATUS_PLAY) == 0)
    {
        rb->splash(HZ * 2, "No track playing. Exiting..");
        return PLUGIN_OK;
    }

#if LCD_DEPTH > 1
    rb->lcd_set_backdrop(NULL);
#endif
    backlight_force_on();

#ifdef HAVE_ADJUSTABLE_CPU_FREQ
    rb->cpu_boost(true);
#endif

    /* Defaults */
    bool run = true;
    int mode = 0;
    graph_settings.logarithmic = true;
    graph_settings.orientation_vertical = true;
    graph_settings.window_func = 0;
    graph_settings.changed.mode = false;
    graph_settings.changed.scale = false;
    graph_settings.changed.orientation = false;
#ifdef HAVE_LCD_COLOR
    graph_settings.colored = false;
    graph_settings.spectrogram.row = 0;
    graph_settings.spectrogram.column = 0;
#endif
    bool changed_window = false;

    size_t size = sizeof(buffer);
    kiss_fft_cfg state = kiss_fft_alloc(FFT_SIZE, 0, buffer, &size);

    if (state == 0)
    {
        DEBUGF("needed data: %i", (int) size);
        return PLUGIN_ERROR;
    }


    kiss_fft_scalar left, right;
    kiss_fft_scalar* value;
    int count;

    /* set the end of the first time slot - rest of the
     * next_update work is done in draw() */
    next_update = *rb->current_tick + HZ / REFRESH_RATE;

    while (run)
    {
        value = (kiss_fft_scalar*) rb->pcm_get_peak_buffer(&count);
        if (value == 0 || count == 0)
        {
            rb->yield();
        }
        else
        {
            int idx = 0; /* offset in the buffer */
            int fft_idx = 0; /* offset in input */

            do
            {
                left = *(value + idx);
                idx += 2;

                right = *(value + idx);
                idx += 2;

                input[fft_idx].r = left;
                input[fft_idx].i = right;
                fft_idx++;

                if (fft_idx == ARRAYSIZE_IN)
                    break;
            } while (idx < count);

            if (fft_idx == ARRAYSIZE_IN)
            {
                apply_window_func(graph_settings.window_func);

                /* Play nice - the sleep at the end of draw()
                 * only tries to maintain the frame rate */
                rb->yield();
                kiss_fft(state, input, output);
                rb->yield();
                if(changed_window)
                {
                    draw(mode, window_text[graph_settings.window_func]);
                    changed_window = false;
                }
                else
                    draw(mode, 0);
                fft_idx = 0;
            };
        }
        int button = rb->button_get(false);
        switch (button)
        {
            case FFT_QUIT:
                run = false;
                break;
            case FFT_PREV_GRAPH: {
                mode -= 1;
                if (mode < 0)
                    mode = MODES_COUNT-1;
                draw(mode, modes_text[(int) mode]);
                break;
            }
            case FFT_NEXT_GRAPH: {
                mode += 1;
                if (mode >= MODES_COUNT)
                    mode = 0;
                draw(mode, modes_text[(int) mode]);
                break;
            }
            case FFT_WINDOW: {
                changed_window = true;
                graph_settings.window_func ++;
                if(graph_settings.window_func >= WINDOW_COUNT)
                    graph_settings.window_func = 0;
                break;
            }
            case FFT_SCALE: {
                graph_settings.logarithmic = !graph_settings.logarithmic;
                draw(mode, scales_text[graph_settings.logarithmic ? 1 : 0]);
                break;
            }
            case FFT_ORIENTATION: {
                graph_settings.orientation_vertical = !graph_settings.orientation_vertical;
                draw(mode, 0);
                break;
            }
#       ifdef HAVE_LCD_COLOR
            case FFT_COLOR: {
                graph_settings.colored = !graph_settings.colored;
                draw(mode, 0);
                break;
            }
#       endif
            default: {
                if (rb->default_event_handler(button) == SYS_USB_CONNECTED)
                    return PLUGIN_USB_CONNECTED;
            }

        }
    }
    backlight_use_settings();
    return PLUGIN_OK;
}
