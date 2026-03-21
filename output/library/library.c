#include <stdio.h>
#include <stdlib.h>
#include "runtime/lisp.h"

/* GC initialization is done in main() */

/* Forward declarations */
LispValue* zero_p(LispValue* x);
LispValue* positive_p(LispValue* x);
LispValue* negative_p(LispValue* x);
LispValue* even_p(LispValue* x);
LispValue* odd_p(LispValue* x);
LispValue* inc(LispValue* x);
LispValue* dec(LispValue* x);
LispValue* square(LispValue* x);
LispValue* cube(LispValue* x);
LispValue* length(LispValue* lst);
LispValue* append(LispValue* lst1, LispValue* lst2);
LispValue* reverse(LispValue* lst);
LispValue* nth(LispValue* n, LispValue* lst);
LispValue* last(LispValue* lst);
LispValue* member(LispValue* x, LispValue* lst);
LispValue* map(LispValue* f, LispValue* lst);
LispValue* filter(LispValue* pred, LispValue* lst);
LispValue* reduce(LispValue* f, LispValue* init, LispValue* lst);
LispValue* any_p(LispValue* pred, LispValue* lst);
LispValue* all_p(LispValue* pred, LispValue* lst);
LispValue* identity(LispValue* x);
LispValue* constantly(LispValue* x);
LispValue* compose(LispValue* f, LispValue* g);
LispValue* flip(LispValue* f);
LispValue* curry(LispValue* f, LispValue* x);

LispValue* zero_p(LispValue* x) {
    return lisp_eq(x, lisp_make_int(0));
}

LispValue* zero_p_wrapper(LispValue* __args, LispValue* __env) {
    LispValue* x = lisp_list_get(__args, 0);
    return zero_p(x);
}

LispValue* positive_p(LispValue* x) {
    return lisp_gt(x, lisp_make_int(0));
}

LispValue* positive_p_wrapper(LispValue* __args, LispValue* __env) {
    LispValue* x = lisp_list_get(__args, 0);
    return positive_p(x);
}

LispValue* negative_p(LispValue* x) {
    return lisp_lt(x, lisp_make_int(0));
}

LispValue* negative_p_wrapper(LispValue* __args, LispValue* __env) {
    LispValue* x = lisp_list_get(__args, 0);
    return negative_p(x);
}

LispValue* even_p(LispValue* x) {
    return lisp_eq(lisp_mod(x, lisp_make_int(2)), lisp_make_int(0));
}

LispValue* even_p_wrapper(LispValue* __args, LispValue* __env) {
    LispValue* x = lisp_list_get(__args, 0);
    return even_p(x);
}

LispValue* odd_p(LispValue* x) {
    return lisp_not(even_p(x));
}

LispValue* odd_p_wrapper(LispValue* __args, LispValue* __env) {
    LispValue* x = lisp_list_get(__args, 0);
    return odd_p(x);
}

LispValue* inc(LispValue* x) {
    return lisp_add(x, lisp_make_int(1));
}

LispValue* inc_wrapper(LispValue* __args, LispValue* __env) {
    LispValue* x = lisp_list_get(__args, 0);
    return inc(x);
}

LispValue* dec(LispValue* x) {
    return lisp_sub(x, lisp_make_int(1));
}

LispValue* dec_wrapper(LispValue* __args, LispValue* __env) {
    LispValue* x = lisp_list_get(__args, 0);
    return dec(x);
}

LispValue* square(LispValue* x) {
    return lisp_mul(x, x);
}

LispValue* square_wrapper(LispValue* __args, LispValue* __env) {
    LispValue* x = lisp_list_get(__args, 0);
    return square(x);
}

LispValue* cube(LispValue* x) {
    return lisp_mul(lisp_mul(x, x), x);
}

LispValue* cube_wrapper(LispValue* __args, LispValue* __env) {
    LispValue* x = lisp_list_get(__args, 0);
    return cube(x);
}

LispValue* length(LispValue* lst) {
    LispValue* __result_0 = NULL;
    if (lisp_is_true(lisp_is_nil_fn(lst))) {
        __result_0 = lisp_make_int(0);
    } else {
        __result_0 = lisp_add(lisp_make_int(1), lisp_length(lisp_rest(lst)));
    }
    return __result_0;
}

LispValue* length_wrapper(LispValue* __args, LispValue* __env) {
    LispValue* lst = lisp_list_get(__args, 0);
    return length(lst);
}

LispValue* append(LispValue* lst1, LispValue* lst2) {
    LispValue* __result_0 = NULL;
    if (lisp_is_true(lisp_is_nil_fn(lst1))) {
        __result_0 = lst2;
    } else {
        __result_0 = lisp_cons(lisp_first(lst1), lisp_append(lisp_rest(lst1), lst2));
    }
    return __result_0;
}

LispValue* append_wrapper(LispValue* __args, LispValue* __env) {
    LispValue* lst1 = lisp_list_get(__args, 0);
    LispValue* lst2 = lisp_list_get(__args, 1);
    return append(lst1, lst2);
}

