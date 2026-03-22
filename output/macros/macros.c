#include <stdio.h>
#include <stdlib.h>
#include "runtime/lisp.h"

/* GC initialization is done in main() */

/* Forward declarations */

LispValue* __lambda_0(LispValue* __args, LispValue* __env) {
    LispValue* x = lisp_list_get(__args, 0);
    return lisp_add(x, lisp_make_int(5));
}

LispValue* __lambda_1(LispValue* __args, LispValue* __env) {
    LispValue* y = lisp_list_get(__args, 0);
    return lisp_mul(y, lisp_make_int(2));
}

int main(int argc, char** argv) {
    gc_init();

    lisp_print(lisp_make_string("=== when macro ==="));
    LispValue* __result_0 = NULL;
    if (lisp_is_true(lisp_eq(lisp_make_int(1), lisp_make_int(1)))) {
        __result_0 = lisp_make_string("true branch");
    } else {
        __result_0 = NULL;
    }
    lisp_print(__result_0);
    LispValue* __result_1 = NULL;
    if (lisp_is_true(lisp_eq(lisp_make_int(1), lisp_make_int(2)))) {
        __result_1 = lisp_make_string("should not print");
    } else {
        __result_1 = NULL;
    }
    lisp_print(__result_1);
    lisp_print(lisp_make_string("=== unless macro ==="));
    LispValue* __result_2 = NULL;
    if (lisp_is_true(lisp_eq(lisp_make_int(1), lisp_make_int(2)))) {
        __result_2 = NULL;
    } else {
        __result_2 = lisp_make_string("condition was false");
    }
    lisp_print(__result_2);
    LispValue* __result_3 = NULL;
    if (lisp_is_true(lisp_eq(lisp_make_int(1), lisp_make_int(1)))) {
        __result_3 = NULL;
    } else {
        __result_3 = lisp_make_string("should not print");
    }
    lisp_print(__result_3);
    lisp_print(lisp_make_string("=== cond macro ==="));
    LispValue* __result_4 = NULL;
    if (lisp_is_true(lisp_eq(lisp_make_int(1), lisp_make_int(2)))) {
        __result_4 = lisp_make_string("one equals two");
    } else {
        LispValue* __result_5 = NULL;
        if (lisp_is_true(lisp_eq(lisp_make_int(1), lisp_make_int(1)))) {
            __result_5 = lisp_make_string("one equals one");
        } else {
            LispValue* __result_6 = NULL;
            if (lisp_is_true(lisp_make_symbol("true"))) {
                __result_6 = lisp_make_string("else branch");
            } else {
                __result_6 = NULL;
            }
            __result_5 = __result_6;
        }
        __result_4 = __result_5;
    }
    lisp_print(__result_4);
    LispValue* __result_7 = NULL;
    if (lisp_is_true(lisp_eq(lisp_make_int(1), lisp_make_int(2)))) {
        __result_7 = lisp_make_string("first");
    } else {
        LispValue* __result_8 = NULL;
        if (lisp_is_true(lisp_eq(lisp_make_int(2), lisp_make_int(3)))) {
            __result_8 = lisp_make_string("second");
        } else {
            LispValue* __result_9 = NULL;
            if (lisp_is_true(lisp_make_symbol("true"))) {
                __result_9 = lisp_make_string("fallback");
            } else {
                __result_9 = NULL;
            }
            __result_8 = __result_9;
        }
        __result_7 = __result_8;
    }
    lisp_print(__result_7);
    lisp_print(lisp_make_string("=== and macro ==="));
    LispValue* __result_10 = NULL;
    if (lisp_is_true(lisp_make_symbol("true"))) {
        __result_10 = lisp_make_symbol("true");
    } else {
        __result_10 = NULL;
    }
    lisp_print(__result_10);
    LispValue* __result_11 = NULL;
    if (lisp_is_true(lisp_make_symbol("true"))) {
        __result_11 = lisp_make_symbol("false");
    } else {
        __result_11 = NULL;
    }
    lisp_print(__result_11);
    LispValue* __result_12 = NULL;
    if (lisp_is_true(lisp_make_symbol("false"))) {
        __result_12 = lisp_make_symbol("true");
    } else {
        __result_12 = NULL;
    }
    lisp_print(__result_12);
    LispValue* __result_13 = NULL;
    if (lisp_is_true(lisp_eq(lisp_make_int(1), lisp_make_int(1)))) {
        __result_13 = lisp_eq(lisp_make_int(2), lisp_make_int(2));
    } else {
        __result_13 = NULL;
    }
    lisp_print(__result_13);
    lisp_print(lisp_make_string("=== or macro ==="));
    LispValue* __result_14 = NULL;
    if (lisp_is_true(lisp_make_symbol("true"))) {
        __result_14 = lisp_make_symbol("true");
    } else {
        __result_14 = lisp_make_symbol("false");
    }
    lisp_print(__result_14);
    LispValue* __result_15 = NULL;
    if (lisp_is_true(lisp_make_symbol("false"))) {
        __result_15 = lisp_make_symbol("true");
    } else {
        __result_15 = lisp_make_symbol("true");
    }
    lisp_print(__result_15);
    LispValue* __result_16 = NULL;
    if (lisp_is_true(lisp_make_symbol("false"))) {
        __result_16 = lisp_make_symbol("true");
    } else {
        __result_16 = lisp_make_symbol("false");
    }
    lisp_print(__result_16);
    LispValue* __result_17 = NULL;
    if (lisp_is_true(lisp_eq(lisp_make_int(1), lisp_make_int(2)))) {
        __result_17 = lisp_make_symbol("true");
    } else {
        __result_17 = lisp_eq(lisp_make_int(2), lisp_make_int(2));
    }
    lisp_print(__result_17);
    lisp_print(lisp_make_string("=== let macro ==="));
    lisp_print(lisp_call_closure(lisp_make_closure(__lambda_0, NULL), lisp_cons(lisp_make_int(10), NULL)));
    lisp_print(lisp_call_closure(lisp_make_closure(__lambda_1, NULL), lisp_cons(lisp_make_int(20), NULL)));
    lisp_print(lisp_make_string("=== swap macro ==="));
    lisp_print(lisp_cons(lisp_make_int(2), lisp_cons(lisp_make_int(1), NULL)));
    lisp_print(lisp_make_string("=== inc macro ==="));
    lisp_print(lisp_add(lisp_make_int(5), lisp_make_int(1)));
    lisp_print(lisp_add(lisp_add(lisp_make_int(10), lisp_make_int(1)), lisp_make_int(1)));
    lisp_print(lisp_make_string("=== dec macro ==="));
    lisp_print(lisp_sub(lisp_make_int(5), lisp_make_int(1)));
    lisp_print(lisp_sub(lisp_sub(lisp_make_int(10), lisp_make_int(1)), lisp_make_int(1)));
    lisp_print(lisp_make_string("=== assert macro ==="));
    LispValue* __result_18 = NULL;
    if (lisp_is_true(lisp_eq(lisp_make_int(1), lisp_make_int(1)))) {
        __result_18 = lisp_cons(lisp_make_string("PASS:"), lisp_make_string("1 equals 1"));
    } else {
        __result_18 = lisp_cons(lisp_make_string("FAIL:"), lisp_make_string("1 equals 1"));
    }
    lisp_print(__result_18);
    LispValue* __result_19 = NULL;
    if (lisp_is_true(lisp_lt(lisp_make_int(1), lisp_make_int(2)))) {
        __result_19 = lisp_cons(lisp_make_string("PASS:"), lisp_make_string("1 is less than 2"));
    } else {
        __result_19 = lisp_cons(lisp_make_string("FAIL:"), lisp_make_string("1 is less than 2"));
    }
    lisp_print(__result_19);
    lisp_print(lisp_make_string("=== not-nil? macro ==="));
    LispValue* __result_20 = NULL;
    if (lisp_is_true(lisp_make_int(1))) {
        __result_20 = lisp_make_symbol("true");
    } else {
        __result_20 = lisp_make_symbol("false");
    }
    lisp_print(__result_20);
    LispValue* __result_21 = NULL;
    if (lisp_is_true(NULL)) {
        __result_21 = lisp_make_symbol("true");
    } else {
        __result_21 = lisp_make_symbol("false");
    }
    lisp_print(__result_21);

    /* Cleanup: release all variables */
    gc_shutdown();
    return 0;
}