from typing import List, Dict
from src.parser.ast_nodes import *

class MacroExpander:
    def __init__(self):
        self.macros: Dict[str, DefmacroNode] = {}
    
    def register_macro(self, node: DefmacroNode):
        self.macros[node.name] = node
    
    def expand(self, node: ASTNode) -> ASTNode:
        if isinstance(node, ListNode):
            if not node.elements:
                return node
            first = node.elements[0]
            if isinstance(first, SymbolNode) and first.name in self.macros:
                return self._expand_macro(first.name, node.elements[1:])
            
            expanded_elements = [self.expand(e) for e in node.elements]
            first_exp = expanded_elements[0]
            
            if isinstance(first_exp, SymbolNode):
                name = first_exp.name
                if name == "if" and len(expanded_elements) >= 3:
                    return IfNode(
                        condition=expanded_elements[1],
                        then_branch=expanded_elements[2],
                        else_branch=expanded_elements[3] if len(expanded_elements) > 3 else NilNode()
                    )
                if name == "lambda" and len(expanded_elements) >= 3:
                    params_node = expanded_elements[1]
                    params = []
                    if isinstance(params_node, ListNode):
                        params = [p.name for p in params_node.elements if isinstance(p, SymbolNode)]
                    elif isinstance(params_node, NilNode):
                        params = []
                    return LambdaNode(params=params, body=expanded_elements[2])
                if name == "define" and len(expanded_elements) >= 3:
                    second = expanded_elements[1]
                    if isinstance(second, SymbolNode):
                        return DefineNode(name=second.name, value=expanded_elements[2])
                    if isinstance(second, ListNode) and second.elements and isinstance(second.elements[0], SymbolNode):
                        fname = second.elements[0].name
                        params = [p.name for p in second.elements[1:] if isinstance(p, SymbolNode)]
                        return DefineNode(name=fname, params=params, body=expanded_elements[2])
            
            return ListNode(elements=expanded_elements)
        
        if isinstance(node, IfNode):
            return IfNode(
                condition=self.expand(node.condition),
                then_branch=self.expand(node.then_branch),
                else_branch=self.expand(node.else_branch)
            )
        
        if isinstance(node, DefineNode):
            if node.params:
                return DefineNode(
                    name=node.name,
                    params=node.params,
                    body=self.expand(node.body)
                )
            return DefineNode(name=node.name, value=self.expand(node.value))
        
        if isinstance(node, LambdaNode):
            return LambdaNode(params=node.params, body=self.expand(node.body))
        
        if isinstance(node, QuoteNode):
            return node
        
        if isinstance(node, UnquoteNode):
            return self.expand(node.value)
        
        return node
    
    def _eval_compile_time(self, node: ASTNode, bindings: Dict[str, ASTNode] = None) -> ASTNode:
        if bindings is None:
            bindings = {}
        
        if isinstance(node, SymbolNode):
            if node.name in bindings:
                return bindings[node.name]
            return node
        
        if isinstance(node, IfNode):
            cond = self._eval_compile_time(node.condition, bindings)
            if isinstance(cond, SymbolNode) and cond.name == "true":
                return self._eval_compile_time(node.then_branch, bindings)
            if isinstance(cond, SymbolNode) and cond.name == "false":
                return self._eval_compile_time(node.else_branch, bindings)
            return IfNode(
                condition=cond,
                then_branch=self._eval_compile_time(node.then_branch, bindings),
                else_branch=self._eval_compile_time(node.else_branch, bindings)
            )
        
        if isinstance(node, ListNode) and node.elements:
            first = node.elements[0]
            if isinstance(first, SymbolNode):
                if first.name == "if" and len(node.elements) >= 3:
                    cond = self._eval_compile_time(node.elements[1], bindings)
                    if isinstance(cond, SymbolNode) and cond.name == "true":
                        return self._eval_compile_time(node.elements[2], bindings)
                    if isinstance(cond, SymbolNode) and cond.name == "false":
                        if len(node.elements) > 3:
                            return self._eval_compile_time(node.elements[3], bindings)
                        return NilNode()
                    return ListNode(elements=[node.elements[0], cond, node.elements[2], node.elements[3] if len(node.elements) > 3 else NilNode()])
                if first.name == "nil?" and len(node.elements) == 2:
                    arg = self._eval_compile_time(node.elements[1], bindings)
                    if isinstance(arg, NilNode) or (isinstance(arg, ListNode) and not arg.elements):
                        return SymbolNode(name="true")
                    return SymbolNode(name="false")
                if first.name == "first" and len(node.elements) == 2:
                    arg = self._eval_compile_time(node.elements[1], bindings)
                    if isinstance(arg, ListNode) and arg.elements:
                        return arg.elements[0]
                    if isinstance(arg, QuoteNode) and isinstance(arg.value, ListNode) and arg.value.elements:
                        return QuoteNode(value=arg.value.elements[0])
                    return NilNode()
                if first.name == "rest" and len(node.elements) == 2:
                    arg = self._eval_compile_time(node.elements[1], bindings)
                    if isinstance(arg, ListNode) and arg.elements:
                        return ListNode(elements=arg.elements[1:])
                    if isinstance(arg, QuoteNode) and isinstance(arg.value, ListNode) and arg.value.elements:
                        return QuoteNode(value=ListNode(elements=arg.value.elements[1:]))
                    return NilNode()
            return ListNode(elements=[self._eval_compile_time(e, bindings) for e in node.elements])
        
        if isinstance(node, ListNode):
            return ListNode(elements=[self._eval_compile_time(e, bindings) for e in node.elements])
        if isinstance(node, UnquoteNode):
            return UnquoteNode(value=self._eval_compile_time(node.value, bindings))
        if isinstance(node, UnquoteSplicingNode):
            return UnquoteSplicingNode(value=self._eval_compile_time(node.value, bindings))
        if isinstance(node, QuoteNode):
            return QuoteNode(value=self._eval_compile_time(node.value, bindings))
        return node

    def _expand_macro(self, name: str, args: List[ASTNode]) -> ASTNode:
        macro = self.macros[name]
        bindings = {}
        for i, param in enumerate(macro.params):
            if i < len(args):
                bindings[param] = args[i]
        
        if macro.rest_param:
            rest_args = args[len(macro.params):]
            bindings[macro.rest_param] = ListNode(elements=rest_args) if rest_args else NilNode()
        
        expanded = self._substitute(macro.body, bindings)
        expanded = self._eval_compile_time(expanded, bindings)
        
        if isinstance(expanded, QuoteNode):
            expanded = expanded.value
        return self.expand(expanded)
    
    def _substitute(self, node: ASTNode, bindings: Dict[str, ASTNode]) -> ASTNode:
        if isinstance(node, SymbolNode):
            if node.name in bindings:
                return bindings[node.name]
            return node
        
        if isinstance(node, ListNode):
            new_elements = []
            for e in node.elements:
                if isinstance(e, UnquoteSplicingNode):
                    spliced = self._substitute(e.value, bindings)
                    if isinstance(spliced, ListNode):
                        new_elements.extend(spliced.elements)
                    elif not isinstance(spliced, NilNode):
                        new_elements.append(spliced)
                else:
                    new_elements.append(self._substitute(e, bindings))
            return ListNode(elements=new_elements)
        
        if isinstance(node, QuoteNode):
            subbed = self._substitute(node.value, bindings)
            return QuoteNode(value=subbed)
        
        if isinstance(node, UnquoteNode):
            subbed = self._substitute(node.value, bindings)
            return self._eval_compile_time(subbed, bindings)
        
        if isinstance(node, UnquoteSplicingNode):
            return self._substitute(node.value, bindings)
        
        if isinstance(node, IfNode):
            return IfNode(
                condition=self._substitute(node.condition, bindings),
                then_branch=self._substitute(node.then_branch, bindings),
                else_branch=self._substitute(node.else_branch, bindings)
            )
            
        if isinstance(node, LambdaNode):
            return LambdaNode(
                params=node.params,
                body=self._substitute(node.body, bindings)
            )
        
        return node

def expand_macros(nodes: List[ASTNode]) -> List[ASTNode]:
    expander = MacroExpander()
    result = []
    
    for node in nodes:
        if isinstance(node, DefmacroNode):
            expander.register_macro(node)
        else:
            result.append(expander.expand(node))
    
    return result
