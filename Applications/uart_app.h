
#define UART_ALT_GPS_AT (unsigned char) 2
#define UART_PORT_GPS_AT (unsigned char) 1

#define UART_BITRATE 57600

#if UART_BITRATE == 9600
/* BITRATE DEFINITION FOR 9600 bps at 26 Mhz clock*/
#define UART_PORT_BITRATE_M (unsigned char) 131
#define UART_PORT_BITRATE_E (unsigned char) 8
#elif UART_BITRATE == 57600
/* BITRATE DEFINITION FOR 57500 bps at 26 Mhz clock*/
#define UART_PORT_BITRATE_M (unsigned char) 34
#define UART_PORT_BITRATE_E (unsigned char) 11
#elif UART_BITRATE == 115200
/* BITRATE DEFINITION FOR 115200 bps at 26 Mhz clock*/
#define UART_PORT_BITRATE_M (unsigned char) 59
#define UART_PORT_BITRATE_E (unsigned char) 12
#endif

extern unsigned char uartCommand[SIZE_OF_UART_RX_BUFFER];
extern int uartCommandSize;

extern void start_gps_at_uart(void);
extern void uart_decode_cmd_callback(void);
