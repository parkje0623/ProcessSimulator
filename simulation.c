#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "list.h"

#define MAX_MSG_SIZE 40
#define SEM_SIZE 5
//states
#define READY 0
#define RUNNING 1
#define BLOCKED 2
//msg type
#define SEND 3
#define RECV 4
#define REPLY 5
//Lists
LIST *priority_Q[3];
LIST *block_send;
LIST *block_recv;

typedef struct PCB {
	int pid;
	int priority;
	int state;
	char *proc_msg;
} PCB;

typedef struct SEM {
	int sid;
	int value;
	LIST *semList;
} SEM;

PCB *init_process;
PCB *item;
PCB *copy;
PCB *search;
SEM *semaphore[5];
int priority;
int pid;
int sid;
int semValue;
int pid_counter = 0;
int high;
int norm;
int low;
int fork_Q;
int quantum_Q;
int quantum_pid;
char *process;
char message[MAX_MSG_SIZE];

void starting_menu() {
	printf("WELCOME TO SIMULATION PROGRAM!\n");
	printf("Commands are as follows: \n");
	printf("C for Create: creates a process\n");
	printf("F for Fork: Copy currently running process and put it on ready Q corresponding to original process\n");
	printf("K for Kill: Kill named process and remove from the system\n");
	printf("E for Exit: Kill currently running process\n");
	printf("Q for Quantum: Time quantum of running process expires\n");
	printf("S for Send: Send message\n");
	printf("R for Receive: Receive message\n");
	printf("Y for Reply: Unblocks sender and delivers reply\n");
	printf("N for New Semaphore: Initialize named semaphore with value given\n");
	printf("P for Semaphore P: Execute semaphore P\n");
	printf("V for Semaphore V: Execute semaphore V\n");
	printf("I for Procinfo: Dump complete state information of process\n");
	printf("T for Totalinfo: Display all processes\n");
}

void init() {

	init_process = (PCB *) malloc(sizeof(PCB));
	pid_counter++;
	init_process -> pid = pid_counter;
	init_process -> priority = 0;
	init_process -> state = RUNNING;
	//init_process -> proc_msg = (char *) malloc(sizeof(char) *40);

}

void create(int priority) {

	item = (PCB *) malloc(sizeof(PCB));
	pid_counter++;
	item -> pid = pid_counter;
	item -> priority = priority;
	item -> state = READY;
	item -> proc_msg = (char *) malloc(sizeof(char) *40);
	fork_Q = priority;
	quantum_Q = priority;
	quantum_pid = pid_counter;
	ListAppend(priority_Q[priority], item);

	printf("Successfully, created\n");
	printf("Current process amount: %d\n", ListCount(priority_Q[priority]));

}

int fork() {

	if (pid_counter <= 1) {
		printf("Fail to fork, we do not want to fork the initial process.\n");
		return 1;
	}

	//Current process
	PCB *temp = ListCurr(priority_Q[fork_Q]);

	//Copy current process and add it on a ready queue
	copy = (PCB *) malloc(sizeof(PCB));
	pid_counter++;
	copy -> pid = pid_counter;
	copy -> priority = temp -> priority;
	copy -> state = temp -> state;
	copy -> proc_msg = temp -> proc_msg;
	ListAppend(priority_Q[copy -> priority], copy);
	printf("Fork finished, copied PCB stored in a ready queue.\n");

	quantum_Q = fork_Q;
	quantum_pid = pid_counter;
	return 0;
}

void kill_process(){

	ListFree(priority_Q[high], NULL);
	ListFree(priority_Q[norm], NULL);
	ListFree(priority_Q[low], NULL);
	ListFree(block_send, NULL);
	ListFree(block_recv, NULL);

	for (int i = 0; i < SEM_SIZE; i++){
		if (semaphore[i] != NULL){
			ListFree(semaphore[i] -> semList, NULL);
		}
	}

	printf("Successfully killed all processes\n");
}

void kill_blocked_process(int pid) {

	for (int i = 0; i < 3; i++){
		ListFirst(priority_Q[i]);
		for (int j = 0; j < ListCount(priority_Q[i]); j++){
			PCB *temp = ListCurr(priority_Q[i]);  
			if (strlen(temp -> pid) == pid) {
				ListRemove(priority_Q[i]);
				pid_counter--;
				printf("Process %d, has been deleted succesfully.\n", pid);
				break;
			}
			ListNext(priority_Q[i]);
		}
	}

}

