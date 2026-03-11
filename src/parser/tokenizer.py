from enum import Enum, auto

class TokenType(Enum):
    LPAREN = auto()
    RPAREN = auto()
    QUOTE = auto()
    QUASIQUOTE = auto()
    UNQUOTE = auto()
    UNQUOTE_SPLICING = auto()
    NUMBER = auto()
    FLOAT = auto()
    STRING = auto()
    SYMBOL = auto()
    EOF = auto()

class Token:
    def __init__(self, type: TokenType, value, line: int = 0, col: int = 0):
        self.type = type
        self.value = value
        self.line = line
        self.col = col
    
    def __repr__(self):
        return f"Token({self.type}, {self.value!r})"

def tokenize(source: str) -> list:
    tokens = []
    i = 0
    line = 1
    col = 1
    
    while i < len(source):
        c = source[i]
        
        if c == '\n':
            line += 1
            col = 1
            i += 1
            continue
        
        if c in ' \t\r':
            col += 1
            i += 1
            continue
        
        if c == ';':
            while i < len(source) and source[i] != '\n':
                i += 1
            continue
        
        if c == '(':
            tokens.append(Token(TokenType.LPAREN, '(', line, col))
            i += 1
            col += 1
            continue
        
        if c == ')':
            tokens.append(Token(TokenType.RPAREN, ')', line, col))
            i += 1
            col += 1
            continue
        
        if c == "'":
            tokens.append(Token(TokenType.QUOTE, "'", line, col))
            i += 1
            col += 1
            continue
        
        if c == '`':
            tokens.append(Token(TokenType.QUASIQUOTE, '`', line, col))
            i += 1
            col += 1
            continue
        
        if c == '~':
            if i + 1 < len(source) and source[i + 1] == '@':
                tokens.append(Token(TokenType.UNQUOTE_SPLICING, '~@', line, col))
                i += 2
                col += 2
                continue
            tokens.append(Token(TokenType.UNQUOTE, '~', line, col))
            i += 1
            col += 1
            continue
        
        if c == '"':
            j = i + 1
            while j < len(source) and source[j] != '"':
                if source[j] == '\\':
                    j += 2
                else:
                    j += 1
            value = source[i+1:j]
            tokens.append(Token(TokenType.STRING, value, line, col))
            col += j - i + 1
            i = j + 1
            continue
        
        j = i
        while j < len(source) and source[j] not in '() \t\r\n;':
            j += 1
        
        text = source[i:j]
        
        if text:
            try:
                tokens.append(Token(TokenType.NUMBER, int(text), line, col))
            except ValueError:
                try:
                    tokens.append(Token(TokenType.FLOAT, float(text), line, col))
                except ValueError:
                    tokens.append(Token(TokenType.SYMBOL, text, line, col))
        
        col += j - i
        i = j
    
    
    tokens.append(Token(TokenType.EOF, None, line, col))
    return tokens