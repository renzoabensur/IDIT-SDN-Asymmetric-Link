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
 */

/**
* \addtogroup SDN-common
* @{
*/

/**
 * \defgroup sdn-debug SDN debug functions
 * @{
 *
 *
 */
#ifndef DEBUG_SDN_H
#define DEBUG_SDN_H

#ifdef SDN_CONTROLLER_PC
#include <stdint.h>
#endif

#include "stdio.h"

#define TO_STRING(value) #value
#define STRINGFY(value) #value " = " TO_STRING(value)

#define SDN_DEBUG_ERROR(format, ...) printf("ERROR: %s:%d: " format, __FILE__, __LINE__, ##__VA_ARGS__);

#ifdef DEBUG_SDN
  #ifdef SDN_CONTROLLER_NODE
    #include "sdn-serial.h"
    #define SDN_DEBUG(...) //printf(__VA_ARGS__);//sdn_serial_printf(__VA_ARGS__)
  #else // SDN_CONTROLLER_NODE
    #define SDN_DEBUG(...) printf(__VA_ARGS__);
  #endif // SDN_CONTROLLER_NODE
#else // DEBUG_SDN
  #define SDN_DEBUG(...)
#endif // DEBUG_SDN

#ifdef SDN_METRIC
#define SDN_METRIC_RXTX(packet_ptr, RXTX) \
  printf("="); \
  sdnaddr_print_nodebug(&SDN_HEADER(packet_ptr)->source); \
  printf("=%s=%02X=%02X=", RXTX, SDN_HEADER(packet_ptr)->type, SDN_HEADER(packet_ptr)->seq_no); \
  if (SDN_ROUTED_BY_ADDR(packet_ptr)) \
    sdnaddr_print_nodebug(&SDN_GET_PACKET_ADDR(packet_ptr)); \
  if (SDN_ROUTED_BY_FLOWID(packet_ptr)) \
    flowid_print(&SDN_GET_PACKET_FLOW(packet_ptr)); \
  if (SDN_ROUTED_BY_SRC(packet_ptr)) \
    sdnaddr_print_nodebug(&SDN_GET_PACKET_REAL_DEST(packet_ptr)); \
  printf("=\n");

#define SDN_METRIC_TX(packet_ptr) SDN_METRIC_RXTX(packet_ptr, "TX")
#define SDN_METRIC_RX(packet_ptr) SDN_METRIC_RXTX(packet_ptr, "RX")
#define SDN_METRIC_ENERGY(...) printf(__VA_ARGS__);
#else //SDN_METRIC
#define SDN_METRIC_RXTX(packet_ptr, RXTX)
#define SDN_METRIC_TX(packet_ptr)
#define SDN_METRIC_RX(packet_ptr)
#define SDN_METRIC_ENERGY(...)
#endif //SDN_METRIC

#ifdef __cplusplus
extern "C" {
#endif

void print_packet(uint8_t *packet, uint16_t len);

#ifdef __cplusplus
}
#endif

#endif /* DEBUG_SDN_H */

/** @} */
/** @} */
/** @} */
