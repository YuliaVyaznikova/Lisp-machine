#include <stdio.h>
#include "runtime/lisp.h"

/* GC initialization is done in main() */

/* Forward declarations */
LispValue* foo(LispValue* x);
LispValue* foo_wrapper(LispValue* __args, LispValue* __env);
LispValue* bar(LispValue* y);
LispValue* bar_wrapper(LispValue* __args, LispValue* __env);

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

int main(int argc, char** argv) {
    gc_init();

    lisp_print(foo(lisp_make_int(42)));

    /* Cleanup: release all variables */
    gc_shutdown();
    return 0;
}