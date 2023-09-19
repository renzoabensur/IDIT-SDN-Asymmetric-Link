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
 * \defgroup sdn-cd Controller Discovery
 * @{
 *
 * \section collect-cd Collect-based controller discovery
 */

#include "contiki.h"
//#include "net/rime/collect-neighbor.h"
#include "collectsdn.h"
//#include "lib/list.h"
#include "sdn-addr.h"
#include "sdn-protocol.h"
#include "sdn-neighbor-table.h"
#include "sdn-network.h"
#include "collect-nd.h"


/**
 *\brief	Initializes the controller discovery process
 *\			This fuction gets the pointer of the collect_conn struct
 *			and starts the process "controller_discovery"
 */
void collect_cd_init(void *cd_event, uint8_t);

/**
 *\brief Close the neighbor discovery communication
 *
 */
void controller_discovery_close();

/**
 *\brief	Checks if the parent changes and if so, actualizes it
 *\param	*tc is a pointer to the collect_conn type struct used
 * 			to open the collect communication
 */
void collect_cd_act(struct collect_conn *tc);

/**
 *\brief	Copies the address of the current parent into sdn_parent
 *\param 	*sdn_parent a pointer to the temp variable where the address
 *			of the parent will be stores in the sdn_core
 */

void collect_cd_get(sdnaddr_t* sdn_parent);

/**
 *\brief	Prints the current parent of the node
 */

void print_parent();
/** @} */
/** @} */
/** @} */
