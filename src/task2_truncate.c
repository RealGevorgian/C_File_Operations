#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>

#define FILENAME "data.txt"
#define FILE_MODE 0644
#define INITIAL_TEXT "ABCDEFGHIJKLMNOPQRSTUVWXYZ"

int main(void)
{
	int file_descriptor = open(FILENAME, O_WRONLY | O_CREAT | O_TRUNC, FILE_MODE);
	if(file_descriptor == -1){
		perror("Open for write");
		return EXIT_FAILURE;
	}

	ssize_t bytes_written = write(file_descriptor, INITIAL_TEXT, strlen(INITIAL_TEXT));
    	if(bytes_written != strlen(INITIAL_TEXT)){
        	perror("Write initial text");
        	close(file_descriptor);
        	return EXIT_FAILURE;
    	}

    	if(close(file_descriptor) == -1){
        	perror("Close after write");
        	return EXIT_FAILURE;
    	}

    	file_descriptor = open(FILENAME, O_RDWR);
    	if(file_descriptor == -1){
        	perror("reopen for read/write");
        	return EXIT_FAILURE;
    	}

    	off_t current_size = lseek(file_descriptor, 0, SEEK_END);
    	if(current_size == -1){
        	perror("lseek initial size");
        	close(file_descriptor);
        	return EXIT_FAILURE;
    	}
    	
	printf("The current size -> %ld bytes\n", (long)current_size);

    	if(ftruncate(file_descriptor, 10) == -1){
        	perror("ftruncate");
        	close(file_descriptor);
        	return EXIT_FAILURE;
    	}

    	off_t new_size = lseek(file_descriptor, 0, SEEK_END);
    	if(new_size == -1){
        	perror("lseek after truncate");
        	close(file_descriptor);
        	return EXIT_FAILURE;
    	}
    	
	printf("New size after truncate -> %ld bytes\n", (long)new_size);

    	if(lseek(file_descriptor, 0, SEEK_SET) == -1){
        	perror("lseek rewind");
        	close(file_descriptor);
        	return EXIT_FAILURE;
    	}

    	char buffer[32];
    	ssize_t bytes_read = read(file_descriptor, buffer, sizeof(buffer) - 1);
    	if (bytes_read == -1) {
        	perror("read after truncate");
        	close(file_descriptor);
        	return EXIT_FAILURE;
    	}
    	
	buffer[bytes_read] = '\0';
    	
	printf("The remaining content => %s\n", buffer);

    	if (close(file_descriptor) == -1) {
        	perror("final close");
        	return EXIT_FAILURE;
    	}

    	return EXIT_SUCCESS;

}
