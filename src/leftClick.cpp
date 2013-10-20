#include "leftClick.h"

#include "Windows.h"

/* experimental left click function - not yet tested on windows */
void leftClick::click() {

    INPUT ip_down;
    ip_down.type = INPUT_MOUSE;
    ip_down.mi.mouseData = 0;
    ip_down.mi.dwFlags = MOUSEEVENTF_LEFTDOWN;

    SendInput(1, &ip_down, sizeof(INPUT));

    INPUT ip_up;
    ip_up.type = INPUT_MOUSE;
    ip_up.mi.mouseData = 0;
    ip_up.mi.dwFlags = MOUSEEVENTF_LEFTUP;

    SendInput(1, &ip_up, sizeof(INPUT));


}