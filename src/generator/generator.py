from typing import List
from parser.ast_nodes import *

class CodeGenerator:
    def __init__(self):
        self.indent = 0
        self.temp_counter = 0
        self.statements = []
    
    def generate(self, nodes: List[ASTNode]) -> str:
        self.statements = []
        parts = []
        parts.append(self._header())
        parts.append(self._forward_decls(nodes))
        parts.append(self._main(nodes))
        return "\n\n".join(parts)
    
    def _header(self) -> str:
        return '''#include <stdio.h>
#include <stdlib.h>
#include "runtime/lisp.h"'''
    
    def _forward_decls(self, nodes: List[ASTNode]) -> str:
        return "/* Forward declarations */"
    
    def _main(self, nodes: List[ASTNode]) -> str:
        lines = ["int main(int argc, char** argv) {"]
        self.indent = 1
        
        for node in nodes:
            expr = self._gen_expr(node)
            if expr:
                lines.append(f"    {expr};")
        
        lines.append("    return 0;")
        lines.append("}")
        
        if self.statements:
            all_lines = ["int main(int argc, char** argv) {"]
            for stmt in self.statements:
                all_lines.append(f"    {stmt}")
            for line in lines[1:]:
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
            return f'lisp_make_symbol("{node.name}")'
        
        if isinstance(node, NilNode):
            return "NULL"
        
        if isinstance(node, QuoteNode):
            return self._gen_quote(node.value)
        
        if isinstance(node, ListNode):
            return self._gen_application(node.elements)
        
        if isinstance(node, IfNode):
            return self._gen_if(node)
        
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