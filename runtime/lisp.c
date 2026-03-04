#define _POSIX_C_SOURCE 200809L
#include "lisp.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

static LispValue* alloc_value(LispType type) {
    LispValue* val = (LispValue*)malloc(sizeof(LispValue));
    val->type = type;
    val->refcount = 1;
    return val;
}

LispValue* lisp_make_int(int64_t value) {
    LispValue* val = alloc_value(LISP_INT);
    val->data.int_val = value;
    return val;
}

LispValue* lisp_make_float(double value) {
    LispValue* val = alloc_value(LISP_FLOAT);
    val->data.float_val = value;
    return val;
}

LispValue* lisp_make_string(const char* value) {
    LispValue* val = alloc_value(LISP_STRING);
    val->data.string_val = strdup(value);
    return val;
}

LispValue* lisp_make_symbol(const char* name) {
    LispValue* val = alloc_value(LISP_SYMBOL);
    val->data.symbol_name = strdup(name);
    return val;
}

LispValue* lisp_cons(LispValue* car, LispValue* cdr) {
    LispValue* val = alloc_value(LISP_PAIR);
    val->data.pair.car = car;
    val->data.pair.cdr = cdr;
    if (car) lisp_retain(car);
    if (cdr) lisp_retain(cdr);
    return val;
}

LispValue* lisp_first(LispValue* pair) {
    if (!pair || pair->type != LISP_PAIR) return NULL;
    return pair->data.pair.car;
}

LispValue* lisp_rest(LispValue* pair) {
    if (!pair || pair->type != LISP_PAIR) return NULL;
    return pair->data.pair.cdr;
}

static double to_float(LispValue* val) {
    if (!val) return 0.0;
    if (val->type == LISP_INT) return (double)val->data.int_val;
    if (val->type == LISP_FLOAT) return val->data.float_val;
    return 0.0;
}

static int64_t to_int(LispValue* val) {
    if (!val) return 0;
    if (val->type == LISP_INT) return val->data.int_val;
    if (val->type == LISP_FLOAT) return (int64_t)val->data.float_val;
    return 0;
}

LispValue* lisp_add(LispValue* a, LispValue* b) {
    if (!a || !b) return NULL;
    if (a->type == LISP_FLOAT || b->type == LISP_FLOAT) {
        return lisp_make_float(to_float(a) + to_float(b));
    }
    return lisp_make_int(to_int(a) + to_int(b));
}

LispValue* lisp_sub(LispValue* a, LispValue* b) {
    if (!a || !b) return NULL;
    if (a->type == LISP_FLOAT || b->type == LISP_FLOAT) {
        return lisp_make_float(to_float(a) - to_float(b));
    }
    return lisp_make_int(to_int(a) - to_int(b));
}

LispValue* lisp_mul(LispValue* a, LispValue* b) {
    if (!a || !b) return NULL;
    if (a->type == LISP_FLOAT || b->type == LISP_FLOAT) {
        return lisp_make_float(to_float(a) * to_float(b));
    }
    return lisp_make_int(to_int(a) * to_int(b));
}

LispValue* lisp_div(LispValue* a, LispValue* b) {
    if (!a || !b) return NULL;
    if (a->type == LISP_FLOAT || b->type == LISP_FLOAT) {
        return lisp_make_float(to_float(a) / to_float(b));
    }
    return lisp_make_int(to_int(a) / to_int(b));
}

LispValue* lisp_eq(LispValue* a, LispValue* b) {
    if (!a || !b) return LISP_FALSE;
    if (a->type != b->type) return LISP_FALSE;
    switch (a->type) {
        case LISP_INT: return a->data.int_val == b->data.int_val ? LISP_TRUE : LISP_FALSE;
        case LISP_FLOAT: return a->data.float_val == b->data.float_val ? LISP_TRUE : LISP_FALSE;
        case LISP_STRING: return strcmp(a->data.string_val, b->data.string_val) == 0 ? LISP_TRUE : LISP_FALSE;
        case LISP_SYMBOL: return strcmp(a->data.symbol_name, b->data.symbol_name) == 0 ? LISP_TRUE : LISP_FALSE;
        default: return a == b ? LISP_TRUE : LISP_FALSE;
    }
}

LispValue* lisp_lt(LispValue* a, LispValue* b) {
    if (!a || !b) return LISP_FALSE;
    return to_float(a) < to_float(b) ? LISP_TRUE : LISP_FALSE;
}

LispValue* lisp_gt(LispValue* a, LispValue* b) {
    if (!a || !b) return LISP_FALSE;
    return to_float(a) > to_float(b) ? LISP_TRUE : LISP_FALSE;
}

bool lisp_is_nil(LispValue* val) {
    return val == NULL || val->type == LISP_NIL;
}

bool lisp_is_true(LispValue* val) {
    return val != NULL && val != LISP_FALSE;
}

void lisp_retain(LispValue* val) {
    if (val && val != LISP_TRUE && val != LISP_FALSE) {
        val->refcount++;
    }
}

void lisp_release(LispValue* val) {
    if (!val || val == LISP_TRUE || val == LISP_FALSE) return;
    if (--val->refcount > 0) return;
    
    switch (val->type) {
        case LISP_STRING:
            free(val->data.string_val);
            break;
        case LISP_SYMBOL:
            free(val->data.symbol_name);
            break;
        case LISP_PAIR:
            lisp_release(val->data.pair.car);
            lisp_release(val->data.pair.cdr);
            break;
        case LISP_CLOSURE:
            lisp_release(val->data.closure.env);
            break;
        default:
            break;
    }
    free(val);
}

static void print_value(LispValue* val) {
    if (val == LISP_TRUE) {
        printf("true");
        return;
    }
    if (val == LISP_FALSE) {
        printf("false");
        return;
    }
    if (!val) {
        printf("nil");
        return;
    }
    
    switch (val->type) {
        case LISP_NIL:
            printf("nil");
            break;
        case LISP_INT:
            printf("%ld", val->data.int_val);
            break;
        case LISP_FLOAT:
            printf("%g", val->data.float_val);
            break;
        case LISP_STRING:
            printf("\"%s\"", val->data.string_val);
            break;
        case LISP_SYMBOL:
            printf("%s", val->data.symbol_name);
            break;
        case LISP_PAIR:
            printf("(");
            print_value(val->data.pair.car);
            LispValue* curr = val->data.pair.cdr;
            while (curr && curr->type == LISP_PAIR) {
                printf(" ");
                print_value(curr->data.pair.car);
                curr = curr->data.pair.cdr;
            }
            if (curr && !lisp_is_nil(curr)) {
                printf(" . ");
                print_value(curr);
            }
            printf(")");
            break;
        default:
            printf("#<unknown>");
    }
}

void lisp_print(LispValue* val) {
    print_value(val);
    printf("\n");
}