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
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "sdn-security-module.h"
#include "sdn-constants.h"
#include "digraph.h"
#include "sdn-debug.h"
#include "sdn-addr.h"
#include "sdn-metrics-cal.h"

#define D_THRESHOLD 0.5
#define CTRL_PCKTS_OVHD 100
#define DATA_PCKTS_DR 200
#define FDFF 10
#define FNI 20


#ifdef DIST_DETECT
struct struct_suspect{
  sdnaddr_t suspect_id;
  // number of times this suspect was reported 
  uint8_t alarms;
};

typedef struct struct_suspect suspect_t;

struct struct_attacker{
  sdnaddr_t attacker_id;
  // if state == 1, the node was banned
  // if state == 0, the node was not banned yet
  uint8_t state;
};

typedef struct struct_attacker attacker_t;

// uint8_t suspect_flag = 0;
// number of suspects
uint16_t num_suspects = 0;
// number of suspects classified as attackers
uint16_t num_attackers = 0;
// pointer to the list of suspects
suspect_t *sus_list;
// pointer to the list of attackers
attacker_t *att_list;

void security_copy_topo_graph(){

}

//modify management packet to include the suspect address
void security_process_node_alarm(sdnaddr_t *source, sdnaddr_t *attacker_candidate){
  // First alarm received, first memory allocation
  // Copy the attacker id into the struct and ++ the alarm counter	
  if(num_suspects == 0){
  	sus_list = (suspect_t *)malloc(sizeof(suspect_t));
  	if(sus_list != NULL){
  	  sdnaddr_copy(&sus_list->suspect_id, attacker_candidate);
  	  sus_list->alarms = 1;
  	  // suspect_flag = 1;
  	  num_suspects++;
      SDN_DEBUG("First suspect: ");
      sdnaddr_print(attacker_candidate);
      SDN_DEBUG("\n");      
  	}
  }else{ // ++ new alarms and memory reallocation  
    // check it the suspect was reported before, if so, alarms++
    // if not, include the new suspect
    int sus_index;
    sus_index = security_find_suspect(attacker_candidate);
    if(sus_index != -1){
      sus_list[sus_index].alarms++;
      SDN_DEBUG("New alarm: ");
      sdnaddr_print(attacker_candidate);
      SDN_DEBUG(", Alarms: %u", sus_list[sus_index].alarms)
      SDN_DEBUG("\n");  
    }else{
      num_suspects++;
      sus_list = (suspect_t *)realloc(sus_list, num_suspects*sizeof(suspect_t));
      if(sus_list != NULL){
        sdnaddr_copy(&sus_list[num_suspects-1].suspect_id, attacker_candidate);
        sus_list[num_suspects-1].alarms = 1;
        SDN_DEBUG("New suspect: ");
        sdnaddr_print(attacker_candidate);
        SDN_DEBUG("\n");
      }
    }   	
  }
  SDN_DEBUG("Suspects: %u\n", num_suspects);
  security_print_list_suspects();
  // Check if the suspect is classified as attacker 
  // If security_attacker_ident(attacker_candidate) == 1, the node is classified as attacker
  if(security_attacker_ident(attacker_candidate) == 1){
    // If the node is not in the blocklist, include it in it
    if(security_find_node_in_blocklist(attacker_candidate) == -1){
      // include attacker to a blocklist
      security_node_to_blocklist(attacker_candidate);
      SDN_DEBUG("Attacker indentified: ");
      sdnaddr_print(attacker_candidate);
      SDN_DEBUG("\n");
    }
  }
}

// Calculates the detection factor and compare it to the detection threshold
int security_attacker_ident(sdnaddr_t *attacker_candidate){
  int total_neighbors;
  uint8_t suspect_alarms;
  total_neighbors = digraph_count_edges((unsigned char*)attacker_candidate);
  SDN_DEBUG("Total neighbors: %d\n", total_neighbors);
  if(total_neighbors != 0) {
    float detec_th;
    suspect_alarms = sus_list[security_find_suspect(attacker_candidate)].alarms;
    SDN_DEBUG("Suspect alarms: %u\n", suspect_alarms);
    detec_th = (float)suspect_alarms/(float)total_neighbors;
    if(detec_th >= D_THRESHOLD) {
      return 1;
    }else{
      return 0;
    } 	
  }else{
  	return 0;
  }
}

