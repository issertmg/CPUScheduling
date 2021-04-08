#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

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

typedef struct Node {
  process *p;
  struct Node *next;
} node;

typedef struct Queue {
  node *front;
  node *rear;
  int n;
} queue;

//function prototypes
void displayOutput();
void displayProcesses();
void sortProcessesByArrivalTime();
int getIndexOfShortestProcess(int currentTime);
void simulateNSJF();
void simulatePSJF();
void simulateFCFS();
int areAllProcessesDone();
void simulateRR();

//queue related function prototypes
queue* createQueue();
node* createNode(process* p1);
void enqueue(queue* q, process* p1);
node* dequeue(queue* q);
int isEmptyQueue(queue* q);
int areAllQueuedProcessesDone(queue* q);

//global variables
process p[100];
int n;
int quantum;

int main(void) {
  
  //SAMPLE DATA FOR TESTING
  /* n = 4; 
  p[0].pID = 1; p[0].arrivalTime = 0; p[0].totalExecutionTime = 8; 
  p[1].pID = 2; p[1].arrivalTime = 1; p[1].totalExecutionTime = 4; 
  p[2].pID = 3; p[2].arrivalTime = 2; p[2].totalExecutionTime = 9;
  p[3].pID = 4; p[3].arrivalTime = 3; p[3].totalExecutionTime = 5; */

  /* n = 3; 
  p[0].pID = 0; p[0].arrivalTime = 0; p[0].totalExecutionTime = 24; 
  p[1].pID = 1; p[1].arrivalTime = 0; p[1].totalExecutionTime = 3; 
  p[2].pID = 2; p[2].arrivalTime = 0; p[2].totalExecutionTime = 3; */

  n = 5; 
  p[0].pID = 1; p[0].arrivalTime = 4; p[0].totalExecutionTime = 5; 
  p[1].pID = 2; p[1].arrivalTime = 6; p[1].totalExecutionTime = 4; 
  p[2].pID = 3; p[2].arrivalTime = 0; p[2].totalExecutionTime = 3;
  p[3].pID = 4; p[3].arrivalTime = 6; p[3].totalExecutionTime = 2; 
  p[4].pID = 5; p[4].arrivalTime = 5; p[4].totalExecutionTime = 4;
  
  //for RR testing
  quantum = 4;
  //initialize other attributes of processes
  int i;
  for (i = 0; i < n; i++) {
    p[i].executionTimeLeft = p[i].totalExecutionTime;
    p[i].startEndLength = 1; //todo:
  }


  displayProcesses();
  //simulateNSJF();
  //simulatePSJF();
  simulateFCFS();
  //simulateRR();
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
      p[i].turnaroundTime = p[i].endTime[0] - p[i].arrivalTime;
      p[i].waitingTime = p[i].turnaroundTime - p[i].totalExecutionTime;
    } else {
      p[i].startTime[0] = max(p[i].arrivalTime, p[i-1].endTime[0]);
      /* p[i].endTime[0] = p[i].totalExecutionTime + p[i-1].endTime[0];
      p[i].turnaroundTime = p[i].endTime[0] - p[i].arrivalTime; */
      p[i].endTime[0] = p[i].totalExecutionTime + p[i].startTime[0];
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

  printf("Average waiting time: %lf \n", totalWaitingTime/n);
}

queue* createQueue() {
  queue* q = (queue*) malloc(sizeof(queue));
  q->front = NULL;
  q->rear = NULL;
  return q;
}

node* createNode(process *p1) {
  node* qNode = (node*) malloc(sizeof(node));
  qNode->p = p1;
  qNode->next = NULL;
  return qNode;
}

void enqueue(queue* q, process *p1) {
  node* qNode = createNode(p1);

  if (isEmptyQueue(q)) {
    q->front = qNode;
    q->rear = qNode;
  }

  q->rear->next = qNode;
  q->rear = qNode;
}

node* dequeue(queue* q) {
  if (q->front == NULL)
    return NULL;
  
  node* qNode = q->front;
  
  q->front = q->front->next;

  if (q->front == NULL)
    q->rear = NULL;

  return qNode;
}

int isEmptyQueue(queue* q) {
  return q->rear == NULL;
}

void simulateRR() {
  sortProcessesByArrivalTime();

  queue* q = createQueue();
  int i;
  for (i = 0; i < n; i++)
    enqueue(q, &p[i]);

  int currentTime = 0;
  while (!areAllQueuedProcessesDone(q)) {
    node* qNode = dequeue(q);

    L1:
    if (qNode->p->arrivalTime <= currentTime) {
      if (qNode->p->executionTimeLeft != 0) {
        if (qNode->p->executionTimeLeft > quantum) {
          qNode->p->executionTimeLeft -= quantum;

          qNode->p->startTime[qNode->p->startEndLength] = currentTime;
          currentTime += quantum;
          qNode->p->endTime[qNode->p->startEndLength] = currentTime;
          qNode->p->startEndLength++;
        }
        else {
          qNode->p->startTime[qNode->p->startEndLength] = currentTime;
          currentTime += qNode->p->executionTimeLeft;
          qNode->p->executionTimeLeft = 0;
          qNode->p->endTime[qNode->p->startEndLength] = currentTime;
          qNode->p->startEndLength++;
        }
      }
    }
    else {
      currentTime = qNode->p->arrivalTime;
      goto L1;
    }
    
    enqueue(q, qNode->p);
    free(qNode);
  }

  //fill up waitingTime and turnaroundTime of processes
  int j;
  for (i = 0; i < n; i++) {
    p[i].turnaroundTime = p[i].endTime[p[i].startEndLength-1] - p[i].arrivalTime;
    p[i].waitingTime = p[i].turnaroundTime - p[i].totalExecutionTime;
  }
  
}

int areAllQueuedProcessesDone(queue* q) {
  int i, flag = 1;

  for (i = 0; i < n; i++) {
    node* qNode = dequeue(q);
    if (qNode->p->executionTimeLeft != 0) {
      flag = 0;
    }
    enqueue(q, qNode->p);
    free(qNode);
  }
  return flag;
}