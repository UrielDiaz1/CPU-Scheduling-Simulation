#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

struct node {
   int pid;
   int arrival;
   int burst;
   int totalBurst;
   int firstRun;
   struct node* next;
   struct node* prev;
};
struct node* head = NULL;

int SIZE = 20;
int pidArray[20];
int arrivalArray[20];
int burstArray[20];

int downtime = 0;
int systemTime = 0;
float totalWaitTime = 0;
float totalResponseTime = 0;
float totalTurnaroundTime = 0;
float totalProcesses = 0;
int currentProcesses = 0;

void utilizeCpuFCFS(struct node*);
void utilizeCpuRoundRobin(struct node*, int);
void utilizeCpuSRTF(struct node*);
int interruptProcess(struct node*);
void implementScheduler(int, int);
void waitForProcess();
void recordFirstRun(struct node*);
void updateSystemTime(struct node*);
void startTerminationProcess(struct node*);
void gatherProcessTimeData(struct node*);
void finishedProcessFeedback(struct node*);
void scheduleReadyQueue();
char* padding();
void processDowntime();
void checkProcessArrived();
void terminateProcess(struct node* node);
void calculateAvgCpuUsage();
void calculateAvgWaitTime();
void calculateAvgResponseTime();
void calculateAvgTurnaroundTime();
void displayCompletion();
void displayLineSeparator();
void gatherFileData(FILE*, char*);
void determineTotalProcesses(char*);

int main(int argc, char *argv[]) {
   int algorithm;
   int quantum ;
   FILE *fp;
   
   displayLineSeparator();
   if(argc <= 2 || argc > 4) {
      printf("\nUsage: proj2 input_file FCFS|RR|SRJF [quantum]\n");
      exit(1);  
   }
   if(argc == 3) {
      if(strcmp(argv[2], "FCFS") == 0) {
         printf("\nScheduling algorithm: %s", argv[2]);
         algorithm = 1;
      }
      else if(strcmp(argv[2], "SRTF") == 0) {
         printf("\nScheduling algorithm: %s", argv[2]);
         algorithm = 3;        
      }
      else {
         printf("\nUsage: proj2 input_file FCFS|RR|SRJF [quantum]\n");
         exit(1);
      }
   }
   if(argc == 4)
   {
      quantum = atoi(argv[3]);
      if(strcmp(argv[2], "RR") == 0 && quantum > 0) {
         printf("\nScheduling algorithm: %s (quantum time %d)", argv[2], quantum);
         algorithm = 2;
      }
      else {
         printf("\nUsage: proj2 input_file FCFS|RR|SRJF [quantum]\n");
         exit(1);
      }
   }

   gatherFileData(fp, argv[1]);
   determineTotalProcesses(argv[1]);
   implementScheduler(algorithm, quantum);
   displayCompletion();
   calculateAvgCpuUsage();
   calculateAvgWaitTime();
   calculateAvgResponseTime();
   calculateAvgTurnaroundTime();
   printf("\n");
   return 0;
}

/******************************* FCFS *********************************/
void utilizeCpuFCFS(struct node* process) {
   int i;
   for(i = 0; i < process->totalBurst; i++) {
      if(process->firstRun < 0) {
         recordFirstRun(process);
      }
        
      // Updates ready queue every unit of time.
      //
      updateSystemTime(process);
      checkProcessArrived();
   }
}

/********************************* RR *********************************/
void utilizeCpuRoundRobin(struct node* process, int quantum) {
   int i;
   for(i = 0; i < quantum; i++) {
      if(process->firstRun < 0) {
         recordFirstRun(process);
      }
  		
      updateSystemTime(process);
      checkProcessArrived();
  		        
      // Stops early if process finishes task before the full
      // quantum time is used.
      //
      if(process->burst <= 0) {
         break;
      }
   }
}

