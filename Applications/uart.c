#include "ioCC1110.h"
#include "uart.h"


// Allocate buffer+index for UART RX/TX
unsigned short __xdata uartRxBuffer[SIZE_OF_UART_RX_BUFFER];
unsigned short __xdata uartTxBuffer[SIZE_OF_UART_TX_BUFFER];
unsigned short __xdata uartRxIndex, uartTxIndex;

void uartMapPort(unsigned char uartPortAlt, unsigned char uartNum);
void uartInitBitrate(unsigned char uartBaudM, unsigned char uartBaudE);
void uartInitProtocol(UART_PROT_CONFIG* uartProtConfig);

void uartStartTxForIsr(unsigned char uartNum);
_Pragma("vector=0x3B") __near_func __interrupt void UART0_TX_ISR(void);
_Pragma("vector=0x73") __near_func __interrupt void UART1_TX_ISR(void);

void uartStartRxForIsr(unsigned char uartNum);
_Pragma("vector=0x13") __near_func __interrupt void UART0_RX_ISR(void);
_Pragma("vector=0x1B") __near_func __interrupt void UART1_RX_ISR(void);

UART_PROT_CONFIG __xdata uartProtConfig;

// C language code:
// This function maps/connects the UART to the desired SoC I/O port.
// The application should call this function with "uartPortAlt" = 1 or 2,
// and "uartNum" = 0 or 1.

void uartMapPort(unsigned char uartPortAlt, unsigned char uartNum) {
  
  // If UART Port Alternative 1 desired
  if(uartPortAlt == 1) {
      // If UART0 desired
      if (uartNum == 0) {
          // Configure UART0 for Alternative 1 => Port P0 (PERCFG.U0CFG = 0)
          PERCFG &= ~0x01;
          // Configure relevant Port P0 pins for peripheral function:
          // P0SEL.SELP0_2/3/4/5 = 1 => RX = P0_2, TX = P0_3, CT = P0_4, RT = P0_5
          P0SEL |= 0x3C;
          // Configure relevant Port P1 pins back to GPIO function
          P1SEL &= ~0x3C;
          // Else (UART1 desired)
       } else {
          // Configure UART1 for Alternative 1 => Port P0 (PERCFG.U1CFG = 0)
          PERCFG &= ~0x02;
          // Configure relevant Port P0 pins for peripheral function:
          // P0SEL.SELP0_2/3/4/5 = 1 => CT = P0_2, RT = P0_3, TX = P0_4, RX = P0_5
          P0SEL |= 0x3C;
          // Configure relevant Port P1 pins back to GPIO function
          P1SEL &= ~0xF0;
       }
   
      // Else (UART Port Alternative 2 desired)
   } else {
   // If UART0 desired
  if (uartNum == 0) {
   // Configure UART0 for Alternative 2 => Port P1 (PERCFG.U0CFG = 1)
        PERCFG |= 0x01;
        // P1SEL.SELP1_2/3/4/5 = 1 => CT = P1_2, RT = P1_3, RX = P1_4, TX = P1_5
        P1SEL |= 0x3C;
        // Configure relevant Port P0 pins back to GPIO function
        P0SEL &= ~0x3C;
        // Else (UART1 desired)
   } else {
        // Configure UART1 for Alternative 2 => Port P1 (PERCFG.U1CFG = 1)
        PERCFG |= 0x02;
        // P1SEL.SELP1_4/5/6/7 = 1 => CT = P1_4, RT = P1_5, TX = P1_6, RX = P1_7
        P1SEL |= 0xF0;
        // Configure relevant Port P0 pins back to GPIO function
        P0SEL &= ~0x3C;
        }
    }
}

// C language code:
// This function initializes the UART bit rate.
void uartInitBitrate(unsigned char uartBaudM, unsigned char uartBaudE) {
///////////////////////////////////////////////////////////////
// This initial code section ensures that the SoC system clock is driven
// by the HS XOSC:
// Clear CLKCON.OSC to make the SoC operate on the HS XOSC.
// Set CLKCON.TICKSPD/CLKSPD = 000 => system clock speed = HS RCOSC speed.
      CLKCON &= 0x80;
      // Monitor CLKCON.OSC to ensure that the HS XOSC is stable and actually
      // applied as system clock source before continuing code execution
      while(CLKCON & 0x40);
      // Set SLEEP.OSC_PD to power down the HS RCOSC.
      SLEEP |= 0x04;
      ///////////////////////////////////////////////////////////////
      // Initialize bitrate (U0BAUD.BAUD_M, U0GCR.BAUD_E)
      // Initialise bitrate = 57.6 kbps (U0BAUD.BAUD_M = 34, U0GCR.BAUD_E = 11)
  U1BAUD = uartBaudM;
  U1GCR = (U1GCR&~uartBaudE) | uartBaudE;
/*
      U1BAUD = uartBaudM;
      U1GCR = (U1GCR&~0x1F) | uartBaudE; */
}

