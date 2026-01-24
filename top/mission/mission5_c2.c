#include <stdio.h>
#include <dirent.h> //opendir, readdir
#include <ctype.h> //isdigit
#include <string.h>
#include <stdlib.h> //qsort, atoi

typedef struct {

    int pid;
    char name[256];

} ProcessInfo;

int compare_procs(const void *a, const void *b){ //qsort는 void*만 받을 수 있음
    ProcessInfo *procA = (ProcessInfo *)a;
    ProcessInfo *procB = (ProcessInfo *)b;
    return strcmp(procA->name,procB->name); // < 부등호로 비교하면 두 값이 같을 때 비교 못함
}

int main() {
    DIR *dir;
    struct dirent *entry; //dirent는 Directory Entry 구조체
    ProcessInfo procs[1024]; 
    int count = 0;

    dir = opendir("/proc");
    if (dir == NULL) return 1;

    while((entry = readdir(dir)) != NULL && count < 1024){
        if (isdigit(entry->d_name[0])) {
            char path[512];
            snprintf(path, sizeof(path), "/proc/%s/status", entry->d_name); //문자열 만들어 path에 저장

            FILE *fp = fopen(path, "r");
            if (fp != NULL) {
                char line[256];
                if (fgets(line, sizeof(line), fp)) {
                    procs[count].pid = atoi(entry->d_name);
                    sscanf(line, "Name: %s", procs[count].name);
                    count++;
                }
                fclose(fp); //항상 파일 꼭 닫기
            }
        }
    }
    closedir(dir);

    qsort(procs, count, sizeof(ProcessInfo), compare_procs);
    
    printf("%-10s %-20s\n", "PID", "COMMAND");
    printf("--------------------------------\n");
    for (int i=0;i<(count > 20 ? 20 : count); i++) { //최대 20줄만 보여주기
        printf("%-10d %-20s\n", procs[i].pid, procs[i].name);
    }
    return 0;
} 
