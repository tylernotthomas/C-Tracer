#include "trace.h"

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <sys/types.h>

//#define MAX_CHAR 20

struct event{
	char* name; //name
	char* cat; //category
	char ph; //phase
	int pid; //process id
	int tid; //thread id
	int ts; //timestamp
	char* s; //scope (for instant events only)
	void* id; //object identifier (for object events only) 
	char* args;
};

struct event eventMemory[10000];
int eventMemoryIndex = 0; //stores the next cell that can be written to in eventMemory
FILE* globalFile;

void handle_SIGSEGV(){
	printf("ENTERING EXCEPTION HANDLER\n");
	trace_flush();
	fclose(globalFile);
}

void checkEventMemory(){
	if(eventMemoryIndex == 10000){
		trace_flush();
		eventMemoryIndex = 0;
	}
}

int getTimeStamp_us(){
	//returns time as a integer in microseconds(us)
	struct timeval time;
	gettimeofday(&time, NULL);
	return time.tv_usec;
}

void trace_start(char* filename){
	struct sigaction handler;
	handler.sa_handler = handle_SIGSEGV; 
	handler.sa_flags = 0;
	sigemptyset(&handler.sa_mask);
	sigaction(SIGSEGV, &handler, NULL);

	printf("TRACE STARTED\n");
	printf("Opening File: %s\n", filename);

	FILE* filePointer;
	filePointer = fopen(filename, "a");
	//printf("[ ");
	fprintf(filePointer, "[ ");
	globalFile = filePointer;
}

void trace_event_start(char* name, char* categories, char* arguments){
	checkEventMemory();

	eventMemory[eventMemoryIndex].name = name;
	eventMemory[eventMemoryIndex].cat = categories;
	eventMemory[eventMemoryIndex].ph = 'B';
	eventMemory[eventMemoryIndex].pid = 1;

	eventMemory[eventMemoryIndex].tid = 1;

	eventMemory[eventMemoryIndex].ts = getTimeStamp_us();

	eventMemory[eventMemoryIndex].s = NULL;
	eventMemory[eventMemoryIndex].id = NULL;
	eventMemory[eventMemoryIndex].args = arguments;

	// printf("{\"name\": \"%s\", ", newEvent.name);
	// printf("\"cat\": \"%s\", ", newEvent.cat);
	// printf("\"ph\": \"%c\", ", newEvent.ph);
	// printf("\"pid\": %d, ", newEvent.pid);
	// printf("\"tid\": %d, ", newEvent.tid);
	// printf("\"ts\": %d},\n", newEvent.ts);

	eventMemoryIndex++;
}

void trace_event_end(char* arguments){
	checkEventMemory();

	eventMemory[eventMemoryIndex].name = NULL;
	eventMemory[eventMemoryIndex].cat = NULL;
	eventMemory[eventMemoryIndex].ph = 'E';
	eventMemory[eventMemoryIndex].pid = 1;
	eventMemory[eventMemoryIndex].tid = 1;

	eventMemory[eventMemoryIndex].ts = getTimeStamp_us();

	eventMemory[eventMemoryIndex].s = NULL;
	eventMemory[eventMemoryIndex].id = NULL;
	eventMemory[eventMemoryIndex].args = arguments;

	// printf("{\"ph\": \"%c\", ", newEnd.ph);
	// printf("\"pid\": %d, ", newEnd.pid);
	// printf("\"tid\": %d, ", newEnd.tid);
	// printf("\"ts\": %d},\n", newEnd.ts);

	eventMemoryIndex++;
}

void trace_instant_global(char* name){
	checkEventMemory();

	eventMemory[eventMemoryIndex].name = name;
	eventMemory[eventMemoryIndex].cat = NULL;
	eventMemory[eventMemoryIndex].ph = 'i';

	eventMemory[eventMemoryIndex].ts = getTimeStamp_us();

	eventMemory[eventMemoryIndex].pid = 1;
	eventMemory[eventMemoryIndex].tid = 1;
	eventMemory[eventMemoryIndex].s = "g";
	eventMemory[eventMemoryIndex].id = NULL;
	eventMemory[eventMemoryIndex].args = NULL;

	eventMemoryIndex++;
}

