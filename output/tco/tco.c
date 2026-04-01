#include <stdio.h>
#include "runtime/lisp.h"

/* GC initialization is done in main() */

/* Forward declarations */
LispValue* factorial_acc(LispValue* n, LispValue* acc);
LispValue* factorial_acc_wrapper(LispValue* __args, LispValue* __env);
LispValue* factorial(LispValue* n);
LispValue* factorial_wrapper(LispValue* __args, LispValue* __env);
LispValue* sum_to_acc(LispValue* n, LispValue* acc);
LispValue* sum_to_acc_wrapper(LispValue* __args, LispValue* __env);
LispValue* sum_to(LispValue* n);
LispValue* sum_to_wrapper(LispValue* __args, LispValue* __env);
LispValue* fib_acc(LispValue* n, LispValue* a, LispValue* b);
LispValue* fib_acc_wrapper(LispValue* __args, LispValue* __env);
LispValue* fib(LispValue* n);
LispValue* fib_wrapper(LispValue* __args, LispValue* __env);

LispValue* factorial_acc(LispValue* n, LispValue* acc) {
    LispValue* __new_n = NULL;
    LispValue* __new_acc = NULL;
    while (1) {
        if (lisp_is_true(lisp_eq(n, lisp_make_int(0)))) {
            return acc;
        } else {
            __new_n = lisp_sub(n, lisp_make_int(1));
            __new_acc = lisp_mul(n, acc);
            n = __new_n;
            acc = __new_acc;
            continue;
        }
    }
}

LispValue* factorial_acc_wrapper(LispValue* __args, LispValue* __env) {
    LispValue* n = lisp_list_get(__args, 0);
    LispValue* acc = lisp_list_get(__args, 1);
    return factorial_acc(n, acc);
}

LispValue* factorial(LispValue* n) {
    return factorial_acc(n, lisp_make_int(1));
}

LispValue* factorial_wrapper(LispValue* __args, LispValue* __env) {
    LispValue* n = lisp_list_get(__args, 0);
    return factorial(n);
}

LispValue* sum_to_acc(LispValue* n, LispValue* acc) {
    LispValue* __new_n = NULL;
    LispValue* __new_acc = NULL;
    while (1) {
        if (lisp_is_true(lisp_eq(n, lisp_make_int(0)))) {
            return acc;
        } else {
            __new_n = lisp_sub(n, lisp_make_int(1));
            __new_acc = lisp_add(n, acc);
            n = __new_n;
            acc = __new_acc;
            continue;
        }
    }
}

LispValue* sum_to_acc_wrapper(LispValue* __args, LispValue* __env) {
    LispValue* n = lisp_list_get(__args, 0);
    LispValue* acc = lisp_list_get(__args, 1);
    return sum_to_acc(n, acc);
}

LispValue* sum_to(LispValue* n) {
    return sum_to_acc(n, lisp_make_int(0));
}

LispValue* sum_to_wrapper(LispValue* __args, LispValue* __env) {
    LispValue* n = lisp_list_get(__args, 0);
    return sum_to(n);
}

LispValue* fib_acc(LispValue* n, LispValue* a, LispValue* b) {
    LispValue* __new_n = NULL;
    LispValue* __new_a = NULL;
    LispValue* __new_b = NULL;
    while (1) {
        if (lisp_is_true(lisp_eq(n, lisp_make_int(0)))) {
            return a;
        } else {
            __new_n = lisp_sub(n, lisp_make_int(1));
            __new_a = b;
            __new_b = lisp_add(a, b);
            n = __new_n;
            a = __new_a;
            b = __new_b;
            continue;
        }
    }
}

LispValue* fib_acc_wrapper(LispValue* __args, LispValue* __env) {
    LispValue* n = lisp_list_get(__args, 0);
    LispValue* a = lisp_list_get(__args, 1);
    LispValue* b = lisp_list_get(__args, 2);
    return fib_acc(n, a, b);
}

LispValue* fib(LispValue* n) {
    return fib_acc(n, lisp_make_int(0), lisp_make_int(1));
}

LispValue* fib_wrapper(LispValue* __args, LispValue* __env) {
    LispValue* n = lisp_list_get(__args, 0);
    return fib(n);
}

int main(int argc, char** argv) {
    gc_init();

    lisp_print(factorial(lisp_make_int(5)));
    lisp_print(factorial(lisp_make_int(10)));
    lisp_print(sum_to(lisp_make_int(100)));
    lisp_print(fib(lisp_make_int(10)));
    lisp_print(fib(lisp_make_int(20)));

    /* Cleanup: release all variables */
    gc_shutdown();
    return 0;
}