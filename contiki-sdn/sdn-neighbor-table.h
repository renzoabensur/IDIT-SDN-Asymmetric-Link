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
 * \defgroup sdn-nt SDN neighbor table
 * @{
 *
 */

#ifndef SDN_NEIGHBOR_TABLE_H_
#define SDN_NEIGHBOR_TABLE_H_

#include "lib/memb.h"
#include "sdn-addr.h"
#include "sdn-constants.h"

struct sdn_neighbor_entry {
  /* The ->next pointer is needed since we are placing these on a
     Contiki list. */
  struct sdn_neighbor_entry *next;

  sdnaddr_t neighbor_addr;
  uint16_t metric;
  void* extra_info;
};

#ifndef SDN_CONF_NEIGHBOR_TABLE_SIZE
#define SDN_NEIGHBOR_TABLE_SIZE 10
#else //SDN_CONF_NEIGHBOR_TABLE_SIZE
#define SDN_NEIGHBOR_TABLE_SIZE SDN_CONF_NEIGHBOR_TABLE_SIZE
#endif//SDN_CONF_NEIGHBOR_TABLE_SIZE

/**
 * \brief      Initializes the SDN neighbor table
 * \retval     SDN_SUCCESS or SDN_ERROR
 *
 */
uint8_t sdn_neighbor_table_init();

/**
 * \brief               Insert or updates an entry in the the SDN neighbor table
 * \param neighbor_addr Neighbor SDN address
 * \param metric        Metric value associated to the neighbor
 * \retval pointer      Poninter to an existing entry w.r.t. neighbor_addr
 * \retval NULL         It was not possible to create new entry
 *
 */
struct sdn_neighbor_entry * sdn_neighbor_table_insert(sdnaddr_t neighbor_addr, struct memb *m);

/**
 * \brief Remove an entry in the the SDN neighbor table based on address
 * \param neighbor_addr Neighbor SDN address
 * \retval SDN_SUCCESS  Entry was removed
 * \retval SDN_ERROR    Entry not found
 *
 */
uint8_t sdn_neighbor_table_remove_by_addr(sdnaddr_t neighbor_addr, struct memb *m);

/**
 * \brief Remove an entry in the the SDN neighbor table based on the entry pointer
 * \param n Neighbor table entry pointer
 * \retval SDN_SUCCESS  Entry was removed
 * \retval SDN_ERROR    Entry not found
 *
 */
uint8_t sdn_neighbor_table_remove_by_pointer(struct sdn_neighbor_entry *n, struct memb *m);

/**
 * \brief Retrieve neighbor information from SDN neighbor table
 * \param neighbor_addr Neighbor SDN address
 * \retval  Non-NULL if the entry was found, NULL otherwise
 *
 */
struct sdn_neighbor_entry * sdn_neighbor_table_get(sdnaddr_t neighbor_addr);

/**
 * \brief Counts the number of neighbors in the SDN neighbor table
 *
 */
uint8_t sdn_neighbor_table_count();

/**
 * \brief Retrieve first element from SDN neighbor table
 * \retval  Non-NULL if table is not empty, NULL otherwise
 *
 */
struct sdn_neighbor_entry * sdn_neighbor_table_head();

/**
 * \brief Retrieve next element from SDN neighbor table
 * \param neighbor_addr Neighbor SDN address
 * \retval  Non-NULL if there is another element, NULL otherwise
 *
 */
struct sdn_neighbor_entry * sdn_neighbor_table_next(sdnaddr_t neighbor_addr);

/**
 * \brief Prints all neighbor data, for debugging purposes
 *
 */
void sdn_neighbor_table_print();

#endif //SDN_NEIGHBOR_TABLE_H_

/** @} */
/** @} */
/** @} */
