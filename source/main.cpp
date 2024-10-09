#include "stack.h"
#include "log.h"
#include <cstdio>

int main()
{
    Stack stk = {}; //

    stk.logFile = fopen("logfile.htm", "w");
    logInit(stk.logFile);

    DO_STACK_CTOR(&stk, 20);

    stackElem_t x = 0;

    for (int i = 0; i < 100; i++) {
        DO_STACK_PUSH(&stk, 11);
    }

    DO_STACK_POP(&stk, &x);

    DO_STACK_DUMP(&stk);

    DO_STACK_DTOR(&stk);

    return 0;
}
