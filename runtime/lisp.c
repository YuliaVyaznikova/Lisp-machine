#define _POSIX_C_SOURCE 200809L
#include "lisp.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <pthread.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdatomic.h>

#define GC_MAX_OBJECTS 100000
#define GC_MAX_ROOTS 10000
#define GC_DEBUG 0

typedef struct {
    LispValue* objects[GC_MAX_OBJECTS];
    size_t count;
    LispValue* roots[GC_MAX_ROOTS];  
    size_t root_count;
    pthread_mutex_t lock;
    pthread_mutex_t world_lock;
    pthread_t thread;
    int running;
    int world_stopped;
    size_t total_allocated;
    size_t total_freed;
} GCGlobals;

static GCGlobals gc = {0};

void gc_add_object(LispValue* val) {
    if (!val || val == LISP_TRUE || val == LISP_FALSE) return;
    pthread_mutex_lock(&gc.world_lock);
    pthread_mutex_lock(&gc.lock);
    if (gc.count < GC_MAX_OBJECTS) {
        gc.objects[gc.count++] = val;
        gc.total_allocated++;
    }
    pthread_mutex_unlock(&gc.lock);
    pthread_mutex_unlock(&gc.world_lock);
}

void gc_remove_object(LispValue* val) {
    if (!val || val == LISP_TRUE || val == LISP_FALSE) return;
    pthread_mutex_lock(&gc.world_lock);
    pthread_mutex_lock(&gc.lock);
    for (size_t i = 0; i < gc.count; i++) {
        if (gc.objects[i] == val) {
            gc.objects[i] = gc.objects[--gc.count];
            gc.total_freed++;
            break;
        }
    }
    pthread_mutex_unlock(&gc.lock);
    pthread_mutex_unlock(&gc.world_lock);
}

static void gc_stop_world(void) {
    pthread_mutex_lock(&gc.world_lock);
    gc.world_stopped = 1;
}

static void gc_resume_world(void) {
    gc.world_stopped = 0;
    pthread_mutex_unlock(&gc.world_lock);
}

void gc_push_root(LispValue* val) {
    if (!val || val == LISP_TRUE || val == LISP_FALSE) return;
    pthread_mutex_lock(&gc.lock);
    if (gc.root_count < GC_MAX_ROOTS) {
        gc.roots[gc.root_count++] = val;
    }
    pthread_mutex_unlock(&gc.lock);
}

void gc_pop_root(LispValue* val) {
    if (!val || val == LISP_TRUE || val == LISP_FALSE) return;
    pthread_mutex_lock(&gc.lock);
    for (size_t i = gc.root_count; i > 0; i--) {
        if (gc.roots[i-1] == val) {
            gc.roots[i-1] = gc.roots[--gc.root_count];
            break;
        }
    }
    pthread_mutex_unlock(&gc.lock);
}

static void gc_decrement_all(void) {
    for (size_t i = 0; i < gc.count; i++) {
        LispValue* val = gc.objects[i];
        switch (val->type) {
            case LISP_PAIR:
                if (val->data.pair.car && val->data.pair.car != LISP_TRUE && val->data.pair.car != LISP_FALSE) {
                    atomic_fetch_sub(&val->data.pair.car->refcount, 1);
                }
                if (val->data.pair.cdr && val->data.pair.cdr != LISP_TRUE && val->data.pair.cdr != LISP_FALSE) {
                    atomic_fetch_sub(&val->data.pair.cdr->refcount, 1);
                }
                break;
            case LISP_CLOSURE:
                if (val->data.closure.env) {
                    atomic_fetch_sub(&val->data.closure.env->refcount, 1);
                }
                break;
            case LISP_BINDING:
                if (val->data.binding.value && val->data.binding.value != LISP_TRUE && val->data.binding.value != LISP_FALSE) {
                    atomic_fetch_sub(&val->data.binding.value->refcount, 1);
                }
                if (val->data.binding.parent) {
                    atomic_fetch_sub(&val->data.binding.parent->refcount, 1);
                }
                break;
            default:
                break;
        }
    }
}

