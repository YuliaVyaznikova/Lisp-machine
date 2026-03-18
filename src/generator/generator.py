from typing import List, Optional, Tuple
import sys
import os
sys.path.insert(0, os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
from parser.ast_nodes import *

class CodeGenerator:
    BUILTINS = {'+', '-', '*', '/', '=', '<', '>', 'first', 'rest', 'cons', 'print', 'if', 'define', 'lambda', 'quote', 'nil', 'true', 'false'}
    
    def __init__(self):
        self.indent = 0
        self.temp_counter = 0
        self.statements = []
        self.variables = {}
        self.functions = []
        self.func_names = set()
        self.func_counter = 0
        self.local_vars = set()
        self.closures = []
        self.current_function = None
        self.builtin_wrappers = {
            '+': 'lisp_add_wrapper',
            '-': 'lisp_sub_wrapper',
            '*': 'lisp_mul_wrapper',
            '/': 'lisp_div_wrapper',
            '=': 'lisp_eq_wrapper',
            '<': 'lisp_lt_wrapper',
            '>': 'lisp_gt_wrapper',
            'first': 'lisp_first_wrapper',
            'rest': 'lisp_rest_wrapper',
            'cons': 'lisp_cons_wrapper',
            'print': 'lisp_print_wrapper',
        }
    
    def _is_tail_call(self, node: ASTNode, func_name: str) -> bool:
        if isinstance(node, ListNode) and node.elements:
            first = node.elements[0]
            if isinstance(first, SymbolNode):
                mangled = self._mangle_name(first.name)
                if mangled == func_name:
                    return True
        return False
    
    def _find_tail_calls(self, node: ASTNode, func_name: str) -> List[Tuple[ASTNode, bool]]:
        calls = []
        self._find_tail_calls_recursive(node, func_name, True, calls)
        return calls
    
    def _find_tail_calls_recursive(self, node: ASTNode, func_name: str, is_tail: bool, calls: List):
        if isinstance(node, ListNode) and node.elements:
            first = node.elements[0]
            if isinstance(first, SymbolNode):
                mangled = self._mangle_name(first.name)
                if mangled == func_name:
                    calls.append((node, is_tail))
                    return
            
            if isinstance(first, SymbolNode) and first.name == "if":
                if len(node.elements) >= 3:
                    self._find_tail_calls_recursive(node.elements[1], func_name, False, calls)
                    self._find_tail_calls_recursive(node.elements[2], func_name, is_tail, calls)
                    if len(node.elements) > 3:
                        self._find_tail_calls_recursive(node.elements[3], func_name, is_tail, calls)
                return
            
            for elem in node.elements[1:]:
                self._find_tail_calls_recursive(elem, func_name, False, calls)
        
        if isinstance(node, IfNode):
            self._find_tail_calls_recursive(node.condition, func_name, False, calls)
            self._find_tail_calls_recursive(node.then_branch, func_name, is_tail, calls)
            self._find_tail_calls_recursive(node.else_branch, func_name, is_tail, calls)
    
    def _can_optimize_tail_recursion(self, node: DefineNode) -> bool:
        if not node.params:
            return False
        
        mangled = self._mangle_name(node.name)
        calls = self._find_tail_calls(node.body, mangled)
        
        for call_node, is_tail in calls:
            if not is_tail:
                return False
        
        return len(calls) > 0
    
    def _get_recursive_call_args(self, node: ListNode, func_name: str) -> Optional[List[ASTNode]]:
        if not node.elements:
            return None
        first = node.elements[0]
        if isinstance(first, SymbolNode):
            mangled = self._mangle_name(first.name)
            if mangled == func_name:
                return node.elements[1:]
        return None
    
    def _find_free_vars(self, node: ASTNode, bound: set) -> set:
        if isinstance(node, IntNode) or isinstance(node, FloatNode) or isinstance(node, StringNode) or isinstance(node, NilNode):
            return set()
        
        if isinstance(node, SymbolNode):
            if node.name not in bound and node.name not in self.BUILTINS:
                return {node.name}
            return set()
        
        if isinstance(node, QuoteNode):
            return set()
        
        if isinstance(node, ListNode):
            return self._find_free_vars_list(node.elements, bound)
        
        if isinstance(node, IfNode):
            free = self._find_free_vars(node.condition, bound)
            free |= self._find_free_vars(node.then_branch, bound)
            free |= self._find_free_vars(node.else_branch, bound)
            return free
        
        if isinstance(node, LambdaNode):
            body_free = self._find_free_vars(node.body, set())
            return body_free - set(node.params)
        
        if isinstance(node, DefineNode):
            if node.params:
                new_bound = bound | set(node.params)
                return self._find_free_vars(node.body, new_bound)
            return self._find_free_vars(node.value, bound)
        
        return set()
    
    def _find_free_vars_list(self, elements: List[ASTNode], bound: set) -> set:
        free = set()
        for elem in elements:
            free |= self._find_free_vars(elem, bound)
        return free
    
    def generate(self, nodes: List[ASTNode]) -> str:
        self.statements = []
        self.variables = {}
        self.functions = []
        self.func_names = set()
        self.local_vars = set()
        self.func_counter = 0
        
        self._collect_defines(nodes)
        
        main_code = self._main(nodes)
        
        parts = []
        parts.append(self._header())
        parts.append(self._forward_decls(nodes))
        parts.append(self._functions())
        parts.append(main_code)
        return "\n\n".join(parts)
    
    def _header(self) -> str:
        return '''#include <stdio.h>
#include <stdlib.h>
#include "runtime/lisp.h"'''
    
    def _mangle_name(self, name: str) -> str:
        return name.replace('-', '_').replace('?', '_p')
    
    def _forward_decls(self, nodes: List[ASTNode]) -> str:
        decls = []
        for node in nodes:
            if isinstance(node, DefineNode) and node.params:
                params_str = ", ".join(["LispValue* " + p for p in node.params])
                decls.append(f"LispValue* {self._mangle_name(node.name)}({params_str});")
        if decls:
            return "/* Forward declarations */\n" + "\n".join(decls)
        return "/* Forward declarations */"
    
    def _functions(self) -> str:
        if not self.functions:
            return ""
        return "\n\n".join(self.functions)
    
    def _collect_defines(self, nodes: List[ASTNode]):
        for node in nodes:
            if isinstance(node, DefineNode):
                if node.params:
                    self._gen_function(node)
                else:
                    self.variables[node.name] = node.value
    
    def _gen_function(self, node: DefineNode):
        mangled = self._mangle_name(node.name)
        self.func_names.add(mangled)
        self.temp_counter = 0
        self.local_vars = set(node.params)
        self.current_function = mangled
        self.statements = []
        func_stmts = []
        
        params_str = ", ".join(["LispValue* " + p for p in node.params])
        func_stmts.append(f"LispValue* {mangled}({params_str}) {{")
        
        if self._can_optimize_tail_recursion(node):
            func_stmts.extend(self._gen_function_tco(node, mangled))
        else:
            body_expr = self._gen_expr(node.body)
            for stmt in self.statements:
                func_stmts.append(f"    {stmt}")
            if body_expr:
                func_stmts.append(f"    return {body_expr};")
            else:
                func_stmts.append("    return NULL;")
        
        func_stmts.append("}")
        
        wrapper_stmts = [f"LispValue* {mangled}_wrapper(LispValue* __args, LispValue* __env) {{"]
        for i, p in enumerate(node.params):
            wrapper_stmts.append(f"    LispValue* {p} = lisp_list_get(__args, {i});")
        args_str = ", ".join(node.params)
        wrapper_stmts.append(f"    return {mangled}({args_str});")
        wrapper_stmts.append("}")
        
        self.functions.append("\n".join(func_stmts))
        self.functions.append("\n".join(wrapper_stmts))
        self.local_vars = set()
        self.current_function = None
    
    def _gen_function_tco(self, node: DefineNode, func_name: str) -> List[str]:
        stmts = []
        
        for p in node.params:
            stmts.append(f"    LispValue* __new_{p} = NULL;")
        
        stmts.append("    while (1) {")
        
        body_stmts = self._gen_tco_body(node.body, func_name, node.params)
        stmts.extend(body_stmts)
        
        stmts.append("    }")
        
        return stmts
    
    def _gen_tco_body(self, node: ASTNode, func_name: str, params: List[str]) -> List[str]:
        stmts = []
        
        if isinstance(node, IfNode):
            cond = self._gen_expr(node.condition)
            stmts.append(f"        if (lisp_is_true({cond})) {{")
            
            then_stmts = self._gen_tco_branch(node.then_branch, func_name, params)
            stmts.extend(then_stmts)
            
            stmts.append("        } else {")
            
            else_stmts = self._gen_tco_branch(node.else_branch, func_name, params)
            stmts.extend(else_stmts)
            
            stmts.append("        }")
        else:
            expr = self._gen_expr(node)
            stmts.append(f"        return {expr};")
        
        return stmts
    
    def _gen_tco_branch(self, node: ASTNode, func_name: str, params: List[str]) -> List[str]:
        stmts = []
        
        if isinstance(node, ListNode) and node.elements:
            first = node.elements[0]
            if isinstance(first, SymbolNode):
                mangled = self._mangle_name(first.name)
                if mangled == func_name:
                    args = node.elements[1:]
                    for i, p in enumerate(params):
                        if i < len(args):
                            stmts.append(f"            __new_{p} = {self._gen_expr(args[i])};")
                        else:
                            stmts.append(f"            __new_{p} = NULL;")
                    
                    for p in params:
                        stmts.append(f"            {p} = __new_{p};")
                    
                    stmts.append("            continue;")
                    return stmts
        
        expr = self._gen_expr(node)
        stmts.append(f"            return {expr};")
        return stmts
    
    def _main(self, nodes: List[ASTNode]) -> str:
        lines = ["int main(int argc, char** argv) {"]
        
        for name in self.variables:
            lines.append(f"    LispValue* {self._mangle_name(name)} = NULL;")
        
        for node in nodes:
            if isinstance(node, DefineNode) and not node.params:
                expr = self._gen_expr(node.value)
                lines.append(f"    {self._mangle_name(node.name)} = {expr};")
            elif not isinstance(node, DefineNode):
                self.statements = []
                result = self._gen_expr(node)
                for stmt in self.statements:
                    lines.append(f"    {stmt}")
                if result and result.startswith("__"):
                    pass
                elif result and result != "NULL":
                    lines.append(f"    {result};")
        
        lines.append("    return 0;")
        lines.append("}")
        
        return "\n".join(lines)
    
    def _gen_expr(self, node: ASTNode) -> str:
        if isinstance(node, IntNode):
            return f"lisp_make_int({node.value})"
        
        if isinstance(node, FloatNode):
            return f"lisp_make_float({node.value})"
        
        if isinstance(node, StringNode):
            return f'lisp_make_string("{node.value}")'
        
        if isinstance(node, SymbolNode):
            if node.name in self.variables:
                return self._mangle_name(node.name)
            if node.name in self.local_vars:
                return node.name
            if node.name in self.builtin_wrappers:
                return f"lisp_make_closure({self.builtin_wrappers[node.name]}, NULL)"
            mangled = self._mangle_name(node.name)
            if mangled in self.func_names:
                return f"lisp_make_closure({mangled}_wrapper, NULL)"
            return f'lisp_make_symbol("{node.name}")'
        
        if isinstance(node, NilNode):
            return "NULL"
        
        if isinstance(node, QuoteNode):
            return self._gen_quote(node.value)
        
        if isinstance(node, ListNode):
            return self._gen_application(node.elements)
        
        if isinstance(node, IfNode):
            return self._gen_if(node)
        
        if isinstance(node, DefineNode):
            return None
        
        if isinstance(node, LambdaNode):
            return self._gen_lambda(node)
        
        return "NULL"
    
    def _gen_quote(self, node: ASTNode) -> str:
        if isinstance(node, IntNode):
            return f"lisp_make_int({node.value})"
        if isinstance(node, SymbolNode):
            return f'lisp_make_symbol("{node.name}")'
        if isinstance(node, NilNode):
            return "NULL"
        if isinstance(node, ListNode):
            return self._gen_list_literal(node.elements)
        return "NULL"
    
    def _gen_list_literal(self, elements: List[ASTNode]) -> str:
        if not elements:
            return "NULL"
        
        result = "NULL"
        for elem in reversed(elements):
            val = self._gen_quote(elem)
            result = f"lisp_cons({val}, {result})"
        return result
    
    def _gen_application(self, elements: List[ASTNode]) -> str:
        if not elements:
            return "NULL"
        
        func = elements[0]
        args = elements[1:]
        
        if isinstance(func, LambdaNode):
            closure = self._gen_lambda(func)
            args_list = self._build_args_list(args)
            return f"lisp_call_closure({closure}, {args_list})"
        
        if isinstance(func, SymbolNode):
            match func.name:
                case "+":
                    return self._gen_binop("lisp_add", args)
                case "-":
                    return self._gen_binop("lisp_sub", args)
                case "*":
                    return self._gen_binop("lisp_mul", args)
                case "/":
                    return self._gen_binop("lisp_div", args)
                case "=":
                    return self._gen_binop("lisp_eq", args)
                case "<":
                    return self._gen_binop("lisp_lt", args)
                case ">":
                    return self._gen_binop("lisp_gt", args)
                case "first":
                    return f"lisp_first({self._gen_expr(args[0])})"
                case "rest":
                    return f"lisp_rest({self._gen_expr(args[0])})"
                case "cons":
                    return f"lisp_cons({self._gen_expr(args[0])}, {self._gen_expr(args[1])})"
                case "length":
                    return f"lisp_length({self._gen_expr(args[0])})"
                case "append":
                    return f"lisp_append({self._gen_expr(args[0])}, {self._gen_expr(args[1])})"
                case "reverse":
                    return f"lisp_reverse({self._gen_expr(args[0])})"
                case "not":
                    return f"lisp_not({self._gen_expr(args[0])})"
                case "mod":
                    return f"lisp_mod({self._gen_expr(args[0])}, {self._gen_expr(args[1])})"
                case "abs":
                    return f"lisp_abs({self._gen_expr(args[0])})"
                case "min":
                    return f"lisp_min({self._gen_expr(args[0])}, {self._gen_expr(args[1])})"
                case "max":
                    return f"lisp_max({self._gen_expr(args[0])}, {self._gen_expr(args[1])})"
                case "nil?":
                    return f"lisp_is_nil_fn({self._gen_expr(args[0])})"
                case "print":
                    return f"lisp_print({self._gen_expr(args[0])})"
                case "apply":
                    func_expr = self._gen_expr(args[0])
                    args_expr = self._gen_expr(args[1])
                    return f"lisp_apply({func_expr}, {args_expr})"
                case _:
                    mangled = self._mangle_name(func.name)
                    if mangled in self.func_names:
                        args_str = ", ".join(self._gen_expr(a) for a in args)
                        return f"{mangled}({args_str})"
                    if func.name in self.variables:
                        args_list = self._build_args_list(args)
                        return f"lisp_call_closure({self._mangle_name(func.name)}, {args_list})"
                    if func.name in self.local_vars:
                        args_list = self._build_args_list(args)
                        return f"lisp_call_closure({func.name}, {args_list})"
                    return "NULL"
        
        return "NULL"
    
    def _build_args_list(self, args: List[ASTNode]) -> str:
        if not args:
            return "NULL"
        result = "NULL"
        for arg in reversed(args):
            val = self._gen_expr(arg)
            result = f"lisp_cons({val}, {result})"
        return result
    
    def _gen_binop(self, func: str, args: List[ASTNode]) -> str:
        if len(args) == 0:
            return "lisp_make_int(0)"
        if len(args) == 1:
            return self._gen_expr(args[0])
        
        result = self._gen_expr(args[0])
        for arg in args[1:]:
            result = f"{func}({result}, {self._gen_expr(arg)})"
        return result
    
    def _gen_if(self, node: IfNode) -> str:
        temp = f"__result_{self.temp_counter}"
        self.temp_counter += 1
        
        self.statements.append(f"LispValue* {temp} = NULL;")
        
        cond = self._gen_expr(node.condition)
        
        saved_stmts = self.statements.copy()
        self.statements = []
        then_val = self._gen_expr(node.then_branch)
        then_stmts = self.statements
        self.statements = saved_stmts
        
        self.statements = []
        else_val = self._gen_expr(node.else_branch)
        else_stmts = self.statements
        self.statements = saved_stmts
        
        self.statements.append(f"if (lisp_is_true({cond})) {{")
        for stmt in then_stmts:
            self.statements.append(f"    {stmt}")
        self.statements.append(f"    {temp} = {then_val};")
        self.statements.append("} else {")
        for stmt in else_stmts:
            self.statements.append(f"    {stmt}")
        self.statements.append(f"    {temp} = {else_val};")
        self.statements.append("}")
        
        return temp

    def _gen_lambda(self, node: LambdaNode) -> str:
        func_name = f"__lambda_{self.func_counter}"
        self.func_counter += 1
        
        free_vars = self._find_free_vars(node, self.local_vars)
        
        saved_local = self.local_vars.copy()
        self.local_vars = set(node.params) | free_vars
        
        func_lines = [f"LispValue* {func_name}(LispValue* __args, LispValue* __env) {{"]
        
        for i, p in enumerate(node.params):
            func_lines.append(f"    LispValue* {p} = lisp_list_get(__args, {i});")
        
        for fv in sorted(free_vars):
            func_lines.append(f"    LispValue* {fv} = lisp_env_lookup(__env, \"{fv}\");")
        
        body_expr = self._gen_expr(node.body)
        if body_expr:
            func_lines.append(f"    return {body_expr};")
        else:
            func_lines.append("    return NULL;")
        func_lines.append("}")
        
        self.functions.append("\n".join(func_lines))
        self.local_vars = saved_local
        
        env_build = "NULL"
        for fv in sorted(free_vars):
            env_build = f'lisp_env_extend({env_build}, "{fv}", {fv})'
        return f"lisp_make_closure({func_name}, {env_build})"