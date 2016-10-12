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

#if 0
Mutex _lock_mutex;
#define LOGF(...) do {_lock_mutex.lock(); printf(__VA_ARGS__); _lock_mutex.unlock();} while (0)
#else
#define LOGF(...)
#endif

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
        return -1;
    }
}
