from .tokenizer import tokenize, Token, TokenType
from .ast_nodes import *

class ParseError(Exception):
    def __init__(self, message: str, token: Token = None):
        self.message = message
        self.token = token
        super().__init__(message)

class Parser:
    def __init__(self, tokens: list):
        self.tokens = tokens
        self.pos = 0
    
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
            if name == "if":
                return self._parse_if(elements, start)
        
        return ListNode(elements=elements)
    
    def _parse_if(self, elements: list, start: Token) -> IfNode:
        if len(elements) < 3:
            raise ParseError("if requires condition and then branch", start)
        
        condition = elements[1]
        then_branch = elements[2]
        else_branch = elements[3] if len(elements) > 3 else NilNode()
        
        return IfNode(condition=condition, then_branch=then_branch, else_branch=else_branch)

def parse(source: str) -> list:
    tokens = tokenize(source)
    parser = Parser(tokens)
    return parser.parse()