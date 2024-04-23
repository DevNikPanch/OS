/**
 * Driver.c
 *
 * Schedule is in the format
 *
 *  [name] [priority] [CPU burst]
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "task.h"
#include "list.h"
#include "schedulers.h"

#define SIZE    100

int main(int argc, char *argv[])
{
    FILE *in;
    char *temp;
    char task[SIZE];

    char *name;
    int priority;
    int burst;

    int countTasks = 0;

    in = fopen(argv[1],"r");

    while (fgets(task,SIZE,in) != NULL) {
        temp = strdup(task);
        name = strsep(&temp,",");
        priority = atoi(strsep(&temp,","));
        burst = atoi(strsep(&temp,","));

        // add the task to the scheduler's list of tasks
        add(name,priority,burst);

        free(temp);

        countTasks++;
    }

    fclose(in);

    Timer timer = {0, 0, 0};

    // invoke the scheduler
    schedule(&timer);

    float revTime = (float)timer.revTime / countTasks;
    float responseTime = (float)timer.responseTime / countTasks;
    float waitingTime = (float)timer.waitingTime / countTasks;

    printf("Turnaround time: %.3f\n", revTime);
    printf("Response time: %.3f\n", responseTime);
    printf("Waiting time: %.3f\n", waitingTime);

    return 0;
}