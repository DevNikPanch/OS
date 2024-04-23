/**
 * Representation of a task in the system.
 */

#ifndef TASK_H
#define TASK_H

typedef struct {
    int revTime;
    int responseTime;
    int waitingTime;
} Timer;

// representation of a task
typedef struct task {
    char *name;
    int tid;
    int priority;
    int burst;
} Task;

#endif