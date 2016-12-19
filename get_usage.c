#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/sysinfo.h>
#include <sys/statvfs.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <netdb.h>
#include <syslog.h>

extern char *trimwhitespace(char *str);


long double get_system_cpu_usage()
{
    long double a[4], b[4], loadavg;
    FILE *fp;
    printf("%s begins.\n", __FUNCTION__);    

    //for(;;)
    {
        fp = fopen("/proc/stat","r");
        fscanf(fp,"%*s %Lf %Lf %Lf %Lf",&a[0],&a[1],&a[2],&a[3]);
        fclose(fp);
        sleep(1);

        fp = fopen("/proc/stat","r");
        fscanf(fp,"%*s %Lf %Lf %Lf %Lf",&b[0],&b[1],&b[2],&b[3]);
        fclose(fp);

        loadavg = ((b[0]+b[1]+b[2]) - (a[0]+a[1]+a[2])) / ((b[0]+b[1]+b[2]+b[3]) - (a[0]+a[1]+a[2]+a[3]));
        printf("The current CPU utilization is : %Lf\n",loadavg);

        return loadavg;
    }

}


double get_mem_info()
{
    //printf("%s begins.\n", __FUNCTION__);
    struct sysinfo info;
    

    int ret = sysinfo(&info);
    if(ret != 0)
    {
        return 0.0;
    }

    //printf("Uptime: %ld:%ld:%ld\n", info.uptime/3600, info.uptime%3600/60, info.uptime%60);
    //printf("Total RAM: %ld MB\n", info.totalram/1024/1024);
    //printf("usage RAM: %ld MB\n", (info.totalram-info.freeram)/1024/1024);
    //printf("Process count: %d\n", info.procs);
    //printf("Page size: %ld bytes\n", sysconf(_SC_PAGESIZE));

    return ((double)info.totalram - info.freeram) / info.totalram;
}

double get_disk_usage(const char * path)
{
    struct statvfs vfs;

    if(statvfs(path, &vfs) != 0)
    {
        perror("statvfs");
        return 0;
    }
    
    double used = vfs.f_blocks - vfs.f_bfree;
    
    printf("total bolkcs %lu.\n", vfs.f_blocks);
    printf("free blocks %lu.\n", vfs.f_bfree);    

    double used_percent = used /(used + vfs.f_bavail);
    printf("the disk used percent is : %f.\n", used_percent);



    return used_percent;
}

int get_cpu_and_mem(const char * service_cmd, char *cpu_mem)
{
    char ps_cmd[200] = {0};
    char line[300] = {0};

    snprintf(ps_cmd, 200, "ps aux| grep %s | awk \'{print $3,$4,$11}\'", service_cmd);


    FILE *fp = NULL;

    fp = popen(ps_cmd, "r");
    if(fp == NULL)
    {
        printf("popen failed.\n");
        return;
    }

    while(fgets(line, sizeof(line), fp) != NULL)
    {

        if(strstr(service_cmd, "ssh"))
        {
            #if DEBUG
            //printf("the line does not include sshd keyword: %s\n", line);
            #endif

            // ssh related service
            if(strstr(line, service_cmd))
            {
                trimwhitespace(line);
                char *space_p = strrchr(line, ' ');


                strncpy(cpu_mem, line, space_p - line);
            }


            #if 0
            char *space_p = strrchr(line, ' ');
            space_p++;

            if( strncmp(space_p, "/root/") == 0 && strchr(line, ';') == NULL)
            {

                awk '{print $2}'
            }
            #endif

        }
    }

    pclose(fp);

    #if 0
    get_mem_info();
    get_system_cpu_usage();
    #endif
    return 0;
}
