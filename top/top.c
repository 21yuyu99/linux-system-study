#include <stdio.h>
#include <time.h>
#include <stdlib.h> //system(), qsort, atoi
#include <unistd.h> //sleep
#include <ctype.h> //isdigit
#include <dirent.h> //DIR
#include <string.h>
typedef struct {
    int pid;
    char name[256];
    long utime;
    long stime;
    float cpu_usage;
} ProcessInfo;
long prev_cpu_total = 0;
long prev_idle = 0;
long total_cpu_delta = 0;
ProcessInfo proc_db[1024];
long prev_proc_ticks[1000000] = {0, };
int compare_procs(const void *a, const void *b){
    ProcessInfo *procA = (ProcessInfo *)a;
    ProcessInfo *procB = (ProcessInfo *)b;
    if(procA->cpu_usage>procB->cpu_usage) return -1;
    if(procA->cpu_usage<procB->cpu_usage) return 1;
    return 0;
}
int get_memory_info(){
    FILE *mem_fp = fopen("/proc/meminfo","r");
    long total_mem = 0;
    long free_mem = 0;
    long avail_mem = 0;
    if (mem_fp == NULL) return 1;
    char line[256];
    while (fgets(line,sizeof(line),mem_fp)){
        if(strncmp(line,"MemTotal:",9) == 0){
            sscanf(line,"%*s %ld", &total_mem);
        }
        else if(strncmp(line,"MemAvailable:",13) == 0){
            sscanf(line,"%*s %ld",&avail_mem);
        }
    }
    fclose(mem_fp);    
    printf("Memory Usage : %.2f\n",(double)(total_mem-avail_mem)/total_mem*100.0);
    return 0;
}
int get_cpu_info(){
    FILE *cpu_fp = fopen("/proc/stat","r");
    char line[256];
    long u, n, sys, idle, io, irq, sirq;
    long idle_delta = 0;
    if(cpu_fp==NULL) return 1;
    if(fgets(line,sizeof(line),cpu_fp)){
        sscanf(line,"%*s %ld %ld %ld %ld %ld %ld %ld", &u, &n, &sys, &idle, &io, &irq, &sirq);      
        long current_cpu_total = u + n + sys + idle + io + irq + sirq;
        total_cpu_delta = current_cpu_total - prev_cpu_total;
        prev_cpu_total = current_cpu_total;
        idle_delta = idle - prev_idle;
        prev_idle = idle;
     }   
    fclose(cpu_fp);
    printf("Total Cpu Usage : %.2f\n",(double)(total_cpu_delta-idle_delta)/total_cpu_delta*100.0);
    return 0;
}
int get_process_list(){
    DIR *dir = opendir("/proc");
    struct dirent *entry;
    int db_count = 0;
    if (dir == NULL ) return 1;
    while((entry = readdir(dir)) != NULL && db_count < 1024){
        if(isdigit(entry->d_name[0])){ //isdigit는 문자 하나 비교함
            //stat파일 열기
            char stat_path[512];
            snprintf(stat_path, sizeof(stat_path), "/proc/%s/stat", entry->d_name);
            FILE *stat_fp = fopen(stat_path, "r");
            if(stat_fp == NULL) return 1;
            char line[256];
            if(fgets(line,sizeof(line),stat_fp)){
                //PID, 파일명, utime, stime 가져오기
                sscanf(line,"%d (%[^)]) %*c %*d %*d %*d %*d %*d %*u %*u %*u %*u %*u %ld %ld", &proc_db[db_count].pid,proc_db[db_count].name,&proc_db[db_count].utime,&proc_db[db_count].stime);
                long current_total = proc_db[db_count].utime + proc_db[db_count].stime;
                proc_db[db_count].cpu_usage = (double)(current_total - prev_proc_ticks[proc_db[db_count].pid])/total_cpu_delta*100.0;
                prev_proc_ticks[proc_db[db_count].pid] = current_total;
                ++db_count;
            }
            fclose(stat_fp);
        }
    }
    closedir(dir);
    qsort(proc_db,db_count,sizeof(ProcessInfo),compare_procs);
    printf("%-10s %-20s %-20s\n", "PID", "NAME", "CPU usage");
    printf("------------------------------------------------\n");
    for(int i=0;i<db_count;i++){
        printf("%-10d %-20s %-20.2f\n", proc_db[i].pid, proc_db[i].name, proc_db[i].cpu_usage);
    }
    return 0;
};
int main(){

    time_t now;
    struct tm *t;
    while(1){
        system("clear");
        printf("-----System Monitor-----\n");
        time(&now);
        t = localtime(&now);
        printf("현재 시간 : %d년 %d월 %d일%02d:%02d:%02d\n",
        t->tm_year + 1900, t->tm_mon + 1, t->tm_mday,
        t->tm_hour, t->tm_min, t->tm_sec);
        get_memory_info();
        get_cpu_info();
        get_process_list();
        sleep(1);
    }
    return 0;
}