LispValue* reverse(LispValue* lst) {
    LispValue* __result_0 = NULL;
    if (lisp_is_true(lisp_is_nil_fn(lst))) {
        __result_0 = NULL;
    } else {
        __result_0 = lisp_append(lisp_reverse(lisp_rest(lst)), lisp_cons(lisp_first(lst), NULL));
    }
    return __result_0;
}

LispValue* reverse_wrapper(LispValue* __args, LispValue* __env) {
    LispValue* lst = lisp_list_get(__args, 0);
    return reverse(lst);
}

LispValue* nth(LispValue* n, LispValue* lst) {
    LispValue* __new_n = NULL;
    LispValue* __new_lst = NULL;
    while (1) {
        if (lisp_is_true(lisp_eq(n, lisp_make_int(0)))) {
            return lisp_first(lst);
        } else {
            __new_n = lisp_sub(n, lisp_make_int(1));
            __new_lst = lisp_rest(lst);
            n = __new_n;
            lst = __new_lst;
            continue;
        }
    }
}

LispValue* nth_wrapper(LispValue* __args, LispValue* __env) {
    LispValue* n = lisp_list_get(__args, 0);
    LispValue* lst = lisp_list_get(__args, 1);
    return nth(n, lst);
}

LispValue* last(LispValue* lst) {
    LispValue* __new_lst = NULL;
    while (1) {
        if (lisp_is_true(lisp_is_nil_fn(lisp_rest(lst)))) {
            return lisp_first(lst);
        } else {
            __new_lst = lisp_rest(lst);
            lst = __new_lst;
            continue;
        }
    }
}

LispValue* last_wrapper(LispValue* __args, LispValue* __env) {
    LispValue* lst = lisp_list_get(__args, 0);
    return last(lst);
}

LispValue* member(LispValue* x, LispValue* lst) {
    LispValue* __new_x = NULL;
    LispValue* __new_lst = NULL;
    while (1) {
        if (lisp_is_true(lisp_is_nil_fn(lst))) {
            return NULL;
        } else {
            if (lisp_is_true(lisp_eq(x, lisp_first(lst)))) {
            return lst;
            } else {
            __new_x = x;
            __new_lst = lisp_rest(lst);
            x = __new_x;
            lst = __new_lst;
            continue;
            }
        }
    }
}

LispValue* member_wrapper(LispValue* __args, LispValue* __env) {
    LispValue* x = lisp_list_get(__args, 0);
    LispValue* lst = lisp_list_get(__args, 1);
    return member(x, lst);
}

LispValue* map(LispValue* f, LispValue* lst) {
    LispValue* __result_0 = NULL;
    if (lisp_is_true(lisp_is_nil_fn(lst))) {
        __result_0 = NULL;
    } else {
        __result_0 = lisp_cons(lisp_call_closure(f, lisp_cons(lisp_first(lst), NULL)), map(f, lisp_rest(lst)));
    }
    return __result_0;
}

LispValue* map_wrapper(LispValue* __args, LispValue* __env) {
    LispValue* f = lisp_list_get(__args, 0);
    LispValue* lst = lisp_list_get(__args, 1);
    return map(f, lst);
}

LispValue* filter(LispValue* pred, LispValue* lst) {
    LispValue* __result_0 = NULL;
    if (lisp_is_true(lisp_is_nil_fn(lst))) {
        __result_0 = NULL;
    } else {
        LispValue* __result_1 = NULL;
        if (lisp_is_true(lisp_call_closure(pred, lisp_cons(lisp_first(lst), NULL)))) {
            __result_1 = lisp_cons(lisp_first(lst), filter(pred, lisp_rest(lst)));
        } else {
            __result_1 = filter(pred, lisp_rest(lst));
        }
        __result_0 = __result_1;
    }
    return __result_0;
}

LispValue* filter_wrapper(LispValue* __args, LispValue* __env) {
    LispValue* pred = lisp_list_get(__args, 0);
    LispValue* lst = lisp_list_get(__args, 1);
    return filter(pred, lst);
}

LispValue* reduce(LispValue* f, LispValue* init, LispValue* lst) {
    LispValue* __new_f = NULL;
    LispValue* __new_init = NULL;
    LispValue* __new_lst = NULL;
    while (1) {
        if (lisp_is_true(lisp_is_nil_fn(lst))) {
            return init;
        } else {
            __new_f = f;
            __new_init = lisp_call_closure(f, lisp_cons(init, lisp_cons(lisp_first(lst), NULL)));
            __new_lst = lisp_rest(lst);
            f = __new_f;
            init = __new_init;
            lst = __new_lst;
            continue;
        }
    }
}

LispValue* reduce_wrapper(LispValue* __args, LispValue* __env) {
    LispValue* f = lisp_list_get(__args, 0);
    LispValue* init = lisp_list_get(__args, 1);
    LispValue* lst = lisp_list_get(__args, 2);
    return reduce(f, init, lst);
}