int kill(int pid) {

	if (pid == 1 && pid_counter == 1) {
		kill_process();
		exit(0);
	} else if (pid == 1 && pid_counter > 1) {
		printf("Cannot kill an initial process because other processes exists.\n");
		return 1;
	}

	int pid_state;
	PCB *temp;
	for (int i = 0; i < 3; i++) {
		ListFirst(priority_Q[i]);
		for (int j = 0; j < ListCount(priority_Q[i]); j++) {
			temp = ListCurr(priority_Q[i]);  
			if (temp -> pid == pid) {
				pid_state = temp -> state;
				break;
			}
			ListNext(priority_Q[i]);
		}
	}
	
	if (pid_state == READY) {

		ListRemove(priority_Q[temp -> priority]);
		pid_counter--;
		printf("Process %d killed.\n", pid);
		fork_Q = temp -> priority;
		quantum_Q = temp -> priority;
		quantum_pid = pid - 1;
		return 0;
	} else if (pid_state == BLOCKED) {
		int *blocked_process = -1;
		ListFirst(block_send);
		for (int i = 0; i < ListCount(block_send); i++) {
			if (ListCurr(block_send) == pid) {
				blocked_process = ListRemove(block_send);
				pid_counter--;
				break;
			}
			ListNext(block_send);
		}

		if (blocked_process == -1) {
			ListFirst(block_recv);
			for (int i = 0; i < ListCount(block_recv); i++) {
				if (ListCurr(block_recv) == pid) {
					blocked_process = ListRemove(block_recv);
					pid_counter--;
					break;
				}
				ListNext(block_recv);
			}
		}

		if (blocked_process == -1) {
			for (int i = 0; i < 5; i++) {
				ListFirst(semaphore[i] -> semList);
				for (int j = 0; j < ListCount(semaphore[i] -> semList); j++){
					if (ListCurr(semaphore[i] -> semList) == pid) {
						blocked_process = ListRemove(semaphore[i] -> semList);
						pid_counter--;
						break;
					}
					ListNext(semaphore[i] -> semList);
				}	
			}
		}

		if (blocked_process != -1) {
			kill_blocked_process(blocked_process);
			return 0;
		}

	} else if (pid_state == RUNNING) {
		exit_process();
	} else {
		printf("Cannot kill a process, because such PID does not exist.\n");
		return 1;
	}

	return 0;
}

int exit_process() {

	if (pid_counter == 1) {
		kill_process();
		exit(0);
	}
	
	PCB *temp = ListCurr(priority_Q[quantum_Q]);
	while (temp -> pid != quantum_pid) {
		temp -> pid = ((PCB *)ListNext(priority_Q[quantum_Q])) -> pid;
	}

	printf("Sucessfully exited and removed PID: %d\n", quantum_pid);
	ListRemove(priority_Q[quantum_Q]);
	quantum_pid = (temp -> pid) - 1;
	pid_counter--;
	quantum();
	return 0;
}

int quantum() {

	if (pid_counter == 1) {
		printf("Quantum function does not work for initial process since it's not stored in a queue.\n");
		return 1;
	}

	if (quantum_pid != 1) {
		int copy_Q = quantum_Q;
		printf("Currently running process's priority will be demoted and added back to end of the new priority list.\n");
		//Demote priority only if current priority is high or norm
		if (quantum_Q != 2) {
			quantum_Q++;
		}
		//Remove the currently running process and add it back with the new priority assigned
		item = (PCB *) malloc(sizeof(PCB));
		item -> pid = quantum_pid;
		item -> priority = quantum_Q;
		item -> state = READY;
		item -> proc_msg = ((PCB *)ListCurr(priority_Q[copy_Q])) -> proc_msg;
		ListAppend(priority_Q[quantum_Q], item);

		ListFirst(priority_Q[copy_Q]);
		while (((PCB *)ListCurr(priority_Q[copy_Q])) -> pid != quantum_pid){
			ListNext(priority_Q[copy_Q]);
		}
		ListRemove(priority_Q[copy_Q]);

		//Assign process with higher priority to be the current process running
		if (ListCount(priority_Q[0]) != 0){
			quantum_Q = 0;
			fork_Q = 0;
			ListFirst(priority_Q[quantum_Q]);
			quantum_pid = ((PCB *) ListCurr(priority_Q[quantum_Q])) -> pid;
		} else if (ListCount(priority_Q[1]) != 0) {
			quantum_Q = 1;
			fork_Q = 1;
			ListFirst(priority_Q[quantum_Q]);
			quantum_pid = ((PCB *) ListCurr(priority_Q[quantum_Q])) -> pid;
		} else {
			quantum_Q = 2;
			fork_Q = 2;
			ListFirst(priority_Q[quantum_Q]);
			quantum_pid = ((PCB *) ListCurr(priority_Q[quantum_Q])) -> pid;
		} 

		((PCB *) ListCurr(priority_Q[quantum_Q])) -> state = RUNNING;
		printf("New currently running process is now: %d\n",  quantum_pid);
	}

	return 0;
}

