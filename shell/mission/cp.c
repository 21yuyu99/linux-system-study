#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

int main(int argc, char *argv[]){

    if (argc < 3){
        printf("Usage: %s <source> <target>\n", argv[0]);

    }

    int src_fd = open(argv[1], O_RDONLY);
    if (src_fd == -1){
        perror("Error opening source file");
        return 1;
    }

    int dst_fd = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (dst_fd == -1){
        perror("Error opening/creating target file");
        close(src_fd);
        return 1;
    }

    char buffer[1024];
    ssize_t bytes_read; //성공하면 양수, 파일 끝 0, 실패 -1

    while ((bytes_read = read(src_fd, buffer, sizeof(buffer))) > 0) {
        write(dst_fd, buffer,bytes_read);
    }

    printf("Copy completed: %s -> %s\n", argv[1], argv[2]);

    close(src_fd);
    close(dst_fd);

    return 0;
}