LispValue* any_p(LispValue* pred, LispValue* lst) {
    LispValue* __new_pred = NULL;
    LispValue* __new_lst = NULL;
    while (1) {
        if (lisp_is_true(lisp_is_nil_fn(lst))) {
            return NULL;
        } else {
            if (lisp_is_true(lisp_call_closure(pred, lisp_cons(lisp_first(lst), NULL)))) {
            return lisp_first(lst);
            } else {
            __new_pred = pred;
            __new_lst = lisp_rest(lst);
            pred = __new_pred;
            lst = __new_lst;
            continue;
            }
        }
    }
}

LispValue* any_p_wrapper(LispValue* __args, LispValue* __env) {
    LispValue* pred = lisp_list_get(__args, 0);
    LispValue* lst = lisp_list_get(__args, 1);
    return any_p(pred, lst);
}

LispValue* all_p(LispValue* pred, LispValue* lst) {
    LispValue* __new_pred = NULL;
    LispValue* __new_lst = NULL;
    while (1) {
        if (lisp_is_true(lisp_is_nil_fn(lst))) {
            return lisp_make_int(1);
        } else {
            if (lisp_is_true(lisp_call_closure(pred, lisp_cons(lisp_first(lst), NULL)))) {
            __new_pred = pred;
            __new_lst = lisp_rest(lst);
            pred = __new_pred;
            lst = __new_lst;
            continue;
            } else {
            return NULL;
            }
        }
    }
}

LispValue* all_p_wrapper(LispValue* __args, LispValue* __env) {
    LispValue* pred = lisp_list_get(__args, 0);
    LispValue* lst = lisp_list_get(__args, 1);
    return all_p(pred, lst);
}

LispValue* identity(LispValue* x) {
    return x;
}

LispValue* identity_wrapper(LispValue* __args, LispValue* __env) {
    LispValue* x = lisp_list_get(__args, 0);
    return identity(x);
}

LispValue* __lambda_0(LispValue* __args, LispValue* __env) {
    LispValue* y = lisp_list_get(__args, 0);
    LispValue* x = lisp_env_lookup(__env, "x");
    return x;
}

LispValue* constantly(LispValue* x) {
    return lisp_make_closure(__lambda_0, lisp_env_extend(NULL, "x", x));
}

LispValue* constantly_wrapper(LispValue* __args, LispValue* __env) {
    LispValue* x = lisp_list_get(__args, 0);
    return constantly(x);
}

LispValue* __lambda_1(LispValue* __args, LispValue* __env) {
    LispValue* x = lisp_list_get(__args, 0);
    LispValue* f = lisp_env_lookup(__env, "f");
    LispValue* g = lisp_env_lookup(__env, "g");
    return lisp_call_closure(f, lisp_cons(lisp_call_closure(g, lisp_cons(x, NULL)), NULL));
}

LispValue* compose(LispValue* f, LispValue* g) {
    return lisp_make_closure(__lambda_1, lisp_env_extend(lisp_env_extend(NULL, "f", f), "g", g));
}

LispValue* compose_wrapper(LispValue* __args, LispValue* __env) {
    LispValue* f = lisp_list_get(__args, 0);
    LispValue* g = lisp_list_get(__args, 1);
    return compose(f, g);
}

LispValue* __lambda_2(LispValue* __args, LispValue* __env) {
    LispValue* x = lisp_list_get(__args, 0);
    LispValue* y = lisp_list_get(__args, 1);
    LispValue* f = lisp_env_lookup(__env, "f");
    return lisp_call_closure(f, lisp_cons(y, lisp_cons(x, NULL)));
}

LispValue* flip(LispValue* f) {
    return lisp_make_closure(__lambda_2, lisp_env_extend(NULL, "f", f));
}

LispValue* flip_wrapper(LispValue* __args, LispValue* __env) {
    LispValue* f = lisp_list_get(__args, 0);
    return flip(f);
}

LispValue* __lambda_3(LispValue* __args, LispValue* __env) {
    LispValue* y = lisp_list_get(__args, 0);
    LispValue* f = lisp_env_lookup(__env, "f");
    LispValue* x = lisp_env_lookup(__env, "x");
    return lisp_call_closure(f, lisp_cons(x, lisp_cons(y, NULL)));
}

LispValue* curry(LispValue* f, LispValue* x) {
    return lisp_make_closure(__lambda_3, lisp_env_extend(lisp_env_extend(NULL, "f", f), "x", x));
}

LispValue* curry_wrapper(LispValue* __args, LispValue* __env) {
    LispValue* f = lisp_list_get(__args, 0);
    LispValue* x = lisp_list_get(__args, 1);
    return curry(f, x);
}

int main(int argc, char** argv) {
    gc_init();


    /* Cleanup: release all variables */
    gc_shutdown();
    return 0;
}