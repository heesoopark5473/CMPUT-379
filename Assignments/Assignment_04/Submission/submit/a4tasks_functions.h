int     file_exists(char* filename);
char*   trimwhitespace(char *str);
void    store_resources(char* line);
void    store_tasks(char* line, int index);
void*   task_thread(void *arg);
void*   monitor_thread(void* arg);
