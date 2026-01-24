#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main() {
    FILE *fp;
    char buffer[256];
    long total_mem = 0;
    long free_mem = 0;

    // 리눅스 커널이 실시간으로 생성하는 메모리 정보 파일 열기
    fp = fopen("/proc/meminfo", "r");
    if (fp == NULL) {
        perror("파일 열기 실패");
        return 1;
    }

    // 파일 내용을 한 줄씩 읽어서 MemTotal과 MemFree 찾기
    while (fgets(buffer, sizeof(buffer), fp)) {
        if (strncmp(buffer, "MemTotal:", 9) == 0) {
            sscanf(buffer, "MemTotal: %ld", &total_mem);
        }
        if (strncmp(buffer, "MemFree:", 8) == 0) {
            sscanf(buffer, "MemFree: %ld", &free_mem);
        }
    }
    fclose(fp);

    // 결과 출력
    printf("\n--- My First System Monitor ---\n");
    printf("Total Memory : %ld kB\n", total_mem);
    printf("Free Memory  : %ld kB\n", free_mem);
    printf("Usage Rate   : %.2f%%\n", (double)(total_mem - free_mem) / total_mem * 100);
    printf("-------------------------------\n");

    return 0;
}
