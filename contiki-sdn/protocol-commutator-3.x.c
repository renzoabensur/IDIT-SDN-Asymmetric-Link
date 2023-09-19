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

#include "protocol-commutator.h"
#include "sdn-network.h"
#include "sdn-debug.h"
#include "net/mac/frame802154.h"
#include "net/netstack.h"
#include "net/packetbuf.h"

static uint8_t *packetbuf_ptr;

/**
 * Function to initialize the protocol.
 *
 */
static void
init()
{
}

/**
 * Sends the data to the radio.
 *
 * @param sent  The callback function after the message has been sent.
 * @param *ptr  The pointer to the data array to send.
 */
static void
send(mac_callback_t sent, void *ptr)
{
  packetbuf_ptr = packetbuf_dataptr();

  packetbuf_set_attr(PACKETBUF_ATTR_FRAME_TYPE, FRAME802154_DATAFRAME);
  NETSTACK_MAC.send(sent, ptr);
}

/**
 * Function from callback MAC to signal packet received from radio.
 *
 */
static void
input(void)
{
  packetbuf_ptr = packetbuf_dataptr();
  // printf("PROTOCOL-COMMUTATOR %02X %02X\n", packetbuf_ptr[0], packetbuf_ptr[1]);
  /* Filters SDN packets according to "reserved" header field. */
  if ( packetbuf_ptr[1] == 0xFF) {
    sdn_network_driver.input();
  } else {
    NETSTACK_NETWORK.input();
  }
}

/**
 * Struct to store protocol functions.
 */
const struct llsec_driver commutator_protocol_driver = {
  "commutator-protocol",
  init,
  send,
  input
};
