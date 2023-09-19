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

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "dev/uart1.h"
#ifdef CC26XX_UART_H_
#include <strformat.h>
#endif

#include "contiki.h"

#include "sdn-debug.h"
#include "sdn-serial.h"
#include "sdn-serial-packet.h"

/* HDLC Asynchronous framing */
/* The frame boundary octet is 01111110, (7E in hexadecimal notation) */
#define FRAME_BOUNDARY_OCTET 0x7E

/* A "control escape octet", has the bit sequence '01111101', (7D hexadecimal) */
#define CONTROL_ESCAPE_OCTET 0x7D

/* If either of these two octets appears in the transmitted data, an escape octet is sent, */
/* followed by the original data octet with bit 5 inverted */
#define INVERT_OCTET 0x20

/************************** Buffers Definition ****************************************************/

static sdn_serial_packet_t p1, p2;
static char *currentSerialPacket = NULL;
static uint16_t frame_length = 0;

static inline void initSerialPacketBuffer() {
  currentSerialPacket = (char*)&p1;
}

static inline void updateSerialPacketBuffer() {
  if(((sdn_serial_packet_t*)currentSerialPacket) == &p1) {
    currentSerialPacket = (char*)&p2;
  }
  else if(((sdn_serial_packet_t*)currentSerialPacket) == &p2) {
    currentSerialPacket = (char*)&p1;
  }
}

/******************** End of Buffer Definition ****************************************************/


/***********************  Callbacks and process **************************************************/

sendchar_type sendchar_function = NULL;

PROCESS(serial_sembei_process, "Serial Sembei");

process_event_t sdn_raw_binary_packet_ev;
process_event_t sdn_custom_packet_ev;
process_event_t sdn_new_serial_packet_ev;
PROCESS_NAME(sdn_core_process);
/***********************  End of Callbacks *******************************************/

static int byteReceived(unsigned char data);

void sdn_serial_init(sendchar_type sendchar_fun) {
  sdn_new_serial_packet_ev = process_alloc_event();
  sendchar_function = sendchar_fun;
  initSerialPacketBuffer();
  /* Warning: This section is platform dependent */
  /* If using Wismote, sky or Sensortag platforms  */
#ifndef CC26XX_UART_H_
  uart1_init(BAUD2UBR(115200)); //set the baud rate as necessary
#endif
  uart1_set_input(&byteReceived); //set the callback function

  /* If using Z1 - platform */
  //uart0_init(BAUD2UBR(115200)); //set the baud rate as necessary
  //uart0_set_input(&byteReceived); //set the callback function

  process_start(&serial_sembei_process, NULL);
}


static int byteReceived(unsigned char data) {

  static bool escape_character = false;
  static bool overflow = false;

  // SDN_DEBUG("GOT: %02X\n", data);

  /* FRAME FLAG */
  if(data == FRAME_BOUNDARY_OCTET){

    if(escape_character == true){
      SDN_DEBUG("serial: escape_character == true\n");
      escape_character = false;

    } else if(overflow) { /* We lost consistence, begin again */
      /* Clear Buffer */
      SDN_DEBUG("serial overflow\n");
      overflow = false;
      frame_length=0;

    /* If a valid frame is detected> FRAME_BOUNDARY + PACKET MINIMUM SIZE (HEADER), otherwise discard. */
    } else if(frame_length >= sizeof(sdn_serial_packet_header_t)) {
      /* Wake up consumer process */
      frame_length=0;
      SDN_DEBUG("Call\n");
      process_post(&serial_sembei_process, sdn_new_serial_packet_ev, (void*)currentSerialPacket);
      updateSerialPacketBuffer();
      return 1;

    } else {
      /* re-synchronization. Start over*/
      SDN_DEBUG("serial re-synchronization\n");
      frame_length=0;
      return 0;
    }

    return 0;
  }

  if(escape_character){

    escape_character = false;
    data ^= INVERT_OCTET;

  } else if(data == CONTROL_ESCAPE_OCTET){

    escape_character = true;
    return 0;
  }

  if(frame_length < (SDN_MAX_SERIAL_PACKET_SIZE + 2)) { // Adding 2 bytes from serial communication

    currentSerialPacket[frame_length] = data;
    frame_length++;

  } else {

    overflow = true;
    SDN_DEBUG_ERROR("Packet size overflow: %u bytes\n", frame_length);
  }

  return 0;
}

/* Function to send a byte through Serial*/
void sendchar(char data)
{
  if(sendchar_function) {
    (*sendchar_function)((int)data);
  }
}

/* Wrap given data in HDLC frame and send it out byte at a time*/
void frameDecode(const char *framebuffer, uint8_t frame_length)
{
  uint8_t data=0;

  sendchar((uint8_t)FRAME_BOUNDARY_OCTET);

  while(frame_length) {

    data = *framebuffer++;

    if((data == CONTROL_ESCAPE_OCTET) || (data == FRAME_BOUNDARY_OCTET))
    {
      sendchar((uint8_t)CONTROL_ESCAPE_OCTET);
      data ^= INVERT_OCTET;
    }
    sendchar((uint8_t)data);
    frame_length--;
  }

  sendchar(FRAME_BOUNDARY_OCTET);
}

void sdn_serial_send(sdn_serial_packet_t *packet) {
  uint8_t size = sizeof(sdn_serial_packet_header_t) + packet->header.payload_len;
  frameDecode((const char *)packet,  size);
}