int send(int pid, char *msg){

	if (pid > pid_counter) {
		printf("Cannot send a message, because such PID does not exist.\n");
		return 1;
	}
	
	ListFirst(block_recv);
	for (int i = 0; i < 3; i++){
		ListFirst(priority_Q[i]);

		//Unblock receiver if the receiver is blocked
		for (int k = 0; k < ListCount(block_recv); k++) {
			if (ListCurr(block_recv) == pid) {
				ListRemove(block_recv);
				break;
			}
			ListNext(block_recv);
		}

		for (int j = 0; j < ListCount(priority_Q[i]); j++) {
			PCB *temp = ListCurr(priority_Q[i]);
			//Finds pid position and delivers message to that pid
			if (temp -> pid == pid){
				((PCB *)ListCurr(priority_Q[i])) -> state = RUNNING;
				strcpy(((PCB *)ListCurr(priority_Q[i])) -> proc_msg, msg);
				printf("Message sent.\n");
				//Set the current pid state to be blocked, sender is blocked
				if (pid != 1) {
					ListAppend(block_send, (void *) pid);
					((PCB *)ListCurr(priority_Q[i])) -> state = BLOCKED;
					printf("Sending message blocked on process %d\n", pid);
					fork_Q = i;
					quantum_Q = i;
					quantum_pid = pid;
					quantum();
					return 0;
				}
			}
			ListNext(priority_Q[i]);
		}
	}

	return 0;
}

int receive() {

	if (pid_counter <= 1) {
		printf("Cannot receive when no processes are in queue.\n");
		return 1;
	}

	for (int i = 0; i < 3; i++){
		ListFirst(priority_Q[i]);
		for (int j = 0; j < ListCount(priority_Q[i]); j++){
			PCB *temp = ListCurr(priority_Q[i]);  
			//Block receiver until actual message is sent
			if (strlen(temp -> proc_msg) == 0) {
				ListAppend(block_recv, (void *) temp -> pid);
				((PCB *) ListCurr(priority_Q[i])) -> state = BLOCKED;
				printf("Receiving message blocked on process %d\n", temp -> pid);
			} else {
				//Prints message  received
				printf("From process: %d, Message received: %s\n", temp -> pid, temp -> proc_msg);
			}

			ListNext(priority_Q[i]);
		}
	}

	return 0;
}

int reply(int pid, char *msg) {

	if (pid_counter <= 1) {
		printf("Cannot reply when no processes are in queue.\n");
		return 1;
	}

	ListFirst(block_send);
	ListFirst(block_recv);

	for (int i = 0; i < 3; i++){
		ListFirst(priority_Q[i]);
		for (int j = 0; j < ListCount(priority_Q[i]); j++){
			PCB *temp = ListCurr(priority_Q[i]); 

			//Reply to sender, sender is set back to ready state
			//Reply to receiver, receiver is set back to ready state
			if (temp -> state = BLOCKED && temp -> pid == pid) {
				((PCB *) ListCurr(priority_Q[i])) -> state = READY;
				printf("Message replied to sender and receiver, sender and receiver are now back to ready.\n");
				printf("Replied Message: %s, to process number: %d\n", msg, pid);
				quantum_Q = i;
				quantum_pid = pid;
				fork_Q = i;
				break;
			}

			ListNext(priority_Q[i]);
		}
	}

	while (ListCurr(block_send) != NULL) {
		if (ListCurr(block_send) == pid) {
			ListRemove(block_send);
			break;
		}
		ListNext(block_send);
	}

	while (ListCurr(block_recv) != NULL) {
		if (ListCurr(block_recv) == pid) {
			ListRemove(block_recv);
			break;
		}
		ListNext(block_recv);
	}
	return 0;
}

