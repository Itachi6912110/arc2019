#ifndef _HC05_H_
#define _HC05_H_

#include "ez_sio.h"

#define HC05_RX_BUFFER_SIZE 256

typedef enum hc05_state {
	HC05_STATE_INITIALIZING,
	HC05_STATE_READY,
	HC05_STATE_WAITING_FOR_OK,
} HC05_STATE;

typedef struct hc05_def {
	uint8_t uart;
	EZ_SIO *sio_uart;
	uint32_t baud;
	volatile uint8_t connected;
	volatile HC05_STATE state;

	uint8_t rx_buffer[HC05_RX_BUFFER_SIZE];
	uint16_t rx_cnt;

} HC05_DEF, *HC05_DEF_PTR;

#define HC05_DEFINE(name, uart_id, baud_rate) \
	HC05_DEF __ ## name = { \
		.uart = uart_id, \
		.baud = baud_rate, \
	}; \
	HC05_DEF_PTR name = &__ ## name