#include <stdio.h>
#include <stdlib.h>
#include "runtime/lisp.h"

/* GC initialization is done in main() */

/* Forward declarations */
LispValue* square(LispValue* x);

LispValue* square(LispValue* x) {
    return lisp_mul(x, x);
}

LispValue* square_wrapper(LispValue* __args, LispValue* __env) {
    LispValue* x = lisp_list_get(__args, 0);
    return square(x);
}

int main(int argc, char** argv) {
    gc_init();

    lisp_print(square(lisp_make_int(5)));
    lisp_print(square(lisp_make_int(10)));

    /* Cleanup: release all variables */
    gc_shutdown();
    return 0;
}