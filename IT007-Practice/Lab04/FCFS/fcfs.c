#include <stdio.h>
#include <stdlib.h>

#define SORT_BY_ARRIVAL 0
#define SORT_BY_PID 1
#define SORT_BY_BURST 2
#define SORT_BY_START 3

typedef struct {
    int iPID;
    int iArrival, iBurst;
    int iStart, iFinish, iWaiting, iResponse, iTaT;
} PCB;

// Function Prototypes
void inputProcess(int n, PCB P[]);
void printProcess(int n, PCB P[]);
void exportGanttChart(int n, PCB P[]);
void pushProcess(int *n, PCB P[], PCB Q);
void removeProcess(int *n, int index, PCB P[]);
int swapProcess(PCB *P, PCB *Q);
int partition(PCB P[], int low, int high, int iCriteria);
void quickSort(PCB P[], int low, int high, int iCriteria);
void calculateAWT(int n, PCB P[]);
void calculateATaT(int n, PCB P[]);

// Function Definitions
void inputProcess(int n, PCB P[]) {
    for (int i = 0; i < n; i++) {
        P[i].iPID = i + 1;
        printf("Enter Arrival Time and Burst Time for Process %d: ", i + 1);
        scanf("%d %d", &P[i].iArrival, &P[i].iBurst);
        P[i].iStart = P[i].iFinish = P[i].iWaiting = P[i].iResponse = P[i].iTaT = 0;
    }
}

void printProcess(int n, PCB P[]) {
    printf("PID | Arrival | Burst | Start | Finish | Response | Waiting | Turnaround\n");
    for (int i = 0; i < n; i++) {
        printf("%3d | %7d | %5d | %5d | %6d | %8d | %7d | %10d\n",
               P[i].iPID, P[i].iArrival, P[i].iBurst,
               P[i].iStart, P[i].iFinish, P[i].iResponse,
               P[i].iWaiting, P[i].iTaT);
    }
}

void exportGanttChart(int n, PCB P[]) {
    printf("\nGantt chart: \n|");
    for (int i = 0; i < n; i++) {
        printf(" P%d |", P[i].iPID);
    }
    printf("\n");
}

void pushProcess(int *n, PCB P[], PCB Q) {
    P[*n] = Q;
    (*n)++;
}

void removeProcess(int *n, int index, PCB P[]) {
    for (int i = index; i < *n - 1; i++) {
        P[i] = P[i + 1];
    }
    (*n)--;
}

int swapProcess(PCB *P, PCB *Q) {
    PCB temp = *P;
    *P = *Q;
    *Q = temp;
    return 0;
}

int partition(PCB P[], int low, int high, int iCriteria) {
    PCB pivot = P[high];
    int i = low - 1;

    for (int j = low; j < high; j++) {
        int condition = (iCriteria == SORT_BY_ARRIVAL && P[j].iArrival <= pivot.iArrival) ||
                        (iCriteria == SORT_BY_PID && P[j].iPID <= pivot.iPID);
        if (condition) {
            i++;
            swapProcess(&P[i], &P[j]);
        }
    }
    swapProcess(&P[i + 1], &P[high]);
    return i + 1;
}

void quickSort(PCB P[], int low, int high, int iCriteria) {
    if (low < high) {
        int pivot = partition(P, low, high, iCriteria);
        quickSort(P, low, pivot - 1, iCriteria);
        quickSort(P, pivot + 1, high, iCriteria);
    }
}

void calculateAWT(int n, PCB P[]) {
    int totalW = 0;
    for (int i = 0; i < n; i++)
        totalW += P[i].iWaiting;
    printf("Average waiting time: %.2f\n", (float)totalW / n);
}

void calculateATaT(int n, PCB P[]) {
    int totalTaT = 0;
    for (int i = 0; i < n; i++)
        totalTaT += P[i].iTaT;
    printf("Average turnaround time: %.2f\n", (float)totalTaT / n);
}

int main() {
    PCB Input[10] = {0};
    PCB ReadyQueue[10] = {0};
    PCB TerminatedArray[10] = {0};
    int iNumberOfProcess;

    printf("Please input number of Process: ");
    scanf("%d", &iNumberOfProcess);

    int iRemain = iNumberOfProcess, iReady = 0, iTerminated = 0;
    inputProcess(iNumberOfProcess, Input);
    quickSort(Input, 0, iNumberOfProcess - 1, SORT_BY_ARRIVAL);

    pushProcess(&iReady, ReadyQueue, Input[0]);
    removeProcess(&iRemain, 0, Input);

    while (iTerminated < iNumberOfProcess) {
        if (iReady > 0) {
            PCB current = ReadyQueue[0];
            removeProcess(&iReady, 0, ReadyQueue);

            current.iStart = (iTerminated == 0) ? current.iArrival : TerminatedArray[iTerminated - 1].iFinish;
            current.iFinish = current.iStart + current.iBurst;
            current.iResponse = current.iStart - current.iArrival;
            current.iWaiting = current.iResponse;
            current.iTaT = current.iFinish - current.iArrival;

            pushProcess(&iTerminated, TerminatedArray, current);

            while (iRemain > 0 && Input[0].iArrival <= current.iFinish) {
                pushProcess(&iReady, ReadyQueue, Input[0]);
                removeProcess(&iRemain, 0, Input);
            }
            quickSort(ReadyQueue, 0, iReady - 1, SORT_BY_ARRIVAL);
        }
    }
    printProcess(iTerminated, TerminatedArray);

    printf("\n===== FCFS Scheduling =====\n");
    exportGanttChart(iTerminated, TerminatedArray);
    quickSort(TerminatedArray, 0, iTerminated - 1, SORT_BY_PID);
    calculateAWT(iTerminated, TerminatedArray);
    calculateATaT(iTerminated, TerminatedArray);
    return 0;
}
