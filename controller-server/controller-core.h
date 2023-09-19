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
 *         Doriedson A. G. O. <doriedson@usp.br>
  */

/**
 * \addtogroup SDN
 * @{
 * \addtogroup SDN-controller
 * @{
 *
 *
*/
#ifndef CONTROLLER_CORE_H
#define CONTROLLER_CORE_H

// #include "contiki-definitions.h"
#include <stdint.h>
#include "sdn-protocol.h"

#ifdef __cplusplus
extern "C" {
#endif

void controller_init(sdnaddr_t *address);

void controller_finalize();

void controller_receive(uint8_t *packet, uint16_t len);

//void process_neighbor_report(sdn_neighbor_report_t* sdn_neighbor_report, void* neighbors);

void proccess_data_flow_request(sdn_data_flow_request_t *sdn_data_flow_request);

// void update_flow_table_cache();

#ifdef __cplusplus
}
#endif

#endif /* CONTROLLER_CORE_H */

/** @} */
/** @} */
