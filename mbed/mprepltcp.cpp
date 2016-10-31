/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2016 Colin Hogben
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

// mbed TCPServer/TCPSocket client for Micropython REPL

#include "mprepl.h"

#ifndef LOGF
#define LOGF(...)
#endif

static void
_mprepl_TCPSocket_output(void *handle, const char *str, size_t len) {
    TCPSocket *socket = (TCPSocket *)handle;
    LOGF("send(%u)...\r\n", len);
    int ret = socket->send(str, len);
    LOGF("send = %d\r\n", ret);
    (void)ret;
}

static void
_mprepl_TCPSocket_input(TCPSocket *socket) {
    while (true) {
        char c;
        LOGF("recv...\r\n");
        int ret = socket->recv(&c, 1);
        LOGF("recv=%d\r\n", ret);
        if (ret == 1) {
            mprepl_input_char(c);
        } else {
            if (ret < 0) {
                // report
            }
            break;
        }
    }
    mprepl_remove_client(&_mprepl_TCPSocket_output, socket);
    delete socket;
}

void
mprepl_add_TCPSocket(TCPSocket &sock) {
    mprepl_add_client(_mprepl_TCPSocket_output, &sock);
    Thread *socket_thread = new Thread();
    socket_thread->start(Callback<void()>(_mprepl_TCPSocket_input, &sock));
}

static void
_mprepl_TCPServer_run(TCPServer *server) {
    LOGF("TCPServer listening...\r\n");
    while (true) {
        TCPSocket *sock = new TCPSocket;
        LOGF("accept...\r\n");
        int err = server->accept(sock);
        if (err < 0) {
            LOGF("accept() failed: error=%d\r\n", err);
            // Report error
        } else {
            LOGF("accept() succeeded\r\n");
            // sock->set_blocking(false);
            sock->send("connected\n", 10);
            mprepl_add_TCPSocket(*sock);
        }
    }
}

void
mprepl_start_TCPServer(NetworkInterface *iface, int port) {
    int err;
    TCPServer *srv = new TCPServer;
    srv->open(iface);
    if ((err = srv->bind(port)) < 0) {
        error("Cannot bind to port %d [error=%d]\n", port, err);
    } else if ((err = srv->listen(1)) < 0) {
        error("Port %d: listen() failed [error=%d]\n", port, err);
    } else {
        Thread *srv_thread = new Thread;
        srv_thread->start(Callback<void()>(_mprepl_TCPServer_run, srv));
    }
}
