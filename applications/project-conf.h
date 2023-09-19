/*
* Header Here!
*/

/*
 * Page used for otimizing size compilation.
 * https://github.com/contiki-os/contiki/wiki/Reducing-Contiki-OS-firmware-size
 */

/* Disable the name strings from being stored. */
#define PROCESS_CONF_NO_PROCESS_NAMES 1

/* Reducing uIP/IPv6 Stack Size by not using TCP */
#define UIP_CONF_TCP 0

/* Reducing uIP/IPv6 Stack Size by not using UDP */
// #define UIP_CONF_UDP 0

/*
 * The RDC protocols require additional program and RAM to
 * store neighbor information. Reduce the sequence number
 * arrray size used for duplicate packet detection.
 * Turn off phase optimization in contikimac to eliminate
 * the neighbor wake time table
 */
#ifdef NETSTACK_CONF_RDC
#undef NETSTACK_CONF_RDC
#endif

// #define SDN_CONF_ND dummy_sdn_nd
#define SDN_CONF_ND collect_sdn_nd
/*#define SDN_CONF_ND collect_sdn_nd*/
/*#define SDN_CONF_CD null_sdn_cd*/
#define SDN_CONF_CD null_sdn_cd

/* COLLECT_CONF_ANNOUNCEMENTS defines if the Collect implementation
   should use Contiki's announcement primitive to announce its routes
   or if it should use periodic broadcasts. */
//#define COLLECT_CONF_ANNOUNCEMENTS 0

// Need to uncomment this line if "CONTIKI_WITH_RIME = 0" in Makefile
// #undef NETSTACK_CONF_NETWORK
// #define NETSTACK_CONF_NETWORK sdn_network_driver

#define NETSTACK_CONF_RDC nullrdc_driver
#define NULLRDC_CONF_ADDRESS_FILTER 0
//#define NETSTACK_CONF_RDC contikimac_driver

#define SDN_CONF_TX_RELIABILITY 1
#define SDN_CONF_1HOP_RELIABILITY 0

/*************************************************/
/*               DISABLING ACKS                  */
/*************************************************/

// First hardware ACKs are disabled in the CC2420 driver
// #ifdef CC2420_CONF_AUTOACK
// #undef CC2420_CONF_AUTOACK
// #endif
// #define CC2420_CONF_AUTOACK 0

// in case ContikiMAC is used, must inform it that CC2420 autoack is disabled
/*#define RDC_CONF_HARDWARE_ACK 0*/
// and not to send software acks
/*#define CONTIKIMAC_CONF_SEND_SW_ACK 0*/

//#define CONTIKIMAC_CONF_CCA_COUNT_MAX_TX 0
//#define CONTIKIMAC_CONF_WITH_PHASE_OPTIMIZATION 0
//#define WITH_FAST_SLEEP 0
/*#define RDC_CONF_HARDWARE_CSMA 1
// As ContikiMAC will report MAC_TX_NOACK to the mac driver, we need to use
// nullmac_driver instead of csma
#ifdef NETSTACK_CONF_MAC
#undef NETSTACK_CONF_MAC
#endif
#define NETSTACK_CONF_MAC nullmac_driver*/
// the configuration above still sets the PACKETBUF_ATTR_MAC_ACK to 1
// as it is not configurable in ContikiMAC
// However, it seems that the behavior is equivalent to a non-acknowledge TX

// macros to disable ACK in nullrdc
// #define NULLRDC_CONF_802154_AUTOACK_HW 0
// #undef NULLRDC_CONF_802154_AUTOACK
// #define NULLRDC_CONF_802154_AUTOACK 0

/*************************************************/
/*             END OF DISABLING ACKS             */
/*************************************************/

#define NETSTACK_CONF_LLSEC commutator_protocol_driver

// #ifdef NETSTACK_CONF_FRAMER
// #undef NETSTACK_CONF_FRAMER
// #define NETSTACK_CONF_FRAMER  framer_802154
// #endif

/* Size of SDN packetbuf pool. */
#define SDN_PACKETBUF_POOL_SIZE 12
/* Size of SDN send queue. */
#define SDN_SEND_QUEUE_SIZE 10
/* Size of SDN recv queue. */
#define SDN_RECV_QUEUE_SIZE 10

/*#define SDN_CONF_FLOW_TABLES_SIZE 4*/

/* The Max size of the SDN packet is 127 bytes (MTU of 802.15.4) - 11 bytes:
  9 bytes (Header of 802.15.4: 2 bytes frame control field, 2 bytes PAN ID,
     1 byte seq num, 2 bytes src addr, 2 bytes dst addr);
  2 bytes (Footer of 802.15.4: CRC). */
#define SDN_MAX_PACKET_SIZE (127 - 11)
/**
* Source: $(CONTIKI)/core/net/ipv6/uip-nd6.h
*
* The time that a neighbor remains with status REACHABLE.

#ifdef UIP_CONF_ND6_REACHABLE_TIME
#undef UIP_CONF_ND6_REACHABLE_TIME
#endif
#define UIP_CONF_ND6_REACHABLE_TIME	30000
*/
