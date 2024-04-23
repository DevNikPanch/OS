//
// Created by nikita on 21.04.24.
//

#include "stdio.h"
#include "stdlib.h"
#include "task.h"
#include "list.h"
#include "cpu.h"

struct node *head = NULL;

void add(char *name, int priority, int burst) {
    Task *task = malloc(sizeof(Task));
    task->name = name;
    task->priority = priority;
    task->burst = burst;

    insert(&head, task);
}

Task *pick_NextTask() {
    if (head != NULL) {
        Task *min = head->task;
        struct node *curr = head->next;


        while (curr != NULL){
            if (curr->task->priority > min->priority){
                min = curr->task;
            }

            curr = curr->next;
        }
        delete(&head, min);
        return min;
    } else {
        return NULL;
    }
}

void schedule(Timer *timer) {
    Task *task = pick_NextTask();
    int currentTime = 0;

    while (task != NULL) {
        timer->responseTime += currentTime;

        run(task, task->burst);

        currentTime += task->burst;
        timer->revTime += currentTime;

        task = pick_NextTask();
    }
}