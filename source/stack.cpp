#include <cstdlib>
#include <cassert>
#include <cstdio>
#include <cstring>
#include "stack.h"
#include "log.h"

static void stackRealloc(Stack * stk, int newCapacity)
{
    size_t sizeOfData = 2 * sizeof(canary_t) + newCapacity * sizeof(stackElem_t);
    size_t leveledSizeOfData = sizeOfData % levelBytes > 0 ? sizeOfData + ( levelBytes - sizeOfData % levelBytes ) : sizeOfData;

    char * ptrToCanary = (char *)realloc((void *)( (char *)stk->data - sizeof(canary_t) ), leveledSizeOfData);

    *( (canary_t *)(ptrToCanary + (leveledSizeOfData - sizeof(canary_t))) ) = canary;

    if (newCapacity > stk->capacity) {
        memset((void *) (stk->data + stk->capacity), poison, (newCapacity - stk->capacity) * sizeof(stackElem_t));
    }

    stk->capacity = newCapacity;
    stk->data     = (stackElem_t *)(ptrToCanary + sizeof(canary_t));
}

int stackCtor(Stack * stk, int initialSize, const char * name, int line, const char * func, const char * file)
{
    if (!stk) return stackVerifier(stk);

    stk->name = name;
    stk->line = line;
    stk->func = func;
    stk->file = file;

    stk->leftCanary  = canary;
    stk->rightCanary = canary;

    size_t sizeOfData = initialSize * sizeof(stackElem_t) + 2 * sizeof(canary_t);
    size_t leveledSizeOfData = sizeOfData % levelBytes > 0 ? sizeOfData + ( levelBytes - sizeOfData % levelBytes ) : sizeOfData;

    char * ptrToCanary = (char *)calloc(leveledSizeOfData, sizeof(char));
    if (!ptrToCanary) return stackVerifier(stk);

    *( (canary_t *)ptrToCanary ) = canary;
    *( (canary_t *)(ptrToCanary + (leveledSizeOfData - sizeof(canary_t))) ) = canary;

    stk->data = (stackElem_t *)(ptrToCanary + sizeof(canary_t));

    stk->size        = 0;
    stk->capacity    = initialSize;
    stk->minCapacity = initialSize;

    memset( (void *) stk->data, poison, initialSize * sizeof(stackElem_t) );

    stk->hash = countHashSum(stk);

    int errorCode = stackVerifier(stk);
    stk->status = errorCode;

    DO_LOG_DUMP(stk->logFile, stk, line, file);

    return errorCode;
}

int stackPush(Stack * stk, stackElem_t newElement, int line, const char * file)
{
    int error = stackVerifier(stk);
    if (error) return error;

    if (stk->size == stk->capacity) stackRealloc(stk, stk->capacity * 2);

    stk->data[stk->size] = newElement;

    stk->size += 1;
    stk->hash = countHashSum(stk);

    int errorCode = stackVerifier(stk);
    stk->status = errorCode;

    DO_LOG_DUMP(stk->logFile, stk, line, file);

    return errorCode;
}

int stackPop(Stack * stk, stackElem_t * poppedElement, int line, const char * file)
{
    int error = stackVerifier(stk);
    if (error) return error;

    if (stk->size == 0) {
        printf("Stack is clear. Nothing to pop!\n");
        return 0;
    }

    stk->size -= 1;

    *poppedElement = stk->data[stk->size];

    stk->data[stk->size] = poison;

    if ( (stk->size > stk->minCapacity) && (stk->size < stk->capacity / 4) ) stackRealloc(stk, stk->capacity / 2);

    stk->hash = countHashSum(stk);

    int errorCode = stackVerifier(stk);
    stk->status = errorCode;

    DO_LOG_DUMP(stk->logFile, stk, line, file);

    return errorCode;
}

void stackDtor(Stack * stk, int line, const char * file)
{
    free( (char *)stk->data - sizeof(canary_t) );
    stk->data = 0;
    stk->capacity = -1;
    DO_LOG_DUMP(stk->logFile, stk, line, file);
}

// html fprintf(file.ht)
void stackDump(Stack * stk, int lineDump, const char * funcDump, const char * fileDump)
{
    printf("INFO ABOUT STACK\n\n");

    int errorCode = stackVerifier(stk);
    printf("Actual code of error : %d\n", errorCode);
    readError(errorCode);

    printf("    Stack [%p]\n", stk);
    printf("    called from %s:%d (%s)\n", fileDump, lineDump, funcDump);
    printf("    name %s born at function %s at %s:%d\n\n", stk->name, stk->func, stk->file, stk->line);
    if (!stk) return;

    printf("ACTUAL INFORMATION\n\n");

    printf("    data     = %p\n", stk->data);
    printf("    size     = %d\n", stk->size);
    printf("    capacity = %d\n\n", stk->capacity);
    if (!stk->data) return;

    printf("ACTUAL DATA IN STACK\n\n");

    for(int i = 0; i < stk->size; i++) {
        printf("*[%d] = %f\n", i, stk->data[i]);
    }

    for(int i = stk->size; i < stk->capacity; i++) {
        printf(" [%d] = %f (poison)\n", i, stk->data[i]);
    }

    printf("\n");

}

