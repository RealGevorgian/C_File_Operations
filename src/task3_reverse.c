#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

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
    char *file_path = read_line("Enter file path -> ");
    if(!file_path){
        perror("read path");
        return EXIT_FAILURE;
    }

    int file_descriptor = open(file_path, O_RDONLY);
    if(file_descriptor == -1){
        perror("open");
        free(file_path);
        return EXIT_FAILURE;
    }

    off_t file_size = lseek(file_descriptor, 0, SEEK_END);
    if(file_size == -1){
        perror("lseek size");
        close(file_descriptor);
        free(file_path);
        return EXIT_FAILURE;
    }

    if(file_size == 0){
        printf("\n");
        close(file_descriptor);
        free(file_path);
        return EXIT_SUCCESS;
    }

    char byte_value;
    ssize_t bytes_read;

    for(off_t position = file_size - 1; position >= 0; position--){
        if(lseek(file_descriptor, position, SEEK_SET) == -1){
            perror("lseek position");
            close(file_descriptor);
            free(file_path);
            return EXIT_FAILURE;
        }

        bytes_read = read(file_descriptor, &byte_value, 1);
        if(bytes_read == -1){
            perror("read byte");
            close(file_descriptor);
            free(file_path);
            return EXIT_FAILURE;
        }

        if(write(STDOUT_FILENO, &byte_value, 1) != 1){
            perror("write byte");
            close(file_descriptor);
            free(file_path);
            return EXIT_FAILURE;
        }
    }

    printf("\n");

    if(close(file_descriptor) == -1){
        perror("close");
    }

    free(file_path);
    return EXIT_SUCCESS;
}
