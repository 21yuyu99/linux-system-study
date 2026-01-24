#include <stdio.h>
#include <dirent.h>
#include <ctype.h>

int main() {
    DIR *dir;
    struct dirent *entry;
    char path[512];
    char line[256];
    char name[256];    

    // 디렉터리 열기
    dir = opendir("/proc");
    if (dir == NULL ) {
        perror("opendir 실패");
        return 1;
    }

    //printf("--- 실행 중인 프로세스 PID 목록 ---\n");
    //최소 10자리, 20자리 공간 차지. - 는 왼쪽 정렬
    printf("%-10s %-20s\n", "PID", "COMMAND");
    printf("--------------------------------\n");

    // 디렉터리 내용 하나씩 읽기
    while ((entry = readdir(dir)) != NULL ) {
        // 폴더 이름의 첫 글자가 숫자인 것만 골라내기
        if (isdigit(entry->d_name[0])) {
            //경로 조립 : /proc/[PID]/status
            sprintf(path, "/proc/%s/status", entry->d_name);
        
            //status 파일 열기
            FILE *fp = fopen(path, "r");
            if (fp != NULL) {
                // 첫 번째 줄만 읽기
                if (fgets(line, sizeof(line), fp)) {
                    //Nmae : 다음의 문자열만 가져옴(공백 전까지만 읽음)
                    sscanf(line, "Name: %s", name);
                    printf("%-10s %-20s\n", entry->d_name, name);
                }
                fclose(fp);
            }
        }
    }

    closedir(dir);
    return 0;
}
