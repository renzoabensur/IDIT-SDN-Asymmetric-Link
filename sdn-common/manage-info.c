#include "manage-info.h"
#include "sdn-debug.h"
#include "sdn-protocol.h"
#include "contiki.h"
#include <inttypes.h>

clock_time_t time_in, time_queue;

// comment qty and delay from Thamires version
// static uint8_t qty = 0;
// static unsigned long delay= 0;
static uint32_t rx_total_data;

// variables to monitor control packets received and transmitted 
static uint16_t data_tx = 0;
static uint16_t ctrl_rx = 0;
static uint16_t ctrl_tx = 0;
static uint16_t data_rx = 0;

/*-----------------------------*/
static uint32_t total_data_packets = 0;
static uint32_t total_data_received = 0;
// static uint32_t delivery_rate = 0;



/*-----------------------------*/

void total_data(uint32_t data) {

  rx_total_data = data;
}

void manag_update(uint32_t time_in, uint8_t operation) {

  // Comment Thamires version just as it was
  // clock_time_t time_send =  clock_time();

  // if (operation == 1) //if is sending
  //   qty = qty + 1;
  //   delay = delay + (time_send - time_in);

  // if (qty > 50){
  //   qty = 1;
  //   delay = (time_send - time_in);
  // }

  // printf ("Manag Update - delay: %lu, timein: %lu, time send: %lu, qtd: %d \n", delay, time_in, time_send, qty);

  // Operation 1: control packets received
  if (operation == 1){
    data_tx++;
    // printf("Data tx %u\n", data_tx);
  }

  if (operation == 2){
    data_rx++;
    // printf("Data rx %u\n", data_rx);
  }

  // Operation 3: control packets received
  if (operation == 3){
    ctrl_rx++;
  }

  // Operation 4: control packets transmitted
  if (operation == 4){
    ctrl_tx++;
    // printf("Control tx %u\n", ctrl_tx);
  }
}

uint32_t manag_get_info(uint8_t metric_id) {

  uint32_t ret = 0;

// Thamires version
  // if (metric_id == SDN_MNGT_METRIC_QTY_DATA) {

  //   ret = qty;
  //   qty = 0;

  // } else if (metric_id == SDN_MNGT_METRIC_QUEUE_DELAY) {

  //   ret = delay;
  // } else if (metric_id == SDN_MNGT_METRIC_TOTAL_DATA) {

  //   ret = rx_total_data;

  if (metric_id == SDN_MNGT_METRIC_QTY_DATA){
    ret = (uint32_t)data_tx;
    data_tx = 0;
    // printf("Data packets transmitted %lu\n", ret);
  }

  if (metric_id == SDN_MNGT_METRIC_TOTAL_DATA){
    ret = (uint32_t)data_rx;
    data_rx = 0;
    // printf("Data packets received %lu\n", ret);
  }
  
  if (metric_id == SDN_MNGT_CTRL_PCKT_RX){
    ret = (uint32_t)ctrl_rx;
    ctrl_rx = 0;
    // printf("Control packets received %lu\n", ret);
  }

  if (metric_id == SDN_MNGT_CTRL_PCKT_TX){
    ret = (uint32_t)ctrl_tx;
    ctrl_tx = 0;
    printf("Control packets transmitted %lu\n", ret);
  }  
  
  printf("Checking return %lu\n", ret);
  return ret;
  //send info
}


/**************************************************************/
void update_total_sent(uint32_t data_packets) {
  total_data_packets+= data_packets;
  printf("Total data packets = %"PRIu32"\n", total_data_packets);
}

uint8_t update_total_received(uint32_t total_received) {

  total_data_received = total_received;
  if (total_received != NULL) {
    return 1 ; 
  }else{
    return 0;  
  }
  
}

// void delivery_rate_cal() {
//   if (total_data_packets != 0) {
//     delivery_rate = (uint32_t)(((float)total_data_received/(float)total_data_packets)*100);
//     printf("Data packets delivery rate = %"PRIu32"\n", delivery_rate);
//   }
// }
/**************************************************************/
