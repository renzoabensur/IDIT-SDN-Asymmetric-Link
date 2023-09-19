#include "sdn-protocol.h"
#include "flow-table-tests.h"
#include "control-flow-table.h"
#include "data-flow-table.h"
#include "stdio.h"

uint8_t flow_tables_test() {
  sdnaddr_t addr1, addr2;
  flowid_t f1, f2;
  action_t ac;
  uint8_t ret;
  int max = 10;
  int i;
  char * ptr;

  struct control_flow_entry * cfe;
  struct data_flow_entry * dfe;

  printf("\nflow_tables_test()\n");

  // addr operations

  for (i = 0; i < sizeof(sdnaddr_t); i++) {
    sdnaddr_setbyte(&addr1, i , 0);
    sdnaddr_setbyte(&addr2, i , 0);
  }

  if (sdnaddr_cmp(&addr1, &addr2) != SDN_EQUAL) {
    printf("  Adresses should be equal\n");
  }

  for (i = 0; i < sizeof(sdnaddr_t); i++) {
    sdnaddr_setbyte(&addr1, i , 1);
  }

  if (sdnaddr_cmp(&addr1, &addr2) == SDN_EQUAL) {
    printf("  Adresses should not be equal\n");
  }

  sdnaddr_copy(&addr2, &addr1);

  if (sdnaddr_cmp(&addr1, &addr2) != SDN_EQUAL) {
    printf("  Adresses should be equal (2)\n");
  }

  // flowid operations

  f1 = f2 = 0;

  if (flowid_cmp(&f1, &f2) != SDN_EQUAL) {
    printf("  flowids should be equal\n");
  }

  f1 = 1;

  if (flowid_cmp(&f1, &f2) == SDN_EQUAL) {
    printf("  flowids should not be equal\n");
  }

  flowid_copy(&f2, &f1);

  if (flowid_cmp(&f1, &f2) != SDN_EQUAL) {
    printf("  flowids should be equal (2)\n");
  }

  // control flow only

  printf("  test 1: insert\n");
  for (i = 0; i < sizeof(sdnaddr_t); i++) {
    sdnaddr_setbyte(&addr1, i , 0);
    sdnaddr_setbyte(&addr2, i , 0);
  }
  sdnaddr_setbyte(&addr1, 0, 1);
  sdnaddr_setbyte(&addr2, 0, 2);
  ac = 3;
  ret = sdn_controlflow_insert(addr1, addr2, ac);

  if (ret == SDN_SUCCESS) {
    ptr = (char*) sdn_controlflow_get(addr1);
    if (ptr != NULL) {
      printf("    get OK\n");
    } else {
      printf("    get NOK\n");
    }
  }

  ret = sdn_controlflow_remove(addr1);
  if (ret == SDN_SUCCESS) {
    printf("  remove OK\n");
  } else {
    printf("  remove NOK\n");
  }

  ptr = (char*) sdn_controlflow_get(addr1);
  if (ptr == NULL) {
    printf("  get after remove OK\n");
  } else {
    printf("  get after remove NOK\n");
  }

  for (i = 0; i < max; i++) {
    sdnaddr_setbyte(&addr1, 0, i);
    ret = sdn_controlflow_insert(addr1, addr2, ac);
    if (ret != SDN_SUCCESS) {
      printf("  error in multiple insert\n");
    }
  }
  sdn_controlflow_print();

  ret = sdn_controlflow_insert(addr1, addr2, ac);
  if (ret == SDN_ERROR) {
    printf("  There should not be an error inserting more than max if address is reused\n");
  } else {
    printf("  Test insert existing addr ok\n");
  }

  sdnaddr_setbyte(&addr1, 0, i+1);
  ret = sdn_controlflow_insert(addr1, addr2, ac);
  if (ret == SDN_SUCCESS) {
    printf("  There should be an error inserting more than max\n");
  } else {
    printf("  Table limit test OK\n");
  }

  for (i = 0; i < max; i++) {
    sdnaddr_setbyte(&addr1, 0, i);
    ret = sdn_controlflow_remove(addr1);
    if (ret != SDN_SUCCESS) {
      printf("  error in multiple remove\n");
    }
  }
  printf("  The following print should be empty:\n");
  sdn_controlflow_print();
  ret = sdn_controlflow_insert(addr1, addr2, ac);
  sdnaddr_setbyte(&addr2, 0, 5);
  ret = sdn_controlflow_insert(addr1, addr2, ac+1);
  printf("  The following print should be contain only on entry starting with 90, 50, 04:\n");
  sdn_controlflow_print();

  // data flow only

  for (i = 0; i < sizeof(sdnaddr_t); i++) {
    sdnaddr_setbyte(&addr1, i , 0);
  }

  printf("  test 1: insert\n");
  sdnaddr_setbyte(&addr1, 0, 1);
  f1 = 1;
  ac = 3;
  ret = sdn_dataflow_insert(f1, addr2, ac);

  if (ret == SDN_SUCCESS) {
    ptr = (char*) sdn_dataflow_get(f1);
    if (ptr != NULL) {
      printf("    get OK\n");
    } else {
      printf("    get NOK\n");
    }
  }

  ret = sdn_dataflow_remove(f1);
  if (ret == SDN_SUCCESS) {
    printf("  remove OK\n");
  } else {
    printf("  remove NOK\n");
  }

  ptr = (char*) sdn_dataflow_get(f1);
  if (ptr == NULL) {
    printf("  get after remove OK\n");
  } else {
    printf("  get after remove NOK\n");
  }

  for (i = 0; i < max; i++) {
    sdnaddr_setbyte(&addr1, 0, i);
    f1 = i;
    ret = sdn_dataflow_insert(f1, addr1, ac);
    if (ret != SDN_SUCCESS) {
      printf("  error in multiple insert\n");
    }
  }
  sdn_dataflow_print();

  ret = sdn_dataflow_insert(f1, addr1, ac);
  if (ret == SDN_ERROR) {
    printf("  There should not be an error inserting more than max if address is reused\n");
  } else {
    printf("  Test insert existing addr ok\n");
  }

  f1++;
  ret = sdn_dataflow_insert(f1, addr1, ac);
  if (ret == SDN_SUCCESS) {
    printf("  There should be an error inserting more than max\n");
  } else {
    printf("  Table limit test OK\n");
  }

  for (i = 0; i < max; i++) {
    f1 = i;
    ret = sdn_dataflow_remove(f1);
    if (ret != SDN_SUCCESS) {
      printf("  error in multiple remove\n");
    }
  }
  printf("  The following print should be empty:\n");
  sdn_dataflow_print();
  ret = sdn_dataflow_insert(f1, addr2, ac);
  sdnaddr_setbyte(&addr1, 0, 5);
  ret = sdn_dataflow_insert(f1, addr2, ac+1);
  printf("  The following print should be contain only on entry starting with 90, 50, 04:\n");
  sdn_dataflow_print();

  // mixed allocations of data and control flow

  for (i = 0; i < sizeof(sdnaddr_t); i++) {
    sdnaddr_setbyte(&addr1, i , 0);
    sdnaddr_setbyte(&addr2, i , 0);
  }

  ac = 3;

  for (i = 0; i < max/2; i++) {
    sdnaddr_setbyte(&addr1, 0, i);
    sdnaddr_setbyte(&addr2, 0, i*2);
    f1 = i*2-1;
    ret = sdn_controlflow_insert(addr1, addr2, ac);
    if (ret != SDN_SUCCESS) {
      printf("  error in multiple insert\n");
    }
    ret = sdn_dataflow_insert(f1, addr1, ac);
    if (ret != SDN_SUCCESS) {
      printf("  error in multiple insert\n");
    }
  }

  printf("  testing inserting and removing control and data flow alternately:\n");
  for (i = 0; i < max/2; i++) {
    sdnaddr_setbyte(&addr1, 0, i);
    sdnaddr_setbyte(&addr2, 0, i*2);
    f1 = i*2-1;
    cfe = sdn_controlflow_get(addr1);
    if (cfe == NULL) {
      printf("    error in get: ");
      sdnaddr_print(&addr1);
      printf("\n");
    } else if ( !(sdnaddr_cmp(&addr2, &cfe->next_hop) == SDN_EQUAL && ac == cfe->action) ) {
      printf("    missmatch in getting value from: \n");
      sdnaddr_print(&addr1);
      printf("\n");
    }
    dfe = sdn_dataflow_get(f1);
    if (dfe == NULL) {
      printf("    error in get: ");
      flowid_print(&f1);
      printf("\n");
    } else if ( !(sdnaddr_cmp(&addr1, &dfe->next_hop) == SDN_EQUAL && ac == dfe->action) ) {
      printf("    missmatch in getting value from: \n");
      flowid_print(&f1);
      printf("\n");
    }
  }
  printf("  done.\n");

  for (i = 0; i < max/2; i++) {
    sdnaddr_setbyte(&addr1, 0, i);
    sdnaddr_setbyte(&addr2, 0, i*2);
    f1 = i*2-1;
    ret = sdn_controlflow_remove(addr1);
    if (ret == SDN_ERROR) {
      printf("    error in remove: ");
      sdnaddr_print(&addr1);
      printf("\n");
    }
    ret = sdn_dataflow_remove(f1);
    if (ret == SDN_ERROR) {
      printf("    error in remove: ");
      flowid_print(&f1);
      printf("\n");
    }
  }

  sdn_controlflow_clear();
  sdn_dataflow_clear();

  printf("<end> flow_tables_test()\n\n");

  return 0;
}
