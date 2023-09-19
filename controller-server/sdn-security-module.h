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
 *         Gustavo A. N. S <gustavoalonso.nunez@usp.br>
 */

/**
 * \addtogroup SDN
 * @{
 * \addtogroup SDN-controller
 * @{
 * \addtogroup sdn-serial security
 * @{
 *
 *
*/

#ifndef SDN_SECURITY_MOD
#define SDN_SECURITY_MOD

#include "sdn-addr.h"

void security_copy_topo_graph();

void security_process_node_alarm(sdnaddr_t *source, sdnaddr_t *attacker_candidate);

int security_attacker_ident(sdnaddr_t *attacker_candidate);

int security_find_suspect(sdnaddr_t *attacker_candidate);

void security_node_to_blocklist(sdnaddr_t *attacker);

int security_find_node_in_blocklist(sdnaddr_t *attacker);

int security_print_list_suspects();

int security_print_blocklist();

///////////////////////////////////////////////////////////////////////

// void security_process_global_ts(int metric_id, float metric_sample);

int security_ts_construction(int metric_id, float metric_sample);

void security_global_count_update(int metric_id);

void security_sum_q1(uint16_t k_counter, int metric_id);

void security_sum_q2(uint16_t k_counter, int metric_id);

void security_variance_ts(int v_counter, int metric_id);

uint16_t security_cp_detector(int metric_id, uint16_t i_s, uint16_t M, float gamma, float sig_lvl);

int security_global_attack_identifier(int metric_id);

double security_critical_val(float gamma, float sig_lvl);

#ifdef __cplusplus
extern "C" {
#endif
void sdn_security_sample();
#ifdef __cplusplus
}
#endif

#endif