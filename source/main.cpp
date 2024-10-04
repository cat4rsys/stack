#include "stack.h"
#include "log.h"
#include <cstdio>

int main()
{
    Stack stk = {};

    stk.logFile = fopen("logfile.htm", "w");
    logInit(stk.logFile);

    DO_STACK_CTOR(&stk, 2);
    double x = 0;

    for (int i = 0; i<8; i++) {
        DO_STACK_PUSH(&stk, i/3.2);
    }

    for (int i = 0; i<8; i++) {
        DO_STACK_POP(&stk, &x);
    }

    DO_STACK_DTOR(&stk);

    return 0;
}
