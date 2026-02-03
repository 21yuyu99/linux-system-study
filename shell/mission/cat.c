#include <stdio.h>
#include <fcntl.h> //open()
#include <unistd.h> //read(), write(), close()
//0 : 표준입력, 1:표준출력, 2:표준에러
int main(int argc, char *argv[]){

    if (argc < 2){ //파일명 입력을 안했을 때
        printf("Usage: %s <filename>\n", argv[0]);
        return 1;
    }

    int fd = open(argv[1],O_RDONLY);
    if (fd == -1){
        perror("Error opening file");
        return 1;
    }

    printf("Successfully opened file with File Descrptor: %d\n", fd);

    char buffer[1024];
    ssize_t bytes_read;
    while((bytes_read = read(fd,buffer,sizeof(buffer)))!=0){
        write(1,buffer,bytes_read); //1: 터미널에 출력하겠다.
    }

    close(fd);
   

    return 0;
}


