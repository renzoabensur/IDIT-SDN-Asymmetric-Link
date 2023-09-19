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
 */

/**
* \addtogroup SDN-common
* @{
*/

#ifndef SDN_NODE_SEC_H_
#define SDN_NODE_SEC_H_

#include "sdn-addr.h"

// Function to initialize the security process
void security_node_init();

// Function to add the address of a suspect to suspects_list
void security_node_add_suspect(sdnaddr_t suspect);

// Function to determine the suspect candidate to send to the centralized security module
sdnaddr_t *security_node_choose_suspect();

// Function to send an alarm to the centralized security module
void security_node_send_alarm();

// Function to obtain a sample of the transmitting time in ticks
unsigned long security_node_transmitting_time();

// Fuction to obtain a sample of the processing time in ticks
unsigned long security_node_processing_time();

// Function to add a sample to the trainin time series
void security_node_ts_training(uint16_t metric, uint16_t train_counter);

// Function to add a sample to the monitoring time series
void security_node_ts_monitoring(uint16_t metric, uint16_t monitor_counter);

// Function to calculate the cumsum of the training time series
void security_node_sum_q1(uint16_t k_counter);

// Function to calculate the cumsum of the monitoring time series
void security_node_sum_q2(unsigned long metric);

// Function to calculate the variac of a time series
void security_node_variance_ts(unsigned long tot_q1, int v_counter);

// Function to run the change point detector
uint16_t security_node_cp_detector(uint16_t i_s, uint16_t M);

// Moves the samples in the training time series to start a new period and restart the monitoring time series
void security_new_period();

#endif
