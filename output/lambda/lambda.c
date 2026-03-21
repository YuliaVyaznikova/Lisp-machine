#include <stdio.h>
#include <stdlib.h>
#include "runtime/lisp.h"

/* GC initialization is done in main() */

/* Forward declarations */

LispValue* __lambda_0(LispValue* __args, LispValue* __env) {
    LispValue* x = lisp_list_get(__args, 0);
    return lisp_mul(x, x);
}

LispValue* __lambda_1(LispValue* __args, LispValue* __env) {
    LispValue* x = lisp_list_get(__args, 0);
    LispValue* y = lisp_list_get(__args, 1);
    return lisp_add(x, y);
}

int main(int argc, char** argv) {
    gc_init();

    lisp_print(lisp_call_closure(lisp_make_closure(__lambda_0, NULL), lisp_cons(lisp_make_int(5), NULL)));
    lisp_print(lisp_call_closure(lisp_make_closure(__lambda_1, NULL), lisp_cons(lisp_make_int(3), lisp_cons(lisp_make_int(7), NULL))));

    /* Cleanup: release all variables */
    gc_shutdown();
    return 0;
}