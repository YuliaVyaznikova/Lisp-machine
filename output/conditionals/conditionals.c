#include <stdio.h>
#include <stdlib.h>
#include "runtime/lisp.h"

/* Forward declarations */



int main(int argc, char** argv) {
    LispValue* __result_0 = NULL;
    if (lisp_is_true(lisp_make_symbol("true"))) {
        __result_0 = lisp_make_int(1);
    } else {
        __result_0 = lisp_make_int(2);
    }
    lisp_print(__result_0);
    LispValue* __result_1 = NULL;
    if (lisp_is_true(lisp_make_symbol("false"))) {
        __result_1 = lisp_make_int(1);
    } else {
        __result_1 = lisp_make_int(2);
    }
    lisp_print(__result_1);
    LispValue* __result_2 = NULL;
    if (lisp_is_true(lisp_lt(lisp_make_int(3), lisp_make_int(5)))) {
        __result_2 = lisp_make_string("less");
    } else {
        __result_2 = lisp_make_string("greater");
    }
    lisp_print(__result_2);
    return 0;
}