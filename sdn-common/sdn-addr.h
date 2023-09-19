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
 *         Header file for the SDN address representation
 * \author
 *         Doriedson A. G. O. <doriedson@usp.br>
 *         Renan C. A. Alves <ralves@larc.usp.br>
 */

/**
 * \addtogroup SDN
 * @{
 * \addtogroup SDN-common
 * @{
 */

/**
 * \defgroup sdn-addr SDN addresses
 * @{
 *
 * The sdn-addr module is an abstract representation of addresses in
 * SDN.
 *
 */

#ifndef SDNADDR_H_
#define SDNADDR_H_

#include "sdn-protocol.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef SDN_CONTROLLER_PC
extern const linkaddr_t linkaddr_null;
#endif

extern sdnaddr_t sdnaddr_null;

/**
 * \brief Copies SDN address values
 * \param to    copy recipient
 * \param from  pointer to the address to be replicated
 *
 */
void sdnaddr_copy(sdnaddr_t *to, sdnaddr_t *from);

/**
 * \brief Compares SDN addresses
 * \param addr1 pointer to first address
 * \param addr2 pointer to second address
 * \retval SDN_EQUAL if both addresses are equal
 * \retval ~SDN_EQUAL otherwise
 *
 */
uint8_t sdnaddr_cmp(sdnaddr_t* addr1, sdnaddr_t* addr2);

/**
 * \brief         Changes the specified address byte to a chosen value
 * \param addr    pointer to address
 * \param bytenum number of addres byte. Should be in [0, sizeof(sdnaddr_t)[
 * \param value   new byte value
 * \retval SDN_SUCCESS if bytenum is within a valid range
 * \retval SDN_ERROR   otherwise
 *
 */
uint8_t sdnaddr_setbyte(sdnaddr_t* addr, uint8_t bytenum, uint8_t value);

/**
 * \brief      Retrieves the specified byte of the address, to avoid casting
 * \param addr    pointer to address
 * \param bytenum number of addres byte. Should be in [0, sizeof(sdnaddr_t)[
 * \param value   pointer to result, undefined if return value is SDN_ERROR
 * \retval SDN_SUCCESS if bytenum is within a valid range
 * \retval SDN_ERROR   otherwise
 *
 */
uint8_t sdnaddr_getbyte(sdnaddr_t* addr, uint8_t bytenum, uint8_t *value);

/**
 * \brief Prints addr bytes in HEX, for debugging purposes
 *
 */
void sdnaddr_print(sdnaddr_t* addr);
void sdnaddr_print_nodebug(sdnaddr_t* addr);

/**
 * \brief Copies flow identification values
 * \param to    copy recipient
 * \param from  pointer to the flow id to be replicated
 *
 */
void flowid_copy(flowid_t *to, flowid_t *from);

/**
 * \brief Compares flow identification values
 * \param f1 pointer to first flow identification
 * \param f2 pointer to second flow identification
 * \retval SDN_EQUAL if both flow identification are equal
 * \retval ~SDN_EQUAL otherwise
 *
 */
uint8_t flowid_cmp(flowid_t* f1, flowid_t* f2);

/**
 * \brief Prints a flow identification, for debugging purposes
 *
 */
void flowid_print(flowid_t* f);

/**
 * \brief Variable that hold the node own SDN address
 *
 */
extern sdnaddr_t sdn_node_addr;
extern sdnaddr_t *sdn_addr_broadcast;

#ifdef __cplusplus
}
#endif

#endif //SDNADDR_H_

/** @} */
/** @} */
/** @} */
