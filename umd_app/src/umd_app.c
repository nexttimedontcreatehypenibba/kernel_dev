#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include<sys/ioctl.h>

#define WR_VALUE _IOW('a','a',int32_t*)
#define RD_VALUE _IOR('a','b',int32_t*)

int main() {
    printf("read from sysfs node\n");
    system("cat /sys/kernel/cdev_dri_app/sysfs_file");
    printf("write into sysfs node\n");
    system("echo 1> /sys/kernel/cdev_dri_app/sysfs_file");
    
    printf("open device file\n");
    int fd = open("/dev/cdev_dri", O_RDWR);
    if (fd < 0) {
        printf("failed to open device file\n");
    }

    char data[128] = "";
    int count = 0;
    
    printf("read device file\n");
    count = read(fd, data, 32);
    printf("%d bytes of data from file : %s\n", count, data);
    
    printf("write into device file\n");
    count = write(fd, "sample text\n", strlen("sample text\n"));
    printf("%d bytes of data written to device file\n", count);

#if 0
    lseek(fd, 0, SEEK_SET);
    count = read(fd, data, 32);
    printf("%d bytes of data from file : %s\n", count, data);
#endif

    int number = 32;
    printf("IOCTL calls\n");
    ioctl(fd, WR_VALUE, &number);
    ioctl(fd, RD_VALUE, &number);

    printf("close device file\n");
    close(fd);
    return 0;
}
