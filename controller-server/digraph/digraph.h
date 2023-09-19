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

/**
 * \file
 *         ...
 * \author
 *         Doriedson A. G. O. <doriedson@usp.br>
 */

/**
 * \addtogroup SDN
 * @{
 * \addtogroup SDN-controller
 * @{
 * \addtogroup sdn-graph Graph library
 * @{
 *
 *
*/

#ifndef DIGRAPH_H
#define DIGRAPH_H

#ifndef DIGRAPH_VERTICE_ID_SIZE
#error "DIGRAPH_VERTICE_ID_SIZE is not defined."
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef struct vertice *vertice_ptr;
typedef struct edge *edge_ptr;
typedef struct digraph *digraph_ptr;

enum edge_estatus {
  EDGE_NEW = 0,
  EDGE_STATIC = 1,
  EDGE_UPDATED = 2,
  EDGE_DELETED = 3
};

struct vertice {
  unsigned char vertice_id[DIGRAPH_VERTICE_ID_SIZE];
  vertice_ptr back;
  vertice_ptr next;
  edge_ptr edge_header;
  unsigned char energy;
  int visited; // for Dijkstra algorithm
  int route_weight;
  vertice_ptr route_root;
};

struct edge {
  vertice_ptr vertice;
  int etx;
  int weight;
  int status;
  edge_ptr back;
  edge_ptr next;
};

vertice_ptr digraph_get();

void digraph_print_vertice_id(unsigned char *vertice_id);

vertice_ptr digraph_new_vertice(unsigned char *vertice_id);

vertice_ptr digraph_add_vertice(unsigned char *vertice_id);

vertice_ptr digraph_get_vertice(unsigned char *vertice_id);

void digraph_del_vertice(vertice_ptr vertice);

vertice_ptr digraph_find_vertice(unsigned char *vertice_id);

int digraph_count_vertice();

int digraph_update_energy_vertice(vertice_ptr vertice_source, unsigned char energy);

edge_ptr digraph_new_edge(vertice_ptr vertice_to, int etx, int weight);

edge_ptr digraph_add_edge(vertice_ptr vertice_from, vertice_ptr vertice_to, int etx);

void digraph_del_edge(vertice_ptr vertice_from, vertice_ptr vertice_to);

void digraph_del_edges_from(vertice_ptr vertice_from);

void digraph_mark_edges_to_del_from(vertice_ptr vertice_from);

void digraph_mark_edges_to_del_towards(vertice_ptr vertice_towards);

int digraph_del_marked_edges_to_del_from(vertice_ptr vertice_from);

int digraph_del_marked_edges_to_del_towards(vertice_ptr vertice_towards);

void digraph_del_edges_to(vertice_ptr vertice_from);

edge_ptr digraph_find_edge(vertice_ptr vertice_from, vertice_ptr vertice_to);

void digraph_print();

void digraph_free();

int digraph_count_edges(unsigned char *suspect_id);

#ifdef __cplusplus
}
#endif

#endif

/** @} */
/** @} */
/** @} */
