/*
 * Copyright (C) 2019 Marian Buschsieweke
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup tests
 * @{
 *
 * @file
 * @brief   Test application of the DFPlayer Mini driver
 *
 * @author  Marian Buschsieweke <marian.buschsieweke@ovgu.de>
 *
 * @}
 */

#include <stdio.h>

#include "dfplayer.h"
#include "event.h"
#include "event/thread.h"
#include "fmt.h"
#include "shell.h"
#include "xtimer.h"

static const char *_sources[] = {
    [DFPLAYER_SOURCE_USB]       = "USB",
    [DFPLAYER_SOURCE_SDCARD]    = "SD card",
    [DFPLAYER_SOURCE_FLASH]     = "Flash",
    [DFPLAYER_SOURCE_NUMOF]     = NULL,
};

typedef struct {
    event_t event;
    dfplayer_source_t src;
    uint16_t track;
} playback_done_event_t;

typedef struct {
    event_t event;
    dfplayer_source_set_t sources;
} media_changed_event_t;

static void playback_done_handler(event_t *_data)
{
    playback_done_event_t *data = (playback_done_event_t *)_data;

    const char *dev = _sources[data->src];
    if (!dev) {
        dev = "unknown/error";
    }

    print_str("Playback of track ");
    print_u32_dec(data->track);
    print_str(" on device ");
    print_str(dev);
    print_str(" has completed\n");
    xtimer_usleep(DFPLAYER_TIMEOUT_MS * US_PER_MS);
    dfplayer_next(dfplayer_get(0));
}

static void _cb_done(dfplayer_source_t src, uint16_t track, void *data)
{
    (void)data;
    static playback_done_event_t event = {
        .event = { .handler = playback_done_handler }
    };

    event.src = src;
    event.track = track;
    event_post(EVENT_PRIO_LOWEST, &event.event);
}

static void media_changed_handler(event_t *_data)
{
    media_changed_event_t *data = (media_changed_event_t *)_data;
    print_str("List of available playback sources changed. New list:\n");

    print_str("USB: ");
    if (dfplayer_source_set_contains(data->sources, DFPLAYER_SOURCE_USB)) {
        print_str("Attached\n");
    }
    else {
        print_str("-\n");
    }

    print_str("SD card: ");
    if (dfplayer_source_set_contains(data->sources, DFPLAYER_SOURCE_SDCARD)) {
        print_str("Attached\n");
    }
    else {
        print_str("-\n");
    }

    print_str("Flash: ");
    if (dfplayer_source_set_contains(data->sources, DFPLAYER_SOURCE_FLASH)) {
        print_str("Attached\n");
    }
    else {
        print_str("-\n");
    }
}

static void _cb_src(dfplayer_source_set_t srcs, void *data)
{
    (void)data;
    static media_changed_event_t event = {
        .event = { .handler = media_changed_handler }
    };

    event.sources = srcs;
    event_post(EVENT_PRIO_LOWEST, &event.event);
}

int main(void)
{
    print_str(
        "DFPlayer Mini Driver Test\n"
        "=========================\n"
        "\n"
        "Experiment with the shell command \"dfplayer\"\n\n"
    );

    if (dfplayer_set_callbacks(dfplayer_get(0), _cb_done, _cb_src, dfplayer_get(0))) {
        print_str("Error: Failed to register callbacks\n");
    }

    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(NULL, line_buf, SHELL_DEFAULT_BUFSIZE);

    return 0;
}
