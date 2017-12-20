/*
 * can_buffer.h
 *
 *  Created on: Jun 10, 2014
 *      Author: Devan
 */

#ifndef __ZEPHYR_COMMON_CAN_BUFFER_H__
#define __ZEPHYR_COMMON_CAN_BUFFER_H__
#include <stdint.h>

#ifndef __ZEPHYR_COMMON_NO_MBED__
#include <mbed.h>
#endif // __ZEPHYR_COMMON_NO_MBED__

#include "circular_buffer.h"

/** CAN Message circular buffer template class + IRQ handler
 *
 *  @param RXSize size of receive buffer in messages; must be a power of 2
 *
 *  Typical usage:
 *    // 32 message buffer
 *    CANBuffer<32> canBuffer(can);

 *    void main() {
 *        while (1) {
 *            CANMessage msg;
 *            // Use CANRXBuffer::read instead of CAN::read
 *            if (canBuffer.read(msg)) {
 *                doStuff(msg);
 *            }
 *        }
 *    }
 */
template <int Size>
class CANBuffer {
public:
  /** Constructs a new, empty CAN message buffer
   *
   *  @param can CAN interface to read messages from
   *  @param handle message filter handle (0 for any message)
   */
  CANBuffer(CAN& can, int handle=0) : can(can), handle(handle) {
    can.attach(callback(this, &CANBuffer<Size>::handleRxIrq), CAN::RxIrq);
    can.attach(callback(this, &CANBuffer<Size>::handleTxIrq), CAN::TxIrq);
  }

  /** Check if the receive buffer is empty
   *
   *  @returns
   *    true if empty
   *    false if not empty
   */
  bool rxEmpty() const {
    return rxBuffer.empty();
  }

  /** Check if the receive buffer is full
   *
   *  @returns
   *    true if full
   *    false if not full
   */
  bool rxFull() const {
    return rxBuffer.full();
  }

  /** Read a CANMessage from the buffer.
   *
   *  @param msg A CANMessage to read to.
   *
   *  @returns
   *    0 if no message arrived,
   *    1 if message arrived
   */
  int read(CANMessage& msg) {
    int messageValid = 0;
    if (!rxEmpty()) {
      msg = rxBuffer.read();
      messageValid = 1;
    }

    return messageValid;
  }

  /** Buffered write. Returns 0 if the buffer is full.
   */
  int write(CANMessage msg) {
    int success = 0;
    __disable_irq();
    if(txIdle) {
      txIdle = false;
      can.write(msg);
      success = 1;
    } else if(!txBuffer.full()) {
      txBuffer.write(msg);
      success = 1;
    }
    __enable_irq();
    return success;
  }

  /** CAN receive message IRQ handler
   *  Reads any pending CAN messages into the RX buffer
   *  Stops when there are no more pending messages or the RX buffer is full
   */
  void handleRxIrq() {
    CANMessage msg;
    while (can.read(msg, handle) && !rxFull()) {
      rxBuffer.write(msg);
    }
  }

  /** CAN transmit IRQ handler
   *  Will get called each time the CAN peripheral is finished transmitting the message
   *  It will then send a new message until the buffer is empty.
   */
  void handleTxIrq() {
    if(!txBuffer.empty()) {
      can.write(txBuffer.read());
    } else {
      txIdle = true;
    }
  }

  /** Reset the buffers
   *  Should be typically called when the CAN peripheral is reset.
   */
  void reset() {
    rxBuffer.clear();
    txBuffer.clear();
    txIdle = true;
  }

private:
  calsol::util::CircularBuffer<CANMessage, Size> rxBuffer;
  calsol::util::CircularBuffer<CANMessage, Size> txBuffer;
  bool txIdle = true;
  CAN& can;
  const int handle;
};

#endif // __ZEPHYR_COMMON_CAN_BUFFER_H__