/******************************* SRTF *********************************/
void utilizeCpuSRTF(struct node* process) {
   int i;
   for(i = process->burst; i > 0; i--) {
      // Stops early if another task has a lesser burst.
      //
      if(interruptProcess(process) == 1) {
         break;
      }
  
      if(process->firstRun < 0) {
         recordFirstRun(process);
      }
  		
      updateSystemTime(process);
      checkProcessArrived();
   }
}

/* Only used by SRTF to stop a running process. */
int interruptProcess(struct node* process) {
   struct node* current = process;
   int i;
   
   // Returns early if no more processes will arrive.
   //
   if(currentProcesses <= 0) {
      return 0;
   }
   
   // Checks if other process has a lower burst.
   //
   for(i = 0; i < totalProcesses; i++) {
      if (current->arrival <= systemTime && current->burst < process->burst) {		
         return 1;
      }
      current = current->next;
   }
   return 0;
}

/***************************** General *******************************/
/* Scheduler begins to work. */
void implementScheduler(int algorithm, int quantum) {
   struct node* current;
    
   // Keeps the algorithm running until all processes have 
   // finished their burst time.
   //
   while(currentProcesses > 0) {
      waitForProcess();
      current = head;

      if(current->burst > 0 && current != NULL) {
         switch(algorithm) {
            case 1  :
               utilizeCpuFCFS(current);
	       break;
            case 2  :
	       utilizeCpuRoundRobin(current, quantum);
	       break;
  	    case 3  :
	       utilizeCpuSRTF(current);
               break;
            default :
               printf("\nScheduling algorithm unavailable.");
               exit(1);
         }
      }
  		
      // Checks if process finished its task.
      //
      if(current->burst <= 0) {
         startTerminationProcess(current);
      }
      else {
         head = current->next;
      }
   }
}

/* Scheduler waits for a process to arrive when ready queue is empty. */
void waitForProcess() {
   while (head == NULL && currentProcesses > 0) {
      checkProcessArrived();
        
      // If no process has arrived while ready queue is empty, then
      // there's CPU downtime.
      //
      if(head == NULL) {
         processDowntime();
      }
   }
}

/* Keeps record of a process' first run. */
void recordFirstRun(struct node* process) {
   process->firstRun = systemTime;
}

/* Updates system time by the same unit of process burst. */
void updateSystemTime(struct node* process) {
   printf("\n<system time%s %d> process     %d is running", padding(), systemTime, process->pid);
   systemTime++;
   process->burst--;
}

/* Fulfills the necessary steps before process termination. */
void startTerminationProcess(struct node* process) {
   gatherProcessTimeData(process);
   finishedProcessFeedback(process);
		    
   // Updates head node (who will use process next) & terminates
   // current process.
   //
   process = process->next;
   terminateProcess(process->prev);
   currentProcesses--;
}

/* Adds the process times to the total times. */
void gatherProcessTimeData(struct node* process) {
   totalTurnaroundTime += systemTime - process->arrival;
   totalWaitTime += systemTime - process->arrival - process->totalBurst;
   totalResponseTime += process->firstRun - process->arrival;
}

/* Informs of a completed process. */
void finishedProcessFeedback(struct node* process) {
   printf("\n<system time%s %d> process     %d is finished.....", padding(), systemTime, process->pid);    
}

/* Schedules processes into the back of the ready queue. */
void scheduleReadyQueue(int pid, int arrival, int burst) {
   struct node* node;
   node = (struct node*)malloc(sizeof(struct node));
   node->pid = pid;
   node->arrival = arrival;
   node->burst = burst;
   node->totalBurst = burst;
   node->firstRun = -1;  // -1 signifies process hasn't had a first run.

   if (head == NULL) {
      node->next = node;
      node->prev = node;
      head = node;
   }
   else {   // Add to back of queue.
      struct node* current;
      current = head;
	    
      node->next = current;
      node->prev = current->prev;
      current->prev->next = node;
      current->prev = node;
   }
}

