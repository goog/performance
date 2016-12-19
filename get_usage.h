#ifndef GET_USAGE_H_
#define GET_USAGE_H_



extern int get_cpu_and_mem(const char *service_cmd, char *cpu_mem);
extern long double get_system_cpu_usage();
extern double get_mem_info();
extern double get_disk_usage(const char * path);

#endif
