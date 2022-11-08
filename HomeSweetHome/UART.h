/*
 * UART.h
 *
 * Created: 31/08/2022 20:36:49
 *  Author: higgy
 */ 

#ifndef UART_H_
#define UART_H_

#include <atmel_start.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "GNSS_calculations.h"
#include "hidden.h"

// Size of Receive Buffer
#define GNSS_BUFFER_SIZE		256U

extern void GNSS_uart_init(void);
extern void debug_uart_init(void);

extern void check_GNSS(void);
extern void check_password(void);
extern void debug_uart_password_display(void);

extern char *zstring_strtok(char *str, const char *delim) ;

// GNSS Receiving & Complete Flags
volatile uint8_t GNSS_receiving;
volatile uint8_t GNSS_complete;
volatile uint8_t GNSS_gll_received;

#endif /* UART_H_ */