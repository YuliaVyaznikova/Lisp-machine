from abc import ABC, abstractmethod
from .tokenizer import tokenize, Token, TokenType
from .ast_nodes import *

class ParseError(Exception):
    def __init__(self, message: str, token: Token = None):
        self.message = message
        self.token = token
        super().__init__(message)

class SpecialFormHandler(ABC):
    @abstractmethod
    def can_handle(self, name: str) -> bool:
        pass
    
    @abstractmethod
    def parse(self, elements: list, start: Token) -> ASTNode:
        pass

class IfHandler(SpecialFormHandler):
    def can_handle(self, name: str) -> bool:
        return name == "if"
    
    def parse(self, elements: list, start: Token) -> ASTNode:
        if len(elements) < 3:
            raise ParseError("if requires condition and then branch", start)
        condition = elements[1]
        then_branch = elements[2]
        else_branch = elements[3] if len(elements) > 3 else NilNode()
        return IfNode(condition=condition, then_branch=then_branch, else_branch=else_branch)

class DefineHandler(SpecialFormHandler):
    def can_handle(self, name: str) -> bool:
        return name == "define"
    
    def parse(self, elements: list, start: Token) -> ASTNode:
        if len(elements) < 3:
            raise ParseError("define requires name and value", start)
        
        second = elements[1]
        if isinstance(second, SymbolNode):
            return DefineNode(name=second.name, value=elements[2])
        
        if isinstance(second, ListNode):
            if not second.elements or not isinstance(second.elements[0], SymbolNode):
                raise ParseError("define function requires name", start)
            name = second.elements[0].name
            params = [p.name for p in second.elements[1:] if isinstance(p, SymbolNode)]
            return DefineNode(name=name, params=params, body=elements[2])
        
        raise ParseError("invalid define syntax", start)

class LambdaHandler(SpecialFormHandler):
    def can_handle(self, name: str) -> bool:
        return name == "lambda"
    
    def parse(self, elements: list, start: Token) -> ASTNode:
        if len(elements) < 3:
            raise ParseError("lambda requires params and body", start)
        
        params_node = elements[1]
        if not isinstance(params_node, ListNode):
            raise ParseError("lambda params must be a list", start)
        
        params = [p.name for p in params_node.elements if isinstance(p, SymbolNode)]
        body = elements[2]
        return LambdaNode(params=params, body=body)

class Parser:
    def __init__(self, tokens: list):
        self.tokens = tokens
        self.pos = 0
        self.handlers = [IfHandler(), DefineHandler(), LambdaHandler()]
    
    def current(self) -> Token:
        if self.pos < len(self.tokens):
            return self.tokens[self.pos]
        return self.tokens[-1]
    
    def advance(self) -> Token:
        tok = self.current()
        self.pos += 1
        return tok
    
    def expect(self, type: TokenType) -> Token:
        tok = self.current()
        if tok.type != type:
            raise ParseError(f"Expected {type}, got {tok.type}", tok)
        return self.advance()
    
    def parse(self) -> list:
        nodes = []
        while self.current().type != TokenType.EOF:
            nodes.append(self.parse_expr())
        return nodes
    
    def parse_expr(self) -> ASTNode:
        tok = self.current()
        
        if tok.type == TokenType.NUMBER:
            self.advance()
            return IntNode(value=tok.value)
        
        if tok.type == TokenType.FLOAT:
            self.advance()
            return FloatNode(value=tok.value)
        
        if tok.type == TokenType.STRING:
            self.advance()
            return StringNode(value=tok.value)
        
        if tok.type == TokenType.SYMBOL:
            self.advance()
            if tok.value == "true":
                return SymbolNode(name="true")
            if tok.value == "false":
                return SymbolNode(name="false")
            if tok.value == "nil":
                return NilNode()
            return SymbolNode(name=tok.value)
        
        if tok.type == TokenType.QUOTE:
            self.advance()
            return QuoteNode(value=self.parse_expr())
        
        if tok.type == TokenType.LPAREN:
            return self.parse_list()
        
        raise ParseError(f"Unexpected token: {tok.type}", tok)
    
    def parse_list(self) -> ASTNode:
        start = self.expect(TokenType.LPAREN)
        elements = []
        
        while self.current().type not in (TokenType.RPAREN, TokenType.EOF):
            elements.append(self.parse_expr())
        
        if self.current().type == TokenType.EOF:
            raise ParseError("Unclosed parenthesis", start)
        
        self.expect(TokenType.RPAREN)
        
        if not elements:
            return NilNode()
        
        if isinstance(elements[0], SymbolNode):
            name = elements[0].name
            for handler in self.handlers:
                if handler.can_handle(name):
                    return handler.parse(elements, start)
        
        return ListNode(elements=elements)

def parse(source: str) -> list:
    tokens = tokenize(source)
    parser = Parser(tokens)
    return parser.parse()