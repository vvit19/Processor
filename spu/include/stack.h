#ifndef STACK_H
#define STACK_H

#include <cstdio>
#define _DEBUG

typedef double elem_t;

const int MIN_CAPACITY = 5;
const int GARBAGE = 696969;
const long long DTOR_GARBAGE = 0xCAFEBABE;

#ifdef _CANARY_PROTECTION

typedef unsigned long long canary_t;
const canary_t CANARY_CONST = 0xDEADBABE;

#endif

const int ERRORS_NUM = 7;

enum stack_errors
{
    NO_ERROR = 0,
    STACK_NULLPTR = 1 << 0,
    NEGATIVE_SIZE = 1 << 1,
    NEGATIVE_CAPACITY = 1 << 2,
    DATARRAY_NULLPTR  = 1 << 3,
    SIZE_BIGGER_THAN_CAPACITY = 1 << 4,
    ACTIONS_AFTER_DTOR        = 1 << 5,

    #ifdef _CANARY_PROTECTION
        LEFT_CANARY_DATA_ERROR    = 1 << 6,
        RIGHT_CANARY_DATA_ERROR   = 1 << 7,
        LEFT_CANARY_STRUCT_ERROR  = 1 << 8,
        RIGHT_CANARY_STRUCT_ERROR = 1 << 9,
    #endif

    #ifdef _HASH_PROTECTION
        HASH_DETECTED_INVALID_CHANGES_STRUCT = 1 << 10,
        HASH_DETECTED_INVALID_CHANGES_DATA   = 1 << 11,
    #endif
};  //do not forget to change errors num due to flags you have

#ifdef _DEBUG

struct function_info
{
    const char* variable_name;
    const char* function_name;
    const char* filename;
    int         line;
};

#endif

struct stack
{
    #ifdef _CANARY_PROTECTION
        canary_t left_canary_struct;
    #endif

    #ifdef _HASH_PROTECTION
        long unsigned int hash_struct;
        long unsigned int hash_data;
    #endif

    elem_t* data;
    long long capacity;
    long long size;

    #ifdef _DEBUG
        function_info func_info;
    #endif

    #ifdef _CANARY_PROTECTION
        canary_t* left_canary_data;
        canary_t* right_canary_data;
    #endif

    #ifdef _CANARY_PROTECTION
        canary_t right_canary_struct;
    #endif
};

#ifdef _DEBUG
    #define ON_DEBUG(...)  __VA_ARGS__
#else
    #define ON_DEBUG(...)
#endif

stack_errors stack_ctor(stack* stk ON_DEBUG(, function_info func_info));
#define STACK_CTOR(stk) stack_ctor(stk ON_DEBUG(, {#stk, __PRETTY_FUNCTION__, __FILE__, __LINE__} ))

stack_errors stack_push (stack* stk, elem_t value);
stack_errors stack_pop(stack* stk, elem_t* popped_value);
stack_errors stack_verify(stack* stk);
void stack_dtor (stack* stk);

// #ifdef _DEBUG
    void stack_dump(stack* stk);
// #endif

#ifdef _HASH_PROTECTION

long unsigned int poltorashka_hash(const char* key, long unsigned int len);

#define HASH_PROTECTION_FUNCTION_CALL() \
    stk->hash_struct = stk->hash_data = 0; \
    stk->hash_struct = poltorashka_hash((const char*) stk, sizeof(stack)); \
    stk->hash_data   = poltorashka_hash((const char*) stk->data, sizeof(elem_t) * stk->capacity); \

#endif

#endif
