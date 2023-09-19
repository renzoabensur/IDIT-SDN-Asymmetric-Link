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
 *         Renan C. A. Alves <ralves@larc.usp.br>
 */

 /**
  * \addtogroup SDN
  * @{
  * \addtogroup SDN-common
  * @{
  * \addtogroup flowtable Flow Table
  * @{
  *
  *
 */

#ifndef SDN_DATA_FLOW_TABLE_H_
#define SDN_DATA_FLOW_TABLE_H_

#include "sdn-addr.h"
#include "sdn-constants.h"
#include "sdn-protocol.h"

#ifdef __cplusplus
extern "C" {
#endif

struct data_flow_entry {
  /* The ->next pointer is needed since we are placing these on a
     Contiki list. */
  struct data_flow_entry *next;

  flowid_t flowid;
  sdnaddr_t next_hop;
  action_t action;
  uint16_t times_used;
  uint8_t age;
};

/**
 * \brief      Initializes the data flow table
 * \retval     SDN_SUCCESS or SDN_ERROR
 *
 */
uint8_t sdn_dataflow_init();

/**
 * \brief             Insert or updates an entry in the the data flow table
 * \param flowid      Flow identification value
 * \param next_hop    Address of next hop to reach the flow final destination
 * \param action      action related to the flowid
 * \retval SDN_SUCCESS  Entry was added or updated
 * \retval SDN_ERROR    It was not possible to create new entry
 *
 */
uint8_t sdn_dataflow_insert(flowid_t flowid, sdnaddr_t next_hop, \
                            action_t action);

/**
 * \brief Remove all entry in the the data flow table based on next hop address
 * \param next_hop next hop address
 * \retval SDN_SUCCESS  default
 *
 */
uint8_t sdn_dataflow_remove_next_hop(sdnaddr_t next_hop);

/**
 * \brief Remove an entry in the the data flow table based on flow identification
 * \param flowid identification value
 * \retval SDN_SUCCESS  Entry was removed
 * \retval SDN_ERROR    Entry not found
 *
 */
uint8_t sdn_dataflow_remove(flowid_t flowid);

/**
 * \brief   Retrieve data flow table entry
 * \param   flowid identification value
 * \retval  Non-NULL if the entry was found, NULL otherwise
 *
 */
struct data_flow_entry * sdn_dataflow_get(flowid_t flowid);

/**
 * \brief   Removes all entries
 *
 */
void sdn_dataflow_clear();

/**
 * \brief Prints all data flow table entries, for debugging purposes
 *
 */
void sdn_dataflow_print();

#ifdef __cplusplus
}
#endif

#endif //SDN_DATA_FLOW_TABLE_H_

/** @} */
/** @} */
/** @} */
