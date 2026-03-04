#ifndef LISP_H
#define LISP_H

#include <stdint.h>
#include <stdbool.h>

typedef enum {
    LISP_NIL,
    LISP_INT,
    LISP_FLOAT,
    LISP_STRING,
    LISP_SYMBOL,
    LISP_PAIR,
    LISP_CLOSURE
} LispType;

typedef struct LispValue {
    LispType type;
    uint32_t refcount;
    union {
        int64_t int_val;
        double float_val;
        char* string_val;
        char* symbol_name;
        struct {
            struct LispValue* car;
            struct LispValue* cdr;
        } pair;
        struct {
            void* func_ptr;
            struct LispValue* env;
        } closure;
    } data;
} LispValue;

#define LISP_TRUE ((LispValue*)1)
#define LISP_FALSE ((LispValue*)2)

LispValue* lisp_make_int(int64_t value);
LispValue* lisp_make_float(double value);
LispValue* lisp_make_string(const char* value);
LispValue* lisp_make_symbol(const char* name);
LispValue* lisp_cons(LispValue* car, LispValue* cdr);
LispValue* lisp_first(LispValue* pair);
LispValue* lisp_rest(LispValue* pair);

LispValue* lisp_add(LispValue* a, LispValue* b);
LispValue* lisp_sub(LispValue* a, LispValue* b);
LispValue* lisp_mul(LispValue* a, LispValue* b);
LispValue* lisp_div(LispValue* a, LispValue* b);

LispValue* lisp_eq(LispValue* a, LispValue* b);
LispValue* lisp_lt(LispValue* a, LispValue* b);
LispValue* lisp_gt(LispValue* a, LispValue* b);

bool lisp_is_nil(LispValue* val);
bool lisp_is_true(LispValue* val);

void lisp_retain(LispValue* val);
void lisp_release(LispValue* val);

void lisp_print(LispValue* val);

#endif