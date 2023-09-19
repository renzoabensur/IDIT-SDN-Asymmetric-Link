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

#ifndef SERIALCONNECTOR_H
#define SERIALCONNECTOR_H

#include <QObject>
#include <QIODevice>

#include "serializer.h"

class SerialConnector : public QObject
{
    Q_OBJECT
public:
    explicit SerialConnector(QObject *parent = 0);
    SerialConnector(const QString& serial_port, QObject *parent = 0);
    SerialConnector(const QString& ip, quint16 port, QObject* parent = 0);
    ~SerialConnector();

    void initConnection(const QString &ip, quint16 port);
    void initConnection(const QString& serial_port);
    void close();
    bool ready();
    void requestNodeId();

    static sdn_serial_packet_t* castToPacketType(QByteArray &packet) {
        return (sdn_serial_packet_t*)packet.data();
    }

    void sendGenericPacket(sdn_serial_packet_t *packet);

signals:

    void state_changed_stringfy(QString state);
    void nodeIdReceived(sdnaddr_t *nodeId);
    void printfString(QString string);
    void defaultPacket(QByteArray packet);
    void customPacket(QByteArray packet);
    void ackReceived(int status);
    void nackReceived(int status);

protected:

    void decodeSerialPacket(sdn_serial_packet_t *packet);
    void data_received();
    void decodePacket(sdn_serial_packet_t *packet);

private:

    Serializer serializer;
    QIODevice *device = nullptr;
    bool isReady = false;

};

#endif // SERIALCONNECTOR_H