static sdn_serial_packet_t static_ack_serial_packet;

inline void sdn_serial_send_ack() {
    static_ack_serial_packet.header.payload_len = 0;
    static_ack_serial_packet.header.msg_type = SDN_SERIAL_MSG_TYPE_RADIO_ACK;
    memcpy(&static_ack_serial_packet.header.node_addr, &linkaddr_node_addr, sizeof(sdnaddr_t));
    sdn_serial_send(&static_ack_serial_packet);
}

inline void sdn_serial_send_nack() {
    static_ack_serial_packet.header.payload_len = 0;
    static_ack_serial_packet.header.msg_type = SDN_SERIAL_MSG_TYPE_RADIO_NACK;
    memcpy(&static_ack_serial_packet.header.node_addr, &linkaddr_node_addr, sizeof(sdnaddr_t));
    sdn_serial_send(&static_ack_serial_packet);
}

void sdn_serial_printf(char *format, ...)
{
  va_list args;
  va_start(args, format);

  static char str_buffer[SDN_SERIAL_MAX_PRINT_LEN+1];
  uint8_t headerSize = sizeof(sdn_serial_packet_header_t);

  sdn_serial_packet_header_t header;// = (sdn_serial_packet_header_t) {0};

  sdnaddr_copy(&header.node_addr, &sdn_node_addr);
  header.msg_type = SDN_SERIAL_MSG_TYPE_PRINT;

  vsprintf(str_buffer, format, args);

  header.payload_len = strlen(format);

  // \n to better separate from other messages
  // sendchar('\n');
  sendchar((uint8_t)FRAME_BOUNDARY_OCTET);

  char *ptr = (char*)&header;

  while(headerSize) {
    sendchar((char)(*ptr));
    headerSize--;
    ptr++;
  }

  uint8_t data=0;
  ptr = str_buffer;

  while(header.payload_len) {

    data = *ptr++;

    if((data == CONTROL_ESCAPE_OCTET) || (data == FRAME_BOUNDARY_OCTET))
    {
      sendchar((uint8_t)CONTROL_ESCAPE_OCTET);
      data ^= INVERT_OCTET;
    }
    sendchar((uint8_t)data);
    header.payload_len--;
  }

  sendchar(FRAME_BOUNDARY_OCTET);

  va_end(args);
}


/*****************************************************************************

This Thread handles packet. It decodes and takes necessary action.

*****************************************************************************/

static inline bool decodeSerialPacket(sdn_serial_packet_t *packet) {
  if(packet->header.msg_type == SDN_SERIAL_MSG_TYPE_ID_REQUEST) {
    sdn_serial_packet_header_t header;// = (sdn_serial_packet_header_t) {0};
    header.msg_type = SDN_SERIAL_MSG_TYPE_ID_REQUEST;
    sdnaddr_copy(&header.node_addr, &sdn_node_addr);
    header.payload_len = 0;
    SDN_DEBUG("DECODING REQUEST ID\n");
    sdn_serial_send((sdn_serial_packet_t*)&header);
  }
  else if(packet->header.msg_type == SDN_SERIAL_MSG_TYPE_RADIO) {
    /* Broadcast event */
    SDN_DEBUG("SDN_SERIAL_MSG_TYPE_RADIO\n");
    process_post(&sdn_core_process, sdn_raw_binary_packet_ev, (char*)packet);
    return false; //false: Indicates process must wait for all processes to handle the event
  }
  else {
    /* Broadcast event */
    SDN_DEBUG("other SDN_SERIAL_MSG_TYPE_\n");
    process_post(&sdn_core_process, sdn_custom_packet_ev, (char*)packet);
    return false; //false: Indicates process must wait for all processes to handle the event
  }

  return true;
}

PROCESS_THREAD(serial_sembei_process, ev, data)
{
  static sdn_serial_packet_t *packet = NULL;

  PROCESS_BEGIN();

  sdn_raw_binary_packet_ev = process_alloc_event();
  sdn_custom_packet_ev = process_alloc_event();

  SDN_DEBUG("sdn serial initialized\n");

  while(1) {

    PROCESS_WAIT_EVENT_UNTIL(ev == sdn_new_serial_packet_ev);

    packet = (sdn_serial_packet_t*)data;

    SDN_DEBUG("new packet\n");

    if(!decodeSerialPacket(packet)) {
      /* Wait until all processes have handled the serial event */
      if(PROCESS_ERR_OK == process_post(PROCESS_CURRENT(), PROCESS_EVENT_CONTINUE, NULL)) {
        PROCESS_WAIT_EVENT_UNTIL(ev == PROCESS_EVENT_CONTINUE);
      }
    }
  }

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
/*Process used to test the serial dirver implemented */

// #include <stdio.h>
// #include "contiki.h"
// #include "sys/etimer.h"
//
// PROCESS(hello_world_process, "Hello world process");
// AUTOSTART_PROCESSES(&hello_world_process);
//
// PROCESS_THREAD(hello_world_process, ev, data)
// {
//     static struct etimer et;
//     PROCESS_BEGIN();
//
//     printf("Main Process started\n");
//
//     sdn_serial_init(&putchar);
//
//     /* Delay 1 second */
//     etimer_set(&et, 5*CLOCK_SECOND);
//
//     while(1) {
//
//         PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));
//
//         /* Reset the etimer to trig again in 1 second */
//         etimer_reset(&et);
//
//     }
//
//     PROCESS_END();
// }
/*******************************************************************/