/* Adds padding to the displayed output. */
char* padding() {
   int digits = 1;
    
   if((log10(systemTime) + 1) > digits) {
      digits = log10(systemTime) + 1;
   }

   // Padding is determined by the amount of system time digits.
   // The less digits, the more padding.
   //
   switch(digits) {
      case 1  :
         return "     ";
      case 2  :
         return "    ";
      case 3  :
         return "   ";
      case 4  :
         return "  ";
      default :
         return " ";
   }
}

/* Informs of any downtime the scheduler comes across. */
void processDowntime() {
   printf("\n<system time%s %d> No process is currently running.....", padding(), systemTime);
                                                    
   // System time keeps running even during downtime.
   //
   systemTime++;
   downtime++;
}

/* Checks if a process has arrived at the current system time. */
void checkProcessArrived() {
   if(currentProcesses <= 0) {
      return;
   }

   int i;
   for(i = 0; i < totalProcesses; i++) {
      // Ignores processes that cannot exist, such as processes with a PID
      // less than 1, a negative arrival time, or have no burst time to
      // complete.
      //
      if(pidArray[i] <= 0 || arrivalArray[i] < 0 || burstArray[i] <= 0) {
         continue;
      }
      else if (arrivalArray[i] <= systemTime) {
         scheduleReadyQueue(pidArray[i], arrivalArray[i], burstArray[i]);
            
         // Marks the process as having arrived.
         //
         arrivalArray[i] = -1;
      }
   }
}

/* Terminates a process by deleting the node from the list. */
void terminateProcess(struct node* node) {
   if(node == NULL) {
      return;
   }
    
   // If this node is the last node.
   //
   if(node->pid == node->next->pid) {
      free(node);
      head = NULL;
   }
   // If the node being removed is the head.
   //
   else if(node->pid == head->pid) {
      node->prev->next = node->next;
      node->next->prev = node->prev;
      head = node->next;
      free(node);
   }
}

void calculateAvgCpuUsage() {
   double totalActive = systemTime - downtime;
   double totalTime = systemTime;
   double avgUsage = totalActive / totalTime * 100;
   printf("\nAverage CPU usage      : %.2f %%", avgUsage);
}

void calculateAvgWaitTime() {
   float avgWait = totalWaitTime / totalProcesses;
   printf("\nAverage Waiting Time   : %.2f", avgWait);
}

void calculateAvgResponseTime() {
   float avgResp = totalResponseTime / totalProcesses;
   printf("\nAverage Response Time  : %.2f", avgResp);
}

void calculateAvgTurnaroundTime() {
   float avgTurnaround = totalTurnaroundTime / totalProcesses;
   printf("\nAverage Turnaround Time: %.2f", avgTurnaround);
   displayLineSeparator();
}

void displayCompletion() {
   printf("\n<system time%s %d> All processes finished ..................\n", padding(), systemTime);
   displayLineSeparator();
}

void displayLineSeparator() {
   printf("\n============================================================");
}

void gatherFileData(FILE* fp, char *fileName) {
   char path[] = "./";
   fp = fopen(strcat(path, fileName), "r");
   int i;

   // Scans the file.
   //
   for(i = 0; i < SIZE; i++) {
      // fscanf returns number of fields that it succesfully assigned.
      //
      if(fscanf(fp, "%d %d %d", &pidArray[i], &arrivalArray[i], &burstArray[i]) == 3);
      else {    // EOF reached.
         pidArray[i] = -1;
         arrivalArray[i] = -1;
         burstArray[i] = -1;        
      }
   }
   fclose(fp);
}

/* Counts the amount of processes in the input file. */
void determineTotalProcesses(char* fileName) {
   // SIZE will be the size limit of the ready queue.
   //
   int i;
   for(i = 0; i < SIZE; i++) {
      if(pidArray[i] > 0 && burstArray[i] >= 1 && arrivalArray[i] >= 0) {
         totalProcesses++;
      }
   }
   currentProcesses = totalProcesses;
   printf("\nTotal %.f tasks are read from %s.", totalProcesses, fileName);
   displayLineSeparator();
}
