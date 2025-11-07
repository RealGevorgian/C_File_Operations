#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/types.h>

#define FILENAME    "log.txt"
#define FILE_MODE   0644
#define BUFFER_SIZE 4096

int main(void)
{
    int file_descriptor = open(FILENAME, O_WRONLY | O_CREAT | O_APPEND, FILE_MODE);
    if(file_descriptor == -1){
        perror("open log");
        return EXIT_FAILURE;
    }

    char input_buffer[BUFFER_SIZE];
    ssize_t input_length = read(STDIN_FILENO, input_buffer, BUFFER_SIZE - 1);
    if(input_length == -1){
        perror("read input");
        close(file_descriptor);
        return EXIT_FAILURE;
    }

    if(input_length == 0){
        close(file_descriptor);
        return EXIT_SUCCESS;
    }

    if(input_buffer[input_length - 1] == '\n'){
        input_length--;
    }

    pid_t process_id = getpid();
    char pid_prefix[32];
    int prefix_length = snprintf(pid_prefix, sizeof(pid_prefix), "PID=%d: ", process_id);

    char output_buffer[BUFFER_SIZE + 64];
    int output_index = 0;

    memcpy(output_buffer + output_index, pid_prefix, prefix_length);
    output_index += prefix_length;

    memcpy(output_buffer + output_index, input_buffer, input_length);
    output_index += input_length;

    output_buffer[output_index++] = '\n';

    ssize_t bytes_written = write(file_descriptor, output_buffer, output_index);
    if(bytes_written != output_index){
        perror("write log");
        close(file_descriptor);
        return EXIT_FAILURE;
    }

    off_t final_offset = lseek(file_descriptor, 0, SEEK_CUR);
    if(final_offset == -1){
        perror("lseek final offset");
        close(file_descriptor);
        return EXIT_FAILURE;
    }

    printf("Final file offset => %ld\n", (long)final_offset);

    if(close(file_descriptor) == -1){
        perror("close");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
