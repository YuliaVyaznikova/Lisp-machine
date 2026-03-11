from typing import List
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
        self.func_counter = 0
        self.local_vars = set()
        self.closures = []
    
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
        return name.replace('-', '_')
    
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
        self.temp_counter = 0
        self.local_vars = set(node.params)
        func_stmts = []
        
        params_str = ", ".join(["LispValue* " + p for p in node.params])
        func_stmts.append(f"LispValue* {self._mangle_name(node.name)}({params_str}) {{")
        
        body_expr = self._gen_expr(node.body)
        if body_expr:
            func_stmts.append(f"    return {body_expr};")
        else:
            func_stmts.append("    return NULL;")
        func_stmts.append("}")
        
        self.functions.append("\n".join(func_stmts))
        self.local_vars = set()
    
    def _main(self, nodes: List[ASTNode]) -> str:
        lines = ["int main(int argc, char** argv) {"]
        self.indent = 1
        
        for name in self.variables:
            lines.append(f"    LispValue* {self._mangle_name(name)} = NULL;")
        
        for node in nodes:
            if isinstance(node, DefineNode) and not node.params:
                expr = self._gen_expr(node.value)
                lines.append(f"    {self._mangle_name(node.name)} = {expr};")
            elif not isinstance(node, DefineNode):
                expr = self._gen_expr(node)
                if expr:
                    lines.append(f"    {expr};")
        
        lines.append("    return 0;")
        lines.append("}")
        
        if self.statements:
            all_lines = ["int main(int argc, char** argv) {"]
            for name in self.variables:
                all_lines.append(f"    LispValue* {self._mangle_name(name)} = NULL;")
            for stmt in self.statements:
                all_lines.append(f"    {stmt}")
            for line in lines[1 + len(self.variables):]:
                all_lines.append(line)
            return "\n".join(all_lines)
        
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
                case "print":
                    return f"lisp_print({self._gen_expr(args[0])})"
                case "apply":
                    func_expr = self._gen_expr(args[0])
                    args_expr = self._gen_expr(args[1])
                    return f"lisp_apply({func_expr}, {args_expr})"
                case _:
                    mangled = self._mangle_name(func.name)
                    for node in self.functions:
                        if node.startswith(f"LispValue* {mangled}("):
                            args_str = ", ".join(self._gen_expr(a) for a in args)
                            return f"{mangled}({args_str})"
                    if func.name in self.variables:
                        args_list = self._build_args_list(args)
                        return f"lisp_call_closure({self._mangle_name(func.name)}, {args_list})"
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
        then_val = self._gen_expr(node.then_branch)
        else_val = self._gen_expr(node.else_branch)
        
        self.statements.append(f"if (lisp_is_true({cond})) {{")
        self.statements.append(f"    {temp} = {then_val};")
        self.statements.append("} else {")
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