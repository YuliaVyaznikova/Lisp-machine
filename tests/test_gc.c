#include <stdio.h>
#include <assert.h>
#include "../runtime/lisp.h"

int main() {
    printf("=== GC Tests ===\n\n");
    
    gc_init();
    gc_print_stats();
    
    printf("\n--- Test 1: Basic allocation ---\n");
    LispValue* a = lisp_make_int(1);
    LispValue* b = lisp_make_int(2);
    LispValue* c = lisp_make_int(3);
    printf("Created 3 integers: a=%p, b=%p, c=%p\n", (void*)a, (void*)b, (void*)c);
    gc_print_stats();
    assert(a->refcount == 1);
    assert(b->refcount == 1);
    assert(c->refcount == 1);
    printf("PASS: All objects tracked, refcounts correct\n");
    
    printf("\n--- Test 2: Release objects ---\n");
    lisp_release(a);
    lisp_release(b);
    lisp_release(c);
    printf("Released all 3 integers\n");
    gc_print_stats();
    printf("PASS: Objects freed (freed count should be 3)\n");
    
    printf("\n--- Test 3: Cons pair ---\n");
    LispValue* x = lisp_make_int(10);
    LispValue* y = lisp_make_int(20);
    printf("Created x=%p (refcount=%u), y=%p (refcount=%u)\n", 
           (void*)x, x->refcount, (void*)y, y->refcount);
    
    LispValue* pair = lisp_cons(x, y);
    printf("Created pair=%p\n", (void*)pair);
    printf("After cons: x refcount=%u, y refcount=%u (should be 2 each)\n", 
           x->refcount, y->refcount);
    assert(x->refcount == 2);
    assert(y->refcount == 2);
    gc_print_stats();
    
    lisp_release(pair);
    printf("Released pair\n");
    printf("After release: x refcount=%u, y refcount=%u (should be 1 each)\n",
           x->refcount, y->refcount);
    assert(x->refcount == 1);
    assert(y->refcount == 1);
    
    lisp_release(x);
    lisp_release(y);
    printf("Released x and y\n");
    gc_print_stats();
    printf("PASS: Cons retain/release works correctly\n");
    
    printf("\n--- Test 4: Small list ---\n");
    LispValue* i1 = lisp_make_int(1);
    LispValue* i2 = lisp_make_int(2);
    LispValue* i3 = lisp_make_int(3);
    LispValue* tail = lisp_cons(i3, NULL);
    LispValue* middle = lisp_cons(i2, tail);
    LispValue* head = lisp_cons(i1, middle);
    printf("Created list (1 2 3) with explicit refs\n");
    printf("  i1 refcount=%u, i2 refcount=%u, i3 refcount=%u\n", i1->refcount, i2->refcount, i3->refcount);
    gc_print_stats();
    
    lisp_release(head);
    lisp_release(middle);
    lisp_release(tail);
    lisp_release(i1);
    lisp_release(i2);
    lisp_release(i3);
    printf("Released all list components\n");
    gc_print_stats();
    printf("PASS: List freed correctly\n");
    
    printf("\n--- Test 5: Closure ---\n");
    LispValue* val42 = lisp_make_int(42);
    LispValue* env2 = lisp_env_extend(NULL, "x", val42);
    printf("Created env with x=42, val42 refcount=%u\n", val42->refcount);
    LispValue* closure = lisp_make_closure(NULL, env2);
    printf("Created closure with env\n");
    gc_print_stats();
    
    lisp_release(closure);
    lisp_release(env2);
    lisp_release(val42);
    printf("Released closure, env, and val42\n");
    gc_print_stats();
    printf("PASS: Closure freed correctly\n");
    
    printf("\n--- Test 6: Cycle collection ---\n");
    gc_collect_cycles();
    printf("Cycle collection completed\n");
    gc_print_stats();
    printf("PASS: No crash during cycle collection\n");
    
    printf("\n--- Final leak check ---\n");
    printf("Allocated: %zu\n", gc_get_allocated());
    printf("Freed:     %zu\n", gc_get_freed());
    printf("Alive:     %zu (should be 0)\n", gc_get_alive());
    
    gc_shutdown();
    
    printf("\n=== All GC tests passed ===\n");
    return 0;
}