// This function return the index of the suspect in the struct, in case it is in
// if ret == NULL, the node is not in the suspect's list
int security_find_suspect(sdnaddr_t *attacker_candidate) {
  int ret = -1;
  uint16_t candidates;
  for(candidates = 0; candidates < num_suspects; candidates++){
    if(sdnaddr_cmp(&sus_list[candidates].suspect_id, attacker_candidate) == SDN_EQUAL){
      SDN_DEBUG("Suspect found: ")
      sdnaddr_print(attacker_candidate);
      SDN_DEBUG("\n")
      return candidates;
    }
  }
  SDN_DEBUG("Suspect not found\n")
  return ret;
}

// This function includes a node classified as attacker into the blocklist
void security_node_to_blocklist(sdnaddr_t *attacker){
  // if num_attackers == 0, this means this is the first attacker detected
  if(num_attackers == 0){
  	att_list = (attacker_t *)malloc(sizeof(attacker_t));
  	sdnaddr_copy(&att_list->attacker_id, attacker);
  	att_list->state = 0;
  	num_attackers = 1;
  }else{ // if num_attackers != 0, this is not the first attacker thus we reallocate memory for it
    num_attackers++;
    att_list = (attacker_t *)realloc(att_list, num_attackers*sizeof(attacker_t));
    sdnaddr_copy(&att_list[num_attackers-1].attacker_id, attacker);
    att_list[num_attackers-1].state = 0;
  }
  sdnaddr_print(attacker);
  SDN_DEBUG(" included in Blocklist\n");
  security_print_blocklist();
}

// This function return the index of the attacker in the struct, in case it is in
// if ret == -1, the node is not in the attacker's list
int security_find_node_in_blocklist(sdnaddr_t *attacker){
  int ret = -1;
  uint16_t att_index;
  for(att_index = 0; att_index < num_attackers; att_index++){
    SDN_DEBUG("Comparing attackers: ");
    sdnaddr_print(&att_list[att_index].attacker_id);
    SDN_DEBUG(" and ");
    sdnaddr_print(attacker);
    SDN_DEBUG("\n");
    if(sdnaddr_cmp(&att_list[att_index].attacker_id, attacker) == SDN_EQUAL){
      sdnaddr_print(attacker);
      SDN_DEBUG(" found in Blocklist\n");
      return att_index;
    }
  }
  SDN_DEBUG("Attacker not found\n")
  return ret;
}

// Print the list of suspects and the number of alarms reported 
int security_print_list_suspects(){
  if(num_suspects == 0){
    printf("No suspects listed\n");
    return 0;
  }
  printf("------Suspects' list------\n");
  int suspects_index;
  for(suspects_index = 0; suspects_index < num_suspects; suspects_index++){
    sdnaddr_print(&sus_list[suspects_index].suspect_id);
    printf("--------- Alarms: %u\n", sus_list[suspects_index].alarms);
  }
  return 1;
}

// Print the blocklist and informs if the node is already banned or not
int security_print_blocklist(){
  if(num_attackers == 0){
    printf("No attackers listed\n");
    return 0;
  }
  printf("------Attackers' list------\n");
  int att_index;
  for(att_index = 0; att_index < num_attackers; att_index++){
    sdnaddr_print(&att_list[att_index].attacker_id);
    if(att_list[att_index].state == 1){
      printf("--------- Banned: YES\n");
    }else{
      printf("--------- Banned: NO\n");  
    }
  }
  return 1;  
}
#endif


//////////////////////////
/*Centralized detection*/
/////////////////////////

// #define M_WIN 205
#define S_SKIP 10
#define M_WIN 210
float GAMMA = 0.45;
float S_VAL = 0.95;

#ifdef CENTR_DETECT
float *ctrl_overhead;
uint16_t ctrl_overhead_counter = 0;
float *data_pckt_dr;
uint16_t data_pckt_dr_counter = 0;

// Variables to store the summatories of each time series
// variables with _2 are for storing the summatory after the monitoring window
float sum_ctrl_overhead;
float sum_ctrl_overhead_2;
float sum_data_pckt_dr;
float sum_data_pckt_dr_2;

