#ifndef MANAGE_INFO_H
#define MANAGE_INFO_H

#include <stdint.h>

void manag_update(uint32_t time_in, uint8_t operation);

uint32_t manag_get_info(uint8_t metric_id);

void total_data(uint32_t data);

void delivery_rate_cal();

void update_total_sent(uint32_t data_packets);

uint8_t update_total_received(uint32_t total_received);

#endif //MANAGE_INFO_H