// This function initializes the UART protocol (start/stop bit, data bits,
// parity, etc.). The application must call this function with an initialized
// data structure according to the code in Figure 12.
void uartInitProtocol(UART_PROT_CONFIG* uartProtConfig) {
    // Initialize UART protocol for desired UART (0 or 1)
    if (uartProtConfig->uartNum == 0) {
        // USART mode = UART (U0CSR.MODE = 1)
        U0CSR |= 0x80;
        // Start bit level = low => Idle level = high (U0UCR.START = 0)
        // Start bit level = high => Idle level = low (U0UCR.START = 1)
        U0UCR = (U0UCR&~0x01) | uartProtConfig->START;
        // Stop bit level = high (U0UCR.STOP = 1)
        // Stop bit level = low (U0UCR.STOP = 0)
        U0UCR = (U0UCR&~0x02) | (uartProtConfig->STOP << 1);
        // Number of stop bits = 1 (U0UCR.SPB = 0)
        // Number of stop bits = 2 (U0UCR.SPB = 1)
        U0UCR = (U0UCR&~0x04) | (uartProtConfig->SPB << 2);
        // Parity = disabled (U0UCR.PARITY = 0)
        // Parity = enabled (U0UCR.PARITY = 1)
        U0UCR = (U0UCR&~0x08) | (uartProtConfig->PARITY << 3);
        // 9-bit data disable = 8 bits transfer (U0UCR.BIT9 = 0)
        // 9-bit data enable = 9 bits transfer (U0UCR.BIT9 = 1)
        U0UCR = (U0UCR&~0x10) | (uartProtConfig->BIT9 << 4);
        // Level of bit 9 = 0 (U0UCR.D9 = 0), used when U0UCR.BIT9 = 1
        // Level of bit 9 = 1 (U0UCR.D9 = 1), used when U0UCR.BIT9 = 1
        // Parity = Even (U0UCR.D9 = 0), used when U0UCR.PARITY = 1
        // Parity = Odd (U0UCR.D9 = 1), used when U0UCR.PARITY = 1
        U0UCR = (U0UCR&~0x20) | (uartProtConfig->D9 << 5);
        // Flow control = disabled (U0UCR.FLOW = 0)
        // Flow control = enabled (U0UCR.FLOW = 1)
        U0UCR = (U0UCR&~0x40) | (uartProtConfig->FLOW << 6);
        // Bit order = MSB first (U0GCR.ORDER = 1)
        // Bit order = LSB first (U0GCR.ORDER = 0) => For PC/Hyperterminal
        U0GCR = (U0GCR&~0x20) | (uartProtConfig->ORDER << 5);
    } else {
    // USART mode = UART (U1CSR.MODE = 1)
        U1CSR |= 0x80;
        // Start bit level = low => Idle level = high (U1UCR.START = 0)
        // Start bit level = high => Idle level = low (U1UCR.START = 1)
        U1UCR = (U1UCR&~0x01) | uartProtConfig->START;
        // Stop bit level = high (U1UCR.STOP = 1)
        // Stop bit level = low (U1UCR.STOP = 0)
        U1UCR = (U1UCR&~0x02) | (uartProtConfig->STOP << 1);
        // Number of stop bits = 1 (U1UCR.SPB = 0)
        // Number of stop bits = 2 (U1UCR.SPB = 1)
        U1UCR = (U1UCR&~0x04) | (uartProtConfig->SPB << 2);
        // Parity = disabled (U1UCR.PARITY = 0)
        // Parity = enabled (U1UCR.PARITY = 1)
        U1UCR = (U1UCR&~0x08) | (uartProtConfig->PARITY << 3);
        // 9-bit data enable = 8 bits transfer (U1UCR.BIT9 = 0)
        // 9-bit data enable = 8 bits transfer (U1UCR.BIT9 = 1)
        U1UCR = (U1UCR&~0x10) | (uartProtConfig->BIT9 << 4);
        // Level of bit 9 = 0 (U1UCR.D9 = 0), used when U1UCR.BIT9 = 1
        // Level of bit 9 = 1 (U1UCR.D9 = 1), used when U1UCR.BIT9 = 1
        // Parity = Even (U1UCR.D9 = 0), used when U1UCR.PARITY = 1
        // Parity = Odd (U1UCR.D9 = 1), used when U1UCR.PARITY = 1
        U1UCR = (U1UCR&~0x20) | (uartProtConfig->D9 << 5);
        // Flow control = disabled (U1UCR.FLOW = 0)
        // Flow control = enabled (U1UCR.FLOW = 1)
        U1UCR = (U1UCR&~0x40) | (uartProtConfig->FLOW << 6);
        // Bit order = MSB first (U1GCR.ORDER = 1)
        // Bit order = LSB first (U1GCR.ORDER = 0) => For PC/Hyperterminal
        U1GCR = (U1GCR&~0x20) | (uartProtConfig->ORDER << 5);
        }
}

