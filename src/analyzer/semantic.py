from dataclasses import dataclass
from typing import Dict, Optional, List
import sys
import os
sys.path.insert(0, os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
from parser.ast_nodes import *

@dataclass
class SymbolInfo:
    name: str
    kind: str
    params: Optional[List[str]] = None

class SymbolTable:
    def __init__(self, parent: Optional['SymbolTable'] = None):
        self.symbols: Dict[str, SymbolInfo] = {}
        self.parent = parent
    
    def define(self, name: str, kind: str, params: Optional[List[str]] = None):
        self.symbols[name] = SymbolInfo(name=name, kind=kind, params=params)
    
    def lookup(self, name: str) -> Optional[SymbolInfo]:
        if name in self.symbols:
            return self.symbols[name]
        if self.parent:
            return self.parent.lookup(name)
        return None
    
    def has(self, name: str) -> bool:
        return self.lookup(name) is not None

class SemanticAnalyzer:
    BUILTINS = {'+', '-', '*', '/', '=', '<', '>', 'first', 'rest', 'cons', 'print', 'if', 'define', 'lambda', 'quote', 'nil', 'true', 'false', 'apply', 'while', 'set!', 'length', 'append', 'reverse', 'not', 'mod', 'abs', 'min', 'max', 'nil?'}
    
    def __init__(self):
        self.global_table = SymbolTable()
        self.errors = []
        self.warnings = []
    
    def analyze(self, nodes: List[ASTNode]) -> bool:
        self._collect_definitions(nodes)
        
        for node in nodes:
            self._analyze_node(node, self.global_table)
        
        return len(self.errors) == 0
    
    def _collect_definitions(self, nodes: List[ASTNode]):
        for node in nodes:
            if isinstance(node, DefineNode):
                if node.params:
                    self.global_table.define(node.name, 'function', node.params)
                else:
                    self.global_table.define(node.name, 'variable')
    
    def _analyze_node(self, node: ASTNode, table: SymbolTable):
        if isinstance(node, IntNode) or isinstance(node, FloatNode) or isinstance(node, StringNode) or isinstance(node, NilNode):
            return
        
        if isinstance(node, SymbolNode):
            if node.name not in self.BUILTINS and not table.has(node.name):
                self.warnings.append(f"Undefined symbol: {node.name}")
            return
        
        if isinstance(node, QuoteNode):
            return
        
        if isinstance(node, ListNode):
            self._analyze_list(node, table)
            return
        
        if isinstance(node, IfNode):
            self._analyze_node(node.condition, table)
            self._analyze_node(node.then_branch, table)
            self._analyze_node(node.else_branch, table)
            return
        
        if isinstance(node, DefineNode):
            if node.params:
                local_table = SymbolTable(table)
                for param in node.params:
                    local_table.define(param, 'parameter')
                self._analyze_node(node.body, local_table)
            else:
                self._analyze_node(node.value, table)
            return
        
        if isinstance(node, LambdaNode):
            local_table = SymbolTable(table)
            for param in node.params:
                local_table.define(param, 'parameter')
            self._analyze_node(node.body, local_table)
            return
        
        if isinstance(node, WhileNode):
            self._analyze_node(node.condition, table)
            for body_node in node.body:
                self._analyze_node(body_node, table)
            return
        
        if isinstance(node, SetNode):
            if not table.has(node.name):
                self.warnings.append(f"set! on undefined variable: {node.name}")
            self._analyze_node(node.value, table)
            return
    
    def _analyze_list(self, node: ListNode, table: SymbolTable):
        for elem in node.elements:
            self._analyze_node(elem, table)