int new_Semaphore(int semaphore_ID, int initial) {

	if (semaphore_ID < 0 || semaphore_ID > 4) {
		printf("Not a valid semaphore_ID, must enter ID number from 0 to 4.\n");
		return 1;
	} 

	if (initial < 0) {
		printf("Not a valid intial value, must enter a value that is 0 or higher\n");
		return 1;
	}

	if (semaphore[semaphore_ID] != NULL) {
		printf("Fail to initialize semaphore, semaphore already exists for the current semaphore_ID.\n");
		return 1;
	}

	//Only initialize semaphore with given parameter of initial value when semaphoreID is NULL.
	if (semaphore[semaphore_ID] == NULL) {
		SEM *sem = (SEM *) malloc(sizeof(SEM));
		sem -> sid = semaphore_ID;
		sem -> value = initial;
		sem -> semList = ListCreate();
		semaphore[semaphore_ID] = sem;
		printf("Initializing Semaphore Complete with inital value of: %d\n", initial);
	} 

	return 0;
}

int semaphore_P(int semaphore_ID) {

	if (semaphore[semaphore_ID] == NULL) {
		printf("No semaphore created yet.\n");
		return 1;
	}

	if (pid_counter <= 1) {
		printf("Cannot execute semaphore_P when no process in queue.\n");
		return 1;
	}

	(semaphore[semaphore_ID] -> value)--;
	if (semaphore[semaphore_ID] -> value < 0) {
		//Store current pid in location of semaphoreID
		ListAppend(semaphore[semaphore_ID] -> semList, (void *) quantum_pid);
		((PCB *) ListCurr(priority_Q[quantum_Q])) -> state = BLOCKED;
		printf("Successfully blocked process %d for semaphore_ID: %d\n", quantum_pid, semaphore_ID);
		quantum();
	} else {
		printf("Not blocking process, the semaphore value, %d, not smaller than 0 yet.\n");
		return 1;
	}
	
	return 0;
}

int semaphore_V(int semaphore_ID) {
	
	if (pid_counter <= 1) {
		printf("Cannot execute semaphore_V when no process in queue.\n");
		return 1;
	}

	if (semaphore[semaphore_ID] == NULL) {
		printf("No semaphore created yet.\n");
		return 1;
	}
	
	int unblocked_process;
	(semaphore[semaphore_ID] -> value)++;
	if (semaphore[semaphore_ID] -> value <= 0) {
		ListFirst(semaphore[semaphore_ID] -> semList);
		unblocked_process = ListRemove(semaphore[semaphore_ID] -> semList);
		for (int i = 0; i < 3; i++) {
			ListFirst(priority_Q[i]);
			for (int j = 0; j < ListCount(priority_Q[i]); j++) {
				PCB *temp = ListCurr(priority_Q[i]);
				if (temp -> pid == unblocked_process) {
					((PCB *) ListCurr(priority_Q[i])) -> state = READY;	
					printf("Sucessfully unblocked process %d, in semaphore %d. Ready to use the process again.\n", temp -> pid, semaphore_ID);
					return 0;	
				}
				ListNext(priority_Q[i]);
			}
		}
	} else {
		printf("Semaphore value is not <= 0 yet.\n");
		return 1;
	}
	
	return 0;
}

void print_priority(int priority) {

	if (priority == 0) {
       		printf(" Priority: High,");
        } else if (priority == 1) {
       		printf(" Priority: Normal,");
        } else {
                printf(" Priority: Low,");  
       	}

}

void print_state(int state) {

	if (state == READY) {
		printf(" State: READY\n");
	} else if (state == BLOCKED) {
		printf(" State: BLOCKED\n");
	} else {
		printf(" State: RUNNING\n");
	}

}

int procinfo(int pid) {

	if (pid < 1) {
		printf("There is no process ID less than 1");
		return 1;
	}	

	if (pid == 1) {
		printf("There is only initial process.\n");
		printf("PID: 1, Priority: High, State: RUNNING\n");
		return 0;
	} 

	for (int i = 0; i < 3; i++) {
		ListFirst(priority_Q[i]);
		for (int j = 0; j < ListCount(priority_Q[i]); j++) {
			PCB *temp = ListCurr(priority_Q[i]);
			if (temp -> pid == pid) {
				printf("Current Process: %d\n", pid);
				printf("PID: %d,", pid);
				print_priority(temp -> priority);
				print_state(temp -> state);
				printf("Message: %s\n", temp -> proc_msg);
				return 0;
			}
		}
	}

	return 0;
}

