#include <cstdio>
#include <string.h> //
#include "stack.h"
#include "log.h"

static int counterOfDumps = 0;

void logInit(FILE * logFile)
{
    fprintf(logFile, "<html>\n"
                      "\t<head>\n"
                      "\t\t<title>LOG FILE</title>\n"
                      "\t\t<script language=\"JavaScript\">\n"
                      "\t\t\t<!--\n"
                      "\t\t\tfunction flipflop( id )\n"
                      "\t\t\t{\n"
                      "\t\t\t\telement = document.getElementById( id );\n"
                      "\t\t\t\tif( element )\n"
                      "\t\t\t\t\telement.style.display = element.style.display == \"none\" ? \"\" : \"none\";\n"
                      "\t\t\t}\n"
                      "\t\t\t-->\n"
                      "\t\t</script>\n"
                      "\t</head>\n"
                      "\t<body>\n"
                      "\t\t<h1 align=\"center\">LOG-FILE</h1>\n"
                      "\t\t<ul>\n"
                      "\t\t\t<li>\n");
}

void logDump(FILE * logFile, Stack * stk, int line, const char * func, const char * file)
{
    counterOfDumps++;
    int errorCode = stackVerifier(stk);

    if (errorCode && strcmp("stackDtor", func)) {
        fprintf(logFile, "\t\t\t\t<p> !!!!! %d. <a href=\"javascript:flipflop(\'%d\');\">%s %s:%d</a></p>\n", counterOfDumps, counterOfDumps, func, file, line);
    }
    else {
        fprintf(logFile, "\t\t\t\t<p>%d. <a href=\"javascript:flipflop(\'%d\');\">%s %s:%d</a></p>\n", counterOfDumps, counterOfDumps, func, file, line);
    }
    fprintf(logFile, "\t\t\t\t<ul id=\"%d\" style=\"display: none;\">\n", counterOfDumps);

    fprintf(logFile, "<b>INFO ABOUT STACK</b>\n\n");

    fprintf(logFile, "<p>Actual code of error : %d</p>\n", errorCode);
    logReadError(errorCode, logFile);

    fprintf(logFile, "<p>   &nbsp;Stack [%p]</p>\n", stk);
    fprintf(logFile, "<p>   &nbsp;called from %s:%d (%s)</p>\n", file, line, func);
    fprintf(logFile, "<p>   &nbsp;name %s born at function %s at %s:%d</p>\n\n", stk->name, stk->func, stk->file, stk->line);
    if (!stk) {
        fprintf(logFile, "</ul>\n");
        fprintf(logFile, "</li>\n\n");
        return;
    }

    fprintf(logFile, "<b>ACTUAL INFORMATION</b>\n\n");

    fprintf(logFile, "<p>    &nbsp;data     = %p</p>\n", stk->data);
    fprintf(logFile, "<p>    &nbsp;size     = %d</p>\n", stk->size);
    fprintf(logFile, "<p>    &nbsp;capacity = %d</p>\n\n", stk->capacity);
    if (!stk->data) {
        fprintf(logFile, "</ul>\n");
        fprintf(logFile, "</li>\n\n");
        return;
    }

    fprintf(logFile, "<b>ACTUAL DATA IN STACK</b>\n\n");

    for(int i = 0; i < stk->size; i++) {
        fprintf(logFile, "<p>*[%d] = %f</p>\n", i, stk->data[i]);
    }

    for(int i = stk->size; i < stk->capacity; i++) {
        fprintf(logFile, "<p> [%d] = %f (poison)</p>\n", i, stk->data[i]);
    }

    fprintf(logFile, "\n");

    fprintf(logFile, "</ul>\n");
    fprintf(logFile, "</li>\n\n");
}

void logClose(FILE * logFile)
{
    fprintf(logFile, "\t\t</ul>\n"
                     "\t</body>\n"
                     "</html>\n");

    fclose(logFile);
}
