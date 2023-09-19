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
 *         Gustavo Segura <gnunez@larc.usp.br>
 */

/**
 * \addtogroup SDN
 * @{
 * \addtogroup SDN-contiki
 * @{
 * \defgroup sdn-nd Neighbor Discovery
 * @{
 *
 * \section collect-nd Collect-based neighbor discovery
 */

#include "contiki.h"
#include "collectsdn.h"
#include "lib/list.h"
#include "sdn-addr.h"
#include "sdn-protocol.h"
#include "sdn-neighbor-table.h"
#include "sdn-network.h"

#ifndef SDN_NEIGHBOR_ETX_SENSIBILITY
#define SDN_NEIGHBOR_ETX_SENSIBILITY 100 //200 //in percent
#endif
#ifndef SDN_NEIGHBOR_ETX_DELTA
#define SDN_NEIGHBOR_ETX_DELTA 1 //256 //
#endif

/**
 *\brief Close the neighbor discovery communication
 *
 */
void neighbors_close();

/**
 *\brief Actualize the sdn's neighbors table if there's a change
 *       in the collect table
 *\param *neighbor_list  a pointer to a list copy of the collect's neighbors
 *                       list
 *\retval SDN_SUCCESS  Case the table list has changed.
 *\retval SDN_ERROR    Case not.
 */
uint8_t sdn_neighbors_table_act(struct collect_neighbor_list *neighbor_list);

/**
 *\brief Returns the pointer to the collect_conn struct
 */

struct collect_conn *collect_pointer();

/**
 *\brief Prints the copy made from the collect's neighbors list
 *\param *neighbor_list  A pointer to a list copy of the collect's neighbors
 *                       list
 */
void print_neighbors(struct collect_neighbor_list *neighbor_list);

/** @} */
/** @} */
/** @} */
