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
 *         Header file for the SDN Routing
 * \author
 *         Doriedson A. G. O. <doriedson@usp.br>
 */

/**
 * \addtogroup SDN
 * @{
 * \addtogroup SDN-controller
 * @{
 * \addtogroup sdn-metric Graph library
 * @{
 *
 * The sdn-metric module is responsible for the SDN metric structure. 
 *
*/

#ifndef SDN_METRIC_H_
#define SDN_METRIC_H_

#ifndef SDN_METRIC
#ifdef SDN_CONF_METRIC
#define SDN_METRIC SDN_CONF_METRIC
#else /* SDN_CONF_ND */
#define SDN_METRIC dummy_metric
#endif /* SDN_CONF_ND */
#endif /* SDN_ND */

/**
 * The structure of a neighbor discovery in ContikiSDN.
 */
struct sdn_metric {
  //char *name;

  /** Function to get metric weight */
  int (* get_weight)(unsigned char, int);

};

extern const struct sdn_metric SDN_METRIC;

#endif //SDN_METRIC_H_