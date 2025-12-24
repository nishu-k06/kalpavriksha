#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<stdbool.h>

#define SIZE 100
#define HASH_SIZE 11

//process states
typedef enum ProcessState {
    READY,
    RUNNING,
    WAITING,
    TERMINATED,
    KILLED
} ProcessState;

//process control block
typedef struct ProcessControlBlock {
    char name[SIZE];
    int burstTime;
    int remBurstTime;
    int ioStartTime;
    int ioDuration;
    int ioRemaining;
    int cpuExecuted;
    ProcessState state; 
    int turnAroundTime;
    int waitingTime;
    bool ioStartedThisTick;
} ProcessControlBlock;

//hash map nodes
typedef struct HashNode {
    int key;
    ProcessControlBlock* value;
    struct HashNode* next;
} HashNode;

//hash map
typedef struct HashMap {
    HashNode* buckets[HASH_SIZE];
} HashMap;

//queue nodes
typedef struct Node {
    int processId;
    struct Node* next;
} Node;

typedef struct Queue {
    Node* front;
    Node* rear;
} Queue;

//kill event
typedef struct KillEvent {
    int processId;
    int time;
} KillEvent;

//initialize hash map
HashMap* createHashMap() {
    HashMap* map = malloc(sizeof(HashMap));
    if (!map)
    {
        printf("Memory Allocation Failed!\n");
        exit(1);
    }

    for (int index = 0; index < HASH_SIZE; index++)
    {
        map->buckets[index] = NULL;
    }

    return map;
}
//create hash node
HashNode* createHashNode(int key, ProcessControlBlock* pcb) {
    HashNode* newNode = malloc(sizeof(HashNode));
    if (!newNode)
    {
        printf("Memory Allocation Failed!\n");
        exit(1);
    }

    newNode->key = key;
    newNode->value = pcb;
    newNode->next = NULL;

    return newNode;
}

int hash(int key) {
    return (key % HASH_SIZE + HASH_SIZE) % HASH_SIZE;
}

//insert into hash map
void insertIntoHashMap(HashMap* hashMap, int key, ProcessControlBlock* pcb) {
    if (!hashMap || !pcb)
    {
        return;
    }

    int index = hash(key);

    HashNode* newNode = createHashNode(key, pcb);
    newNode->next = hashMap->buckets[index];
    hashMap->buckets[index] = newNode;
}

//get node from hash map
ProcessControlBlock* getFromHashMap(HashMap* hashMap, int key) {
    if (!hashMap)
    {
        return NULL;
    }

    int index = hash(key);

    HashNode* temp = hashMap->buckets[index];

    while(temp) 
    {
        if (temp->key == key)
        {
            return temp->value;
        }
        temp = temp->next;
    }

    return NULL;
}

void freeHashMap(HashMap* hashMap) {
    if (!hashMap)
    {
        return;
    }

    for (int index = 0; index < HASH_SIZE; index++)
    {
        HashNode* temp = hashMap->buckets[index];
        while (temp)
        {
            HashNode* toDelete = temp;
            temp = temp->next;
            free(toDelete->value);
            free(toDelete);
        }
    }
    
    free(hashMap);
}


//initialize queue
Queue* createQueue() {
    Queue* queue = malloc(sizeof(Queue));
    if (!queue) 
    {
        printf("Memory Allocation Failed!\n");
        exit(1);
    }

    queue->front = queue->rear = NULL;

    return queue;
}

// create queue node
Node* createQueueNode(int processId) {
    Node* newNode = malloc(sizeof(Node));
    if (!newNode)
    {
        printf("Memory Allocation Failed!\n");
        exit(1);
    }

    newNode->processId = processId;
    newNode->next = NULL;

    return newNode;
}

//enqueue process
void enqueue(Queue* queue, int processId) {
    if (!queue)
    {
        return;
    }

    Node* newNode = createQueueNode(processId);
    if (!queue->front)
    {
        queue->front = queue->rear = newNode;
        return;
    }

    queue->rear->next = newNode;
    queue->rear = newNode;
}

//dequeue process
int dequeue(Queue* queue) {
    if (!queue || !queue->front)
    {
        printf("Empty Queue\n");
        return -1;
    }

    Node* toDelete  = queue->front;
    int processId = toDelete->processId;

    if (queue->front == queue->rear) 
    {
        queue->rear = NULL;
    }
    queue->front = queue->front->next;
    free(toDelete);

    return processId;
}

//remove specific process from queue
bool removeFromQueue(Queue* queue, int processId) {
    if (!queue || !queue->front)
    {
        return false;
    }

    Node* curr = queue->front, *prev = NULL;

    while (curr) {
        if (curr->processId == processId)
        {
            if (!prev)
            {
                queue->front = curr->next;
            }
            else
            {
                prev->next = curr->next;
            }

            if (queue->rear == curr)
            {
                queue->rear = prev;
            }

            free(curr);
            return true;
        }
        prev = curr;
        curr = curr->next;
    }

    return false;
}


