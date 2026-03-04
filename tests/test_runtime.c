#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "../runtime/lisp.h"

void test_integers() {
    LispValue* a = lisp_make_int(42);
    assert(a->type == LISP_INT);
    assert(a->data.int_val == 42);
    lisp_release(a);
    printf("Integers: OK\n");
}

void test_floats() {
    LispValue* a = lisp_make_float(3.14);
    assert(a->type == LISP_FLOAT);
    assert(a->data.float_val > 3.13 && a->data.float_val < 3.15);
    lisp_release(a);
    printf("Floats: OK\n");
}

void test_strings() {
    LispValue* s = lisp_make_string("hello");
    assert(s->type == LISP_STRING);
    assert(strcmp(s->data.string_val, "hello") == 0);
    lisp_release(s);
    printf("Strings: OK\n");
}

void test_symbols() {
    LispValue* s = lisp_make_symbol("foo");
    assert(s->type == LISP_SYMBOL);
    assert(strcmp(s->data.symbol_name, "foo") == 0);
    lisp_release(s);
    printf("Symbols: OK\n");
}

void test_cons() {
    LispValue* a = lisp_make_int(1);
    LispValue* b = lisp_make_int(2);
    LispValue* pair = lisp_cons(a, b);
    
    assert(pair->type == LISP_PAIR);
    assert(lisp_first(pair)->data.int_val == 1);
    assert(lisp_rest(pair)->data.int_val == 2);
    
    lisp_release(pair);
    printf("Cons: OK\n");
}

void test_arithmetic() {
    LispValue* a = lisp_make_int(10);
    LispValue* b = lisp_make_int(3);
    
    LispValue* sum = lisp_add(a, b);
    assert(sum->data.int_val == 13);
    
    LispValue* diff = lisp_sub(a, b);
    assert(diff->data.int_val == 7);
    
    LispValue* prod = lisp_mul(a, b);
    assert(prod->data.int_val == 30);
    
    LispValue* quot = lisp_div(a, b);
    assert(quot->data.int_val == 3);
    
    lisp_release(a); lisp_release(b);
    lisp_release(sum); lisp_release(diff);
    lisp_release(prod); lisp_release(quot);
    printf("Arithmetic: OK\n");
}

void test_comparisons() {
    LispValue* a = lisp_make_int(5);
    LispValue* b = lisp_make_int(10);
    
    assert(lisp_is_true(lisp_lt(a, b)));
    assert(lisp_is_true(lisp_gt(b, a)));
    assert(lisp_is_true(lisp_eq(a, a)));
    
    lisp_release(a); lisp_release(b);
    printf("Comparisons: OK\n");
}

void test_lists() {
    LispValue* nil = NULL;
    LispValue* one = lisp_make_int(1);
    LispValue* two = lisp_make_int(2);
    LispValue* three = lisp_make_int(3);
    
    LispValue* list = lisp_cons(one, lisp_cons(two, lisp_cons(three, nil)));
    
    assert(lisp_first(list)->data.int_val == 1);
    assert(lisp_first(lisp_rest(list))->data.int_val == 2);
    assert(lisp_first(lisp_rest(lisp_rest(list)))->data.int_val == 3);
    
    lisp_release(list);
    printf("Lists: OK\n");
}

int main() {
    printf("=== Runtime Tests ===\n\n");
    
    test_integers();
    test_floats();
    test_strings();
    test_symbols();
    test_cons();
    test_arithmetic();
    test_comparisons();
    test_lists();
    
    printf("\n=== All tests passed ===\n");
    return 0;
}