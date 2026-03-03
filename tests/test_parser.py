import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).parent.parent / 'src'))

from parser import parse
from parser.tokenizer import tokenize, TokenType
from parser.ast_nodes import *

class TestTokenizer:
    def test_tokenize_numbers(self):
        tokens = tokenize("42 3.14")
        assert tokens[0].type == TokenType.NUMBER
        assert tokens[0].value == 42
        assert tokens[1].type == TokenType.FLOAT
        assert tokens[1].value == 3.14
    
    def test_tokenize_symbols(self):
        tokens = tokenize("foo bar-baz +")
        assert tokens[0].type == TokenType.SYMBOL
        assert tokens[0].value == "foo"
        assert tokens[1].value == "bar-baz"
        assert tokens[2].value == "+"
    
    def test_tokenize_strings(self):
        tokens = tokenize('"hello world"')
        assert tokens[0].type == TokenType.STRING
        assert tokens[0].value == "hello world"
    
    def test_tokenize_parens(self):
        tokens = tokenize("(+ 1 2)")
        assert tokens[0].type == TokenType.LPAREN
        assert tokens[4].type == TokenType.RPAREN

class TestParser:
    def test_parse_int(self):
        ast = parse("42")
        assert len(ast) == 1
        assert isinstance(ast[0], IntNode)
        assert ast[0].value == 42
    
    def test_parse_symbol(self):
        ast = parse("foo")
        assert len(ast) == 1
        assert isinstance(ast[0], SymbolNode)
        assert ast[0].name == "foo"
    
    def test_parse_list(self):
        ast = parse("(+ 1 2)")
        assert len(ast) == 1
        assert isinstance(ast[0], ListNode)
        assert len(ast[0].elements) == 3
    
    def test_parse_if(self):
        ast = parse("(if true 1 2)")
        assert len(ast) == 1
        assert isinstance(ast[0], IfNode)
    
    def test_parse_nil(self):
        ast = parse("nil")
        assert len(ast) == 1
        assert isinstance(ast[0], NilNode)
    
    def test_parse_quote(self):
        ast = parse("'foo")
        assert len(ast) == 1
        assert isinstance(ast[0], QuoteNode)

class TestGenerator:
    def test_generate_int(self):
        from generator import CodeGenerator
        ast = parse("42")
        gen = CodeGenerator()
        code = gen.generate(ast)
        assert "lisp_make_int(42)" in code
    
    def test_generate_arithmetic(self):
        from generator import CodeGenerator
        ast = parse("(+ 1 2)")
        gen = CodeGenerator()
        code = gen.generate(ast)
        assert "lisp_add" in code
    
    def test_generate_if(self):
        from generator import CodeGenerator
        ast = parse("(if true 1 2)")
        gen = CodeGenerator()
        code = gen.generate(ast)
        assert "lisp_is_true" in code

if __name__ == "__main__":
    import pytest
    pytest.main([__file__, "-v"])