
// #ifndef SDN_CONTROLLER_NODE
extern uint8_t sdn_seq_no;
#define INCREMENT_SEQ_NO(packet_ptr, queue_ptr) \
    if(sdnaddr_cmp(&SDN_HEADER(packet_ptr)->source, &sdn_node_addr) == SDN_EQUAL) { \
      SDN_HEADER(packet_ptr)->seq_no = sdn_seq_no++; \
      if (queue_ptr != NULL) SDN_HEADER(queue_ptr)->seq_no = SDN_HEADER(packet_ptr)->seq_no; \
    }
// #else
// #define INCREMENT_SEQ_NO(queue_ptr, packet_ptr)
// #endif

#define ALLOC_AND_ENQUEUE(queue_ptr, packet_ptr, packet_len, queue_time) \
  queue_ptr = sdn_packetbuf_pool_get(); \
  \
  if (queue_ptr == NULL) { \
    SDN_DEBUG_ERROR ("SDN packetbuf pool is empty.\n"); \
  } else { \
    memcpy(queue_ptr, packet_ptr, packet_len); \
    INCREMENT_SEQ_NO(packet_ptr, queue_ptr); \
    if (sdn_send_queue_enqueue((uint8_t *) queue_ptr, packet_len, queue_time) != SDN_SUCCESS) { \
      sdn_packetbuf_pool_put((struct sdn_packetbuf *)queue_ptr); \
      SDN_DEBUG_ERROR ("Error on packet enqueue.\n"); \
    } else { \
      sdn_send_down_once(); \
    } \
  }

#define ENQUEUE_AND_SEND(packet_ptr, packet_len, queue_time) \
  INCREMENT_SEQ_NO(packet_ptr, NULL); \
  if (sdn_send_queue_enqueue((uint8_t *) packet_ptr, packet_len, queue_time) != SDN_SUCCESS) { \
    sdn_packetbuf_pool_put((struct sdn_packetbuf *)packet_ptr); \
    SDN_DEBUG_ERROR ("Error on packet enqueue.\n"); \
    return SDN_ERROR; \
  } else { \
    SDN_METRIC_TX(packet_ptr); \
    sdn_send_down_once(); \
    return SDN_SUCCESS; \
  }
