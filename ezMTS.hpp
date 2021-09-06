/*
ezMTS(Easy Multitasking System) is a library for Arduino with ATMEGA 
Microcontrollers. This provides simple multitasking-like system to your 
Arduino. You can use multiple loop()-like functions on your Arduino 
sketch. This is not an operating system but an extension of an interrupt 
handler for Timer2. You should not use tone() in your sketch when using 
this library.

Copyright (c) 2021 Kazuteru Yamada(yeisapporo).  All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

- Redistributions of source code must retain the above copyright notice,
  this list of conditions and the following disclaimer.
- Redistributions in binary form must reproduce the above copyright notice,
  this list of conditions and the following disclaimer in the documentation
  and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef _HPP_EZMTS_HPP_
#define _HPP_EZMTS_HPP_

#include <Arduino.h>

// when use timer1 for this library, comment out the following #define
#define EZMTS_USE_TIMER2

#define EZMTS_TASK_UNUSED   (0)
#define EZMTS_TASK_STOPPED  (1)
#define EZMTS_TASK_RUNNING  (2)

// task handling information
class taskInfo {
    public:
    int _task_id;
    int _task_state;
    long _timeout_val;
    long _time_rest;
    int (*_cb_func)(void *);
};

int g_task_num = 0;
taskInfo *g_taskInfo = NULL;

class ezMTS {
    private:

    public:
    ezMTS(int task_num) {
        // keep the number of tasks available.
        g_task_num = task_num;
        // get task management area.
        g_taskInfo = new taskInfo[task_num];
        // initialize task management variables.
        for(int i = 0; i < g_task_num; i++) {
            g_taskInfo[i]._task_id = -1;
            g_taskInfo[i]._task_state = EZMTS_TASK_UNUSED;
            g_taskInfo[i]._timeout_val = 0;
            g_taskInfo[i]._time_rest = 0;
            g_taskInfo[i]._cb_func = NULL;
        }
        // settings for ATMEGA microcontroller timer interruption.
#ifndef EZMTS_USE_TIMER2
        // set timer interruption.
        TCCR1A = TCCR1B = 0;
        // set prescaler to clk/64
        TCCR1B |= (1 << WGM12) | (1 << CS11) | (1 << CS10);
        // match every 1ms
        OCR1A = 250 - 1;
        TIMSK1 |= (1 << OCIE1A);
#else
        // set timer interruption.
        TCCR2A = TCCR2B = 0;
        // set mode to CTC.
        TCCR2A |= (1 << WGM21);
        // set prescaler to clk/64
        TCCR2B |= (1 << CS22);
        // match every 1ms
        OCR2A = 250 - 1;
        TIMSK2 |= (1 << OCIE2A);
#endif
    }
    int create(int (*cb_func)(void *)) {
        int ret = -1;
        noInterrupts();
        // search unused task info area
        for(int i = 0; i < g_task_num; i++) {
            if(g_taskInfo[i]._task_state == EZMTS_TASK_UNUSED) {
                g_taskInfo[i]._task_id = i;
                g_taskInfo[i]._task_state = EZMTS_TASK_STOPPED;
                g_taskInfo[i]._timeout_val = 0;
                g_taskInfo[i]._time_rest = 0;
                g_taskInfo[i]._cb_func = cb_func;
                ret = i;
                break;
            }
        }
        noInterrupts();
        return ret;
    }
    int start(int task_id, long timeout_val) {
        int ret = -1;
        noInterrupts();
        if((task_id < 0) || (task_id > g_task_num - 1) || timeout_val < 0) {
            return ret;
        }
        g_taskInfo[task_id]._task_id = task_id;
        g_taskInfo[task_id]._task_state = EZMTS_TASK_RUNNING;
        g_taskInfo[task_id]._timeout_val = timeout_val;
        g_taskInfo[task_id]._time_rest = timeout_val;
        ret = 0;
        interrupts();
        return ret;
    }
    int stop(int task_id) {
        noInterrupts();
        g_taskInfo[task_id]._task_state = EZMTS_TASK_STOPPED;
        g_taskInfo[task_id]._time_rest = 0;
        g_taskInfo[task_id]._timeout_val = 0;
        interrupts();
        return 0;
    }
    int del(int task_id) {
        noInterrupts();
        g_taskInfo[task_id]._task_id = -1;
        g_taskInfo[task_id]._task_state = EZMTS_TASK_UNUSED;
        g_taskInfo[task_id]._time_rest = 0;
        g_taskInfo[task_id]._timeout_val = 0;
        g_taskInfo[task_id]._cb_func = NULL;
        interrupts();
        return 0;
    }
    int handle() {
        return 0;
    }
};

// timer interruption handler.
#ifndef EZMTS_USE_TIMER2
ISR (TIMER1_COMPA_vect) {
#else
ISR (TIMER2_COMPA_vect) {
#endif
    noInterrupts();
    for(int i = 0; i < g_task_num; i++) {
        //if(g_taskInfo[i]._task_id != i) {
        //    break;
        //}
        if(g_taskInfo[i]._task_state == EZMTS_TASK_RUNNING) {
            if(--g_taskInfo[i]._time_rest == 0) {
                //_taskInfo[i]._task_state = EZMTS_TASK_STOPPED;
                g_taskInfo[i]._time_rest = g_taskInfo[i]._timeout_val;
                interrupts();
                g_taskInfo[i]._cb_func(NULL);
                noInterrupts();
            }
        }
    }
    interrupts();
}

#endif
