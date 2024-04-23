//
// Created by nikita on 21.04.24.
//

#include "stdio.h"
#include "stdlib.h"
#include "task.h"
#include "list.h"
#include "cpu.h"

struct node *head = NULL;
struct node *temp = NULL;

void add(char *name, int priority, int burst) {
    Task *task = malloc(sizeof(Task));
    task->name = name;
    task->priority = priority;
    task->burst = burst;

    insert(&head, task);
}

Task *pick_NextTask() {
    if (temp != NULL) {
        Task *task = temp->task;
        temp = temp->next;
        return task;
    } else {
        return NULL;
    }
}


void schedule(Timer *timer) {
    temp = head;
    int fl_rt = 0;
    int fl_tr = 0;
    int temp_turnaround = 0;
    while(head != NULL) {
        Task *task = pick_NextTask();
        int currentTime = 0;
        while (task != NULL) {
            if (fl_rt == 0)
                timer->responseTime += currentTime;
            if (QUANTUM >= task->burst) {
                run(task, task->burst);
                delete(&head, task);

                currentTime += task->burst;
                timer->revTime += currentTime;
                timer->waitingTime += currentTime - task->burst;
            } else {
                run(task, QUANTUM);
                task->burst -= QUANTUM;

                currentTime += QUANTUM;
                timer->revTime += currentTime;
                timer->waitingTime += currentTime - QUANTUM;
            }
            task = pick_NextTask();
        }
        temp = head;
        if (fl_tr == 0) {
            temp_turnaround = timer->revTime;
            fl_tr = 1;
        }
        fl_rt = 1;
    }
    timer->revTime += temp_turnaround;
}