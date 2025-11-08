#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

#define FILENAME    "numbers.txt"
#define FILE_MODE   0644
#define MAX_LINE    16
#define TEMP_FILE   "numbers.tmp"

int main(void)
{
    int file_descriptor = open(FILENAME, O_WRONLY | O_CREAT | O_TRUNC, FILE_MODE);
    if(file_descriptor == -1){
        perror("open for write");
        return EXIT_FAILURE;
    }

    for(int i = 1; i <= 10; i++){
        char line[MAX_LINE];
        int length = snprintf(line, sizeof(line), "%d\n", i);
        if (write(file_descriptor, line, length) != length) {
            perror("write number");
            close(file_descriptor);
            return EXIT_FAILURE;
        }
    }

    if(close(file_descriptor) == -1){
        perror("close after write");
        return EXIT_FAILURE;
    }

    file_descriptor = open(FILENAME, O_RDWR);
    if(file_descriptor == -1){
        perror("reopen for edit");
        return EXIT_FAILURE;
    }

    off_t line_start = 0;
    int newline_count = 0;
    char byte_value;
    ssize_t bytes_read;

    while((bytes_read = read(file_descriptor, &byte_value, 1)) == 1){
        if(byte_value == '\n'){
            newline_count++;
            if(newline_count == 3){
                line_start = lseek(file_descriptor, 0, SEEK_CUR);
                if(line_start == -1){
                    perror("lseek line start");
                    close(file_descriptor);
                    return EXIT_FAILURE;
                }
                break;
            }
        }
    }

    if(newline_count < 3){
        fprintf(stderr, "File has fewer than 4 lines\n");
        close(file_descriptor);
        return EXIT_FAILURE;
    }

    off_t file_size = lseek(file_descriptor, 0, SEEK_END);
    if(file_size == -1){
        perror("lseek end");
        close(file_descriptor);
        return EXIT_FAILURE;
    }

    off_t remainder_size = file_size - line_start;
    char *remainder_buffer = malloc(remainder_size);
    if(!remainder_buffer){
        perror("malloc remainder");
        close(file_descriptor);
        return EXIT_FAILURE;
    }

    if(lseek(file_descriptor, line_start, SEEK_SET) == -1){
        perror("lseek back to line start");
        free(remainder_buffer);
        close(file_descriptor);
        return EXIT_FAILURE;
    }

    bytes_read = read(file_descriptor, remainder_buffer, remainder_size);
    if(bytes_read != remainder_size){
        perror("read remainder");
        free(remainder_buffer);
        close(file_descriptor);
        return EXIT_FAILURE;
    }

    const char *replacement = "100\n";
    ssize_t replacement_length = 4;

    if(lseek(file_descriptor, line_start, SEEK_SET) == -1){
        perror("lseek to write replacement");
        free(remainder_buffer);
        close(file_descriptor);
        return EXIT_FAILURE;
    }

    if(write(file_descriptor, replacement, replacement_length) != replacement_length){
        perror("write replacement");
        free(remainder_buffer);
        close(file_descriptor);
        return EXIT_FAILURE;
    }

    if(write(file_descriptor, remainder_buffer, remainder_size) != remainder_size){
        perror("write remainder");
        free(remainder_buffer);
        close(file_descriptor);
        return EXIT_FAILURE;
    }

    free(remainder_buffer);

    off_t new_size = line_start + replacement_length + remainder_size;
    if(ftruncate(file_descriptor, new_size) == -1){
        perror("ftruncate");
        close(file_descriptor);
        return EXIT_FAILURE;
    }

    if(lseek(file_descriptor, 0, SEEK_SET) == -1){
        perror("lseek to beginning");
        close(file_descriptor);
        return EXIT_FAILURE;
    }

    char print_buffer[256];
    ssize_t total_read = 0;
    while((bytes_read = read(file_descriptor, print_buffer, sizeof(print_buffer))) > 0){
        if(write(STDOUT_FILENO, print_buffer, bytes_read) != bytes_read){
            perror("write to stdout");
            close(file_descriptor);
            return EXIT_FAILURE;
        }
        total_read += bytes_read;
    }

    if(close(file_descriptor) == -1){
        perror("final close");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
