#include <stdio.h>
#include <string.h>

#define BUFFSIZE 255
#define ARRAYSIZE 2

//************************************* LINKED LIST *************************************//
// Implement a queue using a linked list
struct processData {
    int pid;
    int arrival;
    int burst;
};

struct node {
    struct processData data;
    struct node *next;
};

// Function to create a new node
struct node *createNode(struct processData data) {
    struct node *newNode = (struct node *)malloc(sizeof(struct node));
    newNode->data = data;
    newNode->next = NULL;
    return newNode;
}

// Function to add a node to the queue
void enqueue(struct node **head, struct processData data) {
    struct node *newNode = createNode(data);
    if (*head == NULL) {
        *head = newNode;
    } else {
        struct node *temp = *head;
        while (temp->next != NULL) {
            temp = temp->next;
        }
        temp->next = newNode;
    }
}

// Function to add a node to the queue ordered by burst time
void enqueueByBurst(struct node **head, struct processData data) {
    struct node *newNode = createNode(data);
    if (*head == NULL) {
        *head = newNode;
    } else {
        struct node *temp = *head;
        struct node *prev = NULL;
        while (temp != NULL && temp->data.burst < data.burst) {
            prev = temp;
            temp = temp->next;
        }
        if (prev == NULL) {
            newNode->next = *head;
            *head = newNode;
        } else {
            prev->next = newNode;
            newNode->next = temp;
        }
    }
}

// Function to remove a node from the queue
struct processData dequeue(struct node **head) {
    struct processData data = (*head)->data;
    struct node *temp = *head;
    *head = (*head)->next;
    free(temp);
    return data;
}

// Function to print the queue
void printQueue(struct node *head) {
    struct node *temp = head;
    while (temp != NULL) {
        printf("pid: %d, arrival: %d, burst %d\n", temp->data.pid, temp->data.arrival, temp->data.burst);
        temp = temp->next;
    }
}
//************************************* LINKED LIST END *************************************//

//************************************* UTILITY *************************************//
// Sort an array of progressDatas according to their arriwal times using quick sort
void quickSortProcessDatas(struct processData arr[], int low, int high) {
    if (low < high) {
        int pi = partition(arr, low, high);
        quickSortProcessDatas(arr, low, pi - 1);
        quickSortProcessDatas(arr, pi + 1, high);
    }
}

// Partition the array using quick sort
int partition(struct processData *arr, int low, int high) {
    int pivot = arr[high].arrival;
    int i = low - 1;
    for (int j = low; j <= high - 1; j++) {
        if (arr[j].arrival <= pivot) {
            i++;
            swap(&arr[i], &arr[j]);
        }
    }
    swap(&arr[i + 1], &arr[high]);
    return (i + 1);
}

// Swap two elements in an array
void swap(struct processData *a, struct processData *b) {
    struct processData temp = *a;
    *a = *b;
    *b = temp;
}

// Print an array of processDatas
void printArray(struct processData *arr, int size) {
    printf("\n");
    for (int i = 0; i < size; i++) {
        printf("pid: %d, arrival: %d, burst %d\n", arr[i].pid, arr[i].arrival, arr[i].burst);
    }
}
//************************************* UTILITY END *************************************//

//************************ READ TEXT FILE ************************//
struct Array {
    struct processData* array;
    int used;
    int size;
};

void initArray(struct Array *a, int initialSize) {
    a->array = malloc(initialSize * sizeof(struct processData));
    a->used = 0; //can be problem
    a->size = initialSize;
}

void insertArray(struct Array *a, struct processData element) {
    // a->used is the number of used entries, because a->array[a->used++] updates a->used only *after* the array has been accessed.
    // Therefore a->used can go up to a->size
    if (a->used == a->size) {
        a->size *= 2;
        a->array = realloc(a->array, a->size * sizeof(struct processData));
    }
    a->array[a->used++] = element;
}

struct Array getProcessDatas(char* fileName) {
    struct Array a;

    FILE *fp;
    char buff[BUFFSIZE];

    fp = fopen(fileName, "r");

    initArray(&a, ARRAYSIZE);

    do {
        struct processData d;
        fscanf(fp, "%s", buff);
        //for not entering the last empty element.
        if(strstr(buff, "\n") != NULL)
            break;
        d.pid = atoi(buff);
        //printf("burst_no\t: %s\n", buff );

        fscanf(fp, "%s", buff);
        //printf("arrival_time\t: %s\n", buff );
        d.arrival = atoi(buff);

        fscanf(fp, "%s", buff);
        //printf("burst_length\t: %s\n", buff );
        d.burst = atoi(buff);

        insertArray(&a,d);
    } while(fgets(buff, BUFFSIZE, fp));

