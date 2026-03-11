from typing import List
import sys
import os
sys.path.insert(0, os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
from parser.ast_nodes import *

class CodeGenerator:
    def __init__(self):
        self.indent = 0
        self.temp_counter = 0
        self.statements = []
        self.variables = {}
        self.functions = []
        self.func_counter = 0
        self.local_vars = set()
    
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
    
    def _forward_decls(self, nodes: List[ASTNode]) -> str:
        decls = []
        for node in nodes:
            if isinstance(node, DefineNode) and node.params:
                params_str = ", ".join(["LispValue* " + p for p in node.params])
                decls.append(f"LispValue* {node.name}({params_str});")
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
        func_stmts.append(f"LispValue* {node.name}({params_str}) {{")
        
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
            lines.append(f"    LispValue* {name} = NULL;")
        
        for node in nodes:
            if isinstance(node, DefineNode) and not node.params:
                expr = self._gen_expr(node.value)
                lines.append(f"    {node.name} = {expr};")
            elif not isinstance(node, DefineNode):
                expr = self._gen_expr(node)
                if expr:
                    lines.append(f"    {expr};")
        
        lines.append("    return 0;")
        lines.append("}")
        
        if self.statements:
            all_lines = ["int main(int argc, char** argv) {"]
            for name in self.variables:
                all_lines.append(f"    LispValue* {name} = NULL;")
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
            if node.name in self.variables or node.name in self.local_vars:
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
            func_name = self._gen_lambda(func)
            args_str = ", ".join(self._gen_expr(a) for a in args)
            return f"{func_name}({args_str})"
        
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
                case _:
                    for node in self.functions:
                        if node.startswith(f"LispValue* {func.name}("):
                            args_str = ", ".join(self._gen_expr(a) for a in args)
                            return f"{func.name}({args_str})"
                    return "NULL"
        
        return "NULL"
    
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
        
        saved_local = self.local_vars.copy()
        self.local_vars = set(node.params)
        
        params_str = ", ".join(["LispValue* " + p for p in node.params])
        func_lines = [f"LispValue* {func_name}({params_str}) {{"]
        
        body_expr = self._gen_expr(node.body)
        if body_expr:
            func_lines.append(f"    return {body_expr};")
        else:
            func_lines.append("    return NULL;")
        func_lines.append("}")
        
        self.functions.append("\n".join(func_lines))
        self.local_vars = saved_local
        return func_name