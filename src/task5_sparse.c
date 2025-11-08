#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

#define FILENAME    "sparse.bin"
#define FILE_MODE   0644
#define ONE_MIB     (1024 * 1024)

int main(void)
{
    int file_descriptor = open(FILENAME, O_WRONLY | O_CREAT | O_TRUNC, FILE_MODE);
    if(file_descriptor == -1){
        perror("open for write");
        return EXIT_FAILURE;
    }

    if(write(file_descriptor, "START", 5) != 5){
        perror("write START");
        close(file_descriptor);
        return EXIT_FAILURE;
    }

    if(lseek(file_descriptor, ONE_MIB, SEEK_CUR) == -1){
        perror("lseek skip");
        close(file_descriptor);
        return EXIT_FAILURE;
    }

    if(write(file_descriptor, "END", 3) != 3){
        perror("write END");
        close(file_descriptor);
        return EXIT_FAILURE;
    }

    if(close(file_descriptor) == -1){
        perror("close after write");
        return EXIT_FAILURE;
    }

    file_descriptor = open(FILENAME, O_RDONLY);
    if(file_descriptor == -1){
        perror("reopen for read");
        return EXIT_FAILURE;
    }

    off_t apparent_size = lseek(file_descriptor, 0, SEEK_END);
    if(apparent_size == -1){
        perror("lseek size");
        close(file_descriptor);
        return EXIT_FAILURE;
    }

    printf("The apparent file size => %ld bytes\n", (long)apparent_size);

    /*
     * Explanation.
     *
     * => The file appears to be over 1 MiB in size because lseek() extended
     * the file offset beyond the end without writing any data. The filesystem
     * recognizes this region as a "hole" and does not allocate physical disk
     * blocks for it. Only the "START" and "END" strings (8 bytes total) consume
     * actual storage. Thus, `du` reports much smaller usage than `ls` or the
     * apparent size from SEEK_END.
     */

    if(close(file_descriptor) == -1){
        perror("final close");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