static void gc_mark_reachable(LispValue* val) {
    if (!val || val == LISP_TRUE || val == LISP_FALSE) return;
    if (val->refcount & 0x80000000) return;
    
    val->refcount |= 0x80000000;
    
    switch (val->type) {
        case LISP_PAIR:
            gc_mark_reachable(val->data.pair.car);
            gc_mark_reachable(val->data.pair.cdr);
            break;
        case LISP_CLOSURE:
            gc_mark_reachable(val->data.closure.env);
            break;
        case LISP_BINDING:
            gc_mark_reachable(val->data.binding.value);
            gc_mark_reachable(val->data.binding.parent);
            break;
        default:
            break;
    }
}

static void gc_restore_refs(LispValue* val) {
    if (!val || val == LISP_TRUE || val == LISP_FALSE) return;
    if (!(val->refcount & 0x80000000)) return;
    
    val->refcount &= 0x7FFFFFFF;
    
    switch (val->type) {
        case LISP_PAIR:
            if (val->data.pair.car && val->data.pair.car != LISP_TRUE && val->data.pair.car != LISP_FALSE) {
                atomic_fetch_add(&val->data.pair.car->refcount, 1);
            }
            if (val->data.pair.cdr && val->data.pair.cdr != LISP_TRUE && val->data.pair.cdr != LISP_FALSE) {
                atomic_fetch_add(&val->data.pair.cdr->refcount, 1);
            }
            gc_restore_refs(val->data.pair.car);
            gc_restore_refs(val->data.pair.cdr);
            break;
        case LISP_CLOSURE:
            if (val->data.closure.env) {
                atomic_fetch_add(&val->data.closure.env->refcount, 1);
            }
            gc_restore_refs(val->data.closure.env);
            break;
        case LISP_BINDING:
            if (val->data.binding.value && val->data.binding.value != LISP_TRUE && val->data.binding.value != LISP_FALSE) {
                atomic_fetch_add(&val->data.binding.value->refcount, 1);
            }
            if (val->data.binding.parent) {
                atomic_fetch_add(&val->data.binding.parent->refcount, 1);
            }
            gc_restore_refs(val->data.binding.value);
            gc_restore_refs(val->data.binding.parent);
            break;
        default:
            break;
    }
}

static void gc_sweep(void) {
    for (size_t i = 0; i < gc.count; ) {
        LispValue* val = gc.objects[i];
        
        if (val->refcount & 0x80000000) {
            i++;
        } else if ((val->refcount & 0x7FFFFFFF) > 0) {
            i++;
        } else {
            gc.objects[i] = gc.objects[--gc.count];
            gc.total_freed++;
            
            switch (val->type) {
                case LISP_STRING:
                    free(val->data.string_val);
                    break;
                case LISP_SYMBOL:
                    free(val->data.symbol_name);
                    break;
                case LISP_BINDING:
                    free(val->data.binding.name);
                    break;
                default:
                    break;
            }
            free(val);
        }
    }
}

void gc_collect_cycles(void) {
    gc_stop_world();
    pthread_mutex_lock(&gc.lock);
    
#if GC_DEBUG
    printf("[GC] Starting cycle collection, objects=%zu, roots=%zu\n", gc.count, gc.root_count);
#endif
    
    gc_decrement_all();
    
    for (size_t i = 0; i < gc.root_count; i++) {
        gc_mark_reachable(gc.roots[i]);
    }
    
    for (size_t i = 0; i < gc.root_count; i++) {
        gc_restore_refs(gc.roots[i]);
    }
    
    gc_sweep();
    
#if GC_DEBUG
    printf("[GC] Collection complete, remaining=%zu\n", gc.count);
#endif
    
    pthread_mutex_unlock(&gc.lock);
    gc_resume_world();
}

static void* gc_thread_func(void* arg) {
    (void)arg;
    while (gc.running) {
        sleep(5);
        if (gc.running) {
            gc_collect_cycles();
        }
    }
    return NULL;
}

