CC=gcc

CFLAGS=-I. -g
# CFLAGS+= -I../ControllerMote

CFLAGS += -DDIGRAPH_VERTICE_ID_SIZE=4
# To use Glib mutexes, some systems will require both glib-2.0 and gthread-2.0.
# CFLAGS+=`pkg-config --cflags glib-2.0 igraph` -I$(UTHASH_DIR)/src
# CFLAGS+=`pkg-config nopoll --cflags`
# LDLIBS=`pkg-config nopoll --libs glib-2.0 igraph` -lpthread -lm

# CFLAGS+=-DMAX_FLOWIDS=50

# SERIAL_H = ../ControllerMote/SerialMoteMsgFormat.h
MIGFLAGS = -D_POSIX_C_SOURCE

digraph-test: digraph.o dijkstra.o

#SerialReportPacket.c SerialReportPacket.h: $(SERIAL_H)
# mig -o SerialReportPacket.h -c-prefix=spacket_topology c $(SERIAL_H) sf_topology_report_msg $(MIGFLAGS)

#SerialFlowSetupPacket.c SerialFlowSetupPacket.h: $(SERIAL_H)
# mig -o SerialFlowSetupPacket.h -c-prefix=spacket_flow_setup c $(SERIAL_H) sf_generic_flow_setup_msg $(MIGFLAGS)


.PHONY: clean
clean:
	rm *.o digraph-test
