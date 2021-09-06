/////////////////////////////////////////////////////////////////////////
// Usage of ezMTS(Easy Multi-tasking System) for ATMEGA-based Arduino.
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

// you can define your tasks like those.
int led1(void *dummy) {
    digitalWrite(A0, !digitalRead(A0));
    return 0;
}
int led2(void *dummy) {
    digitalWrite(A1, !digitalRead(A1));
    return 0;
}
int led3(void *dummy) {
    digitalWrite(A3, !digitalRead(A3));
    return 0;
}
void setup() {
  pinMode(A0, OUTPUT);
  pinMode(A1, OUTPUT);
  pinMode(A3, OUTPUT);

  // create your tasks.
  taskIdA = task.create(led1);
  taskIdB = task.create(led2);
  taskIdC = task.create(led3);

  // start your tasks. arg1:task id, arg2:timeout value(ms).
  task.start(taskIdA, 50);
  task.start(taskIdB, 30);
  task.start(taskIdC, 10);
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
