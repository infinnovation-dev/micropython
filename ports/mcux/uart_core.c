#include <unistd.h>
#include "py/mpconfig.h"

#include "fsl_common.h"
#include "fsl_clock.h"
#include "fsl_port.h"
#include "fsl_uart.h"

#define SOPT5_UART0TXSRC_UART_TX      0x00u   /*!< UART 0 transmit data source select: UART0_TX pin */

static UART_Type *const uart0 = UART0;

/*
 * Core UART functions to implement for a port
 */

void mp_hal_console_init(void) {
    uint32_t clkSrcFreq;
    uart_config_t config;

    /* Enable PortB */
    CLOCK_EnableClock(kCLOCK_PortB);

    PORT_SetPinMux(PORTB,16, kPORT_MuxAlt3); /* PTB16 -> UART0_RX */
    PORT_SetPinMux(PORTB,17, kPORT_MuxAlt3); /* PTB17 -> UART0_RX */
    SIM->SOPT5 = ((SIM->SOPT5 & ~ SIM_SOPT5_UART0TXSRC_MASK)
                  | SIM_SOPT5_UART0TXSRC(SOPT5_UART0TXSRC_UART_TX));

    UART_GetDefaultConfig(&config);
    config.baudRate_Bps = 115200;
    clkSrcFreq = CLOCK_GetCoreSysClkFreq();
    UART_Init(uart0, &config, clkSrcFreq);
    UART_EnableTx(uart0, true);
    UART_EnableRx(uart0, true);
}

// Receive single character
int mp_hal_stdin_rx_chr(void) {
    uint8_t c = 0;
    UART_ReadBlocking(uart0, &c, 1);
    return c;
}

// Send string of given length
void mp_hal_stdout_tx_strn(const char *str, mp_uint_t len) {
    UART_WriteBlocking(uart0, (const uint8_t *)str, len);
}
