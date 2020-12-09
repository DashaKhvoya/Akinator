#pragma once
#include <stdio.h>
#include <assert.h>
#include <math.h>

typedef Node* Elem_t;
typedef bool error_t;

struct Stack_t {
  #ifdef DEBUG
    uint64_t    canary_1;
    const char* name;
  #endif

    size_t      size;
    size_t      capacity;
    Elem_t*     data;

  #ifdef DEBUG
    uint64_t    hash;
    uint64_t    canary_2;
  #endif
};

#ifdef DEBUG
  #define ASSERT_OK(thou) StackDump(thou); if(IsOK(thou) != 0) {assert(0);}
  #define construct(thou) thou.name = #thou; StackConstruct(&thou, 100)
#else
  #define ASSERT_OK(thou)
  #define construct(thou) StackConstruct(&thou, 100)
#endif

uint64_t Hash(const Elem_t* data, size_t capacity);
int      IsOK(const Stack_t* thou);
void     StackDump(const Stack_t* thou);
void     StackConstruct(Stack_t* thou, size_t capacity);
void     StackDestruct(Stack_t* thou);
void     StackRealloc (Stack_t* thou, size_t new_capacity);
error_t  StackPush(Stack_t* thou, Elem_t elem);
error_t  StackPop(Stack_t* thou);
Elem_t   StackTop(const Stack_t* thou);