// Variable to store the variance in the time series
float Var_ctrl_overhead;
float Var_data_packt_dr;


// stores the current sample number
uint16_t global_count = 0;

// void security_process_global_ts(int metric_id, float metric_sample){
//   if(security_ts_construction(metric_id, metric_sample) == 1){
//     security_global_count_update(metric_id);
//     if(global_count == M_WIN){
//       security_sum_q1(global_count, metric_id);
//       security_variance_ts(global_count, metric_id);
//     }else if(global_count > M_WIN){
//       security_sum_q2(global_count, metric_id);
//       security_cp_detector(metric_id, global_count-(M_WIN-1), M_WIN, GAMMA, S_VAL);
//     }
//   }
// }

void sdn_security_sample(){
  #ifdef CTRL_OV_DETECT
  if(security_ts_construction(CTRL_PCKTS_OVHD, (float)sdn_cal_control_overhead()) == 1){
    security_global_count_update(CTRL_PCKTS_OVHD);
    if(global_count == M_WIN){
      printf("Sample %u: ", global_count);
      security_print_ts(CTRL_PCKTS_OVHD);
      security_sum_q1(global_count, CTRL_PCKTS_OVHD);
      security_variance_ts(global_count, CTRL_PCKTS_OVHD);
    }else if(global_count > M_WIN){
      printf("Sample %u: ", global_count);
      security_print_ts(CTRL_PCKTS_OVHD);
      security_sum_q2(global_count, CTRL_PCKTS_OVHD);
      if(security_cp_detector(CTRL_PCKTS_OVHD, global_count-M_WIN, M_WIN-S_SKIP, GAMMA, S_VAL) != 0){
        security_global_attack_identifier(CTRL_PCKTS_OVHD);
      }
    }   
  }
  #endif

  #ifdef DATA_DR_DETECT
  if(security_ts_construction(DATA_PCKTS_DR, ret_data_dr()) == 1){
    security_global_count_update(DATA_PCKTS_DR);
    if(global_count == M_WIN){
      printf("Sample %u: ", global_count);
      security_print_ts(DATA_PCKTS_DR);
      security_sum_q1(global_count, DATA_PCKTS_DR);
      security_variance_ts(global_count, DATA_PCKTS_DR);
    }else if(global_count > M_WIN){
      printf("Sample %u: ", global_count);
      security_print_ts(DATA_PCKTS_DR);
      security_sum_q2(global_count, DATA_PCKTS_DR);
      if(security_cp_detector(DATA_PCKTS_DR, global_count-M_WIN, M_WIN-S_SKIP, GAMMA, S_VAL) != 0){
        security_global_attack_identifier(DATA_PCKTS_DR);  
      }
    }   
  }  
  #endif
}

void security_global_count_update(int metric_id){
  if(metric_id == CTRL_PCKTS_OVHD){
    global_count = ctrl_overhead_counter;
  }else if(metric_id == DATA_PCKTS_DR){
    global_count = data_pckt_dr_counter;
  }
}

int security_ts_construction(int metric_id, float metric_sample){
  if(metric_id == CTRL_PCKTS_OVHD){
  	if(ctrl_overhead_counter == 0){
      ctrl_overhead = (float *)malloc(sizeof(float));
    }else if(ctrl_overhead_counter > 0){
      ctrl_overhead = (float *)realloc(ctrl_overhead, (ctrl_overhead_counter + 1)*sizeof(float));
    }
    ctrl_overhead[ctrl_overhead_counter] = metric_sample;
    ctrl_overhead_counter++;
    return 1;  
  }else if(metric_id == DATA_PCKTS_DR){
  	if(data_pckt_dr_counter == 0){
      data_pckt_dr = (float *)malloc(sizeof(float));
    }else if(data_pckt_dr_counter > 0){
      data_pckt_dr = (float *)realloc(data_pckt_dr, (data_pckt_dr_counter + 1)*sizeof(float));
    }
    data_pckt_dr[data_pckt_dr_counter] = metric_sample;
    data_pckt_dr_counter++;
    return 1;
  }else{
  	printf("Invalid metric id\n");
    return 0;
  }
}

