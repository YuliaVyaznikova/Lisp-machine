from dataclasses import dataclass
from typing import List, Optional

@dataclass
class Location:
    line: int = 0
    col: int = 0

@dataclass
class ASTNode:
    location: Optional[Location] = None

@dataclass
class IntNode(ASTNode):
    value: int = 0

@dataclass
class FloatNode(ASTNode):
    value: float = 0.0

@dataclass
class StringNode(ASTNode):
    value: str = ""

@dataclass
class SymbolNode(ASTNode):
    name: str = ""

@dataclass
class NilNode(ASTNode):
    location: Optional[Location] = None

@dataclass
class ListNode(ASTNode):
    elements: List[ASTNode] = None
    def __post_init__(self):
        if self.elements is None:
            self.elements = []

@dataclass
class QuoteNode(ASTNode):
    value: ASTNode = None

@dataclass
class IfNode(ASTNode):
    condition: ASTNode = None
    then_branch: ASTNode = None
    else_branch: ASTNode = None

@dataclass
class ApplicationNode(ASTNode):
    function: ASTNode = None
    arguments: List[ASTNode] = None
    def __post_init__(self):
        if self.arguments is None:
            self.arguments = []

@dataclass
class DefineNode(ASTNode):
    name: str = ""
    params: List[str] = None
    body: ASTNode = None
    value: ASTNode = None
    def __post_init__(self):
        if self.params is None:
            self.params = []

@dataclass
class LambdaNode(ASTNode):
    params: List[str] = None
    body: ASTNode = None
    def __post_init__(self):
        if self.params is None:
            self.params = []

@dataclass
class DefmacroNode(ASTNode):
    name: str = ""
    params: List[str] = None
    body: ASTNode = None
    def __post_init__(self):
        if self.params is None:
            self.params = []

@dataclass
class UnquoteNode(ASTNode):
    value: ASTNode = None

@dataclass
class UnquoteSplicingNode(ASTNode):
    value: ASTNode = None