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

#include "net/netstack.h"
#include "net/packetbuf.h"
#include "sdn-network.h"
#include "dev/watchdog.h"
#include "sdn-debug.h"
#include "sdn-addr.h"
#include "sdn-constants.h"

#ifdef ENERGY_MANAGER
#include "energy-manager.h"
#endif

// #include "sdn-receive.h"

static uint8_t packetbuf_hdr_len;

/**
 * A pointer to the packetbuf buffer.
 * We initialize it to the beginning of the packetbuf buffer, then
 * access different fields by updating the offset packetbuf_hdr_len.
 */
static uint8_t *packetbuf_ptr;

extern void sdn_send_done(void *ptr, int status, int transmissions);
extern void sdn_receive();

/*--------------------------------------------------------------------*/
static void
init(void) {

}

/*--------------------------------------------------------------------*/
static void
input(void) {
  sdn_receive();
}

/*--------------------------------------------------------------------*/
/**
 * Callback function for the MAC packet sent callback
 */
static void
packet_sent(void *ptr, int status, int transmissions)
{
  sdn_send_done(ptr, status, transmissions);
}

void sdn_network_send(void *data, uint16_t len, linkaddr_t dest) {

#ifdef ENERGY_MANAGER
  if(em_has_battery() == 0) return;
#endif

  /* reset packetbuf buffer */
  packetbuf_clear();
  packetbuf_ptr = packetbuf_dataptr();

  packetbuf_set_addr(PACKETBUF_ADDR_RECEIVER, &dest);

  memcpy(packetbuf_ptr + packetbuf_hdr_len, data, len);
  packetbuf_set_datalen(len + packetbuf_hdr_len);

  /* Provide a callback function to receive the result of
     a packet transmission. */
  NETSTACK_LLSEC.send(&packet_sent, (void *)data);

  /* If we are sending multiple packets in a row, we need to let the
     watchdog know that we are still alive. */
  watchdog_periodic();
}

/*--------------------------------------------------------------------*/
const struct network_driver sdn_network_driver = {
  "sdn_network",
  init,
  input
};
