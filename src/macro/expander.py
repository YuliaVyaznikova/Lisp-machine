from typing import List, Dict
import sys
import os
sys.path.insert(0, os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
from parser.ast_nodes import *

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
            return ListNode(elements=[self.expand(e) for e in node.elements])
        
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
        
        if isinstance(node, WhileNode):
            return WhileNode(
                condition=self.expand(node.condition),
                body=[self.expand(b) for b in node.body]
            )
        
        if isinstance(node, SetNode):
            return SetNode(name=node.name, value=self.expand(node.value))
        
        if isinstance(node, QuoteNode):
            return node
        
        if isinstance(node, UnquoteNode):
            return self.expand(node.value)
        
        return node
    
    def _expand_macro(self, name: str, args: List[ASTNode]) -> ASTNode:
        macro = self.macros[name]
        bindings = {}
        for i, param in enumerate(macro.params):
            if i < len(args):
                bindings[param] = args[i]
        
        expanded = self._substitute(macro.body, bindings)
        return self.expand(expanded)
    
    def _substitute(self, node: ASTNode, bindings: Dict[str, ASTNode]) -> ASTNode:
        if isinstance(node, SymbolNode):
            if node.name in bindings:
                return bindings[node.name]
            return node
        
        if isinstance(node, ListNode):
            return ListNode(elements=[self._substitute(e, bindings) for e in node.elements])
        
        if isinstance(node, QuoteNode):
            return node
        
        if isinstance(node, UnquoteNode):
            return self._substitute(node.value, bindings)
        
        if isinstance(node, UnquoteSplicingNode):
            return node
        
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