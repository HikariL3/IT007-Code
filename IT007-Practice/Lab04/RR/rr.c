#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

struct Process
{
    int pid;
    int arrivalTime;
    int burstTime;
    int waitingTime, turnaroundTime, startTime;
    int burstTime_remaining;
};

struct Gantt
{
    int pid;     
    int start;   
    int end;     
};

int compareArrivalTime(const void* a, const void* b)
{
    struct Process* pa = (struct Process*)a;
    struct Process* pb = (struct Process*)b;
    return (pa->arrivalTime - pb->arrivalTime);
}

int comparePID(const void* a, const void* b)
{
    struct Process* pa = (struct Process*)a;
    struct Process* pb = (struct Process*)b;
    return (pa->pid - pb->pid);
}

int main()
{
    int n, qt; // qt = quantum time
    struct Process p[100];
    struct Gantt gantt[200]; 
    int iGantt = 0;      

    int q[100], front = -1, last = -1; // Ready queue
    bool visited[100] = { false };     
    int currentTime = 0, completed = 0;
    float avg_tat = 0, avg_wt = 0;

    printf("Enter the number of processes: ");
    scanf("%d", &n);
    printf("Enter the quantum time: ");
    scanf("%d", &qt);

    for (int i = 0; i < n; i++)
    {
        printf("Enter Arrival Time and Burst Time for Process %d: ", i + 1);
        p[i].pid = i + 1;
        scanf("%d %d", &p[i].arrivalTime, &p[i].burstTime);
        p[i].burstTime_remaining = p[i].burstTime;
    }

    qsort(p, n, sizeof(struct Process), compareArrivalTime);

    last++;
    q[last] = 0;
    front = 0;
    visited[0] = true;

    // begin scheduling
    while (completed != n)
    {
        int j = q[front];
        front++;

        // If the process is running for the first time
        if (p[j].burstTime_remaining == p[j].burstTime)
        {
            p[j].startTime = currentTime > p[j].arrivalTime ? currentTime : p[j].arrivalTime;
            currentTime = p[j].startTime;
        }

        // Execute the process
        int executionTime = (p[j].burstTime_remaining > qt) ? qt : p[j].burstTime_remaining;
        gantt[iGantt].pid = p[j].pid;     
        gantt[iGantt].start = currentTime;
        gantt[iGantt].end = currentTime + executionTime;
        iGantt++;

        p[j].burstTime_remaining -= executionTime;
        currentTime += executionTime;

        if (p[j].burstTime_remaining == 0)
        {
            completed++;

            p[j].turnaroundTime = currentTime - p[j].arrivalTime;
            p[j].waitingTime = p[j].turnaroundTime - p[j].burstTime;

            avg_tat += p[j].turnaroundTime;
            avg_wt += p[j].waitingTime;
        }

        for (int i = 1; i < n; i++)
            if (p[i].burstTime_remaining > 0 && p[i].arrivalTime <= currentTime && !visited[i])
            {
                last++;
                q[last] = i;
                visited[i] = true;
            }

        if (p[j].burstTime_remaining > 0)
        {
            last++;
            q[last] = j;
        }

        // If the ready queue is empty, add the next process
        if (front > last)
            for (int i = 1; i < n; i++)
                if (p[i].burstTime_remaining > 0)
                {
                    last++;
                    q[last] = i;
                    visited[i] = true;
                    break;
                }
    }

    // Sort processes by PID for final output
    qsort(p, n, sizeof(struct Process), comparePID);

    // Print process details in a table
    printf("\n===== Round Robin Scheduling =====\n");
    printf("PID | Arrival | Burst | Start | Waiting | Turnaround\n");
    for (int i = 0; i < n; i++)
    {
        printf("%3d | %7d | %5d | %5d | %7d | %10d\n",
               p[i].pid, p[i].arrivalTime, p[i].burstTime,
               p[i].startTime, p[i].waitingTime, p[i].turnaroundTime);
    }
    printf("\n===== Gantt Chart =====\n|");
    for (int i = 0; i < iGantt; i++) 
        printf(" P%-2d |", gantt[i].pid);

    printf("\n");

    printf("0"); 
     for (int i = 0; i < iGantt; i++) 
        printf("%6d", gantt[i].end); 
        
    printf("\n");

    printf("\nAverage Waiting Time: %.2f\n", avg_wt / n);
    printf("Average Turnaround Time: %.2f\n", avg_tat / n);

    return 0;
}