bool isQueueEmpty(Queue* queue) {
    return !queue || !queue->front;
}

void freeQueue(Queue* queue) {
    if (!queue)
    {
        return;
    }

    while (queue->front)
    {
        dequeue(queue);
    }

    free(queue);
}

//kill events
KillEvent* createKillEvents(int killCount) {
    KillEvent* kills = malloc(killCount * sizeof(KillEvent));
    if (!kills)
    {
        printf("Memory Allocation Failed\n");
        exit(1);
    }

    for (int index = 0; index < killCount; index++)
    {
        kills[index].processId = -1;
        kills[index].time = -1;
    }

    return kills;
}

//move to waiting
void moveToWaiting(ProcessControlBlock* pcb, int processId, Queue* waitingQueue) {
    pcb->state = WAITING;
    pcb->ioRemaining = pcb->ioDuration;
    pcb->ioStartedThisTick = true;
    enqueue(waitingQueue, processId);
}

//move to terminated
void moveToTerminated(ProcessControlBlock* pcb, int processId, int clock, Queue* terminatedQueue) {
    pcb->turnAroundTime = clock;
    pcb->waitingTime = pcb->turnAroundTime - pcb->burstTime;
    pcb->state = TERMINATED;
    enqueue(terminatedQueue, processId);
}

//kill process
void killProcess(ProcessControlBlock* pcb, int processId, int clock, Queue* terminatedQueue) {
    pcb->turnAroundTime = -1;
    pcb->waitingTime = -1;
    pcb->state = KILLED;
    enqueue(terminatedQueue, processId);
}

//update waiting processes
void updateWaitingProcesses(Queue* waitingQueue, Queue* readyQueue, HashMap* hashMap) {
    Node* curr = waitingQueue->front;
    Node* prev = NULL;

    while (curr)
    {
        int processId = curr->processId;
        ProcessControlBlock* pcb = getFromHashMap(hashMap, processId);

        if (!pcb)
        {
            prev = curr;
            curr = curr->next;
            continue;
        }

        if (pcb->ioStartedThisTick)
        {
            pcb->ioStartedThisTick = false;
            prev = curr;
            curr = curr->next;
            continue;
        }

        pcb->ioRemaining--;

        if (pcb->ioRemaining <= 0)
        {
            Node* toRemove = curr;

            if (!prev)
            {
                waitingQueue->front = curr->next;
            }
            else
            {
                prev->next = curr->next;
            }

            if (waitingQueue->rear == curr)
            {
                waitingQueue->rear = prev;
            }

            curr = curr->next;
            free(toRemove);

            pcb->state = READY;
            enqueue(readyQueue, processId);
        }
        else
        {
            prev = curr;
            curr = curr->next;
        }
    }
}

//update and apply kill events
void applyKillEvents(KillEvent* kills, int killCount, int clock, HashMap* hashMap, Queue* readyQueue, Queue* waitingQueue, Queue* terminatedQueue,
int* runningProcessId,  int* terminatedCount) {
    for (int index = 0; index < killCount; index++)
    {
        if (kills[index].time != clock)
        {
            continue;
        }

        int processId = kills[index].processId;
        ProcessControlBlock* pcb = getFromHashMap(hashMap, processId);
        if (!pcb || pcb->state == TERMINATED || pcb->state == KILLED) {
            continue;
        }

        if (*runningProcessId == processId)
        {
            killProcess(pcb, processId, clock, terminatedQueue);
            (*terminatedCount)++;
            *runningProcessId = -1;
            continue;
        }

        if (removeFromQueue(readyQueue, processId) || removeFromQueue(waitingQueue, processId))
        {
            killProcess(pcb, processId, clock, terminatedQueue);
            (*terminatedCount)++;
        }

    }
}

//print process results
void printResults(HashMap* hashMap) {
    printf("\n%-8s%-15s%-10s%-10s%-15s%-12s%-10s\n",
           "PID", "Name", "CPU", "IO", "Turnaround", "Waiting", "Status");
    printf("--------------------------------------------------------------------------------\n");
    
    for (int index = 0; index < HASH_SIZE; index++) {
        HashNode* node = hashMap->buckets[index];
        while (node) 
        {
            ProcessControlBlock* pcb = node->value;
            const char* status = (pcb->state == KILLED) ? "KILLED" : "OK";
            
            printf("%-8d%-15s%-10d%-10d%-15d%-12d%-10s\n",
                   node->key, pcb->name, pcb->burstTime, pcb->ioDuration, pcb->turnAroundTime, pcb->waitingTime, status);
            
            node = node->next;
        }
    }
}

