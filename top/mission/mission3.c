#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h> // sleep 함수를 쓰기 위해 필요합니다

int main() {
    long prev_user,prev_nice,prev_system,prev_idle;
    int  first_run = 1;
    
    while(1) { // 1. 무한 루프 시작
        long total_mem = 0;
        long free_mem = 0;
        long user,nice,sys_time,idle;
        char mem_buffer[256]; 
        char cpu_buffer[256]; 
        FILE *mem_fp = fopen("/proc/meminfo","r");
        FILE *cpu_fp = fopen("/proc/stat", "r");
        
        if (mem_fp == NULL || cpu_fp == NULL) {
            perror("파일 열기 실패");
            return 1;
        }
        while(fgets(mem_buffer,sizeof(mem_buffer),mem_fp)) {
            if(strncmp(mem_buffer,"MemTotal:",9) == 0) {
                sscanf(mem_buffer,"MemTotal: %ld", &total_mem);
            }
            if(strncmp(mem_buffer,"MemFree:",8) == 0){
                sscanf(mem_buffer,"MemFree: %ld", &free_mem);
            }    
        }
        fclose(mem_fp);
        fgets(cpu_buffer, sizeof(cpu_buffer), cpu_fp); // /proc/stat 첫 번째 줄만 가져오기
        sscanf(cpu_buffer,"cpu %ld %ld %ld %ld",&user, &nice, &sys_time, &idle);
      
        fclose(cpu_fp);

        system("clear"); // 2. 화면을 깨끗하게 지웁니다 (Mac/Linux 명령어)
        
        double cpu_usage = 0.0;
        if(!first_run){
            long diff_user = user - prev_user;
            long diff_system = sys_time - prev_system;
            long diff_idle = idle - prev_idle;
            long total_diff = diff_user + diff_system + diff_idle;
        
            if(total_diff > 0){
                cpu_usage = (double)(diff_user + diff_system) / total_diff * 100;    
            }
        }
        prev_user = user; prev_system = sys_time; prev_idle = idle;
        first_run = 0; 
        printf("\n--- Real-time System Monitor (Press Ctrl+C to stop) ---\n");
        
        printf("CPU Usage : %.2f %%\n", cpu_usage);
        printf("Total Memory : %ld kB\n", total_mem);
        printf("Free Memory  : %ld kB\n", free_mem);
        printf("Usage Rate   : %.2f%%\n", (double)(total_mem - free_mem) / total_mem * 100);
        printf("-------------------------------------------------------\n");

        sleep(1); // 3. 1초 동안 멈췄다가 다시 루프를 돕니다
    }

    return 0;
}
