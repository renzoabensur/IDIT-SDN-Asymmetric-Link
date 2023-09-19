#ifndef SDN_IDS_DATA_
#define SDN_IDS_DATA_

#include "sdn-addr.h"

struct ids_flows_usage_db {
	uint16_t data_times_used;
	uint16_t ctrl_times_used;
	uint16_t flow_request_pckt;
};

void process_ids_data(sdnaddr_t *source, uint16_t data_times);

void process_ids_ctrl(sdnaddr_t *source, uint16_t ctrl_times);

void flow_request_pckt_counter(sdnaddr_t *source);

#endif
