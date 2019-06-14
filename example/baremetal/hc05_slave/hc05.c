#define DEBUG
#include "embARC.h"
#include "embARC_debug.h"
#include "dev_gpio.h"
#include "ez_sio.h"

#include "hc05.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


int32_t rn4020_setup(HC05_DEF_PTR hc05)
{
    int32_t ret;

    hc05->connected = false;

    hc05->sio_uart = ez_sio_open(hc05->uart, 115200, 1024, 1024);

    board_delay_ms(100, 0);

    _hc05_set_state(hc05, HC05_STATE_INITIALIZING);

    ret = _hc05_wait_for_ready(hc05);

    return ret;
}

static int32_t _hc05_wait_for_ready(HC05_DEF_PTR hc05)
{
    if (hc05->state == HC05_STATE_READY) {
        return E_OK;
    }

    while (1) {

        _hc05_get_line(hc05);
        _hc05_line_parser(hc05, (char *)hc05->rx_buffer);
        rn4020->rx_cnt = 0;

        if (hc05->state == HC05_STATE_READY) {
            return E_OK;
        }
    }

    return E_SYS;
}

static void _hc05_get_line(HC05_DEF_PTR hc05)
{
    char data = 0;
    uint8_t rd_cnt;

    do {
        rd_cnt = ez_sio_read(hc05->sio_uart, &data, 1);
        
        if (rd_cnt != 0 && data != 0) {
            hc05>rx_buffer[hc05->rx_cnt++] = data;
        }

    } while (data != '\n' && hc05->rx_cnt < RN4020_RX_BUFFER_SIZE);

    if (hc05->rx_buffer[hc05->rx_cnt - 2] == '\r') {
        /* remove \r\n */
        hc05->rx_buffer[hc05->rx_cnt - 2] = 0;
    } else {
        hc05->rx_buffer[rn4020->rx_cnt - 1] = 0;
    }
}

void hc05_send(HC05_DEF_PTR hc05, const char *line)
{
    ez_sio_write(hc05->sio_uart, (char *)line, strlen(line));
    ez_sio_write(hc05->sio_uart, "\r\n", 1);
    DBG("tx: %s\n", line);
}

static void _hc05_set_state(HC05_DEF_PTR hc05, HC05_STATE new_state)
{
#ifdef DEBUG

    switch (new_state) {
        case HC05_STATE_INITIALIZING:
            DBG("state: INITIALIZING\n");
            break;

        case HC05_STATE_READY:
            DBG("state: READY\n");
            break;

        case HC05_STATE_WAITING_FOR_OK:
            DBG("state: WAIT OK\n");
            break;

        default:
            DBG("state: unknown\n");
            break;
    }

#endif
    hc05->state = new_state;
}