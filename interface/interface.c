// Prescribes the interface library to the VM system.
// Created by Edwin Heerschap on 2020-05-27.
//

#include <sys/ioctl.h>
#include "../vmioctl.h"


typedef struct vmMouseEvent vmMouseEvent;

int fdSendIOCTLEvent(int file_desc, const vmMouseEvent* event) {

    return ioctl(file_desc, VM_IOCTL_MOUSE_EVENT, event);

}

int fdSendIOCTLEvents(int file_desc, const vmMouseEvent* events, const unsigned int size) {

    unsigned int n = 0;
    int retVal = 0;
    for(; n < size; n++) {

            retVal = ioctl(file_desc, VM_IOCTL_MOUSE_EVENT, &events[n]);
            if (retVal) return retVal;

    }

    return retVal;
}

vmMouseEvent combineEvents(const vmMouseEvent* ev1, const vmMouseEvent* ev2) {

    vmMouseEvent event = {
            .dx = ev1->dx + ev2->dx,
            .dy = ev1->dy + ev2->dy,
            .buttons = ev1->buttons | ev2->buttons
    };

    return event;

}

vmMouseEvent vmBuildMouseEvent(const int dx, const int dy, const char buttons) {

    struct vmMouseEvent event = {
            .dy = dy,
            .dx = dx,
            .buttons = buttons
    };

    return event;

}

vmMouseEvent noneEvent(void) {

    return vmBuildMouseEvent(0,0,0);

}

vmMouseEvent leftDownEvent(void) {

    return vmBuildMouseEvent(0,0,VM_PROTOCOL_LEFT_CLICK);

}

vmMouseEvent rightDownEvent(void) {

    return vmBuildMouseEvent(0,0,VM_PROTOCOL_RIGHT_CLICK);

}

vmMouseEvent moveEvent(int x, int y) {

    return vmBuildMouseEvent(x,y,0x00);

}

/**
 * Moves the mouse. This will 'teleport' the most x and y pixels
 * in the respective directions. If abs(x) or abs(y) is over 127 then
 * it is executed over MAX(abs(x), abs(y)) / 127 IOCTL commands.
 * @param file_desc File descriptor of the device.
 * @param x number of pixels in x direction
 * @param y number of pixels in y direction
 * @return ioctl return value.
 */
int fdMoveMouse(int file_desc, int x, int y) {

    vmMouseEvent ev = moveEvent(x,y);
    return ioctl(file_desc, VM_IOCTL_MOUSE_EVENT, &ev);

}

/**
 * Sends a right click to the device given by the passed file_descriptor.
 * This is done by two IOCTL events.
 * @param file_desc File descriptor of device.
 * @return IOCTL event value. If error on first, event it will return this value.
 */
int fdRightClick(int file_desc) {

    int val = 0;
    vmMouseEvent rde = rightDownEvent();
    vmMouseEvent none = noneEvent();
    val = ioctl(file_desc, VM_IOCTL_MOUSE_EVENT, &rde);
    if(val) return val;
    val = ioctl(file_desc, VM_IOCTL_MOUSE_EVENT, &none);
    return val;
}

int fdLeftClick(int file_desc) {
    int val = 0;
    vmMouseEvent lde = leftDownEvent(), none = noneEvent();
    val = ioctl(file_desc, VM_IOCTL_MOUSE_EVENT, &lde);
    if (val) return val;
    val = ioctl(file_desc, VM_IOCTL_MOUSE_EVENT, &none);
    return val;
}