#ifndef TRACE_H   
#define TRACE_H

void trace_start(char* filename);
void trace_event_start(char* name, char* categories, char* arguments);
void trace_event_end(char* arguments);
void trace_instant_global(char* name);
void trace_object_new(char* name, void* obj_pointer);
void trace_object_gone(char* name, void* obj_pointer);
void trace_counter(char* name, char* key, char* value);
void trace_flush();
void trace_end();

#endif