#include "trace.h"

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <sys/wait.h>
#include <sys/types.h>

void print_foo(){
	printf("foo\n");
}

int main(){
	//char fileName[20] = "TylerTrace.json";
	trace_start("TylerTrace.json");

	trace_event_start("fooLoop", "loop", "{\"name\": \"Tyler\"}");
	char* myName = "Tyler :)";
	trace_object_new("objMyName", myName);
	printf("myName: %s\n", myName);
	for(int i=0; i<10; i++){
		trace_event_start("print_foo", "print", NULL);
	 	print_foo();
	 	trace_event_end("{\"name\": \"Tyler\"}");
	}
	trace_object_gone("objMyName", myName);

	trace_instant_global("finishLoop");
	trace_event_end(NULL);

	trace_end();

	return 0;
}