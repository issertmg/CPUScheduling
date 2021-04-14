#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <unistd.h>

typedef struct Process {
  /* input*/
  int pID;
  int arrivalTime;
  int totalExecutionTime;
  
  int executionTimeLeft; //initialized to value of totalExecutionTime (time left of totalExecutionTime)

  /* output */
  int startEndLength; //initialized to 0 (length of startTime and endTime array)
  int startTime[500];
  int endTime[500];
  int waitingTime;
  int turnaroundTime;
} process;

//function prototypes
void displayOutput();
void displayProcesses();
void sortProcessesByArrivalTime();
int getIndexOfShortestProcess(int currentTime);
int areAllProcessesDone();
void simulateNSJF();
void simulatePSJF();
void simulateFCFS();
void simulateRR();
void readTextFile();
void initializeProcesses();
int isInteger(float value);
void errorCheckInputs (float X, float Y, float Z, float A[], float B[], float C[], int processLines);

//global variables
process p[100];
int cpuSchedAlgo;
int n;
int quantum;

int main(void) {
  
  readTextFile();
  initializeProcesses();

  switch(cpuSchedAlgo) {
    case 0: simulateFCFS(); break;
    case 1: simulateNSJF(); break;
    case 2: simulatePSJF(); break;
    case 3: simulateRR(); break;
  }

  displayOutput();
  return 0;
}

int max (int a, int b) {
	if(a > b)
		return a;
	else
		return b;		
}

void simulateFCFS () {
  
  sortProcessesByArrivalTime();
  
  // calculate waiting time
  for (int i = 0; i < n; i++) {
    if (i == 0) {
      // first process
      p[i].startTime[0] = p[i].arrivalTime;
      p[i].endTime[0] = p[i].arrivalTime + p[i].totalExecutionTime;
      p[i].startEndLength++;
      p[i].turnaroundTime = p[i].endTime[0] - p[i].arrivalTime;
      p[i].waitingTime = p[i].turnaroundTime - p[i].totalExecutionTime;
    } else {
      p[i].startTime[0] = max(p[i].arrivalTime, p[i-1].endTime[0]);
      p[i].endTime[0] = p[i].totalExecutionTime + p[i].startTime[0];
      p[i].startEndLength++;
      p[i].turnaroundTime = p[i].endTime[0] - p[i].arrivalTime;
      p[i].waitingTime = p[i].turnaroundTime - p[i].totalExecutionTime;
    }
  }

}
void simulatePSJF() {
  sortProcessesByArrivalTime();

  
  int currentTime = 0;
  while (!areAllProcessesDone()) {
    int i;
    //get shortest process available at currentTime
    do {
      i = getIndexOfShortestProcess(currentTime);
      if (i == -1)
        currentTime++;
    } while (i == -1);
    
    
    p[i].startTime[p[i].startEndLength] = currentTime;

    /* 
      Increase time until finding another shorter process
      or until current process is done
    */
    int j = i; //previous index
    do {
      i = getIndexOfShortestProcess(currentTime);
      if (i == j) {
        currentTime++;
        p[i].executionTimeLeft--;
      }
    } while (i == j && !areAllProcessesDone());

    //update endTime and length of startTime/endTime array of previous process
    p[j].endTime[p[j].startEndLength] = currentTime;
    p[j].startEndLength++;
  }


  //fill up waitingTime and turnaroundTime of processes
  int i, j;
  for (i = 0; i < n; i++) {
    p[i].turnaroundTime = p[i].endTime[p[i].startEndLength-1] - p[i].arrivalTime;
    p[i].waitingTime = p[i].turnaroundTime - p[i].totalExecutionTime;
  }

}

int areAllProcessesDone() {
  int i, flag = 1;
  for (i = 0; i < n; i++) {
    if (p[i].executionTimeLeft != 0) {
      flag = 0;
      break;
    }
  }
  return flag;
}

void simulateNSJF() {
  sortProcessesByArrivalTime();

  int i, index;
  int currentTime = 0;
  for (i = 0; i < n; i++) {
    
    do {
      index = getIndexOfShortestProcess(currentTime);
      if (index == -1)
        currentTime++;
    } while (index == -1);

    p[index].startEndLength = 1;
    p[index].startTime[0] = currentTime;
    p[index].endTime[0] = currentTime + p[index].totalExecutionTime;
    p[index].waitingTime = p[index].startTime[0] - p[index].arrivalTime;
    p[index].turnaroundTime = p[index].waitingTime + p[index].totalExecutionTime;

    currentTime += p[index].totalExecutionTime;
    p[index].executionTimeLeft = 0;
  }
}

