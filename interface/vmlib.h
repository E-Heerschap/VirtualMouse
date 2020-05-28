//
// Created by edwin on 2020-05-27.
//

#include "../vmioctl.h"

#ifndef SNEAKYMOUSE_VMLIB_H
#define SNEAKYMOUSE_VMLIB_H

struct vmMouseEvent vmBuildMouseEvent(int dx, int dy, char buttons);

#endif //SNEAKYMOUSE_VMLIB_H
