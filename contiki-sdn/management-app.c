#include "contiki.h"
#include <stdio.h>
#include <inttypes.h>
#include "sdn-send-packet.h"
#include "management-app.h"


uint16_t metrics_param_send = 7;

void management_trigger(){
  printf("Triggering message\n");
  sdn_send_data_management(metrics_param_send);

}