void trace_object_new(char* name, void* obj_pointer){
	checkEventMemory();

	eventMemory[eventMemoryIndex].name = name;
	eventMemory[eventMemoryIndex].cat = NULL;
	eventMemory[eventMemoryIndex].ph = 'N';

	eventMemory[eventMemoryIndex].ts = getTimeStamp_us();

	eventMemory[eventMemoryIndex].pid = 1;
	eventMemory[eventMemoryIndex].tid = 1;
	eventMemory[eventMemoryIndex].s = NULL;
	eventMemory[eventMemoryIndex].id = obj_pointer;
	eventMemory[eventMemoryIndex].args = NULL;

	eventMemoryIndex++;
}

void trace_object_snap(char* name, void* obj_pointer){
	checkEventMemory();

	eventMemory[eventMemoryIndex].name = name;
	eventMemory[eventMemoryIndex].cat = NULL;
	eventMemory[eventMemoryIndex].ph = 'O';

	eventMemory[eventMemoryIndex].ts = getTimeStamp_us();

	eventMemory[eventMemoryIndex].pid = 1;
	eventMemory[eventMemoryIndex].tid = 1;
	eventMemory[eventMemoryIndex].s = NULL;
	eventMemory[eventMemoryIndex].id = obj_pointer;
	eventMemory[eventMemoryIndex].args = NULL;

	eventMemoryIndex++;
}

void trace_object_gone(char* name, void* obj_pointer){
	checkEventMemory();

	eventMemory[eventMemoryIndex].name = name;
	eventMemory[eventMemoryIndex].cat = NULL;
	eventMemory[eventMemoryIndex].ph = 'D';

	eventMemory[eventMemoryIndex].ts = getTimeStamp_us();

	eventMemory[eventMemoryIndex].pid = 1;
	eventMemory[eventMemoryIndex].tid = 1;
	eventMemory[eventMemoryIndex].s = NULL;
	eventMemory[eventMemoryIndex].id = obj_pointer;
	eventMemory[eventMemoryIndex].args = NULL;

	eventMemoryIndex++;
}

void trace_counter(char* name, char* key, char* value){
	checkEventMemory();

	eventMemory[eventMemoryIndex].name = name;
	eventMemory[eventMemoryIndex].cat = NULL;
	eventMemory[eventMemoryIndex].ph = 'C';

	eventMemory[eventMemoryIndex].ts = getTimeStamp_us();

	eventMemory[eventMemoryIndex].pid = 1;
	eventMemory[eventMemoryIndex].tid = 1;
	eventMemory[eventMemoryIndex].s = NULL;
	eventMemory[eventMemoryIndex].id = NULL;
	eventMemory[eventMemoryIndex].args = key;

	eventMemoryIndex++;
}

void trace_flush(){
	for(int i=0; i<eventMemoryIndex; i++){
		fprintf(globalFile, "{");
	 	if(eventMemory[i].name != NULL){
	 		fprintf(globalFile, "\"name\": \"%s\", ", eventMemory[i].name);
	 	}
	 	if(eventMemory[i].cat != NULL){
	 		fprintf(globalFile, "\"cat\": \"%s\", ", eventMemory[i].cat);
	 	}

	 	fprintf(globalFile, "\"ph\": \"%c\", ", eventMemory[i].ph);
		fprintf(globalFile, "\"pid\": %d, ", eventMemory[i].pid);
		fprintf(globalFile, "\"tid\": %d, ", eventMemory[i].tid);
		fprintf(globalFile, "\"ts\": %d", eventMemory[i].ts);

		if(eventMemory[i].s != NULL){
			fprintf(globalFile, ", \"s\": \"%s\"", eventMemory[i].s);
		}
		if(eventMemory[i].id != NULL){
			fprintf(globalFile, ", \"id\": \"%p\"", eventMemory[i].id);
		}
		if(eventMemory[i].args != NULL){
			fprintf(globalFile, ", \"args\": %s", eventMemory[i].args);
		}
		fprintf(globalFile, "}");

		if(i == eventMemoryIndex-1){
			continue;
		}
		fprintf(globalFile, ",\n");
	}
	fprintf(globalFile, " ]");
	eventMemoryIndex = 0;
}

void trace_end(){
	trace_flush();
	fclose(globalFile);
	printf("TRACE ENDED\n");
}
