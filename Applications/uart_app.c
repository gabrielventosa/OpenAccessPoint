#include "ioCC1110.h"
#include "uart.h"
#include "uart_app.h"



void start_gps_at_uart(void);

void start_gps_at_uart() {

  uartMapPort(UART_ALT_GPS_AT,UART_PORT_GPS_AT);
  uartInitBitrate(UART_PORT_BITRATE_M,UART_PORT_BITRATE_E);
  uartProtConfig.uartNum = UART_PORT_GPS_AT;
  uartProtConfig.START = 0;                //Start bit low
  uartProtConfig.STOP  = 1;                //Stop bit high
  uartProtConfig.SPB   = 0;                //One stop bit
  uartProtConfig.PARITY  = 0;              //Paritty disabled
  uartProtConfig.BIT9  = 0;                //Bit9 disabled
  uartProtConfig.FLOW  = 0;                //Hardware Flow control disabled
  uartProtConfig.D9   = 0;                
  uartProtConfig.ORDER = 0;                //LSB bit first
  uartInitProtocol(&uartProtConfig);
}