int totalinfo() {

	if (pid_counter == 1) {
		printf("There is only initial process in the system.\n");
		printf("PID: 1, Priority: High, State: RUNNNING\n");
		return 0;
	}
	
	printf("Initial process:\n");
	printf("PID: 1, Priority: High, State: RUNNNING\n\n");
	
	for (int i = 0; i < 3; i++) {
		ListFirst(priority_Q[i]);
		for (int j = 0; j < ListCount(priority_Q[i]); j++) {
			PCB *temp = ListCurr(priority_Q[i]);
			printf("Current Process: %d\n", temp -> pid);
			printf("PID: %d,", temp -> pid);
			print_priority(temp -> priority);
			print_state(temp -> state);
			printf("Message: %s\n\n", temp -> proc_msg);
			ListNext(priority_Q[i]);
		}
	}

	for (int i = 0; i < 5; i++) {
		if (semaphore[i] == NULL) {
			printf("Semaphore %d not created, empty.\n", i);
		} else {
			printf("Semaphore number %d, process ID of: %d, semaphore value of: %d\n", i, semaphore[i] -> sid, semaphore[i] -> value);
		}
	}
	
	if (ListCount(block_send) != 0) {
		ListFirst(block_send);
		printf("Process ID that are blocked on sending: ");
		for (int i = 0; i < ListCount(block_send); i++) {
			printf("%d, ", ListCurr(block_send));
		}
		printf("\n");
	} else {
		printf("There are no process being blocked on sending.\n");
	}
	
	if (ListCount(block_recv) != 0) {
		ListFirst(block_recv);
		printf("Process ID that are blocked on receiving: ");
		for (int i = 0; i < ListCount(block_recv); i++) {
			printf("%d, ", ListCurr(block_recv));
		}
		printf("\n");
	} else {
		printf("There are no process being blocked on receiving.\n");
	}

	return 0;
}

int main(int argc, char *argv[]) {

	starting_menu();
	init();

	high = 0;
	norm = 1;
	low = 2;
	priority_Q[high] = ListCreate();
	priority_Q[norm] = ListCreate();
	priority_Q[low] = ListCreate();
	block_send = ListCreate();
	block_recv = ListCreate();
	
	for (int i = 0; i < SEM_SIZE; i++){
		semaphore[i] = NULL;
	}

	while (1) {
		char command;
		printf("Enter a command: ");
		scanf(" %c", &command);

		switch(toupper(command)) {
			case 'C':
				while (1) {
					printf("Enter Priority, 0, 1, or 2: ");
					scanf("%d", &priority);
					if (priority >= 0 && priority < 3){
						break;
					}
				}
				create(priority);
				break;
			case 'F':
				fork();
				break;
			case 'K':
				printf("Enter pid: ");
				scanf("%d", &pid);
				kill(pid);
				break;
			case 'E':
				exit_process();
				break;
			case 'Q':
				quantum();
				break;
			case 'S':
				printf("Enter pid: ");
				scanf("%d", &pid);
				printf("Enter message: ");
				scanf(" %[^\n]s", message);
				send(pid, message);
				break;
			case 'R':
				receive();
				break;
			case 'Y':
				printf("Enter pid: ");
				scanf("%d", &pid);
				printf("Enter message: ");
				scanf(" %[^\n]s", message);
				reply(pid, message);
				break;
			case 'N':
				printf("Semaphore ID: ");
				scanf("%d", &sid);
				printf("Semaphore value: ");
				scanf("%d", &semValue);
				new_Semaphore(sid, semValue);
				break;
			case 'P':
				printf("Semaphore ID: ");
				scanf("%d", &sid);
				semaphore_P(sid);
				break;
			case 'V':
				printf("Semaphore ID: ");
				scanf("%d", &sid);
				semaphore_V(sid);
				break;
			case 'I':
				printf("pid: ");
				scanf("%d", &pid);
				procinfo(pid);
				break;
			case 'T':
				totalinfo();
				break;
			case '\n':
				break;
			default:
				printf("Invalid Command\n");
				break;
		}
	}

}
