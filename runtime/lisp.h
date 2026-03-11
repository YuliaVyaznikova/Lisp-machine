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
    LISP_CLOSURE,
    LISP_BINDING
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
        struct {
            char* name;
            struct LispValue* value;
            struct LispValue* parent;
        } binding;
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
LispValue* lisp_list_get(LispValue* list, int index);
int lisp_list_length(LispValue* list);

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

LispValue* lisp_make_binding(const char* name, LispValue* value, LispValue* parent);
LispValue* lisp_env_lookup(LispValue* env, const char* name);
LispValue* lisp_env_extend(LispValue* env, const char* name, LispValue* value);
LispValue* lisp_make_closure(void* func_ptr, LispValue* env);
LispValue* lisp_call_closure(LispValue* closure, LispValue* args);

void lisp_print(LispValue* val);

#endif