// C language code:
// This function starts the UART TX session and leaves the transmission
// of the remaining bytes to the associated UART TX ISR in Figure 17.
// Before this function is called the application must initialize the
// UART peripheral according to the code shown in Figure 3, Figure 11,
// Figure 12, and Figure 13.
// The code implements the following steps:
// 1. Initialize the UART TX buffer index.
// 2. Clear UART TX Interrupt Flag (IRCON2.UTXxIF = 0.
// 3. Enable UART TX Interrupt (IEN2.UTXxIE = 1)
// 4. Send very first UART byte
// 5. Enable global interrupt (IEN0.EA = 1).
void uartStartTxForIsr(unsigned char uartNum) {
    uartTxIndex = 0;
    if (uartNum == 0) {
        UTX0IF = 0;
        IEN2 |= 0x04;
        U0DBUF = uartTxBuffer[uartTxIndex++];
    } else {
        UTX1IF = 0;
        IEN2 |= 0x08;
        U1DBUF = uartTxBuffer[uartTxIndex++];
    }
    IEN0 |= 0x80;
}

_Pragma("vector=0x3B") __near_func __interrupt void UART0_TX_ISR(void){
    UTX0IF = 0;
    if (uartTxIndex >= SIZE_OF_UART_TX_BUFFER) {
        uartTxIndex = 0; IEN2 &= ~0x08; return;
    }
    U0DBUF = uartTxBuffer[uartTxIndex++];
}


_Pragma("vector=0x73") __near_func __interrupt void UART1_TX_ISR(void){
    UTX1IF = 0;
    if (uartTxIndex >= SIZE_OF_UART_TX_BUFFER) {
        uartTxIndex = 0; IEN2 &= ~0x08; return;
    }
    U1DBUF = uartTxBuffer[uartTxIndex++];
}


void uartStartRxForIsr(unsigned char uartNum) {
    uartRxIndex = 0;
    if (uartNum == 0) {
        URX0IF = 0;
        U0CSR |= 0x40;
        IEN0 |= 0x04;
    } else {
    
     URX1IF = 0;
     U1CSR |= 0x40;
     IEN0 |= 0x08;
    }
    IEN0 |= 0x80;
}

// C language code:
// The UARTx RX ISR assumes that the code in Figure 18 has initialized the
// UART RX session, by enabling the UART RX interrupt. Then this UART RX ISR
// will receive the data based in interrupt request generation by the
// USART peripheral.
// The code implements the following steps:
// 1. Clear UARTx RX Interrupt Flag (TCON.URXxIF = 0)
// 2. Read UxDBUF and store the value in the allocated UART RX target buffer
// 3. If all UART data received, stop this UART RX session
// Note that in order to start another UART RX session the application
// just needs to re-enable the UART RX interrupt(IEN0.URXxIE = 1).


_Pragma("vector=0x13") __near_func __interrupt void UART0_RX_ISR(void){
    URX0IF = 0;
    uartRxBuffer[uartRxIndex++] = U0DBUF;
    sUartCmd++;
    if (uartRxIndex >= SIZE_OF_UART_RX_BUFFER) {
        uartRxIndex = 0; IEN0 &= ~0x04;
    }
}


_Pragma("vector=0x1B") __near_func __interrupt void UART1_RX_ISR(void){
     URX1IF = 0;
     if(U1DBUF==0x0d)   sUartCmd++;
     uartRxBuffer[uartRxIndex++] = U1DBUF;
     if (uartRxIndex >= SIZE_OF_UART_RX_BUFFER) {
        uartRxIndex = 0; IEN0 &= ~0x08;
      }
}