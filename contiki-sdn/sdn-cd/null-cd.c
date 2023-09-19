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

/**
 * \file
 *         ...
 * \author
 *         Renan C. A. Alves <ralves@larc.usp.br>
 */

/**
 * \addtogroup SDN
 * @{
 * \addtogroup SDN-contiki
 * @{
 * \defgroup sdn-cd Controller Discovery
 * @{
 *
 * \section null-cd Null controller discovery
 */

#include "sdn-network.h"
#include "sdn-protocol.h"
#include "sdn-addr.h"
#include "sdn-debug.h"
#include "contiki.h"

//extern struct process sdn_core_process;

void (* controller_discovery_event)();

//PROCESS(null_cd_process, "SDN null CD process");

void null_cd_init(void *cd_event, uint8_t is_controller) {
  SDN_DEBUG ("Null Controller Discovery initialized!\n");

  controller_discovery_event = cd_event;

  //process_start(&null_cd_process, NULL);
}

//PROCESS_THREAD(null_cd_process, ev, data)
//{
//  PROCESS_BEGIN();

//  PROCESS_END();
//}

void null_cd_get(sdnaddr_t* next_hop) {
  next_hop = NULL;
}

void null_cd_input(uint8_t* packet_ptr, uint16_t packet_len) {

}

const struct sdn_cd null_sdn_cd = {
  null_cd_init,
  null_cd_get,
  null_cd_input
};
/** @} */
/** @} */
/** @} */
