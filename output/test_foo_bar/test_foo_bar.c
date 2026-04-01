#include <stdio.h>
#include "runtime/lisp.h"

/* GC initialization is done in main() */

/* Forward declarations */
LispValue* foo(LispValue* x);
LispValue* foo_wrapper(LispValue* __args, LispValue* __env);
LispValue* bar(LispValue* y);
LispValue* bar_wrapper(LispValue* __args, LispValue* __env);
LispValue* even_p(LispValue* n);
LispValue* even_p_wrapper(LispValue* __args, LispValue* __env);
LispValue* odd_p(LispValue* n);
LispValue* odd_p_wrapper(LispValue* __args, LispValue* __env);

LispValue* foo(LispValue* x) {
    return bar(x);
}

LispValue* foo_wrapper(LispValue* __args, LispValue* __env) {
    LispValue* x = lisp_list_get(__args, 0);
    return foo(x);
}

LispValue* bar(LispValue* y) {
    return y;
}

LispValue* bar_wrapper(LispValue* __args, LispValue* __env) {
    LispValue* y = lisp_list_get(__args, 0);
    return bar(y);
}

LispValue* even_p(LispValue* n) {
    LispValue* __result_0 = NULL;
    if (lisp_is_true(lisp_eq(n, lisp_make_int(0)))) {
        __result_0 = lisp_make_symbol("true");
    } else {
        __result_0 = odd_p(lisp_sub(n, lisp_make_int(1)));
    }
    return __result_0;
}

LispValue* even_p_wrapper(LispValue* __args, LispValue* __env) {
    LispValue* n = lisp_list_get(__args, 0);
    return even_p(n);
}

LispValue* odd_p(LispValue* n) {
    LispValue* __result_0 = NULL;
    if (lisp_is_true(lisp_eq(n, lisp_make_int(0)))) {
        __result_0 = lisp_make_symbol("false");
    } else {
        __result_0 = even_p(lisp_sub(n, lisp_make_int(1)));
    }
    return __result_0;
}

LispValue* odd_p_wrapper(LispValue* __args, LispValue* __env) {
    LispValue* n = lisp_list_get(__args, 0);
    return odd_p(n);
}

int main(int argc, char** argv) {
    gc_init();

    lisp_print(lisp_make_string("=== Mutual recursion test ==="));
    lisp_print(lisp_make_string("foo calls bar, bar defined AFTER foo:"));
    lisp_print(foo(lisp_make_int(42)));
    lisp_print(lisp_make_string("Expected: 42"));
    lisp_print(lisp_make_string(""));
    lisp_print(lisp_make_string("=== even? / odd? mutual recursion ==="));
    lisp_print(lisp_make_string("even? 10:"));
    lisp_print(even_p(lisp_make_int(10)));
    lisp_print(lisp_make_string("odd? 10:"));
    lisp_print(odd_p(lisp_make_int(10)));
    lisp_print(lisp_make_string("even? 7:"));
    lisp_print(even_p(lisp_make_int(7)));
    lisp_print(lisp_make_string("odd? 7:"));
    lisp_print(odd_p(lisp_make_int(7)));

    /* Cleanup: release all variables */
    gc_shutdown();
    return 0;
}