void gc_init(void) {
    gc.count = 0;
    gc.root_count = 0;
    gc.running = 1;
    gc.world_stopped = 0;
    gc.total_allocated = 0;
    gc.total_freed = 0;
    pthread_mutex_init(&gc.lock, NULL);
    pthread_mutex_init(&gc.world_lock, NULL);
    pthread_create(&gc.thread, NULL, gc_thread_func, NULL);
}

void gc_shutdown(void) {
    gc.running = 0;
    pthread_join(gc.thread, NULL);
    pthread_mutex_destroy(&gc.lock);
    pthread_mutex_destroy(&gc.world_lock);
    
    for (size_t i = 0; i < gc.count; i++) {
        LispValue* val = gc.objects[i];
        switch (val->type) {
            case LISP_STRING:
                free(val->data.string_val);
                break;
            case LISP_SYMBOL:
                free(val->data.symbol_name);
                break;
            case LISP_BINDING:
                free(val->data.binding.name);
                break;
            default:
                break;
        }
        free(val);
    }
}

void gc_print_stats(void) {
    printf("GC Stats: allocated=%zu, freed=%zu, alive=%zu\n",
           gc.total_allocated, gc.total_freed, gc.count);
}

size_t gc_get_allocated(void) { return gc.total_allocated; }
size_t gc_get_freed(void) { return gc.total_freed; }
size_t gc_get_alive(void) { return gc.count; }

