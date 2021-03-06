#include "ioCC1110.h"
#include <string.h>
#include "uart.h"
#include "uart_app.h"


//Buffer for the command to be decoded
unsigned char uartCommand[SIZE_OF_UART_RX_BUFFER];
int uartCommandSize;
extern volatile unsigned char Lock_Flag;
extern volatile unsigned char Panic_Flag;

unsigned char HelloCmd[]= "HEllo";
unsigned char GetLockFlagCmd[] = "GET_LOCK_FLAG";
unsigned char GetPanicStatus[] = "GET_PANNIC_STATUS";

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

void uart_decode_cmd_callback(void)
{
  if (memcmp(uartCommand,HelloCmd,uartCommandSize)== 0){
      memcpy(uartTxBuffer,&HelloCmd,uartCommandSize);
      uartTxLength = uartCommandSize;
      uartStartTxForIsr(UART_PORT_GPS_AT);
  }
  
  else if(memcmp(uartCommand,GetLockFlagCmd,uartCommandSize) == 0) {
    if (Lock_Flag){
      memcpy(uartTxBuffer,(unsigned char*) "LOCKED\r",7);
      uartTxLength = 7;
      uartStartTxForIsr(UART_PORT_GPS_AT);
    }else{
      memcpy(uartTxBuffer,(unsigned char*) "UNLOCKED\r",9);
      uartTxLength = 9;
      uartStartTxForIsr(UART_PORT_GPS_AT);
    }
  }
  
  else if(memcmp(uartCommand,GetPanicStatus,uartCommandSize) == 0) {
    if (Panic_Flag){
      memcpy(uartTxBuffer,(unsigned char*) "PANNIC\r",7);
      uartTxLength = 7;
      uartStartTxForIsr(UART_PORT_GPS_AT);
    }else{
      memcpy(uartTxBuffer,(unsigned char*) "NOPANNIC\r",9);
      uartTxLength = 9;
      uartStartTxForIsr(UART_PORT_GPS_AT);
    }
  } 
      
      
}