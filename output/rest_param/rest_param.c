#include <stdio.h>
#include "runtime/lisp.h"

/* GC initialization is done in main() */

/* Forward declarations */
LispValue* sum_all(LispValue* nums);
LispValue* sum_all_wrapper(LispValue* __args, LispValue* __env);

LispValue* sum_all(LispValue* nums) {
    LispValue* __result_0 = NULL;
    if (lisp_is_true(lisp_is_nil_fn(nums))) {
        __result_0 = lisp_make_int(0);
    } else {
        __result_0 = lisp_add(lisp_first(nums), lisp_apply(lisp_make_closure(sum_all_wrapper, NULL), lisp_rest(nums)));
    }
    return __result_0;
}

LispValue* sum_all_wrapper(LispValue* __args, LispValue* __env) {
    LispValue* nums = NULL;
    LispValue* __curr = __args;
    for (int __i = 0; __i < 0; __i++) __curr = lisp_rest(__curr);
    nums = __curr;
    return sum_all(nums);
}

int main(int argc, char** argv) {
    gc_init();

    lisp_print(lisp_make_string("=== &rest parameter test ==="));
    lisp_print(lisp_make_string("sum-all with &rest:"));
    lisp_print(sum_all_wrapper(NULL, NULL));
    lisp_print(sum_all_wrapper(lisp_cons(lisp_make_int(1), NULL), NULL));
    lisp_print(sum_all_wrapper(lisp_cons(lisp_make_int(1), lisp_cons(lisp_make_int(2), NULL)), NULL));
    lisp_print(sum_all_wrapper(lisp_cons(lisp_make_int(1), lisp_cons(lisp_make_int(2), lisp_cons(lisp_make_int(3), NULL))), NULL));
    lisp_print(sum_all_wrapper(lisp_cons(lisp_make_int(1), lisp_cons(lisp_make_int(2), lisp_cons(lisp_make_int(3), lisp_cons(lisp_make_int(4), lisp_cons(lisp_make_int(5), NULL))))), NULL));
    lisp_print(lisp_make_string("=== Done ==="));

    /* Cleanup: release all variables */
    gc_shutdown();
    return 0;
}