static LispValue* alloc_value(LispType type) {
    LispValue* val = (LispValue*)malloc(sizeof(LispValue));
    val->type = type;
    val->refcount = 1;
    gc_add_object(val);
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

LispValue* lisp_list_get(LispValue* list, int index) {
    LispValue* curr = list;
    for (int i = 0; i < index && curr && curr->type == LISP_PAIR; i++) {
        curr = curr->data.pair.cdr;
    }
    if (curr && curr->type == LISP_PAIR) {
        return curr->data.pair.car;
    }
    return NULL;
}

int lisp_list_length(LispValue* list) {
    int len = 0;
    LispValue* curr = list;
    while (curr && curr->type == LISP_PAIR) {
        len++;
        curr = curr->data.pair.cdr;
    }
    return len;
}

LispValue* lisp_length(LispValue* list) {
    return lisp_make_int(lisp_list_length(list));
}

LispValue* lisp_append(LispValue* lst1, LispValue* lst2) {
    if (!lst1 || lst1->type != LISP_PAIR) return lst2;
    return lisp_cons(lst1->data.pair.car, lisp_append(lst1->data.pair.cdr, lst2));
}

LispValue* lisp_reverse(LispValue* lst) {
    LispValue* result = NULL;
    LispValue* curr = lst;
    while (curr && curr->type == LISP_PAIR) {
        result = lisp_cons(curr->data.pair.car, result);
        curr = curr->data.pair.cdr;
    }
    return result;
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

LispValue* lisp_not(LispValue* val) {
    return lisp_is_true(val) ? LISP_FALSE : LISP_TRUE;
}

LispValue* lisp_mod(LispValue* a, LispValue* b) {
    if (!a || !b) return NULL;
    return lisp_make_int(to_int(a) % to_int(b));
}

LispValue* lisp_abs(LispValue* val) {
    if (!val) return NULL;
    if (val->type == LISP_FLOAT) return lisp_make_float(fabs(val->data.float_val));
    int64_t v = to_int(val);
    return lisp_make_int(v < 0 ? -v : v);
}

LispValue* lisp_min(LispValue* a, LispValue* b) {
    if (!a || !b) return NULL;
    return to_float(a) < to_float(b) ? a : b;
}

LispValue* lisp_max(LispValue* a, LispValue* b) {
    if (!a || !b) return NULL;
    return to_float(a) > to_float(b) ? a : b;
}

bool lisp_is_nil(LispValue* val) {
    return val == NULL || val->type == LISP_NIL;
}

bool lisp_is_true(LispValue* val) {
    return val != NULL && val != LISP_FALSE;
}

LispValue* lisp_is_nil_fn(LispValue* val) {
    return lisp_is_nil(val) ? LISP_TRUE : LISP_FALSE;
}

void lisp_retain(LispValue* val) {
    if (val && val != LISP_TRUE && val != LISP_FALSE) {
        atomic_fetch_add(&val->refcount, 1);
    }
}

void lisp_release(LispValue* val) {
    if (!val || val == LISP_TRUE || val == LISP_FALSE) return;
    
    // Atomic decrement, check if we should free
    uint32_t old_count = atomic_fetch_sub(&val->refcount, 1);
    if (old_count > 1) return;  // Still has references
    
    // refcount is now 0, safe to free (no race possible)
    // Remove from GC registry first
    pthread_mutex_lock(&gc.world_lock);
    pthread_mutex_lock(&gc.lock);
    for (size_t i = 0; i < gc.count; i++) {
        if (gc.objects[i] == val) {
            gc.objects[i] = gc.objects[--gc.count];
            gc.total_freed++;
            break;
        }
    }
    pthread_mutex_unlock(&gc.lock);
    pthread_mutex_unlock(&gc.world_lock);
    
    // Free children (they may have other references)
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
        case LISP_BINDING:
            free(val->data.binding.name);
            lisp_release(val->data.binding.value);
            lisp_release(val->data.binding.parent);
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

LispValue* lisp_make_binding(const char* name, LispValue* value, LispValue* parent) {
    LispValue* val = alloc_value(LISP_BINDING);
    val->data.binding.name = strdup(name);
    val->data.binding.value = value;
    val->data.binding.parent = parent;
    if (value) lisp_retain(value);
    if (parent) lisp_retain(parent);
    return val;
}

LispValue* lisp_env_lookup(LispValue* env, const char* name) {
    while (env) {
        if (env->type == LISP_BINDING) {
            if (strcmp(env->data.binding.name, name) == 0) {
                return env->data.binding.value;
            }
            env = env->data.binding.parent;
        } else {
            break;
        }
    }
    return NULL;
}

LispValue* lisp_env_extend(LispValue* env, const char* name, LispValue* value) {
    return lisp_make_binding(name, value, env);
}

LispValue* lisp_make_closure(void* func_ptr, LispValue* env) {
    LispValue* val = alloc_value(LISP_CLOSURE);
    val->data.closure.func_ptr = func_ptr;
    val->data.closure.env = env;
    if (env) lisp_retain(env);
    return val;
}

typedef LispValue* (*ClosureFunc)(LispValue* args, LispValue* env);

LispValue* lisp_call_closure(LispValue* closure, LispValue* args) {
    if (!closure || closure->type != LISP_CLOSURE) return NULL;
    ClosureFunc func = (ClosureFunc)closure->data.closure.func_ptr;
    return func(args, closure->data.closure.env);
}

LispValue* lisp_apply(LispValue* func, LispValue* args) {
    if (!func) return NULL;
    
    if (func->type == LISP_CLOSURE) {
        return lisp_call_closure(func, args);
    }
    
    if (func->type == LISP_SYMBOL) {
        const char* name = func->data.symbol_name;
        
        LispValue* a = lisp_list_get(args, 0);
        LispValue* b = lisp_list_get(args, 1);
        
        if (strcmp(name, "+") == 0) {
            LispValue* result = a;
            LispValue* curr = args;
            while (curr && curr->type == LISP_PAIR) {
                if (curr != args) {
                    result = lisp_add(result, curr->data.pair.car);
                }
                curr = curr->data.pair.cdr;
            }
            return result;
        }
        if (strcmp(name, "-") == 0) return lisp_sub(a, b);
        if (strcmp(name, "*") == 0) {
            LispValue* result = a;
            LispValue* curr = args;
            while (curr && curr->type == LISP_PAIR) {
                if (curr != args) {
                    result = lisp_mul(result, curr->data.pair.car);
                }
                curr = curr->data.pair.cdr;
            }
            return result;
        }
        if (strcmp(name, "/") == 0) return lisp_div(a, b);
        if (strcmp(name, "=") == 0) return lisp_eq(a, b);
        if (strcmp(name, "<") == 0) return lisp_lt(a, b);
        if (strcmp(name, ">") == 0) return lisp_gt(a, b);
        if (strcmp(name, "first") == 0) return lisp_first(a);
        if (strcmp(name, "rest") == 0) return lisp_rest(a);
        if (strcmp(name, "cons") == 0) return lisp_cons(a, b);
        if (strcmp(name, "print") == 0) {
            lisp_print(a);
            return a;
        }
    }
    
    return NULL;
}

LispValue* lisp_add_wrapper(LispValue* __args, LispValue* __env) {
    LispValue* a = lisp_list_get(__args, 0);
    LispValue* b = lisp_list_get(__args, 1);
    return lisp_add(a, b);
}

LispValue* lisp_sub_wrapper(LispValue* __args, LispValue* __env) {
    LispValue* a = lisp_list_get(__args, 0);
    LispValue* b = lisp_list_get(__args, 1);
    return lisp_sub(a, b);
}

LispValue* lisp_mul_wrapper(LispValue* __args, LispValue* __env) {
    LispValue* a = lisp_list_get(__args, 0);
    LispValue* b = lisp_list_get(__args, 1);
    return lisp_mul(a, b);
}

LispValue* lisp_div_wrapper(LispValue* __args, LispValue* __env) {
    LispValue* a = lisp_list_get(__args, 0);
    LispValue* b = lisp_list_get(__args, 1);
    return lisp_div(a, b);
}

LispValue* lisp_eq_wrapper(LispValue* __args, LispValue* __env) {
    LispValue* a = lisp_list_get(__args, 0);
    LispValue* b = lisp_list_get(__args, 1);
    return lisp_eq(a, b);
}

LispValue* lisp_lt_wrapper(LispValue* __args, LispValue* __env) {
    LispValue* a = lisp_list_get(__args, 0);
    LispValue* b = lisp_list_get(__args, 1);
    return lisp_lt(a, b);
}

LispValue* lisp_gt_wrapper(LispValue* __args, LispValue* __env) {
    LispValue* a = lisp_list_get(__args, 0);
    LispValue* b = lisp_list_get(__args, 1);
    return lisp_gt(a, b);
}

LispValue* lisp_first_wrapper(LispValue* __args, LispValue* __env) {
    return lisp_first(lisp_list_get(__args, 0));
}

LispValue* lisp_rest_wrapper(LispValue* __args, LispValue* __env) {
    return lisp_rest(lisp_list_get(__args, 0));
}

LispValue* lisp_cons_wrapper(LispValue* __args, LispValue* __env) {
    LispValue* a = lisp_list_get(__args, 0);
    LispValue* b = lisp_list_get(__args, 1);
    return lisp_cons(a, b);
}

LispValue* lisp_print_wrapper(LispValue* __args, LispValue* __env) {
    lisp_print(lisp_list_get(__args, 0));
    return NULL;
}

// --- Новые функции для GC и циклов ---

LispValue* lisp_set_cdr(LispValue* pair, LispValue* value) {
    if (!pair || pair->type != LISP_PAIR) return NULL;
    LispValue* old = pair->data.pair.cdr;
    pair->data.pair.cdr = value;
    
    if (value) lisp_retain(value);
    if (old) lisp_release(old); // Очищаем старую ссылку, если она была
    return pair;
}

LispValue* lisp_set_cdr_wrapper(LispValue* __args, LispValue* __env) {
    return lisp_set_cdr(lisp_list_get(__args, 0), lisp_list_get(__args, 1));
}

LispValue* lisp_gc_collect_wrapper(LispValue* __args, LispValue* __env) {
    gc_collect_cycles();
    return LISP_NIL;
}

LispValue* lisp_gc_stats_wrapper(LispValue* __args, LispValue* __env) {
    gc_print_stats();
    return LISP_NIL;
}

LispValue* lisp_drop_wrapper(LispValue* __args, LispValue* __env) {
    lisp_release(lisp_list_get(__args, 0));
    return LISP_NIL;
}
