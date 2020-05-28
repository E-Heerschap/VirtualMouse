// Prescribes the interface library to the VM system.
// Created by Edwin Heerschap on 2020-05-27.
//
#include "../vmioctl.h"

struct vmMouseEvent vmBuildMouseEvent(int dx, int dy, char buttons) {

    struct vmMouseEvent event = {
            .dy = dy,
            .dx = dx,
            .buttons = buttons
    };

    return event;

}