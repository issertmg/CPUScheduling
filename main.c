#include <stdio.h>
#include <stdlib.h>

typedef struct Process {
  int pID;
  int arrivalTime;
  int totalExecutionTime;
  int executionTimeLeft;
} process;

typedef struct Output {
  int pID;
  int startEndLength;
  int startTime[500];
  int endTime[500];
  int waitingTime;
  int turnaroundTime;
} output;

//function prototypes
void displayOutput();
void displayProcesses();
void sortProcessesByArrivalTime();
int getIndexOfShortestProcess(int currentTime);
void simulateNSJF();

//global variables
process p[100];
output o[100];
int n;

int main(void) {
  
  //SAMPLE DATA FOR TESTING
  n = 3; 
  p[0].pID = 0; p[0].arrivalTime = 0; p[0].totalExecutionTime = 24; p[0].executionTimeLeft = p[0].totalExecutionTime;
  p[1].pID = 1; p[1].arrivalTime = 0; p[1].totalExecutionTime = 3; p[1].executionTimeLeft = p[1].totalExecutionTime;
  p[2].pID = 2; p[2].arrivalTime = 0; p[2].totalExecutionTime = 3; p[2].executionTimeLeft = p[2].totalExecutionTime;

  displayProcesses();
  simulateNSJF();
  displayOutput();
  
  return 0;
}

void simulateNSJF() {
  sortProcessesByArrivalTime();

  int i, j;
  int currentTime = 0;
  for (i = 0; i < n; i++) {
    int index = getIndexOfShortestProcess(currentTime);
    o[i].pID = p[index].pID;
    o[i].startEndLength = 1;
    o[i].startTime[0] = currentTime;
    o[i].endTime[0] = currentTime + p[index].totalExecutionTime;
    o[i].waitingTime = o[i].startTime[0];
    o[i].turnaroundTime = o[i].waitingTime + p[index].totalExecutionTime;

    currentTime += p[index].totalExecutionTime;
    p[index].executionTimeLeft = 0;
  }
}

int getIndexOfShortestProcess(int currentTime) {
  int shortestExecutionTime = INT_MAX;
  int index;
  int i;
  for (i = 0; i < n; i++) {
    if (p[i].arrivalTime > currentTime) {
      break;
    }
    else if (p[i].executionTimeLeft != 0 && p[i].executionTimeLeft < shortestExecutionTime) {
      shortestExecutionTime = p[i].executionTimeLeft;
      index = i;
    }
  }
  return index;
}

void displayProcesses() {
  int i;
  for (i = 0; i < n; i++) {
    printf("pID: %d, arrival: %d, burst: %d \n", 
          p[i].pID, p[i].arrivalTime, p[i].totalExecutionTime);
  }
}

void sortProcessesByArrivalTime() {
  int i, j;

  for (i = 0; i < n-1; i++) {
    for (j = 0; j < n-i-1; j++) {
      if ( (p[j].totalExecutionTime > p[j+1].totalExecutionTime) ||
           ((p[j].totalExecutionTime == p[j+1].totalExecutionTime) &&
            (p[j].arrivalTime > p[j+1].arrivalTime)) ) {
        process temp = p[j];
        p[j] = p[j+1];
        p[j+1] = temp;
      }
    }
  }
}


void displayOutput() {
  double totalWaitingTime = 0;
  int i, j;
  
  for (i = 0; i < n; i++) {
    printf("P[%d] \n", o[i].pID);

    for (j = 0; j < o[i].startEndLength; j++) {
      printf("Start time: %d \t End time: %d \n", o[i].startTime[j], o[i].endTime[j]);
    }

    printf("Waiting time: %d \n", o[i].waitingTime);
    printf("Turnaround time: %d \n", o[i].turnaroundTime);
    printf("**************************************** \n");
    totalWaitingTime += o[i].waitingTime;
  }

  printf("Average waiting time: %lf \n", totalWaitingTime/n);
}

