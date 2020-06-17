// Implements the mechanics for structures shared between interface
// and the driver.
// Created by edwin on 2020-06-08.
//
#include "vmioctl.h"

/**
 * For languages with native support to get the buttons.
 * These languages should still treat the returned buttons
 * as an opaque data type.
 */
#ifdef VM_EXPORT_GET_BUTTONS
    Buttons getRightClick(void) {
        return VM_PROTOCOL_RIGHT_CLICK;
    }

    Buttons getLeftClick(void) {
        return VM_PROTOCOL_LEFT_CLICK;
    }
#endif

/**
 * Performs an OR operation on the base button with the add button.
 * @param b1 Base set of buttons. This will be changed.
 * @param b2 Second set of buttons to add to b1.
 */
void combineButton(Buttons* base, Buttons* add) {

    base->_buttons = base->_buttons | add->_buttons;

}

/**
 * Adds the set of buttons together (OR operation)
 * @param b1 First set of buttons
 * @param b2 Second set of buttons
 * @return A set of buttons consisting of b1 + b2.
 */
Buttons addButtons(const Buttons* b1, const Buttons* b2) {

    char b3 = b1->_buttons | b2->_buttons;
    return (Buttons) {
            ._buttons = b3
    };

}

/**
 * Returns a byte representation of the buttons.
 * Can only show first 8 buttons.
 * @param b buttons change to byte representation
 * @return Byte representation. Bit in position x is 0 if mouse button x
 * is clicked. Mouse button defined from left to right.
 */
char buttonsToByte(Buttons* b) {
    return b->_buttons;
}

/**
 * Sets buttons from a byte representation
 * @param but buttons to set
 * @param b byte representation
 */
void setButtonsFromByte(Buttons* but, char b) {
    but->_buttons = b;
}

/**
 * Creates a buttons struct from a byte representation
 * @param b byte representation of buttons pressed
 * @return Buttons struct from the byte representation
 */
Buttons buttonsFromByte(char b) {
    return (Buttons) {
            ._buttons = b
    };
}