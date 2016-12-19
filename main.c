#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <signal.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <net/if.h>
#include <arpa/inet.h>
#include "get_usage.h"

#define IF_NAME "bond0"
#define DST_IP "40.10.5.1"
#define SERVER_PORT 6666
#define BUFFER_LEN 256

char *trimwhitespace(char *str)
{
    char *end;

    // Trim leading space
    while(isspace(*str)) str++;

    if(*str == 0)
        return str;



    end = str + strlen(str) - 1;
    while(end > str && isspace(*end)) end--;

    // Write new null terminator
    *(end+1) = 0;


    return str;
}



void err_exit(const char *err)
{
    printf("%s\n", err);
    exit(1);
}


int get_ip(char *ip)
{
    int fd;
    struct ifreq ifr;

    fd = socket(AF_INET, SOCK_DGRAM, 0);
    if(fd == -1)
    {
        //perror("socket create failed");
        return -1;
    }

    ifr.ifr_addr.sa_family = AF_INET;
    strncpy(ifr.ifr_name, IF_NAME, IFNAMSIZ-1);

    ioctl(fd, SIOCGIFADDR, &ifr);

    close(fd);


    strcpy(ip, inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr));

    return 0;
}

int get_mac(const char *iface, char *mac)
{

    FILE *fp;

    char filename[512] = {0};
    sprintf(filename, "/sys/class/net/%s/address", iface);

    fp = fopen(filename, "r");
    if(fp == NULL)
    {
        //printf("open file failed.\n");
        return -1;
    }

    fscanf(fp, "%s", mac);

    fclose(fp);

    return 0;
}


int get_agent_status(const char *service)
{
    char cmd[200] = {0};
    char line[256] = {0};

    snprintf(cmd, 200, "service %s status", service);


    FILE *fp;

    fp = popen(cmd, "r");
    if(fp == NULL)
    {
        //printf("popen failed.\n");
        return -1;
    }

    while(fgets(line, 256, fp) != NULL)
    {
        break;
    }
    pclose(fp);


    if(strstr(line, "running"))
    {
        return 1;
    }

    //default: 0 stop
    return 0;
}

int main(int argc, char **argv)
{
    int sockfd;
    //int n;

    char sendline[BUFFER_LEN] = {0};
    char ip_addr[16] = {0};
    char mac_addr[18] = {0};
    char ihostname[256] = {0};
    long double cpu_usage = 0; 
    struct sockaddr_in servaddr;


    #if 0
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd < 0)
    {
        err_exit("create socket error");
    }

    bzero(&servaddr,sizeof servaddr);

    servaddr.sin_family = AF_INET;
    servaddr.sin_port=htons(SERVER_PORT);
    inet_pton(AF_INET, DST_IP, &(servaddr.sin_addr));

    int ret = connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr));
    if(ret == -1)
    {
        //perror("connect()");
        return -1;
    }
    #endif

    char sendline_dup[1000] = {0};
    char cpu_mem[512] = {0};
    char *servicename= "ssh_agent";    
    get_ip(ip_addr);
    get_mac(IF_NAME, mac_addr);
    gethostname(ihostname, sizeof(ihostname));
    
    cpu_usage = get_system_cpu_usage();
    
    snprintf(sendline_dup, sizeof(sendline_dup), "19 %s %s %s %Lf %f %f", ip_addr, mac_addr, ihostname, 
                                                          cpu_usage, get_mem_info(), get_disk_usage("/"));

    printf("the msg we send: %s.\n", sendline_dup);
    #if 0
    int n = write(sockfd, sendline_dup, strlen(sendline_dup));
    printf("we have written %d bytes data.\n", n);

    close(sockfd);
    #endif

    return 0;
}
