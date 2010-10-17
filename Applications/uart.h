#ifndef UART_H_
#define UART_H_

// C language code:
// Define and allocate a setup structure for the UART protocol:
typedef struct {
      unsigned char uartNum : 1; // UART peripheral number (0 or 1)
      unsigned char START : 1; // Start bit level (low/high)
      unsigned char STOP : 1; // Stop bit level (low/high)
      unsigned char SPB : 1; // Stop bits (0 => 1, 1 => 2)
      unsigned char PARITY : 1; // Parity control (enable/disable)
      unsigned char BIT9 : 1; // 9 bit enable (8bit / 9bit)
      unsigned char D9 : 1; // 9th bit level or Parity type
      unsigned char FLOW : 1; // HW Flow Control (enable/disable)
      unsigned char ORDER : 1; // Data bit order(LSB/MSB first)
} UART_PROT_CONFIG;

extern UART_PROT_CONFIG __xdata uartProtConfig;

// Define size of allocated UART RX/TX buffer (just an example)
#define SIZE_OF_UART_RX_BUFFER 50
#define SIZE_OF_UART_TX_BUFFER SIZE_OF_UART_RX_BUFFER

// Allocate buffer+index for UART RX/TX
extern unsigned char __xdata uartRxBuffer[SIZE_OF_UART_RX_BUFFER];
extern unsigned char __xdata uartTxBuffer[SIZE_OF_UART_TX_BUFFER];
extern volatile unsigned short __xdata uartRxIndex, uartTxIndex;
extern volatile unsigned short __xdata uartTxLength;

extern void uartMapPort(unsigned char uartPortAlt, unsigned char uartNum);
extern void uartInitBitrate(unsigned char uartBaudM, unsigned char uartBaudE);
extern void uartInitProtocol(UART_PROT_CONFIG* uartProtConfig);

extern void uartStartTxForIsr(unsigned char uartNum);
//_Pragma("vector=0x3B") __near_func __interrupt void UART0_TX_ISR(void);
//_Pragma("vector=0x73") __near_func __interrupt void UART1_TX_ISR(void);

extern void uartStartRxForIsr(unsigned char uartNum);
//_Pragma("vector=0x13") __near_func __interrupt void UART0_RX_ISR(void);
//_Pragma("vector=0x1B") __near_func __interrupt void UART1_RX_ISR(void);


extern volatile unsigned char sUartCmd;

#endif //UART_H_
