#include "process.h"
void task_tty()
{
    while (1)
    {
        keyboard_read();
        }
}