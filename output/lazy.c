#include <stdio.h>
#include "../runtime/lisp.h"

/* GC initialization is done in main() */

/* Forward declarations */
LispValue* lazy_first(LispValue* s);
LispValue* lazy_rest(LispValue* s);
LispValue* lazy_map(LispValue* f, LispValue* s);
LispValue* lazy_filter(LispValue* pred, LispValue* s);
LispValue* take(LispValue* n, LispValue* s);
LispValue* force_list(LispValue* s);
LispValue* integers_from(LispValue* n);
LispValue* even_p(LispValue* x);
LispValue* square(LispValue* x);

LispValue* lazy_first(LispValue* s) {
    return lisp_first(s);
}

LispValue* lazy_first_wrapper(LispValue* __args, LispValue* __env) {
    LispValue* s = lisp_list_get(__args, 0);
    return lazy_first(s);
}

LispValue* lazy_rest(LispValue* s) {
    return lisp_call_closure(lisp_rest(s), NULL);
}

LispValue* lazy_rest_wrapper(LispValue* __args, LispValue* __env) {
    LispValue* s = lisp_list_get(__args, 0);
    return lazy_rest(s);
}

LispValue* __lambda_0(LispValue* __args, LispValue* __env) {
    LispValue* f = lisp_env_lookup(__env, "f");
    LispValue* s = lisp_env_lookup(__env, "s");
    return lazy_map(f, lazy_rest(s));
}

LispValue* lazy_map(LispValue* f, LispValue* s) {
    LispValue* __result_0 = NULL;
    if (lisp_is_true(lisp_is_nil_fn(s))) {
        __result_0 = NULL;
    } else {
        __result_0 = lisp_cons(lisp_call_closure(f, lisp_cons(lazy_first(s), NULL)), lisp_make_closure(__lambda_0, lisp_env_extend(lisp_env_extend(NULL, "f", f), "s", s)));
    }
    return __result_0;
}

LispValue* lazy_map_wrapper(LispValue* __args, LispValue* __env) {
    LispValue* f = lisp_list_get(__args, 0);
    LispValue* s = lisp_list_get(__args, 1);
    return lazy_map(f, s);
}

LispValue* __lambda_1(LispValue* __args, LispValue* __env) {
    LispValue* pred = lisp_env_lookup(__env, "pred");
    LispValue* s = lisp_env_lookup(__env, "s");
    return lazy_filter(pred, lazy_rest(s));
}

LispValue* lazy_filter(LispValue* pred, LispValue* s) {
    LispValue* __new_pred = NULL;
    LispValue* __new_s = NULL;
    while (1) {
        if (lisp_is_true(lisp_is_nil_fn(s))) {
            return NULL;
        } else {
            if (lisp_is_true(lisp_call_closure(pred, lisp_cons(lazy_first(s), NULL)))) {
            return lisp_cons(lazy_first(s), lisp_make_closure(__lambda_1, lisp_env_extend(lisp_env_extend(NULL, "pred", pred), "s", s)));
            } else {
            __new_pred = pred;
            __new_s = lazy_rest(s);
            pred = __new_pred;
            s = __new_s;
            continue;
            }
        }
    }
}

LispValue* lazy_filter_wrapper(LispValue* __args, LispValue* __env) {
    LispValue* pred = lisp_list_get(__args, 0);
    LispValue* s = lisp_list_get(__args, 1);
    return lazy_filter(pred, s);
}

LispValue* __lambda_2(LispValue* __args, LispValue* __env) {
    LispValue* n = lisp_env_lookup(__env, "n");
    LispValue* s = lisp_env_lookup(__env, "s");
    return take(lisp_sub(n, lisp_make_int(1)), lazy_rest(s));
}

LispValue* take(LispValue* n, LispValue* s) {
    LispValue* __result_0 = NULL;
    if (lisp_is_true(lisp_eq(n, lisp_make_int(0)))) {
        __result_0 = NULL;
    } else {
        LispValue* __result_1 = NULL;
        if (lisp_is_true(lisp_is_nil_fn(s))) {
            __result_1 = NULL;
        } else {
            __result_1 = lisp_cons(lazy_first(s), lisp_make_closure(__lambda_2, lisp_env_extend(lisp_env_extend(NULL, "n", n), "s", s)));
        }
        __result_0 = __result_1;
    }
    return __result_0;
}

LispValue* take_wrapper(LispValue* __args, LispValue* __env) {
    LispValue* n = lisp_list_get(__args, 0);
    LispValue* s = lisp_list_get(__args, 1);
    return take(n, s);
}

LispValue* force_list(LispValue* s) {
    LispValue* __result_0 = NULL;
    if (lisp_is_true(lisp_is_nil_fn(s))) {
        __result_0 = NULL;
    } else {
        __result_0 = lisp_cons(lazy_first(s), force_list(lazy_rest(s)));
    }
    return __result_0;
}

LispValue* force_list_wrapper(LispValue* __args, LispValue* __env) {
    LispValue* s = lisp_list_get(__args, 0);
    return force_list(s);
}

LispValue* __lambda_3(LispValue* __args, LispValue* __env) {
    LispValue* n = lisp_env_lookup(__env, "n");
    return integers_from(lisp_add(n, lisp_make_int(1)));
}

LispValue* integers_from(LispValue* n) {
    return lisp_cons(n, lisp_make_closure(__lambda_3, lisp_env_extend(NULL, "n", n)));
}

LispValue* integers_from_wrapper(LispValue* __args, LispValue* __env) {
    LispValue* n = lisp_list_get(__args, 0);
    return integers_from(n);
}

LispValue* even_p(LispValue* x) {
    return lisp_eq(lisp_mod(x, lisp_make_int(2)), lisp_make_int(0));
}

LispValue* even_p_wrapper(LispValue* __args, LispValue* __env) {
    LispValue* x = lisp_list_get(__args, 0);
    return even_p(x);
}

LispValue* square(LispValue* x) {
    return lisp_mul(x, x);
}

LispValue* square_wrapper(LispValue* __args, LispValue* __env) {
    LispValue* x = lisp_list_get(__args, 0);
    return square(x);
}

int main(int argc, char** argv) {
    gc_init();

    LispValue* ints = NULL;
    LispValue* evens = NULL;
    LispValue* squares_of_evens = NULL;
    ints = integers_from(lisp_make_int(1));
    evens = lazy_filter(lisp_make_closure(even_p_wrapper, NULL), ints);
    squares_of_evens = lazy_map(lisp_make_closure(square_wrapper, NULL), evens);
    lisp_print(force_list(take(lisp_make_int(5), squares_of_evens)));

    /* Cleanup: release all variables */
    lisp_release(squares_of_evens);
    lisp_release(evens);
    lisp_release(ints);
    gc_shutdown();
    return 0;
}