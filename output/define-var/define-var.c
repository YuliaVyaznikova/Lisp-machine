#include <stdio.h>
#include "runtime/lisp.h"

/* GC initialization is done in main() */

/* Forward declarations */



int main(int argc, char** argv) {
    gc_init();

    LispValue* x = NULL;
    x = lisp_make_int(42);
    lisp_print(x);

    /* Cleanup: release all variables */
    lisp_release(x);
    gc_shutdown();
    return 0;
}