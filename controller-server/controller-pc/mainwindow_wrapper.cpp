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
// #include "sdn-debug.h"

MainWindow *my_window = NULL;

static int nnodes;

extern "C" void createMainWindow()
{
    try
    {
        if (!my_window)
            my_window = new MainWindow;
    }
    catch(...)
    {
        fprintf(stderr, "Uhoh, caught an exception, exiting...\n");
        exit(1);
    }
}

extern "C" void callSendPacket(unsigned char* packet, unsigned short packet_len, sdnaddr_t to)
{
    // unsigned int index;

    // for (i=0; i < packet_len; i++) {
    //     printf("%02X ", packet[i]);
    // }
    // printf("\n");

    // May need try/catch here.
    my_window->sendPacket(packet, packet_len, to);
}

extern "C" void callSendCustomPacket(unsigned char* packet, unsigned short packet_len, uint8_t custom_type)
{
    my_window->sendCustomPacket(packet, packet_len, custom_type);
}


extern "C" void sdn_set_cli_nnodes(int nnodesp) {
    nnodes = nnodesp;
}

extern "C" int sdn_get_cli_nnodes() {
    return nnodes;
}

extern "C" void *getMainWindow()
{
    return my_window;
}

extern "C" void destroyMainWindow()
{
    delete my_window;
}
