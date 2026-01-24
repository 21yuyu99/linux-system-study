#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h> // sleep 함수를 쓰기 위해 필요합니다

int main() {
    FILE *fp;
    char buffer[256];

    while(1) { // 1. 무한 루프 시작
        long total_mem = 0;
        long free_mem = 0;

        fp = fopen("/proc/meminfo", "r");
        if (fp == NULL) {
            perror("파일 열기 실패");
            return 1;
        }

        while (fgets(buffer, sizeof(buffer), fp)) {
            if (strncmp(buffer, "MemTotal:", 9) == 0) {
                sscanf(buffer, "MemTotal: %ld", &total_mem);
            }
            if (strncmp(buffer, "MemFree:", 8) == 0) {
                sscanf(buffer, "MemFree: %ld", &free_mem);
            }
        }
        fclose(fp);

        system("clear"); // 2. 화면을 깨끗하게 지웁니다 (Mac/Linux 명령어)
        
        printf("\n--- Real-time System Monitor (Press Ctrl+C to stop) ---\n");
        printf("Total Memory : %ld kB\n", total_mem);
        printf("Free Memory  : %ld kB\n", free_mem);
        printf("Usage Rate   : %.2f%%\n", (double)(total_mem - free_mem) / total_mem * 100);
        printf("-------------------------------------------------------\n");

        sleep(1); // 3. 1초 동안 멈췄다가 다시 루프를 돕니다
    }

    return 0;
}