void security_sum_q1(uint16_t k_counter, int metric_id){
  uint16_t k, sum_counter;
  if(metric_id == CTRL_PCKTS_OVHD){
    for(k = S_SKIP; k < k_counter; k++){
      sum_ctrl_overhead += ctrl_overhead[k];
      sum_counter++;
    }
    printf("Control packets overhead summatory in %u: %.2f\n", sum_counter, sum_ctrl_overhead);
    sum_counter = 0; 
  }else if(metric_id == DATA_PCKTS_DR){
    for(k = S_SKIP; k < k_counter; k++){
      sum_data_pckt_dr += data_pckt_dr[k];
      sum_counter++;
    }
    printf("Data packets dr summatory in %u:: %.2f\n", sum_counter, sum_data_pckt_dr);
    sum_counter = 0; 
  }else{
  	printf("Invalid metric id\n");
  }
  // start in k = 1 to discard the first sample because it represents the total time of 1 hour
}

// summatory of monitoring window

void security_sum_q2(uint16_t k_counter, int metric_id){
  if(metric_id == CTRL_PCKTS_OVHD){
    printf("Testing sum 2 = %.2f\n", ctrl_overhead[k_counter-1]);
    sum_ctrl_overhead_2 += ctrl_overhead[k_counter-1];
  }else if(metric_id == DATA_PCKTS_DR){
    sum_data_pckt_dr_2 += data_pckt_dr[k_counter-1];
  }else{
  	printf("Invalid metric id\n");
  }
}

// Calculate the variance of the time series

void security_variance_ts(int v_counter, int metric_id){
  float tot_q1, Var;
  float *training;
  int var_counter = 0;
  if(metric_id == CTRL_PCKTS_OVHD){
    tot_q1 = sum_ctrl_overhead;	
    training = ctrl_overhead;
  }else if(DATA_PCKTS_DR){
  	tot_q1 = sum_data_pckt_dr;
    training = data_pckt_dr;
  }
  float average = tot_q1/(float)(v_counter-S_SKIP);
  printf("Average = %.2f\n", average);
  float sum_var = 0.0;
  int v;
  // start in v = S_SKIP-1 to discard the first samples because it represents the total time of 1 hour
  for(v = S_SKIP; v < v_counter; v++){
    sum_var += (float)(pow((training[v] - average), 2));
    var_counter++; 
  }
  Var = sum_var/(v_counter-S_SKIP);
  if(metric_id == CTRL_PCKTS_OVHD){
    Var_ctrl_overhead = Var;
    printf("V counter = %d and Var counter = %d\n", v_counter, var_counter);
    printf("Variance control packets overhead: %0.2f\n", Var_ctrl_overhead); 
  }else if(DATA_PCKTS_DR){
    Var_data_packt_dr = Var;
    printf("Variance data packets delivery rate: %0.2f\n", Var_data_packt_dr);
    printf("V counter = %d and Var counter = %d\n", v_counter, var_counter); 
  }
  var_counter = 0;
} 

// Determine if there is a change point

uint16_t security_cp_detector(int metric_id, uint16_t i_s, uint16_t M, float gamma, float sig_lvl){
  printf("Starting CP detection: ");
  double sum_tot_q2;
  double sum_tot_q1;
  double Var;
  double DT;
  double ca = security_critical_val(gamma, sig_lvl);
  if(metric_id == CTRL_PCKTS_OVHD){
    printf("Control packets\n");
    sum_tot_q1 = (double)sum_ctrl_overhead;
    sum_tot_q2 = (double)sum_ctrl_overhead_2;
    Var = (double)Var_ctrl_overhead; 
    printf("sum_tot_q1= %.2lf sum_tot_q2=%.2lf Var=%.2lf M=%.2lf i_s=%u\n", sum_tot_q1, sum_tot_q2, Var, (double)M, i_s);
  }else if(DATA_PCKTS_DR){
    printf("Delivery rate\n");
    sum_tot_q1 = (double)sum_data_pckt_dr;
    sum_tot_q2 = (double)sum_data_pckt_dr_2;
    Var = (double)Var_data_packt_dr;
    printf("sum_tot_q1= %.2lf sum_tot_q2=%.2lf Var=%.2lf M=%.2lf i_s=%u\n", sum_tot_q1, sum_tot_q2, Var, (double)M, i_s);
  }
  double numerator = fabs((sum_tot_q2-((i_s/(double)M)*sum_tot_q1)));
  double denominator = sqrt((double)M*Var)*(1+((double)i_s/(double)M))*(powf((double)i_s/(double)(i_s+M),(double)gamma)); 
  DT = numerator/denominator;
  // printf("Summatory Q2: %lu\n", sum_tot_q2);
  printf("Numerator: %.2lf\n", numerator);
  printf("Denominator: %.2lf\n", denominator);
  printf("DT: %.2lf ca: %.2lf\n", DT, ca);
  if(DT >= ca){
    printf("CP detected in %u\n", M_WIN + i_s);
    return 1;
  }
  else{
    printf("CP not detected yet in %u\n", M_WIN + i_s);
    return 0;
  }
}

