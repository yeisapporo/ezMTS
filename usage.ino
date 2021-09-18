/////////////////////////////////////////////////////////////////////////
// Usage of ezMTS(Easy Multi-tasking System) for ATMEGA-based Arduino.
// Compatible with Arduino Nano.
// Copyright (c) 2021 Kazuteru Yamada(yeisapporo). All rights reserved.
/////////////////////////////////////////////////////////////////////////
#include <Arduino.h>
// include the header file below.
#include <ezMTS.hpp>
// specify the number of tasks you need.
ezMTS task(8);
// define variables to retain your tasks' IDs.
int taskIdA;
int taskIdB;
int taskIdC;

// you can define your tasks like these.
int led1(void *dummy) {
    digitalWrite(A3, !digitalRead(A3));
    return 0;
}
int led2(void *dummy) {
    digitalWrite(A4, !digitalRead(A4));
    return 0;
}
int led3(void *dummy) {
    digitalWrite(A5, !digitalRead(A5));
    return 0;
}
void setup() {
  pinMode(A3, OUTPUT);
  pinMode(A4, OUTPUT);
  pinMode(A5, OUTPUT);

  // create your tasks.
  taskIdA = task.create(led1);
  taskIdB = task.create(led2);
  taskIdC = task.create(led3);

  // start your tasks. arg1:task id, arg2:timeout value(ms),
  // arg3:when first executes the function rgistered(EZMTS_TIMEDOUT / EZMTS_AT_ONCE).
  task.start(taskIdA, 3000, EZMTS_AT_ONCE);
  task.start(taskIdB, 2000, EZMTS_AT_ONCE);
  task.start(taskIdC, 1000, EZMTS_AT_ONCE);
}
void loop() {
  delay(1);
}
// other functions in ezMTS.
// stop the task temporarily.
// you can restart the task again by calling xxx.start(task_id).
// xxx.stop(task_id)
// delete the task permanently.
// xxx.del(task_id)
