#-------------------------------------------------
#
# Project created by QtCreator 2017-01-30T21:36:37
#
#-------------------------------------------------

QT       += core gui network serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = "controller-pc"
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

DEFINES += SDN_CONTROLLER_PC

DEFINES += DEBUG_SDN

DEFINES += SDNADDR_SIZE=2
DEFINES += DIGRAPH_VERTICE_ID_SIZE=SDNADDR_SIZE

DEFINES += SDN_MAX_PACKET_SIZE=115

DEFINES += SDN_SEND_QUEUE_SIZE=100
DEFINES += SDN_RECV_QUEUE_SIZE=100

DEFINES += SDN_CONF_METRIC=dummy_metric
#dummy_metric | mamdani_fuzzy_model_metric

# Defines when changes from route weight can be used in flow setup.
DEFINES += SDN_ROUTE_SENSIBILITY=0.5

DEFINES += SDN_CONF_TX_RELIABILITY=1
DEFINES += SDN_NEIGHBORINFO_NEIGHBORS_TO_SRC
DEFINES += SDN_NEIGHBORINFO_SRC_TO_NEIGHBORS

DEFINES += SDN_SOURCE_ROUTED
#DEFINES += SDN_INFORM_NEW_EDGE_SERIAL

# routes sensibility related setings
DEFINES += WEIGHT_ETX=1.0
DEFINES += WEIGHT_ENERGY=0.0

#Management flag
DEFINES += MANAGEMENT

#IDS flag
#DEFINES += IDS

##################################
# CP detection, main flag
DEFINES += CP_DETECT
##################################
# Flags for distributed detection
DEFINES += DIST_DETECT
DEFINES += TRANS_TS
# DEFINES += RX_CTRL_PCKT
##################################
# Flags for centralized detection
DEFINES += CENTR_DETECT
DEFINES += CTRL_OV_DETECT
DEFINES += DATA_DR_DETECT
##################################
# Monitoring period
# DEFINES += MONETWSEC
DEFINES += MSIXTYSEC
# DEFINES += MTHIRSEC
# DEFINES += MTENSEC

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

#INCLUDEPATH+=../../

include(controller-pc.pro.contiki)

INCLUDEPATH+=$$CONTIKI/core/
INCLUDEPATH+=../../sdn-common/
INCLUDEPATH+=../
INCLUDEPATH+=../digraph/

SOURCES += main.cpp\
        mainwindow.cpp \
    serializer.cpp \
    serialconnector.cpp \
    ../../sdn-common/control-flow-table.c \
    ../../sdn-common/data-flow-table.c \
    ../../sdn-common/sdn-addr.c \
    ../../sdn-common/sdn-debug.c \
    ../../sdn-common/sdn-packetbuf.c \
    ../../sdn-common/sdn-queue.c \
    ../../sdn-common/sdn-send-packet.c \
    ../../sdn-common/sdn-process-packets.c \
    ../../sdn-common/sdn-unknown-route.c \
    ../../sdn-common/ids-data.c \
    $$CONTIKI/core/lib/list.c \
    ../digraph/digraph.c \
    ../digraph/dummy-metric.c \
    ../digraph/mamdani-fuzzy-model-metric.c \
    ../digraph/dijkstra.c \
    ../controller-core.c \
    ../sdn-flow-table-cache.c \
    ../sdn-serial-send.c \
    ../sdn-process-packets-controller.c \
    ../sdn-reliability.c \
    ../sdn-security-module.c \
    ../sdn-metrics-cal.c \
    mainwindow_wrapper.cpp

HEADERS  += mainwindow.h \
    serializer.h \
    serialconnector.h \
    ../../sdn-common/control-flow-table.h \
    ../../sdn-common/data-flow-table.h \
    ../../sdn-common/sdn-addr.h \
    ../../sdn-common/sdn-constants.h \
    ../../sdn-common/sdn-debug.h \
    ../../sdn-common/sdn-packetbuf.h \
    ../../sdn-common/sdn-protocol.h \
    ../../sdn-common/sdn-queue.h \
    ../../sdn-common/sdn-send-packet.h \
    ../../sdn-common/sdn-process-packets.h \
    ../../sdn-common/sdn-unknown-route.h \
    ../../sdn-common/sdn-serial-packet.h \
    ../../sdn-common/ids-data.h \
    $$CONTIKI/core/lib/list.h \
    ../digraph/digraph.h \
    ../digraph/dummy-metric.h \
    ../digraph/mamdani-fuzzy-model-metric.h \
    ../digraph/dijkstra.h \
    ../digraph/sdn-metric.h \
    ../controller-core.h \
    ../sdn-flow-table-cache.h \
    ../sdn-serial-send.h \
    ../sdn-process-packets-controller.h \
    ../sdn-reliability.h \
    ../sdn-security-module.h \
    ../sdn-metrics-cal.h \
    mainwindow_wrapper.h

FORMS    += mainwindow.ui

DISTFILES += \
    controller-pc.pro.contiki
