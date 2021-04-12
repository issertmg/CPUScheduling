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
      /* p[i].endTime[0] = p[i].totalExecutionTime + p[i-1].endTime[0];
      p[i].turnaroundTime = p[i].endTime[0] - p[i].arrivalTime; */
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
  int i;
  char completefilepath[301];
  char filename[101];

  if (getcwd(completefilepath, sizeof(completefilepath)) == NULL) {
    perror("Error getting current working dir.");
    exit(1);
  }

  printf("Enter filename (include .txt): ");
  scanf("%s", filename);
  strcat(completefilepath, "\\");
  strcat(completefilepath, filename);
  printf("File path: %s\n", completefilepath);

  fp = fopen (completefilepath, "r");

  if (fp != NULL) {
    if (fscanf(fp, " %d %d %d", &cpuSchedAlgo, &n, &quantum) == 3) {
      for (int i = 0; i < n; i++) {
        fscanf(fp, " %d %d %d", &p[i].pID, &p[i].arrivalTime, &p[i].totalExecutionTime);
      }
    }
  }
  else {
    printf("Error opening text file.");
    exit(1);
  }

  fclose(fp);
}

void initializeProcesses() {
  int i;
  for (i = 0; i < n; i++) {
    p[i].executionTimeLeft = p[i].totalExecutionTime;
    p[i].startEndLength = 0;
  }
}