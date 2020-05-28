//IOCTL Definitions for VM system
//Author: Edwin Heerschap

#ifndef _VM_IOCTL_H
#define _VM_IOCTL_H
#include <asm-generic/ioctl.h>

#define VM_PROTOCOL_RIGHT_CLICK 0x01
#define VM_PROTOCOL_LEFT_CLICK 0x04

struct vmMouseEvent {

    int dx;
    int dy;
    char buttons;

};



#define VM_IOCTL_MOUSE_EVENT _IO(0x0A, 0)
#define VM_IOCTL_CLEAR_EVENT _IO(0x0A, 1)

#endif