//FCFS Scheduling
void fcfsScheduler(HashMap* hashMap, Queue* readyQueue, int totalProcesses, KillEvent* kills, int killCount) {
    Queue* waitingQueue = createQueue();
    Queue* terminatedQueue = createQueue();

    int clock = 0;
    int runningProcessId = -1;
    int terminatedProcesses = 0;

    while (terminatedProcesses < totalProcesses)
    {
        applyKillEvents(kills, killCount, clock, hashMap, readyQueue, waitingQueue, terminatedQueue, &runningProcessId, &terminatedProcesses);

        if (runningProcessId == -1 && !isQueueEmpty(readyQueue))
        {
            runningProcessId = dequeue(readyQueue);
            ProcessControlBlock* pcb = getFromHashMap(hashMap, runningProcessId);
            if (pcb)
            {
                pcb->state = RUNNING;
            }
        }

        if (runningProcessId != -1)
        {
            ProcessControlBlock* pcb = getFromHashMap(hashMap, runningProcessId);

            if (pcb)
            {
                pcb->cpuExecuted++;
                pcb->remBurstTime--;

                if (pcb->ioStartTime >= 0 && pcb->cpuExecuted == pcb->ioStartTime && pcb->remBurstTime > 0)
                {
                    moveToWaiting(pcb, runningProcessId, waitingQueue);
                    runningProcessId = -1;
                }
                else if (pcb->remBurstTime <= 0)
                {
                    moveToTerminated(pcb, runningProcessId, clock + 1, terminatedQueue);
                    terminatedProcesses++;
                    runningProcessId = -1;
                }
            }
        }

        updateWaitingProcesses(waitingQueue, readyQueue, hashMap);

        clock++;
    }

    printResults(hashMap);

    freeQueue(waitingQueue);
    freeQueue(terminatedQueue);
}


//process creation
ProcessControlBlock* createProcess() {
    ProcessControlBlock* pcb = malloc(sizeof(ProcessControlBlock));
    if (!pcb)
    {
        printf("Memory Allocation Failed!\n");
        exit(1);
    }

    pcb->cpuExecuted = 0;
    pcb->waitingTime = 0;
    pcb->turnAroundTime = 0;
    pcb->ioRemaining = 0;
    pcb->ioStartedThisTick = false;
    pcb->state = READY;

    return pcb;
}

//read process input
void readProcessInput(HashMap* hashMap, Queue* readyQueue, int* totalProcesses) {
    printf("Enter number of processes: "); 
    scanf("%d", totalProcesses);

    for (int index = 0; index < *totalProcesses; index++) 
    {
        printf("\nEnter details of Process %d:\n", index + 1);

        int processId;
        ProcessControlBlock* pcb = createProcess();


    printf("Process Name: ");
    while (getchar() != '\n');
    fgets(pcb->name, sizeof(pcb->name), stdin);
    pcb->name[strcspn(pcb->name, "\n")] = '\0';

    printf("Process ID: "); 
    scanf("%d", &processId);

    printf("Burst Time: "); 
    scanf("%d", &pcb->burstTime);
    pcb->remBurstTime = pcb->burstTime;
    
    printf("IO Start Time (-1 for none): "); 
    scanf("%d", &pcb->ioStartTime);

    printf("IO Duration (0 for none): "); 
    scanf("%d", &pcb->ioDuration);


    if (pcb->ioStartTime < 0 || pcb->ioDuration <= 0) {
        pcb->ioStartTime = -1; 
        pcb->ioDuration = 0;
    }

    insertIntoHashMap(hashMap, processId, pcb);
    enqueue(readyQueue, processId);
}
}

//read kill events
void readKillEvents(KillEvent** kills, int* killCount) {
    printf("\nEnter number of KILL events: ");
    scanf("%d", killCount);

    if (*killCount > 0)
    {
        *kills = createKillEvents(*killCount);

        for (int index = 0; index < *killCount; index++)
        {
            printf("KILL event %d: \n", index + 1);
            
            printf("Process ID: ");
            scanf("%d", &(*kills)[index].processId);

            printf("Time: ");
            scanf("%d", &(*kills)[index].time);
        }
    }
}


int main() {
    HashMap* hashMap = createHashMap();
    Queue* readyQueue = createQueue();

    int totalProcesses = 0;
    readProcessInput(hashMap, readyQueue, &totalProcesses);

    KillEvent* kills = NULL;
    int killCount = 0;
    readKillEvents(&kills, &killCount);

    fcfsScheduler(hashMap, readyQueue, totalProcesses, kills, killCount);

    if (kills)
    {
        free(kills);
    }

    freeQueue(readyQueue);
    freeHashMap(hashMap);
}