int security_global_attack_identifier(int metric_id){
  int attack_type = 0;
  switch(metric_id){
	case CTRL_PCKTS_OVHD:
	  printf("FDFF attack detected\n");
    attack_type = FDFF;
	  break;
	case DATA_PCKTS_DR:
	  printf("FNI attack detected\n");
    attack_type = FNI;
	  break;
  default:
    printf("Invalid metric id\n");
  }
  return attack_type;
}

double security_critical_val(float gamma, float sig_lvl){
  double ca_r = -1;
  int sig_lvl_100, gamma_100;
  sig_lvl_100 = (int)(sig_lvl*(float)100);
  gamma_100 = (int)(gamma*(float)100);
  printf("GAMMA: %d SIG: %d\n", gamma_100, sig_lvl_100);
  if(sig_lvl_100 == 90){
    if(gamma_100 == 0){
      ca_r = 1.9608;
    }else if(gamma_100 == 15){
      ca_r = 2.0265;
    }else if(gamma_100 == 25){
      ca_r = 2.1157;
    }else if(gamma_100 == 35){
      ca_r = 2.2540;
    }else if(gamma_100 == 45){
      ca_r = 2.5434;
    }else if(gamma_100 == 49){
      ca_r = 3.8522;
    }else{
      printf("Invalid value of gamma\n");  
    } 
  }else if(sig_lvl_100 == 95){
    if(gamma_100 == 0){
      ca_r = 2.2365;
    }else if(gamma_100 == 15){
      ca_r = 2.299;
    }else if(gamma_100 == 25){
      ca_r = 2.3898;
    }else if(gamma_100 == 35){
      ca_r = 2.5118;
    }else if(gamma_100 == 45){
      ca_r = 2.7907;
    }else if(gamma_100 == 49){
      ca_r = 3.1049;
    }else{
      printf("Invalid value of gamma\n");
    }
  }else if(sig_lvl_100 == 99){
    if(gamma_100 == 0){
      ca_r = 2.82;
    }else if(gamma_100 == 15){
      ca_r = 2.8878;
    }else if(gamma_100 == 25){
      ca_r = 2.9293;
    }else if(gamma_100 == 35){
      ca_r = 3.058;
    }else if(gamma_100 == 45){
      ca_r = 3.305;
    }else if(gamma_100 == 49){
      ca_r = 3.6027;
    }else{
      printf("Invalid value of gamma\n");  
    }
  }else{
    printf("SIG not valid\n");
  }
  return ca_r;
}

void security_print_ts(int metric_id){
  uint16_t print_counter, print_for;
  float *ts_to_print;
  if(metric_id == CTRL_PCKTS_OVHD){
    print_counter = ctrl_overhead_counter;
    ts_to_print = (float *)malloc(print_counter*sizeof(float));
    memcpy(ts_to_print, ctrl_overhead, print_counter*sizeof(float));
    printf("Control packets overhead time series:\n");
  }else if(metric_id == DATA_PCKTS_DR){
    print_counter = data_pckt_dr_counter;
    ts_to_print = (float *)malloc(print_counter*sizeof(float));
    memcpy(ts_to_print, data_pckt_dr, print_counter*sizeof(float));
    printf("Data packets delivery rate time series\n");
  }
  printf("[ ");
  for(print_for = 0; print_for < print_counter; print_for++){
    printf("%0.2f\t", ts_to_print[print_for]);
  }
  printf(" ]\n");
}

#endif