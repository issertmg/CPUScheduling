#include <stdio.h>
#include <stdlib.h>

typedef struct Process {
  int pID;
  int arrivalTime;
  int totalExecutionTime;
} process;

typedef struct Time {
  int startTime;
  int endTime;
} time;

typedef struct Output {
  int pID;
  int ptrLength;  //size of dynamic array pointed by *ptr
  time* ptr;  //pointer for dynamic array
  int waitingTime;
  int turnaroundTime;
} output;

//function prototypes
void displayOutput(output *o, int n);
output* simulateNSJF(process *p, int n);
void swap(process *a, process *b);


int main(void) {
  process p[100];
  int n;

  //SAMPLE DATA FOR TESTING
  n = 3;
  p[0].pID = 0;
  p[0].arrivalTime = 0;
  p[0].totalExecutionTime = 24;
  p[1].pID = 1;
  p[1].arrivalTime = 0;
  p[1].totalExecutionTime = 3;
  p[2].pID = 2;
  p[2].arrivalTime = 0;
  p[2].totalExecutionTime = 3;

  output *o = simulateNSJF(p, n);
  displayOutput(o, n);
  
  return 0;
}


void displayOutput(output *o, int n) {
  double totalWaitingTime = 0;
  int i, j;
  
  for (i = 0; i < n; i++) {
    printf("P[%d] \n", o[i].pID);

    for (j = 0; j < o[i].ptrLength; j++) {
      printf("Start time: %d \t End time: %d \n", o[i].ptr[j].startTime, o[i].ptr[j].endTime);
    }

    printf("Waiting time: %d \n", o[i].waitingTime);
    printf("Turnaround time: %d \n", o[i].turnaroundTime);
    printf("**************************************** \n");
    totalWaitingTime += o[i].waitingTime;
  }

  printf("Average waiting time: %lf \n", totalWaitingTime/n);
}

output* simulateNSJF(process *p, int n) {
  int i, j;

  /*  
  Sort processes according to Burst time.
  If burst time is equal, use arrival time
  to break tie.
  */

  for (i = 0; i < n-1; i++) {
    for (j = 0; j < n-i-1; j++) {
      if ( (p[j].totalExecutionTime > p[j+1].totalExecutionTime) ||
            ((p[j].totalExecutionTime == p[j+1].totalExecutionTime) &&
                (p[j].arrivalTime > p[j+1].arrivalTime)) ) {
        swap(&p[j], &p[j+1]);
      }
    }
  }
  
  //Create output array and dynamically allocate memory
  output *o;
  o = (output*) malloc(n * sizeof(output));
  if (o == NULL) {
    printf("Error! memory not allocated.");
    exit(0);
  }

  //Fill output array
  int T = 0;
  for (i = 0; i < n; i++) {
    o[i].pID = p[i].pID;
    o[i].ptrLength = 1;
    
    //Allocate memory for time (start and end time) array
    o[i].ptr = (time*) malloc(sizeof(time));
    if (o[i].ptr == NULL) {
      printf("Error! memory not allocated.");
      exit(0);
    }

    o[i].ptr[0].startTime = T;
    o[i].ptr[0].endTime = T + p[i].totalExecutionTime;

    o[i].waitingTime = T - p[i].arrivalTime;
    o[i].turnaroundTime = o[i].waitingTime + p[i].totalExecutionTime - p[i].arrivalTime;

    T += p[i].totalExecutionTime;
  }

  return o;
}

void swap(process *a, process *b) {
  process temp = *a;
  *a = *b;
  *b = temp;
}

