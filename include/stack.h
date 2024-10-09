#include <cstdint>
#include <cstdio>

#define ASSERT_STACK(stk) {                                                 \
    if( !stackOk(stk) ) {                                                   \
		stackDump(stk);                                                     \
        printf("Assertion failed: file %s, line %d\n", __FILE__, __LINE__); \
    }                                                                       \
}

#define DO_STACK_CTOR(stk, initialSize) {                                  \
	stackCtor(stk, initialSize, #stk, __LINE__, __func__, __FILE__);       \
}

#define DO_STACK_DUMP(stk) {                                               \
	stackDump(stk, __LINE__, __func__, __FILE__);                          \
}

#define DO_STACK_REALLOC(stk, newSize) {                                   \
	stackRealloc(stk, newSize, __LINE__, __FILE__);                        \
}

#define DO_STACK_PUSH(stk, newElement) {                                   \
	stackPush(stk, newElement, __LINE__, __FILE__);                        \
}

#define DO_STACK_POP(stk, pushedElement) {                                 \
	stackPop(stk, pushedElement, __LINE__, __FILE__);                      \
}

#define DO_STACK_DTOR(stk) {                                               \
	stackDtor(stk, __LINE__, __FILE__);                                    \
}

typedef double  stackElem_t; //
typedef int64_t canary_t;
typedef int64_t hash_t;

const canary_t    canary     = 0xBEB4ABEB4A;
const stackElem_t poison     = 0xADBADB;
const int         levelBytes = 8;

enum StackError {
	STK_NULL_PTR            = 1 << 1,
	STK_DATA_NULL           = 1 << 2,
    STK_CAPACITY_BELOW_ZERO = 1 << 3,
	STK_CAPACITY_LESS_SIZE  = 1 << 4,
	STK_L_CANARY_DIED       = 1 << 5,
	STK_R_CANARY_DIED       = 1 << 6,
	STK_L_DATA_CANARY_DIED  = 1 << 7,
	STK_R_DATA_CANARY_DIED  = 1 << 8,
	STK_INCORR_DATA_HASH    = 1 << 9,
	STK_INCORR_STR_HASH     = 1 << 10
};

typedef struct
{
	canary_t leftCanary;

	FILE * logFile;

	const char * name;
	const char * func;
	const char * file;
	int line;

	int status;

	stackElem_t * data;
	int size;
	int capacity;
	int minCapacity;

	hash_t hash;
	hash_t strHash;

	canary_t rightCanary;
} Stack;

int stackCtor(Stack * stk, int initialSize, const char * name, int line, const char * func, const char * file);

int stackPush(Stack * stk, stackElem_t newElement, int line, const char * file);

int stackPop(Stack * stk, stackElem_t * poppedElement, int line, const char * file);

void stackDtor(Stack * stk, int line, const char * file);

void stackDump(Stack * stk, int lineDump, const char * funcDump, const char * fileDump);

int stackVerifier(Stack * stk);

hash_t countHashSum(Stack * stk);

hash_t countStrHashSum(Stack * stk);

void readError(int code);

void logReadError(int code, FILE * logFile);