    return a;
}

void freeArray(struct Array *a) {
    free(a->array);
    a->array = NULL;
    a->used = a->size = 0;
}
//************************ READ TEXT FILE END ************************//

//************************************* ALGOS *************************************//
int fcfsAverageTurnaround(int n, struct processData processDatas[]) {
    // Print the arrays
    printArray(processDatas, n);

    // Create a queue
    struct node *head = NULL;    //printf("%d\n", a.array[1].pid);L, *tail = NULL;

    // Insert processes into the queue
    for (int i = 0; i < n; i++) {
        enqueue(&head, processDatas[i]);
    }

    // Print the queue
    printQueue(head);

    int currentTime = 0;
    int sumTurnaroundTime = 0;
    struct processData processsData;

    while (head != NULL) {
        // Get the next process
        processsData = dequeue(&head);

        // If the current time is less than or equal to the arrival time
        if (processsData.arrival > currentTime) {
            currentTime = processsData.arrival;
        }

        // Elapse the time for the current process
        currentTime += processsData.burst;

        printf("processsData.arrival: %d, currentTime: %d\n", processsData.arrival, currentTime);
        // Add the turn around time
        sumTurnaroundTime += currentTime - processsData.arrival;
    }

    return sumTurnaroundTime / n;
}

int sjfAverageTurnaround(int n, struct processData processDatas[]) {
    // Print the arrays
    printArray(processDatas, n);

    // Create a queue
    struct node *head = NULL, *tail = NULL;

    int currentTime = 0;
    int i = 0;
    int sumTurnaroundTime = 0;
    struct processData cur;

    while (i < n || head != NULL) {
        // Add propcesses with arrival times smaller than currentTime to queue
        while (i < n && processDatas[i].arrival <= currentTime) {
            printf("Enqueue process with id %d\n", processDatas[i].pid);
            enqueueByBurst(&head, processDatas[i]);
            i++;
        }

        // If the queue is empty, then increment currentTime
        if (head == NULL) {
            currentTime = processDatas[i].arrival;
            printf("Queue is empty, update currentTime to %d\n", currentTime);
            continue;
        }

        // Get the next process
        cur = dequeue(&head);

        // Elapse the time for the current process
        currentTime += cur.burst;

        // Add the turn around time
        sumTurnaroundTime += currentTime - cur.arrival;

        printf("Finish process with id %d. Arrival time: %d, Current time: %d\n", cur.pid, cur.arrival, currentTime);
    }

    return sumTurnaroundTime / n;
}

int srjfAverageTurnaround(int n, struct processData processDatas[]) {
    // Print the arrays
    //printArray(processDatas, n);

    // Create a queue
    struct node *head = NULL, *tail = NULL;

    int currentTime = 0;
    int i = 0;
    int sumTurnaroundTime = 0;
    struct processData cur;

    printf("i:%d, n:%d\n",i,n);

    while (i < n || head != NULL) {
        // Add processes with arrival times smaller than currentTime to queue
        while (i < n && processDatas[i].arrival <= currentTime) {
            printf("Enqueue process with id %d\n", processDatas[i].pid);
            enqueueByBurst(&head, processDatas[i]);
            i++;
            printf("-i:%d, n:%d\n",i,n);
        }

        // If the queue is empty, then increment currentTime
        if (head == NULL) {
            currentTime = processDatas[i].arrival;
            printf("Queue is empty, update currentTime to %d\n", currentTime);
            continue;
        }

        // Get the next process
        printf("head:%d", (*head).data);
        printf(" i:%d", i);
        printf(" n:%d\n", n);
        cur = dequeue(&head);

        if(i==n) {
            currentTime = currentTime + cur.burst;
            sumTurnaroundTime += currentTime - cur.arrival;
            cur.burst = 0;
            printf("Run process with id %d until the end. Current time: %d\n", cur.pid, currentTime);
        }
        // If the burst of cur is less than or equal to the processDatas[i].arrival - currentTime
        else if (cur.burst <= processDatas[i].arrival - currentTime) {
            printf(" cur burst:%d\n", cur.burst);
            // Elapse the time for the current process
            currentTime += cur.burst;

            // Add the turn around time
            sumTurnaroundTime += currentTime - cur.arrival;

            printf("Finish process with id %d. Arrival time: %d, Current time: %d\n", cur.pid, cur.arrival, currentTime);
            printf("---head:%d", (*head).data);
            printf(" i:%d", i);
            printf(" n:%d\n", n);
        } else {

            // Run until next process arrives
            cur.burst -= processDatas[i].arrival - currentTime;

            // Elapse the time for the current process
            currentTime = processDatas[i].arrival;

            printf("Run process with id %d until proces with id %d arrives. Current time: %d\n", cur.pid, processDatas[i].pid, currentTime);

            // Add the running process back to the queue
            enqueueByBurst(&head, cur);

            // Add the new process to the queue
            enqueueByBurst(&head, processDatas[i]);
            i++;
        }
    }

    return sumTurnaroundTime / n;
}

