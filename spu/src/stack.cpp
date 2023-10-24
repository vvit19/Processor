#include <cassert>
#include <cstdlib>
#include <cstring>

#include "stack.h"

static elem_t* stack_recalloc(stack* stk, long long new_size, long long old_size);
static void stack_recalloc_up(stack* stk);
static void stack_recalloc_down(stack* stk);
static void fill_garbage(stack* stk, long long new_size, long long old_size);

stack_errors stack_ctor(stack* stk ON_DEBUG(, function_info info))
{
    assert(stk);

    #ifdef _CANARY_PROTECTION

    char* temp = (char*) calloc(1, MIN_CAPACITY * sizeof(elem_t) + 2 * sizeof(canary_t));

    stk->data = (elem_t*) ((size_t) temp + sizeof(canary_t));

    stk->left_canary_data  = (canary_t*) temp;
    stk->right_canary_data = (canary_t*) ((size_t) temp + MIN_CAPACITY * sizeof(elem_t) + sizeof(canary_t));

    memcpy(stk->left_canary_data,  &CANARY_CONST, sizeof(canary_t));
    memcpy(stk->right_canary_data, &CANARY_CONST, sizeof(canary_t));

    stk->left_canary_struct  = CANARY_CONST;
    stk->right_canary_struct = CANARY_CONST;

    #else
        stk->data = (elem_t*) calloc(MIN_CAPACITY, sizeof(elem_t));
    #endif

    stk->size = 0;
    stk->capacity = MIN_CAPACITY;

    #ifdef _DEBUG
        stk->func_info = info;
    #endif

    #ifdef _HASH_PROTECTION
        HASH_PROTECTION_FUNCTION_CALL()
    #endif

    stack_errors exit_code = stack_verify(stk);
    return exit_code;
}

stack_errors stack_push(stack* stk, elem_t value)
{
    stack_verify(stk);

    if (stk->size >= stk->capacity)
    {
        stack_recalloc_up(stk);
    }

    if (stk->size < -1) stk->size = 0;

    stk->data[stk->size++] = value;

    #ifdef _HASH_PROTECTION
        HASH_PROTECTION_FUNCTION_CALL()
    #endif

    stack_errors exit_code = stack_verify(stk);
    return exit_code;
}

stack_errors stack_pop(stack* stk, elem_t* popped_value)
{
    stack_verify(stk);

    if (stk->size <= stk->capacity / 4 && stk->capacity >= MIN_CAPACITY)
    {
        stack_recalloc_down(stk);
    }

    stk->size--;
    *popped_value = stk->data[stk->size];
    stk->data[stk->size] = GARBAGE;

    #ifdef _HASH_PROTECTION
        HASH_PROTECTION_FUNCTION_CALL()
    #endif

    stack_errors exit_code = stack_verify(stk);
    return exit_code;
}

void stack_dtor(stack* stk)
{
    stack_verify(stk);

    stk->capacity = DTOR_GARBAGE;
    stk->size     = DTOR_GARBAGE;

    #ifdef _CANARY_PROTECTION
        free(stk->left_canary_data);

        stk->left_canary_struct = stk->right_canary_struct = 0;

        stk->func_info = {};
    #else
        free(stk->data);
    #endif

    #ifdef _HASH_PROTECTION
        stk->hash_struct = stk->hash_data = 0;
        stk->hash_struct = poltorashka_hash((const char*) stk, sizeof(stack));
    #endif
}

static void stack_recalloc_up(stack* stk)
{
    long long old_capacity = stk->capacity;
    stk->capacity *= 2;
    stk->data = stack_recalloc(stk, stk->capacity, old_capacity);
}

static void stack_recalloc_down(stack* stk)
{
    long long old_capacity = stk->capacity;
    stk->capacity /= 2;
    stk->data = stack_recalloc(stk, stk->capacity, old_capacity);
}

static elem_t* stack_recalloc(stack* stk, long long new_size, long long old_size)
{
    #ifdef _CANARY_PROTECTION

    memcpy(stk->left_canary_data,  &GARBAGE, sizeof(GARBAGE));
    memcpy(stk->right_canary_data, &GARBAGE, sizeof(GARBAGE));

    char* temp = (char*) ((size_t) stk->data - sizeof(canary_t));

    temp = (char*) realloc(temp, new_size * sizeof(elem_t) + 2 * sizeof(canary_t));
    assert(temp);

    stk->data = (elem_t*) (temp + sizeof(canary_t));

    stk->left_canary_data  = (canary_t*) temp;
    stk->right_canary_data = (canary_t*) ((size_t) temp + stk->capacity * sizeof(elem_t) + sizeof(canary_t));

    stk->data = (elem_t*) ((size_t) temp + sizeof(canary_t));

    memcpy(stk->left_canary_data,  &CANARY_CONST, sizeof(canary_t));
    memcpy(stk->right_canary_data, &CANARY_CONST, sizeof(canary_t));

    #else

    stk->data = (elem_t*) realloc(stk->data, new_size * sizeof(elem_t));

    #endif

    if (new_size > old_size)
    {
        fill_garbage(stk, new_size, old_size);
    }

    return stk->data;
}

static void fill_garbage(stack* stk, long long new_size, long long old_size)
{
    if (new_size > old_size)
    {
        for (long long i = old_size; i < new_size; i++)
        {
            (stk->data)[i] = GARBAGE;
        }
    }
}

#ifdef _HASH_PROTECTION

long unsigned int poltorashka_hash(const char* key, long unsigned int len)
{
    const long unsigned int m = 0x5bd1e995;
    const long unsigned int seed = 0;
    const int r = 24;

    long unsigned int h = seed ^ len;

    const unsigned char* data = (const unsigned char*) key;
    long unsigned int k = 0;

    while (len >= 4)
    {
        k  = data[0];
        k |= data[1] << 8;
        k |= data[2] << 16;
        k |= data[3] << 24;

        k *= m;
        k ^= k >> r;
        k *= m;

        h *= m;
        h ^= k;

        data += 4;
        len -= 4;
    }

    if (len == 3)
    {
        h ^= data[2] << 16;
        h ^= data[1] << 8;
        h ^= data[0];
        h *= m;
    }
    else if (len == 2)
    {
        h ^= data[1] << 8;
        h ^= data[0];
        h *= m;
    }
    else if (len == 1)
    {
        h ^= data[0];
        h *= m;
    }

    h ^= h >> 13;
    h *= m;
    h ^= h >> 15;

    return h;
}

#endif
