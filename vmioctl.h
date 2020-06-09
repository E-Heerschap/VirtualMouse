//IOCTL Definitions for VM system
//Author: Edwin Heerschap

#ifndef _VM_IOCTL_H
#define _VM_IOCTL_H
#include <asm-generic/ioctl.h>

/**
 * Use buttons type instead.
 */
struct __Buttons {
  char _buttons;
};

/**
 * Use as opaque type.
 */
typedef struct __Buttons Buttons;

#define VM_PROTOCOL_RIGHT_CLICK (Buttons) {._buttons = 0x04}
#define VM_PROTOCOL_LEFT_CLICK (Buttons) {._buttons = 0x01}

/**
 * For languages with native support to get the buttons.
 * These languages should still treat the returned buttons
 * as an opaque data type.
 */
#ifdef VM_EXPORT_GET_BUTTONS
    Buttons getRightClick(void);
    Buttons getLeftClick(void);
#endif

void combineButton(Buttons* base, Buttons* add);
Buttons addButtons(const Buttons* b1, const Buttons* b2);
char buttonsToByte(Buttons* b);
void setButtonsFromByte(Buttons* but, char b);
Buttons buttonsFromByte(char b);
char buttonsToByte(Buttons* b);
void setButtonsFromByte(Buttons* but, char b);

struct vmMouseEvent {

    int dx;
    int dy;
    Buttons buttons;

};

#define VM_IOCTL_MOUSE_EVENT _IO(0x0A, 0)
#define VM_IOCTL_CLEAR_EVENT _IO(0x0A, 1)

#endif