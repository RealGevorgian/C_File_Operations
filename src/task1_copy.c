#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>

#define BUFFER_SIZE 4096
#define FILE_MODE   0644

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
    char *source_path = read_line("Enter source file path: ");
    if(!source_path){
        perror("Failed to read source path");
        return EXIT_FAILURE;
    }

    char *dest_path = read_line("Enter destination file path: ");
    if(!dest_path){
        perror("Failed to read destination path");
        free(source_path);
        return EXIT_FAILURE;
    }

    int source_fd = open(source_path, O_RDONLY);
    if(source_fd == -1){
        perror("open source");
        free(source_path);
        free(dest_path);
        return EXIT_FAILURE;
    }

    int dest_fd = open(dest_path, O_WRONLY | O_CREAT | O_TRUNC, FILE_MODE);
    if(dest_fd == -1){
        perror("open destination");
        close(source_fd);
        free(source_path);
        free(dest_path);
        return EXIT_FAILURE;
    }

    char buffer[BUFFER_SIZE];
    ssize_t bytes_read, bytes_written;
    long long total_copied = 0;

    while((bytes_read = read(source_fd, buffer, BUFFER_SIZE)) > 0){
        char *data_pointer = buffer;
        ssize_t remaining = bytes_read;

        while(remaining > 0){
            bytes_written = write(dest_fd, data_pointer, remaining);
            if (bytes_written == -1) {
                perror("write");
                close(source_fd);
                close(dest_fd);
                free(source_path);
                free(dest_path);
                return EXIT_FAILURE;
            }
            remaining -= bytes_written;
            data_pointer += bytes_written;
            total_copied += bytes_written;
        }
    }

    if(bytes_read == -1){
        perror("read");
        close(source_fd);
        close(dest_fd);
        free(source_path);
        free(dest_path);
        return EXIT_FAILURE;
    }

    if(close(source_fd) == -1) perror("close source");
    if(close(dest_fd) == -1) perror("close destination");

    printf("Copied %lld bytes from '%s' to '%s'\n",
           total_copied, source_path, dest_path);

    free(source_path);
    free(dest_path);

    return EXIT_SUCCESS;
}
