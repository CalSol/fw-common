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
 *    CANRXBuffer<32> canBuffer(can);

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
template <int RXSize>
class CANRXBuffer {
public:
  /** Constructs a new, empty CAN message buffer
   *
   *  @param can CAN interface to read messages from
   *  @param handle message filter handle (0 for any message)
   */
  CANRXBuffer(CAN& can, int handle=0) : can(can), handle(handle) {
    can.attach(callback(this, &CANRXBuffer<RXSize>::handleIrq), CAN::RxIrq);
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

  /** Non-buffered passthrough write.
   */
  int write(CANMessage msg) {
    while (!can.write(msg)) {
    }
    return true;
  }

  /** CAN receive message IRQ handler
   *  Reads any pending CAN messages into the RX buffer
   *  Stops when there are no more pending messages or the RX buffer is full
   */
  void handleIrq() {
    CANMessage msg;
    while (can.read(msg, handle) && !rxFull()) {
      rxBuffer.write(msg);
    }
  }

private:
  calsol::util::CircularBuffer<CANMessage, RXSize> rxBuffer;
  CAN& can;
  const int handle;
};

#endif // __ZEPHYR_COMMON_CAN_BUFFER_H__
