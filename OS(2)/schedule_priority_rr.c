//
// Created by nikita on 21.04.24.
//

#include "stdio.h"
#include "stdlib.h"
#include "task.h"
#include "list.h"
#include "cpu.h"

struct node *tasksHead = NULL;
struct node *currentTask = NULL;
struct node *unsortedPriorityTasks = NULL;
struct node *sortedPriorityTasks = NULL;

int priority_count[10] = {0};

void add(char *name, int priority, int burst) {
    Task *task = malloc(sizeof(Task));
    task->name = name;
    task->priority = priority;
    task->burst = burst;

    priority_count[priority]++;
    insert(&tasksHead, task);
}

Task *pick_PriorityTask() {
    if (unsortedPriorityTasks == NULL){
        return NULL;
    }
    Task *min = unsortedPriorityTasks->task;
    struct node *curr = unsortedPriorityTasks->next;

    while (curr != NULL) {
        if (curr->task->priority < min->priority)
            min = curr->task;

        curr = curr->next;
    }

    delete(&unsortedPriorityTasks, min);

    return min;
}

Task *pick_NextTask() {
    if (currentTask == NULL)
        return NULL;
    Task *task = currentTask->task;
    currentTask = currentTask->next;
    return task;
}

void schedule_rr(Timer *timer, int *currTime) {
    currentTask = tasksHead;
    int firstRun = 1;
    int tempTurnaround = 0;

    while (tasksHead != NULL) {
        Task *task = pick_NextTask();
        int currentTime = 0;

        while (task != NULL) {
            if (firstRun) {
                timer->responseTime += currentTime;
                firstRun = 0;
            }

            int burstTime = 0;

            if (QUANTUM < task->burst){
                burstTime = QUANTUM;
            }
            else{
                burstTime = task->burst;
            }
            run(task, burstTime);

            task->burst -= burstTime;
            currentTime += burstTime;
            *currTime += burstTime;

            if (task->burst <= 0) {
                delete(&tasksHead, task);
                timer->revTime += currentTime;
                timer->waitingTime += currentTime - task->burst;
            } else {
                timer->waitingTime += QUANTUM;
            }

            task = pick_NextTask();
        }

        currentTask = tasksHead;

        if (tempTurnaround == 0) {
            tempTurnaround = timer->revTime;
        }

        firstRun = 1;
    }

    timer->revTime += tempTurnaround;
}

void schedule(Timer *timer) {
    unsortedPriorityTasks = tasksHead;
    sortedPriorityTasks = NULL;

    Task *taskPriority = pick_PriorityTask();

    while (taskPriority != NULL) {
        insert(&sortedPriorityTasks, taskPriority);
        taskPriority = pick_PriorityTask();
    }

    struct node *curr = sortedPriorityTasks;
    int currentTime = 0;

    while (curr != NULL) {
        Task *task = curr->task;

        if (priority_count[task->priority] > 1) {
            struct node *tempPriority = NULL;

            while (priority_count[task->priority] != 0) {
                Task *taskPriority = curr->task;
                insert(&tempPriority, taskPriority);
                priority_count[task->priority]--;
                curr = curr->next;
            }

            tasksHead = tempPriority;
            schedule_rr(timer, &currentTime);
        } else {
            timer->responseTime += currentTime;

            run(task, task->burst);
            curr = curr->next;

            currentTime += task->burst;
            timer->revTime += currentTime;
            timer->waitingTime += currentTime - task->burst;
        }
    }
}
