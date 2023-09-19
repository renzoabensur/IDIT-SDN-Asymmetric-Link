#include <stdio.h>

#define DIGRAPH_VERTICE_ID_SIZE 4

#include "digraph.h"
#include "dijkstra.h"

/* Main function for testing purpose only */
int main() {

  printf("\nBeginning test program for digraph.\n");

  int repeat = 1;

  unsigned char *vertice_id_1 = (unsigned char []) {0, 0, 0, 1};
  unsigned char *vertice_id_2 = (unsigned char []) {0, 0, 0, 2};
  unsigned char *vertice_id_3 = (unsigned char []) {0, 0, 0, 3};
  unsigned char *vertice_id_4 = (unsigned char []) {0, 0, 0, 4};
  unsigned char *vertice_id_5 = (unsigned char []) {0, 0, 0, 5};
  unsigned char *vertice_id_6 = (unsigned char []) {0, 0, 0, 6};

  for(int index = 0; index < repeat; index++) {

    printf("\nTest: Adding Vertice {0, 0, 0, 1} (new vertice)\n\nResult:\n");
    vertice_ptr vertice1 = digraph_get_vertice(vertice_id_1);

    printf("\nTest: Adding Vertice {0, 0, 0, 1} (vertice already exists)\n\nResult:\n");
    vertice1 = digraph_get_vertice(vertice_id_1);

    printf("\nTest: Adding Vertice {0, 0, 0, 2}\n\nResult:\n");
    vertice_ptr vertice2 = digraph_get_vertice(vertice_id_2);

    printf("\nTest: adding edge {0, 0, 0, 1} -> {0, 0, 0, 2} [30]\n\nResult:\n");
    edge_ptr edge = digraph_add_edge(vertice1, vertice2, 30);

    printf("\nTest: adding edge {0, 0, 0, 1} -> {0, 0, 0, 2} [50] (update)\n\nResult:\n");
    edge = digraph_add_edge(vertice1, vertice2, 50);

    printf("\nTest: removing edges from Vertice {0, 0, 0, 2} (but there is no edges)\n\nResult:\n");
    digraph_del_edges_from(vertice2);

    printf("\nTest: removing edges from Vertice {0, 0, 0, 1} (there is 1 edge)\n\nResult:\n");
    digraph_del_edges_from(vertice1);

    printf("\nTest: adding edge {0, 0, 0, 1} -> {0, 0, 0, 2} [30]\n\nResult:\n");
    edge = digraph_add_edge(vertice1, vertice2, 30);

    printf("\nTest: Adding Vertice {0, 0, 0, 3} (new vertice)\n\nResult:\n");
    vertice_ptr vertice3 = digraph_get_vertice(vertice_id_3);

    printf("\nTest: adding edge {0, 0, 0, 1} -> {0, 0, 0, 3} [50]\n\nResult:\n");
    edge = digraph_add_edge(vertice1, vertice3, 50);

    printf("\nTest: updating edge {0, 0, 0, 1} -> {0, 0, 0, 2} [40]\n\nResult:\n");
    edge = digraph_add_edge(vertice1, vertice2, 40);

    printf("\nTest: removing edges from Vertice {0, 0, 0, 1} (There are 2 edges)\n\nResult:\n");
    digraph_del_edges_from(vertice1);

    printf("\nTest: adding edge {0, 0, 0, 2} -> {0, 0, 0, 1} [60]\n\nResult:\n");
    edge = digraph_add_edge(vertice2, vertice1, 60);

    printf("\nTest: adding edge {0, 0, 0, 1} -> {0, 0, 0, 3} [50]\n\nResult:\n");
    edge = digraph_add_edge(vertice1, vertice3, 50);

    printf("\nTest: updating edge {0, 0, 0, 1} -> {0, 0, 0, 2} [40]\n\nResult:\n");
    edge = digraph_add_edge(vertice1, vertice2, 40);

    printf("\nTest: removing vertice {0, 0, 0, 2}\n\nResult:\n");
    digraph_del_vertice(vertice2);

    printf("\nTest: Adding Vertice {0, 0, 0, 2}\n\nResult:\n");
    vertice2 = digraph_get_vertice(vertice_id_2);

    printf("\nTest: Adding Vertice {0, 0, 0, 4}\n\nResult:\n");
    vertice_ptr vertice4 = digraph_get_vertice(vertice_id_4);

    printf("\nTest: adding edge {0, 0, 0, 2} -> {0, 0, 0, 1} [25]\n\nResult:\n");
    edge = digraph_add_edge(vertice2, vertice1, 25);

    printf("\nTest: adding edge {0, 0, 0, 2} -> {0, 0, 0, 3} [25]\n\nResult:\n");
    edge = digraph_add_edge(vertice2, vertice3, 25);

    printf("\nTest: adding edge {0, 0, 0, 2} -> {0, 0, 0, 4} [25]\n\nResult:\n");
    edge = digraph_add_edge(vertice2, vertice4, 25);

    printf("\nTest: removing edge {0, 0, 0, 2} -> {0, 0, 0, 3} (there is 3 edges on vertice 1)\n\nResult:\n");
    digraph_del_edge(vertice2, vertice3);

    printf("\nTest: Destroying Digraph\n\nResult:\n");
    digraph_free();

    printf("\nTest: Adding Vertices {0, 0, 0, 1}, {0, 0, 0, 2}, {0, 0, 0, 3}, {0, 0, 0, 4}\n\nResult:\n");
    vertice1 = digraph_get_vertice(vertice_id_1);
    vertice2 = digraph_get_vertice(vertice_id_2);
    vertice3 = digraph_get_vertice(vertice_id_3);
    vertice4 = digraph_get_vertice(vertice_id_4);

    printf("\nTest: adding edges for all vertices [25]\n\nResult:\n");
    edge = digraph_add_edge(vertice1, vertice2, 25);
    edge = digraph_add_edge(vertice2, vertice1, 25);

    edge = digraph_add_edge(vertice1, vertice3, 25);
    edge = digraph_add_edge(vertice3, vertice1, 25);

    edge = digraph_add_edge(vertice1, vertice4, 25);
    edge = digraph_add_edge(vertice4, vertice1, 25);

    edge = digraph_add_edge(vertice2, vertice3, 25);
    edge = digraph_add_edge(vertice3, vertice2, 25);

    edge = digraph_add_edge(vertice2, vertice4, 25);
    edge = digraph_add_edge(vertice4, vertice2, 25);

    edge = digraph_add_edge(vertice3, vertice4, 25);
    edge = digraph_add_edge(vertice4, vertice3, 25);

    printf("\nTest: Destroying Digraph\n\nResult:\n");
    digraph_free();

/* Test marking edges to delete */
    vertice1 = digraph_get_vertice(vertice_id_1);
    vertice2 = digraph_get_vertice(vertice_id_2);
    vertice3 = digraph_get_vertice(vertice_id_3);
    vertice4 = digraph_get_vertice(vertice_id_4);

    printf("\nTest: adding edge {0, 0, 0, 1} -> {0, 0, 0, 2} [30]\n\nResult:\n");
    edge = digraph_add_edge(vertice1, vertice2, 30);

    printf("\nTest: marking edges to delete from {0, 0, 0, 1}\n\nResult:\n");
    digraph_mark_edges_to_del_from(vertice1);

    printf("\nTest: delete edges marked to delete from {0, 0, 0, 1}\n\nResult:\n");
    digraph_del_marked_edges_to_del_from(vertice1);

// ####
    printf("\nTest: adding edge {0, 0, 0, 1} -> {0, 0, 0, 2} [30]\n\nResult:\n");
    edge = digraph_add_edge(vertice1, vertice2, 30);

    printf("\nTest: adding edge {0, 0, 0, 1} -> {0, 0, 0, 3} [30] (update)\n\nResult:\n");
    edge = digraph_add_edge(vertice1, vertice3, 30);

    printf("\nTest: marking edges to delete from {0, 0, 0, 1}\n\nResult:\n");
    digraph_mark_edges_to_del_from(vertice1);

    printf("\nTest: updating edge {0, 0, 0, 1} -> {0, 0, 0, 3} [30] (update)\n\nResult:\n");
    edge = digraph_add_edge(vertice1, vertice3, 30);

    printf("\nTest: delete edges marked to delete from {0, 0, 0, 1}\n\nResult:\n");
    digraph_del_marked_edges_to_del_from(vertice1);

    printf("\nTest: marking edges to delete from {0, 0, 0, 1}\n\nResult:\n");
    digraph_mark_edges_to_del_from(vertice1);

    printf("\nTest: delete edges marked to delete from {0, 0, 0, 1}\n\nResult:\n");
    digraph_del_marked_edges_to_del_from(vertice1);

    printf("\nTest: adding edge {0, 0, 0, 1} -> {0, 0, 0, 2} [30]\n\nResult:\n");
    edge = digraph_add_edge(vertice1, vertice2, 30);

    printf("\nTest: adding edge {0, 0, 0, 1} -> {0, 0, 0, 3} [30] (update)\n\nResult:\n");
    edge = digraph_add_edge(vertice1, vertice3, 30);

    printf("\nTest: adding edge {0, 0, 0, 1} -> {0, 0, 0, 4} [30] (update)\n\nResult:\n");
    edge = digraph_add_edge(vertice1, vertice4, 30);

    printf("\nTest: marking edges to delete from {0, 0, 0, 1}\n\nResult:\n");
    digraph_mark_edges_to_del_from(vertice1);

    printf("\nTest: adding edge {0, 0, 0, 1} -> {0, 0, 0, 2} [30]\n\nResult:\n");
    edge = digraph_add_edge(vertice1, vertice2, 30);

    printf("\nTest: adding edge {0, 0, 0, 1} -> {0, 0, 0, 3} [30] (update)\n\nResult:\n");
    edge = digraph_add_edge(vertice1, vertice3, 30);

    printf("\nTest: delete edges marked to delete from {0, 0, 0, 1}\n\nResult:\n");
    digraph_del_marked_edges_to_del_from(vertice1);

    printf("\nTest: marking edges to delete from {0, 0, 0, 1}\n\nResult:\n");
    digraph_mark_edges_to_del_from(vertice1);

    printf("\nTest: adding edge {0, 0, 0, 1} -> {0, 0, 0, 3} [30] (update)\n\nResult:\n");
    edge = digraph_add_edge(vertice1, vertice3, 30);

    printf("\nTest: delete edges marked to delete from {0, 0, 0, 1}\n\nResult:\n");
    digraph_del_marked_edges_to_del_from(vertice1);

    printf("\nTest: marking edges to delete from {0, 0, 0, 1}\n\nResult:\n");
    digraph_mark_edges_to_del_from(vertice1);

    printf("\nTest: delete edges marked to delete from {0, 0, 0, 1}\n\nResult:\n");
    digraph_del_marked_edges_to_del_from(vertice1);

/* End of the test marking edges to delete */

/* Test marking edges to delete (towards)*/
    vertice1 = digraph_get_vertice(vertice_id_1);
    vertice2 = digraph_get_vertice(vertice_id_2);
    vertice3 = digraph_get_vertice(vertice_id_3);
    vertice4 = digraph_get_vertice(vertice_id_4);

    printf("\nTest: adding edge {0, 0, 0, 2} -> {0, 0, 0, 1} [30]\n");
    edge = digraph_add_edge(vertice2, vertice1, 30);
    digraph_print();

    printf("\nTest: marking edges towards {0, 0, 0, 1} to delete\n");
    digraph_mark_edges_to_del_towards(vertice1);
    digraph_print();

    printf("\nTest: delete edges towards {0, 0, 0, 1} marked to delete\n");
    digraph_del_marked_edges_to_del_towards(vertice1);
    digraph_print();

    printf("\nTest: adding edge {0, 0, 0, 2} -> {0, 0, 0, 1} [30]\n");
    edge = digraph_add_edge(vertice2, vertice1, 30);

    printf("\nTest: adding edge {0, 0, 0, 3} -> {0, 0, 0, 1} [30]\n");
    edge = digraph_add_edge(vertice3, vertice1, 30);
    digraph_print();

    printf("\nTest: marking edges towards {0, 0, 0, 1} to delete\n");
    digraph_mark_edges_to_del_towards(vertice1);
    digraph_print();

    printf("\nTest: updating edge {0, 0, 0, 3} -> {0, 0, 0, 1} [30] (update)\n");
    edge = digraph_add_edge(vertice3, vertice1, 30);
    digraph_print();

    printf("\nTest: delete edges marked to delete from {0, 0, 0, 1}\n");
    digraph_del_marked_edges_to_del_towards(vertice1);
    digraph_print();

    printf("\nTest: marking edges to delete from {0, 0, 0, 1}\n");
    digraph_mark_edges_to_del_towards(vertice1);

    printf("\nTest: delete edges marked to delete from {0, 0, 0, 1}\n");
    digraph_del_marked_edges_to_del_towards(vertice1);

    printf("\nTest: adding edge {0, 0, 0, 2} -> {0, 0, 0, 1} [30]\n");
    edge = digraph_add_edge(vertice2, vertice1, 30);

    printf("\nTest: adding edge {0, 0, 0, 3} -> {0, 0, 0, 1} [30]\n");
    edge = digraph_add_edge(vertice3, vertice1, 30);

    printf("\nTest: adding edge {0, 0, 0, 4} -> {0, 0, 0, 1} [30]\n");
    edge = digraph_add_edge(vertice4, vertice1, 30);

    printf("\nTest: marking edges to delete from {0, 0, 0, 1}\n");
    digraph_mark_edges_to_del_towards(vertice1);

    printf("\nTest: adding edge {0, 0, 0, 1} -> {0, 0, 0, 2} [30]\n");
    edge = digraph_add_edge(vertice1, vertice2, 30);

    printf("\nTest: adding edge {0, 0, 0, 3} -> {0, 0, 0, 1} [30] (update)\n");
    edge = digraph_add_edge(vertice3, vertice1, 30);

    printf("\nTest: delete edges marked to delete from {0, 0, 0, 1}\n");
    digraph_del_marked_edges_to_del_towards(vertice1);
    digraph_print();

    printf("\nTest: marking edges to delete from {0, 0, 0, 1}\n");
    digraph_mark_edges_to_del_towards(vertice1);

    printf("\nTest: adding edge {0, 0, 0, 3} -> {0, 0, 0, 1} [30] (update)\n");
    edge = digraph_add_edge(vertice3, vertice1, 30);

    printf("\nTest: delete edges marked to delete from {0, 0, 0, 1}\n");
    digraph_del_marked_edges_to_del_towards(vertice1);

    printf("\nTest: marking edges to delete from {0, 0, 0, 1}\n");
    digraph_mark_edges_to_del_towards(vertice1);

    printf("\nTest: delete edges marked to delete from {0, 0, 0, 1}\n");
    digraph_del_marked_edges_to_del_towards(vertice1);

/* End of the test marking edges to delete (towards) */

    printf("\nTest: Adding Vertices {0, 0, 0, 1}, {0, 0, 0, 2}, {0, 0, 0, 3}, {0, 0, 0, 4}\n\nResult:\n");
    vertice1 = digraph_get_vertice(vertice_id_1);
    vertice2 = digraph_get_vertice(vertice_id_2);
    vertice3 = digraph_get_vertice(vertice_id_3);
    // vertice4 = digraph_get_vertice(vertice_id_4);

    printf("\nTest: adding edges for vertices [25]\n\nResult:\n");
    edge = digraph_add_edge(vertice1, vertice2, 25);
    edge = digraph_add_edge(vertice2, vertice1, 25);

    // edge = digraph_add_edge(vertice1, vertice3, 25);
    edge = digraph_add_edge(vertice3, vertice1, 25);

    // edge = digraph_add_edge(vertice1, vertice4, 25);
    // edge = digraph_add_edge(vertice4, vertice1, 25);

    edge = digraph_add_edge(vertice2, vertice3, 25);
    edge = digraph_add_edge(vertice3, vertice2, 25);

    // edge = digraph_add_edge(vertice2, vertice4, 25);
    // edge = digraph_add_edge(vertice4, vertice2, 25);

    // edge = digraph_add_edge(vertice3, vertice4, 25);
    // edge = digraph_add_edge(vertice4, vertice3, 25);

    printf("\nTest: Finding route {0, 0, 0, 1} -> {0, 0, 0, 4}\n\nResult:\n");
    route_ptr route_head = dijkstra_shortest_path(vertice_id_1, vertice_id_4);

    route_ptr route = route_head;

    if(route == NULL) {
        printf("Route not found.\n");
    } else {
      while (route != NULL) {
        digraph_print_vertice_id(route->vertice_id);
        printf("(%d) <- ", route->route_weight);
        route = route->next;
      }

      printf("\n");

      dijkstra_free_route(route_head);
    }

    printf("\nTest: Destroying Digraph\n\nResult:\n");
    digraph_free();

    printf("\nTest: Adding Vertices {0, 0, 0, 1}, {0, 0, 0, 2}, {0, 0, 0, 3}, {0, 0, 0, 4}\n\nResult:\n");
    vertice1 = digraph_get_vertice(vertice_id_1);
    vertice2 = digraph_get_vertice(vertice_id_2);
    vertice3 = digraph_get_vertice(vertice_id_3);
    vertice4 = digraph_get_vertice(vertice_id_4);

    printf("\nTest: adding edges for vertices\n\nResult:\n");
    edge = digraph_add_edge(vertice1, vertice2, 1);
    // edge = digraph_add_edge(vertice2, vertice1, 25);

    edge = digraph_add_edge(vertice1, vertice3, 3);
    // edge = digraph_add_edge(vertice3, vertice1, 25);

    // edge = digraph_add_edge(vertice1, vertice4, 25);
    // edge = digraph_add_edge(vertice4, vertice1, 25);

    edge = digraph_add_edge(vertice2, vertice3, 1);
    // edge = digraph_add_edge(vertice3, vertice2, 25);

    edge = digraph_add_edge(vertice2, vertice4, 4);
    // edge = digraph_add_edge(vertice4, vertice2, 25);

    edge = digraph_add_edge(vertice3, vertice4, 1);
    // edge = digraph_add_edge(vertice4, vertice3, 25);

    printf("\nTest: Finding route {0, 0, 0, 1} -> {0, 0, 0, 4}\n\nResult:\n");
    route_head = dijkstra_shortest_path(vertice_id_1, vertice_id_4);

    route = route_head;

    if(route == NULL) {
        printf("Route not found.\n");
    } else {
      while (route != NULL) {
        digraph_print_vertice_id(route->vertice_id);
        printf("(%d) <- ", route->route_weight);
        route = route->next;
      }

      printf("\n");

      dijkstra_free_route(route_head);
    }


    printf("\nTest: Finding route {0, 0, 0, 3} -> {0, 0, 0, 1}\n\nResult:\n");
    route_head = dijkstra_shortest_path(vertice_id_3, vertice_id_1);

    route = route_head;

    if(route == NULL) {
        printf("Route not found.\n");
    } else {
      while (route != NULL) {
        digraph_print_vertice_id(route->vertice_id);
        printf("(%d) <- ", route->route_weight);
        route = route->next;
      }

      printf("\n");

      dijkstra_free_route(route_head);
    }

    printf("\nTest: Finding route {0, 0, 0, 1} -> {0, 0, 0, 4} (again)\n\nResult:\n");
    route_head = dijkstra_shortest_path(vertice_id_1, vertice_id_4);

    route = route_head;

    if(route == NULL) {
        printf("Route not found.\n");
    } else {
      while (route != NULL) {
        digraph_print_vertice_id(route->vertice_id);
        printf("(%d) <- ", route->route_weight);
        route = route->next;
      }

      printf("\n");

      dijkstra_free_route(route_head);
    }

    printf("\nTest: Destroying Digraph\n\nResult:\n");
    digraph_free();

    printf("\nTest: Adding Vertices {0, 0, 0, 1} to {0, 0, 0, 6}\n\nResult:\n");
    /* source: https://pt.wikipedia.org/wiki/Algoritmo_de_Dijkstra#/media/File:Dijkstra_Animation.gif */
    vertice1 = digraph_get_vertice(vertice_id_1);
    vertice2 = digraph_get_vertice(vertice_id_2);
    vertice3 = digraph_get_vertice(vertice_id_3);
    vertice4 = digraph_get_vertice(vertice_id_4);
    vertice_ptr vertice5 = digraph_get_vertice(vertice_id_5);
    vertice_ptr vertice6 = digraph_get_vertice(vertice_id_6);

    printf("\nTest: adding edges for vertices\n\nResult:\n");
    edge = digraph_add_edge(vertice1, vertice2, 7);
    edge = digraph_add_edge(vertice1, vertice3, 9);
    edge = digraph_add_edge(vertice1, vertice6, 14);
    edge = digraph_add_edge(vertice2, vertice3, 10);
    edge = digraph_add_edge(vertice2, vertice4, 15);
    edge = digraph_add_edge(vertice3, vertice4, 11);
    edge = digraph_add_edge(vertice3, vertice6, 2);
    edge = digraph_add_edge(vertice6, vertice5, 9);
    edge = digraph_add_edge(vertice4, vertice5, 6);

    edge = digraph_add_edge(vertice2, vertice1, 7);
    edge = digraph_add_edge(vertice3, vertice1, 9);
    edge = digraph_add_edge(vertice6, vertice1, 14);
    edge = digraph_add_edge(vertice3, vertice2, 10);
    edge = digraph_add_edge(vertice4, vertice2, 15);
    edge = digraph_add_edge(vertice4, vertice3, 11);
    edge = digraph_add_edge(vertice6, vertice3, 2);
    edge = digraph_add_edge(vertice5, vertice6, 9);
    edge = digraph_add_edge(vertice5, vertice4, 6);

    printf("\nTest: Finding route {0, 0, 0, 1} -> {0, 0, 0, 5} expected (1->3->6->5 [20])\n\nResult:\n");
    route_head = dijkstra_shortest_path(vertice_id_1, vertice_id_5);

    route = route_head;

    if(route == NULL) {
        printf("Route not found.\n");
    } else {
      while (route != NULL) {
        digraph_print_vertice_id(route->vertice_id);
        printf("(%d) <- ", route->route_weight);
        route = route->next;
      }

      printf("\n");

      dijkstra_free_route(route_head);
    }

    printf("\nTest: Destroying Digraph\n\nResult:\n");
    digraph_free();

  }

  printf("\nEnding test program for digraph.\n\n");

  return 0;
}
