#include <stdio.h>
#include <dirent.h> //opendir, readdir
#include <ctype.h> //isdigit
#include <string.h>
#include <stdlib.h> //qsort, atoi
#include <unistd.h> //sleep
typedef struct {

    int pid;
    //char name[256];
    long utime;
    long stime;
    long current_total_time; //utime+stime;
    float cpu_usage;

} ProcessInfo;

int compare_procs(const void *a, const void *b){ //qsort는 void*만 받을 수 있음
    ProcessInfo *procA = (ProcessInfo *)a;
    ProcessInfo *procB = (ProcessInfo *)b;
    
    if(procA->cpu_usage > procB->cpu_usage) return -1; //순서 변경X
    if(procA->cpu_usage < procB->cpu_usage) return 1; //순서 변경O
    return 0;
}

int main() {
    long prev_proc_ticks[1000000] = {0, };
    DIR *dir;
    struct dirent *entry; //dirent는 Directory Entry 구조체
    ProcessInfo procs[1024]; 
    long u, n, sys, id, io, irq, sirq;
    long prev_cpu_total = 0;
    long current_cpu_total = 0;
    long total_cpu_delta = 0;
    while(1){   
        FILE *cpu_fp  = fopen("/proc/stat","r");
        char cpu_line[256];
        if(cpu_fp!=NULL){
            if(fgets(cpu_line, sizeof(cpu_line),cpu_fp)){
                sscanf(cpu_line,"cpu %ld %ld %ld %ld %ld %ld %ld", &u, &n, &sys, &id, &io, &irq, &sirq);  
                current_cpu_total = u + n + sys + id + io + irq + sirq;
                total_cpu_delta = current_cpu_total - prev_cpu_total;
                prev_cpu_total = current_cpu_total;
          }
        }

        int count = 0;
        dir = opendir("/proc");
        if (dir == NULL) return 1;
        while((entry = readdir(dir)) != NULL && count < 1024){
            
            if (isdigit(entry->d_name[0])) {
                char path[512];
                snprintf(path, sizeof(path), "/proc/%s/stat", entry->d_name); //문자열 만들어 path에 저장

                FILE *fp = fopen(path, "r");
                if (fp != NULL) {
                    char line[256];
                    if (fgets(line, sizeof(line), fp)) {
                        procs[count].pid = atoi(entry->d_name);
                        sscanf(line, "%*d %*s %*c %*d %*d %*d %*d %*d %*u %*u %*u %*u %*u %ld %ld",&procs[count].utime,&procs[count].stime);
                        procs[count].current_total_time = procs[count].utime + procs[count].stime;
                        procs[count].cpu_usage = (double)(procs[count].current_total_time-prev_proc_ticks[procs[count].pid]) / total_cpu_delta * 100.0;
                        prev_proc_ticks[procs[count].pid] = procs[count].current_total_time;
                        count++;
                    }
                    fclose(fp); //항상 파일 꼭 닫기
                }
            }
        }
        closedir(dir);

        qsort(procs, count, sizeof(ProcessInfo), compare_procs);

        system("clear");     
        printf("%-10s %-20s\n", "PID", "CPU usage");
        printf("--------------------------------\n");
        for (int i=0;i<(count > 20 ? 20 : count); i++) { //최대 20줄만 보여주기
            printf("%-10d %-20.1f\n", procs[i].pid, procs[i].cpu_usage);
        }
//        printf("%ld",total_cpu_delta);
        sleep(1);
    }
    return 0;
} 