// In this RR, when a tie occurs on arrival times, the protocol is to run the process with the smaller id first.
// Because the initial quicksort preserves the id orders if the arrival times are the same.
int rrAverageTurnaround(int n, struct processData processDatas[], int quantum) {
    // Print the arrays
    printArray(processDatas, n);

    // Create a queue
    struct node *head = NULL, *tail = NULL;

    int currentTime = 0;
    int i = 0;
    int sumTurnaroundTime = 0;
    struct processData cur;

    while (i < n || head != NULL) {
        // Add propcesses with arrival times smaller than currentTime to queue
        while (i < n && processDatas[i].arrival <= currentTime) {
            printf("Enqueue process with id %d\n", processDatas[i].pid);
            enqueue(&head, processDatas[i]);
            i++;
        }

        // If the queue is empty, then increment currentTime
        if (head == NULL) {
            currentTime = processDatas[i].arrival;
            printf("Queue is empty, update currentTime to %d\n", currentTime);
            continue;
        }

        // Get the next process
        cur = dequeue(&head);

        // If the burst of cur is less time quantum
        if (cur.burst <= quantum) {
            // Elapse the time for the current process
            currentTime += cur.burst;

            // Add the turn around time
            sumTurnaroundTime += currentTime - cur.arrival;

            printf("Finish process with id %d. Arrival time: %d, Current time: %d\n", cur.pid, cur.arrival, currentTime);
        } else {
            // Run as long as time quantum
            cur.burst -= quantum;

            // Elapse the time for the current process
            currentTime += quantum;

            printf("Run process with id %d a full cycle. Current time: %d\n", cur.pid, currentTime);

            // Add processes with arrival times smaller than currentTime to queue
            while (i < n && processDatas[i].arrival <= currentTime) {
                printf(".Enqueue process with id %d\n", processDatas[i].pid);
                enqueue(&head, processDatas[i]);
                i++;
                printQueue(head);
            }

            // Add the running process back to the queue
            enqueue(&head, cur);
        }
    }

    return sumTurnaroundTime / n;
}
//************************************* ALGOS END *************************************//

//************************************* MAIN *************************************//
int main() {
    /*struct processData processDatas[] = {
    	{1, 0, 5},
    	{2, 2, 2},
    	{3, 6, 12},
    	{4, 3, 2}};*/

    /*struct processData processDatas[] = {
    	{1, 0, 4},
    	{2, 12, 5},
    	{3, 6, 12},
    	{4, 8, 13}};*/

    struct Array fileData = getProcessDatas("inputs.txt");
    struct processData* processDatas = fileData.array;
    int n = fileData.used;

    //int n = 4;

    /*for(int i = 0; i < n; i++) {
    	printf("Arrival\t: %d, %d\n", processDatas[i].arrival, fileData.array[i].arrival);
    }

    printArray(processDatas, n);*/

    // Sort processDatas according to arrival times.
    quickSortProcessDatas(processDatas, 0, n - 1);

    printArray(processDatas, n);

    int fcfs = fcfsAverageTurnaround(n, processDatas);
    int sjf = sjfAverageTurnaround(n, processDatas);
    int srjf = srjfAverageTurnaround(n, processDatas);
    int rr = rrAverageTurnaround(n, processDatas, 10); // TODO: Get quantum

    // Print the results
    printf("FCFS %d\n", fcfs);
    printf("SJF %d\n", sjf);
    printf("SRJF %d\n", srjf);
    printf("RR %d\n", rr);

    //freeArray(&fileData);

    return 0;
}
//************************ MAIN END ************************//
