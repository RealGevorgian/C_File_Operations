#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

#define BUFFER_SIZE 4096

char *read_line(const char *prompt)
{
    printf("%s", prompt);
    fflush(stdout);

    char *line = NULL;
    size_t capacity = 0;
    ssize_t length = getline(&line, &capacity, stdin);
    if(length == -1){
        free(line);
        return NULL;
    }

    if(length > 0 && line[length - 1] == '\n')
        line[length - 1] = '\0';

    return line;
}

int main(void)
{
    char *path_one = read_line("Enter first file path -> ");
    if(!path_one){
        perror("read first path");
        return EXIT_FAILURE;
    }

    char *path_two = read_line("Enter second file path -> ");
    if(!path_two){
        perror("read second path");
        free(path_one);
        return EXIT_FAILURE;
    }

    int fd_one = open(path_one, O_RDONLY);
    if(fd_one == -1){
        perror("open first file");
        free(path_one);
        free(path_two);
        return EXIT_FAILURE;
    }

    int fd_two = open(path_two, O_RDONLY);
    if(fd_two == -1){
        perror("open second file");
        close(fd_one);
        free(path_one);
        free(path_two);
        return EXIT_FAILURE;
    }

    char buffer_one[BUFFER_SIZE];
    char buffer_two[BUFFER_SIZE];
    ssize_t read_one, read_two;
    off_t byte_index = 0;

    while(1){
        read_one = read(fd_one, buffer_one, BUFFER_SIZE);
        read_two = read(fd_two, buffer_two, BUFFER_SIZE);

        if(read_one == -1){
            perror("read first file");
            close(fd_one);
            close(fd_two);
            free(path_one);
            free(path_two);
            return EXIT_FAILURE;
        }
        if(read_two == -1){
            perror("read second file");
            close(fd_one);
            close(fd_two);
            free(path_one);
            free(path_two);
            return EXIT_FAILURE;
        }

        ssize_t min_bytes = (read_one < read_two) ? read_one : read_two;

        for(ssize_t i = 0; i < min_bytes; i++){
            if(buffer_one[i] != buffer_two[i]){
                printf("Files differ at byte -> %ld\n", (long)(byte_index + i));
                close(fd_one);
                close(fd_two);
                free(path_one);
                free(path_two);
                return EXIT_FAILURE;
            }
        }

        byte_index += min_bytes;

        if(min_bytes < read_one){
            printf("Files differ at byte -> %ld\n", (long)byte_index);
            close(fd_one);
            close(fd_two);
            free(path_one);
            free(path_two);
            return EXIT_FAILURE;
        }
        if(min_bytes < read_two){
            printf("Files differ at byte -> %ld\n", (long)byte_index);
            close(fd_one);
            close(fd_two);
            free(path_one);
            free(path_two);
            return EXIT_FAILURE;
        }

        if(read_one == 0 && read_two == 0) {
            printf("Files are identical\n");
            close(fd_one);
            close(fd_two);
            free(path_one);
            free(path_two);
            return EXIT_SUCCESS;
        }
    }
}

