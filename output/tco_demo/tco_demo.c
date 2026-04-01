#include <stdio.h>
#include "runtime/lisp.h"

/* GC initialization is done in main() */

/* Forward declarations */
LispValue* count_down(LispValue* n);
LispValue* count_down_wrapper(LispValue* __args, LispValue* __env);

LispValue* count_down(LispValue* n) {
    LispValue* __new_n = NULL;
    while (1) {
        if (lisp_is_true(lisp_eq(n, lisp_make_int(0)))) {
            return lisp_make_string("done!");
        } else {
            __new_n = lisp_sub(n, lisp_make_int(1));
            n = __new_n;
            continue;
        }
    }
}

LispValue* count_down_wrapper(LispValue* __args, LispValue* __env) {
    LispValue* n = lisp_list_get(__args, 0);
    return count_down(n);
}

int main(int argc, char** argv) {
    gc_init();

    lisp_print(lisp_make_string("=== TCO Demonstration ==="));
    lisp_print(lisp_make_string(""));
    lisp_print(lisp_make_string("Without TCO, this would overflow the stack:"));
    lisp_print(lisp_make_string("Counting down from 100000..."));
    lisp_print(count_down(lisp_make_int(100000)));
    lisp_print(lisp_make_string(""));
    lisp_print(lisp_make_string("=== Done ==="));

    /* Cleanup: release all variables */
    gc_shutdown();
    return 0;
}