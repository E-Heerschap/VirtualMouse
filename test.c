#include <fcntl.h>      /* open */
#include <unistd.h>     /* exit */
#include <sys/ioctl.h>  /* ioctl */
#include "vmioctl.h"
#include <stdio.h>
// Created by edwin on 2020-05-21.
//

 int main(void) {

    int file_desc;
    int test;

    file_desc = open("/dev/vmouse0",0);
    if (file_desc < 0) {
        printf("Cannot open file\n");
        return 1;
    }

    test = ioctl(file_desc, IOCTL_TEST_MSG);
    if(test < 0) {
        printf("Failed to send ioctl");
        close(file_desc);
        return 1;
    }

    close(file_desc);
    return 0;

}
