/*
 * uart_buffer.h
 *
 *  Created on: Jan 28, 2017
 *      Author: Jean-Etienne Tremblay
 */

#ifndef ZEPHYR_COMMON_API_UART_BUFFER_H_
#define ZEPHYR_COMMON_API_UART_BUFFER_H_

#include <mbed.h>
#include "circular_buffer.h"

/** UART Message circular buffer template class + IRQ handler
 *
 *  @param TXSize size of receive buffer in messages; must be a power of 2
 *
 *  Typical usage:
 *    // 32 message buffer
 *    UARTTXBuffer<32> uartBuffer(uart);
 *
 *    void handleUartMessage() {
 *        uartBuffer.handleIrq();
 *    }
 *
 *    void setup() {
 *       // Do your other setup
 *       uartBuffer.attach(handleUartMessage);
 *    }
 *
 *    void main() {
 *        while (1) {
 *            uint8_t b = 0;
 *            uartBuffer.write(&b,1)
 *        }
 *    }
 */
template <int TXSize>
class UARTTXBuffer {
public:
	/** Constructs a new, empty UART message buffer
	 *
	 *  @param uart UART interface to send data
	 */
	UARTTXBuffer(Serial& uart) : uart(uart) {

	}

	/** Check if the receive buffer is empty
	 *
	 *  @returns
	 *    true if empty
	 *    false if not empty
	 */
	bool txEmpty() const {
		return txBuffer.empty();
	}

	/** Check if the receive buffer is full
	 *
	 *  @returns
	 *    true if full
	 *    false if not full
	 */
	bool txFull() const {
		return txBuffer.full();
	}

	int write(uint8_t* msg, uint8_t len) {
		int i;

		// Message will be dropped if not enough space
		// TODO: check actual remaining space, and compare with size of message
		if(!txBuffer.full())
		{
			for (i = 0; i < len; i++) {
				txBuffer.write(msg[i]);
			}
		}

		// Attach interrupt if first value, this will start transmission
		if(idle) {
			idle = false;
			uart.attach(callback(this,&UARTTXBuffer<TXSize>::handleIrq),Serial::TxIrq);
		}

		return i == len;
	}

	/** UART receive message IRQ handler
	 *  Puts a new character in the UART transmit register
	 *  Stops when buffer is empty
	 */
	void handleIrq() {
		if(!txBuffer.empty()) // This should always be true!
		{
			uart.putc(txBuffer.read());
		}
		if(txBuffer.empty())
		{
			// Do nothing
			uart.attach(NULL,Serial::TxIrq);
			idle = true;
		}
	}

private:
	calsol::util::CircularBuffer<uint8_t, TXSize> txBuffer;
	Serial& uart;
	volatile bool idle = true;
};



#endif /* ZEPHYR_COMMON_API_UART_BUFFER_H_ */
