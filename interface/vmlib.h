//
// Created by edwin on 2020-05-27.
//

#include "../vmioctl.h"

#ifndef SNEAKYMOUSE_VMLIB_H
#define SNEAKYMOUSE_VMLIB_H

typedef struct vmMouseEvent vmMouseEvent;

struct vmMouseEvent vmBuildMouseEvent(int dx, int dy, char buttons);

int fdSendIOCTLEvent(int file_desc, const vmMouseEvent* event);

int fdSendIOCTLEvents(int file_desc, const vmMouseEvent* events, const unsigned int size);

vmMouseEvent combineEvents(const vmMouseEvent* ev1, const vmMouseEvent* ev2);

vmMouseEvent noneEvent(void);

vmMouseEvent leftDownEvent(void);

vmMouseEvent rightDownEvent(void);

vmMouseEvent moveEvent(int x, int y);

/**
 * Moves the mouse. This will 'teleport' the most x and y pixels
 * in the respective directions. If abs(x) or abs(y) is over 127 then
 * it is executed over MAX(abs(x), abs(y)) / 127 IOCTL commands.
 * @param file_desc File descriptor of the device.
 * @param x number of pixels in x direction
 * @param y number of pixels in y direction
 * @return ioctl return value.
 */
int fdMoveMouse(int file_desc, int x, int y);

/**
 * Sends a right click to the device given by the passed file_descriptor.
 * This is done by two IOCTL events.
 * @param file_desc File descriptor of device.
 * @return IOCTL event value. If error on first, event it will return this value.
 */
int fdRightClick(int file_desc);

int fdLeftClick(int file_desc);



#endif //SNEAKYMOUSE_VMLIB_H
