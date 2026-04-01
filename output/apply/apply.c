#include <stdio.h>
#include "runtime/lisp.h"

/* GC initialization is done in main() */

/* Forward declarations */



int main(int argc, char** argv) {
    gc_init();

    lisp_print(lisp_apply(lisp_make_closure(lisp_add_wrapper, NULL), lisp_cons(lisp_make_int(1), lisp_cons(lisp_make_int(2), lisp_cons(lisp_make_int(3), NULL)))));
    lisp_print(lisp_apply(lisp_make_closure(lisp_mul_wrapper, NULL), lisp_cons(lisp_make_int(2), lisp_cons(lisp_make_int(3), lisp_cons(lisp_make_int(4), NULL)))));

    /* Cleanup: release all variables */
    gc_shutdown();
    return 0;
}