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

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "sdn-addr.h"
#include "sdn-send-packet.h"
#include "sdn-serial-send.h"
#include "sdn-reliability.h"
#include "sdn-process-packets-controller.h"

#include <QDebug>
#include <QElapsedTimer>
#include <QTimer>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->ipAddress->setText("127.0.0.1");

    connectionTimer.start();
    getIdTimer = new QTimer(this);
    connect(getIdTimer, SIGNAL(timeout()), this, SLOT(on_getNodeButton_clicked()));

    connect(&serialConnector, &SerialConnector::state_changed_stringfy, [this](QString text){
        connectionTimer.restart();
        ui->statusLabel->setText(text);
        ui->connectButton->setEnabled(!serialConnector.ready());
        ui->disconnectButton->setEnabled(serialConnector.ready());
    });

    connect(&serialConnector, &SerialConnector::printfString, [this](QString text) {
//        ui->terminalTextBox->append(QString("%1(ms): %2").arg(connectionTimer.elapsed()).arg(text));
        if (text.contains("\n"))
            ui->terminalTextBox->insertPlainText(QString("%2 %1(ms): ").arg(connectionTimer.elapsed()).arg(text));
        else
            ui->terminalTextBox->insertPlainText(QString("%1").arg(text));

    });

    connect(&serialConnector, &SerialConnector::nodeIdReceived, this, &MainWindow::processNodeIdMessage);
    connect(&serialConnector, &SerialConnector::defaultPacket, this, &MainWindow::decodePacket);

//    connect(&serialConnector, &SerialConnector::ackReceived, [this](){
//         ui->terminalTextBox->append(QString("Received Serial ACK")); //TODO something
//    });

//    connect(&serialConnector, &SerialConnector::nackReceived, [this](){
//        ui->terminalTextBox->append(QString("Received Serial NACK")); //TODO something
//    });

    connect(&serialConnector, &SerialConnector::ackReceived, this, &sdn_send_done);

    connect(&serialConnector, &SerialConnector::nackReceived, this, &sdn_send_done);

    on_connectButton_clicked();

    QWidget::showMinimized ();

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::decodePacket(QByteArray packet)
{
//    qDebug() << "new SDN PACKET received";
    sdn_serial_packet_t *packet_received = (sdn_serial_packet_t*)packet.data();
    controller_receive((uint8_t *)packet_received->payload, (uint16_t)packet_received->header.payload_len);
}

void MainWindow::on_connectButton_clicked()
{
    if(ui->radioButton_ip->isChecked()) {
        serialConnector.initConnection(ui->ipAddress->text(), (quint16)ui->portNumber->value());
    } else {
        serialConnector.initConnection(ui->ipAddress->text());
    }
    getIdTimer->start(1000);
}

void MainWindow::on_disconnectButton_clicked()
{
    serialConnector.close();
}

void MainWindow::on_clsButton_clicked()
{
    ui->terminalTextBox->clear();
}

void MainWindow::on_getNodeButton_clicked()
{
    serialConnector.requestNodeId();
}

void MainWindow::processNodeIdMessage(sdnaddr_t *nodeId)
{
    if(!initDone) {
        memcpy(controller_addr.u8, nodeId, SDNADDR_SIZE);
        controller_init(&controller_addr);
        initDone = true;
        ui->terminalTextBox->setText(QString("Executed controller_init()"));
    }
    getIdTimer->stop();
    ui->terminalTextBox->append(QString("%1(ms): NODE ID REQUEST ANSWER: %2").
                                arg(connectionTimer.elapsed()).arg(*((int *)nodeId)));
}

void MainWindow::sendPacket(unsigned char* packet, unsigned short packet_len, sdnaddr_t to) {
    sdn_serial_packet_t serial_packet;
    serial_packet.header.msg_type = SDN_SERIAL_MSG_TYPE_RADIO;
    serial_packet.header.payload_len = packet_len;
    sdnaddr_copy(&serial_packet.header.node_addr, &to);
    memcpy(serial_packet.payload, packet, packet_len);

    // unsigned int i;
    // for (i=0; i < serial_packet.header.payload_len; i++) {
    //     printf("%02X ", serial_packet.payload[i]);
    // }
    // printf("\n");
    serialConnector.sendGenericPacket(&serial_packet);
}

void MainWindow::sendCustomPacket(unsigned char* packet, unsigned short packet_len, uint8_t custom_type) {
    sdn_serial_packet_t serial_packet;
    serial_packet.header.msg_type = custom_type;
    serial_packet.header.payload_len = packet_len;
    memcpy(serial_packet.payload, packet, packet_len);

    // unsigned int i;
    // for (i=0; i < serial_packet.header.payload_len; i++) {
    //     printf("%02X ", serial_packet.payload[i]);
    // }
    // printf("\n");
    serialConnector.sendGenericPacket(&serial_packet);
}

void MainWindow::on_radioButton_serial_clicked(bool checked)
{
    if (checked) {
        ui->ipAddress->setText("/dev/ttyUSB0");
    }

}
