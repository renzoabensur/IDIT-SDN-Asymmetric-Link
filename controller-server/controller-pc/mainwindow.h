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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTcpSocket>
#include <QElapsedTimer>

#include "sdn-serial-packet.h"
#include "serialconnector.h"
#include "controller-core.h"
#include "sdn-addr.h"


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void processByte(unsigned char data);
    void frameDecode(const char *framebuffer, uint8_t frame_length);
    //void sendPacket(sdn_serial_packet_t *packet);
    void sendPacket(unsigned char* packet, unsigned short packet_len, sdnaddr_t to);
    void sendCustomPacket(unsigned char* packet, unsigned short packet_len, uint8_t custom_type);

private slots:

    void on_connectButton_clicked();
    void on_disconnectButton_clicked();
    void on_clsButton_clicked();
    void decodePacket(QByteArray packet);
    void on_getNodeButton_clicked();
    void processNodeIdMessage(sdnaddr_t *nodeId);

    void on_radioButton_serial_clicked(bool checked);

signals:

    void new_packet_available();

private:

    Ui::MainWindow *ui;

    SerialConnector serialConnector;
    QElapsedTimer connectionTimer;
    QTimer *getIdTimer;

    sdnaddr_t controller_addr;// = (sdnaddr_t) {{ 1, 0 }};

    bool initDone=false;


};

#endif // MAINWINDOW_H
