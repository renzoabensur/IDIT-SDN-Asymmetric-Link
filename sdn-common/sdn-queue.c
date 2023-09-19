/*
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain this list of conditions
 *    and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the Institute nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY
 * AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL
 * THE SOFTWARE PROVIDER OR DEVELOPERS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

#include "sdn-queue.h"
#include "sdn-debug.h"
#include "sdn-constants.h"
#include <string.h>
#ifdef SDN_CONTROLLER_PC
#include "sdn-serial-packet.h"
#endif

#ifdef MANAGEMENT
#include "manage-info.h"
#ifdef SDN_ENABLED_NODE
#include "contiki.h"
#endif
#endif

typedef struct {
  uint8_t head;
  uint8_t tail;
  uint8_t max_size;
  uint8_t size;
} sdn_queue_t;

sdn_queue_t sdn_send_queue;
sdn_queue_t sdn_recv_queue;

sdn_send_queue_data_t sdn_send_queue_data[SDN_SEND_QUEUE_SIZE];
sdn_recv_queue_data_t sdn_recv_queue_data[SDN_RECV_QUEUE_SIZE];

void sdn_send_queue_init() {
  int i;
  sdn_send_queue.head = 0;
  sdn_send_queue.tail = 0;
  sdn_send_queue.size = 0;
  sdn_send_queue.max_size = SDN_SEND_QUEUE_SIZE ;
  for (i=0; i<SDN_SEND_QUEUE_SIZE ; i++) {
    sdn_send_queue_data[i].data = NULL;
    sdn_send_queue_data[i].len = 0;

#if defined (MANAGEMENT) && defined(SDN_ENABLED_NODE)
    sdn_send_queue_data[i].time = 0;
#endif
  }
}

uint8_t sdn_send_queue_empty() {
  // SDN_DEBUG ("SDN_QUEUE_EMPTY %d\n", queue->size);
  return (sdn_send_queue.size == 0);
}

uint8_t sdn_send_queue_size() {
  // SDN_DEBUG ("SDN_QUEUE_SIZE %d\n", queue->size);
  return sdn_send_queue.size;
}

uint8_t sdn_send_queue_maxSize() {
  // SDN_DEBUG ("SDN_QUEUE_MAX_SIZE %d\n", queue->max_size);
  return sdn_send_queue.max_size;
}

sdn_send_queue_data_t* sdn_send_queue_head() {

  return &sdn_send_queue_data[sdn_send_queue.head];
}

sdn_send_queue_data_t* sdn_send_queue_tail() {
  if (sdn_send_queue.tail == 0)
    return &sdn_send_queue_data[sdn_send_queue.max_size-1];
  else
    return &sdn_send_queue_data[sdn_send_queue.tail-1];
}

sdn_send_queue_data_t* sdn_send_queue_dequeue() {

  sdn_send_queue_data_t *t = sdn_send_queue_head();

  if (!sdn_send_queue_empty()) {
    sdn_send_queue.head++;
    if (sdn_send_queue.head == sdn_send_queue.max_size) sdn_send_queue.head = 0;
    sdn_send_queue.size--;

    //SDN_DEBUG ("Packet for send, dequeued!\n");
    //sdn_send_queue_print();
  } else {
    SDN_DEBUG ("Send queue is empty!\n");
  }

  return t;
}

uint8_t sdn_send_queue_enqueue(uint8_t *data, uint16_t len, uint32_t time) {

#ifdef SDN_CONTROLLER_PC
  return sdn_send_queue_enqueue_custom(data, len, time, SDN_SERIAL_MSG_TYPE_RADIO);
}

uint8_t sdn_send_queue_enqueue_custom(uint8_t *data, uint16_t len, uint32_t time, uint8_t msg_type) {
#endif //SDN_CONTROLLER_PC
  if (sdn_send_queue_size() < sdn_send_queue_maxSize()) {

    sdn_send_queue_data[sdn_send_queue.tail].data = data;
    sdn_send_queue_data[sdn_send_queue.tail].len = len;
#ifdef SDN_CONTROLLER_PC
    sdn_send_queue_data[sdn_send_queue.tail].msg_type = msg_type;
#endif
#if defined (MANAGEMENT) && defined (SDN_ENABLED_NODE)
    sdn_send_queue_data[sdn_send_queue.tail].time = time;
#else
    time = time;
#endif
    sdn_send_queue.tail+= 1;
    if (sdn_send_queue.tail == sdn_send_queue.max_size) sdn_send_queue.tail = 0;
    sdn_send_queue.size+= 1;

    //SDN_DEBUG ("Packet for send, enqueued!\n");
#ifdef SDN_CONTROLLER_PC
    sdn_send_queue_print();
#endif //SDN_CONTROLLER_PC
    return SDN_SUCCESS;
  } else {
    return SDN_ERROR;
  }
}

sdn_send_queue_data_t* sdn_send_queue_find_by_type(uint8_t type) {
  uint8_t indexCount, indexPacket;
  for (indexCount = 0, indexPacket = sdn_send_queue.head; indexCount < sdn_send_queue.size; indexCount++) {

    if (sdn_send_queue_data[indexPacket].data != NULL) {
      if (SDN_HEADER(sdn_send_queue_data[indexPacket].data)->type == type && \
          sdnaddr_cmp(&SDN_HEADER(sdn_send_queue_data[indexPacket].data)->source, &sdn_node_addr) == SDN_EQUAL) {
        return &sdn_send_queue_data[indexPacket];
      }
    }

    indexPacket++;
    if (indexPacket == sdn_send_queue.max_size) {
      indexPacket = 0;
    }
  }
  return NULL;
}

void sdn_send_queue_print() {

  uint8_t indexPacket;
  uint8_t indexByte;
  uint8_t indexCount;

  SDN_DEBUG ("Packets on Send Queue (%d):\n", sdn_send_queue.size);

  for (indexCount = 0, indexPacket = sdn_send_queue.head; indexCount < sdn_send_queue.size; indexCount++) {

    SDN_DEBUG("(%d) ",indexPacket);

    if (sdn_send_queue_data[indexPacket].data != NULL) {
#ifdef SDN_CONTROLLER_PC
      SDN_DEBUG ("[%d] ", sdn_send_queue_data[indexPacket].msg_type);
#endif
      SDN_DEBUG ("[ ");

      for (indexByte = 0; indexByte < 6; indexByte++) //sdn_header_t = 6
        SDN_DEBUG("%02X ", sdn_send_queue_data[indexPacket].data[indexByte]);

      SDN_DEBUG ("] [ ");

      for (; indexByte < sdn_send_queue_data[indexPacket].len; indexByte ++)
        SDN_DEBUG("%02X ", sdn_send_queue_data[indexPacket].data[indexByte]);

      SDN_DEBUG ("]");
      SDN_DEBUG("\n");
    } else {
      SDN_DEBUG("data == NULL\n");
    }

    indexPacket++;
    if (indexPacket == sdn_send_queue.max_size) {
      indexPacket = 0;
    }
  }

  SDN_DEBUG ("\n");
}

void sdn_recv_queue_init() {
  int i;
  sdn_recv_queue.head = 0;
  sdn_recv_queue.tail = 0;
  sdn_recv_queue.size = 0;
  sdn_recv_queue.max_size = SDN_RECV_QUEUE_SIZE ;
  for (i=0; i<SDN_RECV_QUEUE_SIZE ; i++) {
    sdn_recv_queue_data[i].data = NULL;
    sdn_recv_queue_data[i].len = 0;
#if defined(MANAGEMENT) && defined(SDN_ENABLED_NODE)
    sdn_send_queue_data[i].time = 0;
#endif
  }
}

uint8_t sdn_recv_queue_empty() {
  return (sdn_recv_queue.size == 0);
}

uint8_t sdn_recv_queue_size() {
  return sdn_recv_queue.size;
}

uint8_t sdn_recv_queue_maxSize() {
  return sdn_recv_queue.max_size;
}

sdn_recv_queue_data_t* sdn_recv_queue_head() {

  return &sdn_recv_queue_data[sdn_recv_queue.head];
}

sdn_recv_queue_data_t* sdn_recv_queue_dequeue() {

  sdn_recv_queue_data_t *t = NULL;

  if (!sdn_recv_queue_empty()) {
    t = sdn_recv_queue_head();
    sdn_recv_queue.head++;
    if (sdn_recv_queue.head == sdn_recv_queue.max_size) sdn_recv_queue.head = 0;
    sdn_recv_queue.size--;

    //SDN_DEBUG ("Packet for recv, dequeued!\n");
    //sdn_recv_queue_print();
  } else {
    SDN_DEBUG ("Send recv queue is empty!\n");
  }

  return t;
}

uint8_t sdn_recv_queue_enqueue(uint8_t *data, uint16_t len) {

  if (sdn_recv_queue_size() < sdn_recv_queue_maxSize()) {
    sdn_recv_queue_data[sdn_recv_queue.tail].data = data;
    sdn_recv_queue_data[sdn_recv_queue.tail].len = len;
#if defined (MANAGEMENT) && defined (SDN_ENABLED_NODE)
    sdn_recv_queue_data[sdn_recv_queue.tail].time = clock_time();
#endif
    sdn_recv_queue.tail+= 1;
    if (sdn_recv_queue.tail == sdn_recv_queue.max_size) sdn_recv_queue.tail = 0;
    sdn_recv_queue.size+= 1;

    //SDN_DEBUG ("Packet for recv, enqueued!\n");
    //sdn_recv_queue_print();

    return SDN_SUCCESS;
  } else {
    return SDN_ERROR;
  }
}

void sdn_recv_queue_print() {

  uint8_t indexPacket;
  uint8_t indexByte;
  uint8_t indexCount;

  SDN_DEBUG ("Packets on Recv Queue (%d):\n", sdn_recv_queue.size);

  for (indexCount = 0, indexPacket = sdn_recv_queue.head; indexCount < sdn_recv_queue.size; indexCount++) {

    SDN_DEBUG("(%d) ",indexPacket);

    if (sdn_recv_queue_data[indexPacket].data != NULL) {
      SDN_DEBUG ("[ ");

      for (indexByte = 0; indexByte < 6; indexByte++) //sdn_header_t = 6
        SDN_DEBUG("%02X ", sdn_recv_queue_data[indexPacket].data[indexByte]);

      SDN_DEBUG ("] [ ");

      for (; indexByte < sdn_recv_queue_data[indexPacket].len; indexByte ++)
        SDN_DEBUG("%02X ", sdn_recv_queue_data[indexPacket].data[indexByte]);

      SDN_DEBUG ("]");
      SDN_DEBUG("\n");
    } else {
      SDN_DEBUG("data == NULL\n");
    }

    indexPacket++;
    if (indexPacket == sdn_recv_queue.max_size) {
      indexPacket = 0;
    }
  }

  SDN_DEBUG ("\n");
}
