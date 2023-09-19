#ifndef METRICS_CAL
#define METRICS_CAL
#include "sdn-addr.h"


void sdn_process_sink_metrics(uint32_t metric_value);

float sdn_cal_data_dr();

uint32_t sdn_cal_control_overhead();

float ret_data_dr();

#ifdef __cplusplus
extern "C" {
#endif
void sdn_process_node_metrics(int metric_id, uint16_t metric_value);
#ifdef __cplusplus
}
#endif

#endif