int getIndexOfShortestProcess(int currentTime) {
  int shortestExecutionTime = INT_MAX;
  int index = -1;
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
      if ( (p[j].arrivalTime > p[j+1].arrivalTime) ||
           ((p[j].arrivalTime == p[j+1].arrivalTime) &&
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
    printf("P[%d] \n", p[i].pID);

    for (j = 0; j < p[i].startEndLength; j++) {
      printf("Start time: %d \t End time: %d \n", p[i].startTime[j], p[i].endTime[j]);
    }

    printf("Waiting time: %d \n", p[i].waitingTime);
    printf("Turnaround time: %d \n", p[i].turnaroundTime);
    printf("**************************************** \n");
    totalWaitingTime += p[i].waitingTime;
  }

  printf("Average waiting time: %0.1lf \n", totalWaitingTime/n);
}

void simulateRR() {
  sortProcessesByArrivalTime();

  int currentTime = 0;
  while (!areAllProcessesDone()) {

    int i;
    for (i = 0; i < n; i++) {
      L1:
          if (p[i].arrivalTime <= currentTime) {
            if (p[i].executionTimeLeft != 0) {
              if (p[i].executionTimeLeft > quantum) {
                p[i].executionTimeLeft -= quantum;

               p[i].startTime[p[i].startEndLength] = currentTime;
                currentTime += quantum;
                p[i].endTime[p[i].startEndLength] = currentTime;
                p[i].startEndLength++;
              }
              else {
                p[i].startTime[p[i].startEndLength] = currentTime;
                currentTime += p[i].executionTimeLeft;
                p[i].executionTimeLeft = 0;
                p[i].endTime[p[i].startEndLength] = currentTime;
                p[i].startEndLength++;
              }
            }
          }
          else {
            currentTime = p[i].arrivalTime;
            goto L1;
          }
    }
  }

  //fill up waitingTime and turnaroundTime of processes
  int i, j;
  for (i = 0; i < n; i++) {
    p[i].turnaroundTime = p[i].endTime[p[i].startEndLength-1] - p[i].arrivalTime;
    p[i].waitingTime = p[i].turnaroundTime - p[i].totalExecutionTime;
  }
  
}

void readTextFile() {
  FILE *fp;
  int processLineCounter;
  char filepath[261] = "./";
  char filename[261];

  //Variables used for checking whether input are all integers
  float X, Y, Z;
  float A[100], B[100], C[100];

  printf("Enter filename (include .txt): ");
  scanf("%s", filename);

  strcat(filepath, filename);

  fp = fopen (filepath, "r");

  if (fp != NULL) {
    if (fscanf(fp, " %f %f %f", &X, &Y, &Z) == 3) {
      processLineCounter = 0;
      while (fscanf(fp, " %f %f %f", &A[processLineCounter], &B[processLineCounter], &C[processLineCounter]) == 3) {
        processLineCounter++;
      }
    }
    else {
      printf("Error: Problem reading first line of text file. Make sure it contains 3 integers.");
      exit(1);
    }
  }
  else {
    printf("Error opening text file. Make sure filename (with file extension) is correct.");
    exit(1);
  }

  fclose(fp);

  errorCheckInputs(X, Y, Z, A, B, C, processLineCounter);

  //Place read values to global variables after checking for errors.
  cpuSchedAlgo = X;
  n = Y;
  quantum = Z;

  int i;
  for (i = 0; i < n; i++) {
    p[i].pID = (int) A[i];
    p[i].arrivalTime = (int) B[i];
    p[i].totalExecutionTime = (int) C[i];
  }
}

void errorCheckInputs (float X, float Y, float Z, float A[], float B[], float C[], int processLines) {
  //constraint: X should be 0 to 3.
  if (X < 0 || X > 3) {
    printf("Error: Specified CPU scheduling algorithm (X) is invalid. Must be a value from 0 to 3.");
    exit(1);
  }

  //constraint: Y should be from 3 to 100.
  if (Y < 3 || Y > 100) {
    printf("Error: Number of processes (Y) should be from 3 to 100.");
    exit(1);
  }

  //constraint: Z should be from 1 to 100.
  if (Z < 1 || Z > 100) {
    printf("Error: Time slice (Z) should be from 1 to 100.");
    exit(1);
  }

  //Error checking: if not RR algo, then Z should be 1.
  if (X != 3 && Z != 1) {
    printf("Error: CPU scheduling algorithm (X) is not Round Robin. Time slice (Z) value should be 1.");
    exit(1);
  }

  //Error checking for number of processes read from file
  if (processLines < Y) {
    printf("Error: Lines of processes is less than the indicated number of processes.");
    exit(1);
  }
  else if (processLines > Y) {
    printf("Error: Lines of processes is greater than the indicated number of processes.");
    exit(1);
  }

  //Check if all inputs are integers
  if (!isInteger(X) || !isInteger(Y) || !isInteger(Z)) {
    printf("Error: First line containing X, Y, and Z should be integers.");
    exit(1);
  }

  int i;
  for (i = 0; i < Y; i++) {
    if (!isInteger(A[i]) || !isInteger(B[i]) || !isInteger(C[i])) {
      printf("Error: All process ID, arrival time, and total execution time should be integers.");
      exit(1);
    }
  }
}

int isInteger(float value) {
    int truncated = (int) value;
    float integerValue = (float) truncated;
    return (value == truncated);
}

void initializeProcesses() {
  int i;
  for (i = 0; i < n; i++) {
    p[i].executionTimeLeft = p[i].totalExecutionTime;
    p[i].startEndLength = 0;
  }
}