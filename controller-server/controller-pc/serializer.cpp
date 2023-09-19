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

#include "serializer.h"

#include <QByteArray>
#include <QDebug>

/* HDLC Asynchronous framing */
/* The frame boundary octet is 01111110, (7E in hexadecimal notation) */
#define FRAME_BOUNDARY_OCTET 0x7E

/* A "control escape octet", has the bit sequence '01111101', (7D hexadecimal) */
#define CONTROL_ESCAPE_OCTET 0x7D

/* If either of these two octets appears in the transmitted data, an escape octet is sent, */
/* followed by the original data octet with bit 5 inverted */
#define INVERT_OCTET 0x20

Serializer::Serializer(QObject *parent) : QObject(parent)
{
    packetData = new char[sizeof(sdn_serial_packet_t)];
}

Serializer::~Serializer()
{
    delete packetData;
}


/* Wrap given data in HDLC frame and send it out byte at a time*/
QByteArray Serializer::frameDecode(const char *framebuffer, uint8_t frame_length)
{
    uint8_t data=0;
    QByteArray packetBytes;

    packetBytes.append((uint8_t)FRAME_BOUNDARY_OCTET);

    while(frame_length) {

        data = *framebuffer++;

        if((data == CONTROL_ESCAPE_OCTET) || (data == FRAME_BOUNDARY_OCTET))
        {
            packetBytes.append((uint8_t)CONTROL_ESCAPE_OCTET);
            data ^= INVERT_OCTET;
        }
        packetBytes.append(data);
        frame_length--;
    }

    packetBytes.append(FRAME_BOUNDARY_OCTET);
    return packetBytes;
    //serialSocket.write(packetBytes);
}

QByteArray Serializer::packetizer(sdn_serial_packet_t *packet) {
    uint8_t size = sizeof(sdn_serial_packet_header_t) + packet->header.payload_len;
    return frameDecode((const char *)packet,  size);
}

void Serializer::processByte(unsigned char data) {

    /* FRAME FLAG */
    if(data == FRAME_BOUNDARY_OCTET){

        if(escape_character == true){
            escape_character = false;
        }
        else if(overflow) { /* We lost consistence, begin again */
            /* Clear Buffer */
            overflow = false;
            frame_length=0;
        }

        /* If a valid frame is detected> FRAME_BOUNDARY + PACKET MINIMUM SIZE (HEADER), otherwise discard. */
        else if(frame_length >= sizeof(sdn_serial_packet_header_t)) {
            /* Wake up consumer process */
            QByteArray packetBytes(packetData, sizeof(sdn_serial_packet_t));
            emit new_packet_available(packetBytes);
            //qDebug() << "[Serializer::processByte] New Packet arrived";
            frame_length=0;
            return;
        }
        else {
            frame_length=0; /* Synchronization. */
            return;
        }
    }

    if(escape_character){
        escape_character = false;
        data ^= INVERT_OCTET;
    }
    else if(data == CONTROL_ESCAPE_OCTET){
        escape_character = true;
        return;
    }

    if(frame_length < sizeof(sdn_serial_packet_t) && !overflow) {
        char * ptr = (packetData + frame_length);
        *ptr = data;
        frame_length++;
    }

    return;
}

void Serializer::processBytes(const QByteArray& bytes) { /* Not Tested */
    for(char byte : bytes) {
        processByte(byte);
    }
}

void Serializer::processBytes(const char *bytes, int len) {
//    printf("Serializer::processBytes: ");
    for(int i=0; i<len; i++) {
//        printf("%02X ", bytes[i]);
        processByte(bytes[i]);
    }
//    printf("\n");
}