int stackVerifier(Stack * stk)
{
    int error = 0;

    if (!stk)                                                                               return STK_NULL_PTR;
    if (!stk->data)                                                                         error |= STK_DATA_NULL;
    if (stk->capacity < 0)                                                                  error |= STK_CAPACITY_BELOW_ZERO;
    if (stk->capacity < stk->size)                                                          error |= STK_CAPACITY_LESS_SIZE;
    if (stk->leftCanary != canary)                                                          error |= STK_L_CANARY_DIED;
    if (stk->leftCanary != canary)                                                          error |= STK_R_CANARY_DIED;
    if (!stk->data)                                                                         return error;
    if (*(canary_t *)((char *)stk->data - sizeof(canary_t)) != canary )                     error |= STK_L_DATA_CANARY_DIED;
    if (*(canary_t *)((char *)stk->data + stk->capacity * sizeof(stackElem_t)) != canary )  error |= STK_R_DATA_CANARY_DIED;
    if (stk->hash != countHashSum(stk))                                                     error |= STK_INCORR_DATA_HASH;

    return error;
}

hash_t countHashSum(Stack * stk)
{
    hash_t hash = 0;
    for( int i = 0; i < stk->size; i++ ) {
        hash = hash * 33 ^ (hash_t)stk->data[i];
    }

    return hash;
}

void readError(int code)
{
    if (code == 0) {
        printf("No errors\n\n");
        return;
    }

    if (code % STK_DATA_NULL != 0) {
        printf("NULL ptr to structure\n");
        code -= STK_NULL_PTR;
    }
    if (code % STK_CAPACITY_BELOW_ZERO != 0) {
        printf("NULL ptr to data in structure\n");
        code -= STK_DATA_NULL;
    }
    if (code % STK_CAPACITY_LESS_SIZE != 0) {
        printf("Capacity of stack below zero\n");
        code -= STK_CAPACITY_BELOW_ZERO;
    }
    if (code % STK_L_CANARY_DIED != 0) {
        printf("Capacity is lower than size\n");
        code -= STK_CAPACITY_LESS_SIZE;
    }
    if (code % STK_R_CANARY_DIED != 0) {
        printf("Left canary of struct died\n");
        code -= STK_L_CANARY_DIED;
    }
    if (code % STK_L_DATA_CANARY_DIED != 0) {
        printf("Right canary of struct died\n");
        code -= STK_R_CANARY_DIED;
    }
    if (code % STK_R_DATA_CANARY_DIED != 0) {
        printf("Left canary of data died\n");
        code -= STK_L_DATA_CANARY_DIED;
    }
    if (code % STK_INCORR_DATA_HASH != 0) {
        printf("Right canary of data died\n");
        code -= STK_R_DATA_CANARY_DIED;
    }
    if (code % (1 << 10) != 0) {
        printf("Incorrect hash sum of data\n");
        code -= STK_INCORR_DATA_HASH;
    }

    printf("\n");
    return;
}

void logReadError(int code, FILE * logFile)
{
    if (code == 0) {
        fprintf(logFile, "<p>No errors</p>\n\n");
        return;
    }

    if (code % STK_DATA_NULL != 0) {
        fprintf(logFile, "<p>NULL ptr to structure</p>\n");
        code -= STK_NULL_PTR;
    }
    if (code % STK_CAPACITY_BELOW_ZERO != 0) {
        fprintf(logFile, "<p>NULL ptr to data in structure</p>\n");
        code -= STK_DATA_NULL;
    }
    if (code % STK_CAPACITY_LESS_SIZE != 0) {
        fprintf(logFile, "<p>Capacity of stack below zero</p>\n");
        code -= STK_CAPACITY_BELOW_ZERO;
    }
    if (code % STK_L_CANARY_DIED != 0) {
        fprintf(logFile, "<p>Capacity is lower than size</p>\n");
        code -= STK_CAPACITY_LESS_SIZE;
    }
    if (code % STK_R_CANARY_DIED != 0) {
        fprintf(logFile, "<p>Left canary of struct died</p>\n");
        code -= STK_L_CANARY_DIED;
    }
    if (code % STK_L_DATA_CANARY_DIED != 0) {
        fprintf(logFile, "<p>Right canary of struct died</p>\n");
        code -= STK_R_CANARY_DIED;
    }
    if (code % STK_R_DATA_CANARY_DIED != 0) {
        fprintf(logFile, "<p>Left canary of data died</p>\n");
        code -= STK_L_DATA_CANARY_DIED;
    }
    if (code % STK_INCORR_DATA_HASH != 0) {
        fprintf(logFile, "<p>Right canary of data died</p>\n");
        code -= STK_R_DATA_CANARY_DIED;
    }
    if (code % (1 << 10) != 0) {
        fprintf(logFile, "<p>Incorrect hash sum of data</p>\n");
        code -= STK_INCORR_DATA_HASH;
    }

    printf("\n");
    return;
}
