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

// Micropython REPL with multiple I/O channels

#include "mprepl.h"
#include <set>
extern "C" {
#include "py/runtime.h"
#include "py/mphal.h"
#include "lib/utils/pyexec.h"
#include <stdarg.h>
}

Mutex _lock_mutex;

#define LOGF(...) do {_lock_mutex.lock(); printf(__VA_ARGS__); _lock_mutex.unlock();} while (0)

//extern "C" {
//void LOGF(f, ...) {
//    va_args ap;
//    _lock_mutex.lock();
//    va_start(ap, f);
//    vprintf(f, ap);
//    va_end(ap);
//    _lock_mutex.unlock();
//}
//}

class MpReplClient {
public:
    MpReplClient(mprepl_outfunc_t *cb, void *han) :
        output_cb(cb),
        handle(han) {}

    mprepl_outfunc_t *output_cb;
    void *handle;
};
    
class MpRepl {
public:
    set<MpReplClient *> clients;
    Mutex clients_mutex;
    Queue<void, 20> input_queue;
};

// Singleton
MpRepl _repl;

/*-----------------------------------------------------------------------
 *      Send a character to the REPL, which may have come from serial
 *      console or from a network client.
 *-----------------------------------------------------------------------*/
void
mprepl_input_char(char chr) {
    _repl.input_queue.put((void *)(int)chr);
}

// void mprepl_reset(Mprepl *);

void
mprepl_add_client(mprepl_outfunc_t *output_cb, void *handle) {
    MpReplClient *client = new MpReplClient(output_cb, handle);
    _repl.clients_mutex.lock();
    _repl.clients.insert(client);
    _repl.clients_mutex.unlock();
}

void
mprepl_remove_client(mprepl_outfunc_t *output_cb, void *handle) {
    _repl.clients_mutex.lock();
    for (set<MpReplClient *>::iterator it = _repl.clients.begin();
         it != _repl.clients.end();
         ++it) {
        MpReplClient *client = *it;
        if (client->output_cb == output_cb &&
            client->handle == handle) {
            _repl.clients.erase(client);
            break;
        }
    }
    _repl.clients_mutex.unlock();
}

//-----------------------------------------------------------------------
//      serial client (C hal API)
//-----------------------------------------------------------------------
static void
_mprepl_serial_output(void *handle, const char *str, size_t len) {
    serial_t *serial = (serial_t *)handle;
    while (len-- > 0) {
        char c = *str++;
        while (! serial_writable(serial)) {
            Thread::yield();
        }
        serial_putc(serial, (int)c);
    }
}

static void
_mprepl_serial_input(serial_t *serial) {
    while (true) {
        while (! serial_readable(serial)) {
            Thread::yield();
        }
        int c = serial_getc(serial);
        _repl.input_queue.put((void *)c);
    }
}

void
mprepl_add_serial(serial_t *serial) {
    mprepl_add_client(_mprepl_serial_output, serial);
    Thread *serial_thread = new Thread();
    serial_thread->start(Callback<void()>(serial, _mprepl_serial_input));
}

//-----------------------------------------------------------------------
//      Serial client
//-----------------------------------------------------------------------
static void
_mprepl_Serial_output(void *handle, const char *str, size_t len) {
    Serial *serial = (Serial *)handle;
    serial->printf("%.*s", len, str);
}

static void
_mprepl_Serial_input(Serial *serial) {
    while (true) {
        int c = serial->getc();
        _repl.input_queue.put((void *)c);
    }
}

void
mprepl_add_Serial(Serial &serial) {
    mprepl_add_client(_mprepl_Serial_output, serial);
    Thread *serial_thread = new Thread();
    serial_thread->start(Callback<void()>(&serial, _mprepl_Serial_input));
}

//-----------------------------------------------------------------------
//      TCPSocket client
//-----------------------------------------------------------------------
static void
_mprepl_TCPSocket_output(void *handle, const char *str, size_t len) {
    TCPSocket *socket = (TCPSocket *)handle;
    LOGF("send(%u)...\r\n", len);
    int ret = socket->send(str, len);
    LOGF("send = %d\r\n", ret);
}

static void
_mprepl_TCPSocket_input(TCPSocket *socket) {
    while (true) {
        char c;
        LOGF("recv...\r\n");
        int ret = socket->recv(&c, 1);
        LOGF("recv=%d\r\n", ret);
        if (ret == 1) {
            _repl.input_queue.put((void *)(int)c);
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
    socket_thread->start(Callback<void()>(&sock, _mprepl_TCPSocket_input));
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
        srv_thread->start(Callback<void()>(srv, _mprepl_TCPServer_run));
    }
}

int
mprepl_run(void) {
    while (1) {
        int ret;
        if (pyexec_mode_kind == PYEXEC_MODE_FRIENDLY_REPL) {
            ret = pyexec_friendly_repl();
        } else {
            ret = pyexec_raw_repl();
        }
        if (ret == PYEXEC_FORCED_EXIT) {
            mp_hal_stdout_tx_strn("FORCED EXIT\r\n", 13);
            break;
        } else if (ret != 0) {
            mp_hal_stdout_tx_strn("EXIT\r\n", 13);
            break;
        }
    }
    return 0;
}

// Implement the micropython HAL I/O functions
extern "C" void mp_hal_stdout_tx_chr(int c);
void mp_hal_stdout_tx_chr(int chr) {
    char c = chr;
    _repl.clients_mutex.lock();
    for (set<MpReplClient *>::iterator it = _repl.clients.begin();
         it != _repl.clients.end();
         ++it) {
        MpReplClient *client = *it;
        client->output_cb(client->handle, &c, 1);
    }
    _repl.clients_mutex.unlock();
}

extern "C" int mp_hal_stdin_rx_chr(void);
int mp_hal_stdin_rx_chr(void) {
    osEvent event = _repl.input_queue.get();
    if (event.status & osEventMessage) {
        int c = event.value.v;
        return c;
    } else {
        // ser_printf(&pc, "input_queue status=%#x\n", event.status);
        return -1;
    }
}
