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

#ifndef SDN_CONTROLLER_PC
#include "net/packetbuf.h"
#else // SDN_CONTROLLER_PC
#include "stdlib.h"
#endif // SDN_CONTROLLER_PC
#include "sdn-debug.h"
#include "sdn-packetbuf.h"

#ifndef SDN_CONTROLLER_PC
/* Declaring MEMB to hold SDN packets entries. */
MEMB(sdn_packetbuf_pool, sdn_packetbuf, SDN_PACKETBUF_POOL_SIZE);
#endif // SDN_CONTROLLER_PC

sdn_packetbuf* sdn_packetbuf_pool_get() {
#ifndef SDN_CONTROLLER_PC
  return memb_alloc(&sdn_packetbuf_pool);
#else // SDN_CONTROLLER_PC
  return malloc(SDN_MAX_PACKET_SIZE);
#endif // SDN_CONTROLLER_PC
}

void sdn_packetbuf_pool_put(sdn_packetbuf* packet) {
#ifndef SDN_CONTROLLER_PC
  memb_free(&sdn_packetbuf_pool, packet);
#else
  free(packet);
#endif // SDN_CONTROLLER_PC
}
