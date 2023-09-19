/*
 * Copyright (c) 2007, Swedish Institute of Computer Science.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the Institute nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE INSTITUTE AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * This file is part of the Contiki operating system.
 *
 */

/**
 * \file
 *         Header file for hop-by-hop reliable data collection
 * \author
 *         Adam Dunkels <adam@sics.se>
 */

/**
 * \addtogroup adapted Adapted from Contiki
 * @{
 *
 * The collect module implements a hop-by-hop reliable data collection
 * mechanism.
 *
 * \section collect-channels Channels
 *
 * The collect module uses 2 channels; one for neighbor discovery and one
 * for data packets.
 *
 */

#ifndef COLLECTSDN_H_
#define COLLECTSDN_H_

#include "net/rime/announcement.h"
#include "net/rime/runicast.h"
#include "net/rime/neighbor-discovery.h"
#include "net/rime/collect-neighbor.h"
#include "net/rime/packetqueue.h"
#include "sys/ctimer.h"
#include "lib/list.h"
#include "net/rime/collect.h"


/*-----------------------------------------*/
#define NEIGHBOR_CHANGE 0x30
#define PARENT_CHANGE   0X31
/*-----------------------------------------*/




void collect_sdn_open(struct collect_conn *c, uint16_t channels,
                  uint8_t is_router,
                  const struct collect_callbacks *callbacks);
void collect_sdn_close(struct collect_conn *c);

int collect_sdn_send(struct collect_conn *c, int rexmits);

void collect_sdn_set_sink(struct collect_conn *c, int should_be_sink);

int collect_sdn_depth(struct collect_conn *c);
const linkaddr_t *collect_sdn_parent(struct collect_conn *c);

void collect_sdn_set_keepalive(struct collect_conn *c, clock_time_t period);

void collect_sdn_print_stats(void);

//void print_neighbors(struct collect_neighbor_list *neighbors_list);


#endif /* COLLECTSDN_H_ */
/** @} */
