#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/ioctl.h>
#include <errno.h>

#define DEV_IN  "/dev/dma_in"
#define DEV_OUT "/dev/dma_out"
#define BUF_SIZE 4096

void test_write_operation(int fd_out) {
    const char *test_data = "DMA_TEST_PATTERN_1234";
    ssize_t ret;
    
    printf("\n=== Testing WRITE ===\n");
    printf("Writing test pattern: '%s'\n", test_data);
    
    ret = write(fd_out, test_data, strlen(test_data));
    if (ret < 0) {
        perror("Write failed");
    } else {
        printf("Successfully wrote %zd bytes\n", ret);
        
        // Verify kernel received it
        system("dmesg | tail -n 3 | grep 'DMA_OUT:'");
    }
}

void test_read_operation(int fd_in) {
    char buf[BUF_SIZE];
    ssize_t ret;
    
    printf("\n=== Testing READ ===\n");
    printf("Reading from device...\n");
    
    ret = read(fd_in, buf, sizeof(buf));
    if (ret < 0) {
        perror("Read failed");
    } else {
        printf("Read %zd bytes:\n", ret);
        
        // Print first 32 bytes as hex and ASCII
        for (int i = 0; i < ret && i < 32; i++) {
            if (i % 16 == 0) printf("\n%04x: ", i);
            printf("%02x ", buf[i]);
        }
        printf("\n");
        
        // Print as string (null-terminate first)
        buf[ret < BUF_SIZE ? ret : BUF_SIZE-1] = '\0';
        printf("As string: %.32s%s\n", buf, ret > 32 ? "..." : "");
    }
}

void test_ioctl_control(int fd) {
    printf("\n=== Testing IOCTL ===\n");
    // Replace with your actual IOCTL commands
    #define DMA_START _IO('D', 1)
    #define DMA_STATUS _IOR('D', 2, int)
    
    int status;
    if (ioctl(fd, DMA_START) {
        perror("IOCTL DMA_START failed");
    } else {
        printf("DMA started successfully\n");
    }
    
    if (ioctl(fd, DMA_STATUS, &status) {
        perror("IOCTL DMA_STATUS failed");
    } else {
        printf("DMA status: %d\n", status);
    }
}

int main() {
    int fd_in, fd_out;
    
    printf("Opening devices...\n");
    fd_out = open(DEV_OUT, O_WRONLY);
    if (fd_out < 0) {
        perror("Failed to open output device");
        exit(EXIT_FAILURE);
    }
    
    fd_in = open(DEV_IN, O_RDONLY);
    if (fd_in < 0) {
        perror("Failed to open input device");
        close(fd_out);
        exit(EXIT_FAILURE);
    }
    
    // Run test sequence
    test_write_operation(fd_out);
    test_read_operation(fd_in);
    test_ioctl_control(fd_out);  // Can use either fd
    
    printf("\n=== Verification Complete ===\n");
    
    close(fd_in);
    close(fd_out);
    return 0;
}

