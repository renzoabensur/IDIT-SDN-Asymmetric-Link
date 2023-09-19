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

#include <stdio.h>
#include <stdlib.h>
#include "sdn-metrics-cal.h"
#include "sdn-debug.h"
#include "sdn-protocol.h"

uint32_t total_data_pckts_tx;
uint32_t total_data_pckts_rx;
uint32_t total_control_pckts_tx;
uint32_t current_data_pckts_tx;
uint32_t current_data_pckts_rx;
uint32_t current_control_pckts_tx;
// static uint16_t sensor_nodes = 0;

float current_data_pckts_dr;

// Updates the number of current control and data packets transmitted

void sdn_process_node_metrics(int metric_id, uint16_t metric_value){
  if(metric_id == SDN_MNGT_CTRL_PCKT_TX){
    current_control_pckts_tx += (uint32_t)metric_value;
    
  }else if(metric_id == SDN_MNGT_METRIC_QTY_DATA){
  	SDN_DEBUG("Processing data packets transmitted update\n");
  	current_data_pckts_tx += (uint32_t)metric_value;
  }
  SDN_DEBUG("Processing control overhead update: %lu\n", current_control_pckts_tx);
}

// When the sink sends an update to the controller, this function updates the total packets counter,
// calculates the data packets delivery rate and reset the current data packets counters 

void sdn_process_sink_metrics(uint32_t metric_value){
  SDN_DEBUG("Processing data packets received update\n");
  current_data_pckts_rx = metric_value;
  total_data_pckts_rx += metric_value;
  current_data_pckts_dr = sdn_cal_data_dr();
  // SDN_DEBUG("Data packets delivery rate = %0.2f\n", current_data_pckts_dr);
  total_data_pckts_tx += current_data_pckts_tx;
  current_data_pckts_tx = 0;
  current_data_pckts_rx = 0;
}

float sdn_cal_data_dr(){
  // float total_expected = 388;
  // float total_expected = 132;
  float total_expected = 222; // 1 data packer per minute monitoring every 60 seconds
  // float total_expected = 121; // 1 data packer per minute monitoring every 30 seconds
  SDN_DEBUG("Current data packets received %0.2f\n", (float)current_data_pckts_rx);
  // SDN_DEBUG("Current data packets transmitted %0.2f\n", (float)current_data_pckts_tx);
  return (100*(float)current_data_pckts_rx/total_expected);
}

uint32_t sdn_cal_control_overhead(){
  total_control_pckts_tx += current_control_pckts_tx;
  uint32_t ts_ctrl_pckts_tx = current_control_pckts_tx;
  SDN_DEBUG("Control overhead = %lu\n", ts_ctrl_pckts_tx);
  SDN_DEBUG("Total overhead = %lu\n", total_control_pckts_tx);
  current_control_pckts_tx = 0;
  return ts_ctrl_pckts_tx;
}

float ret_data_dr(){
  SDN_DEBUG("Data packets delivery rate = %0.2f\n", current_data_pckts_dr);
  return current_data_pckts_dr;
}
