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
    
    def test_tokenize_negative_numbers(self):
        tokens = tokenize("-5 -3.14")
        assert tokens[0].type == TokenType.NUMBER
        assert tokens[0].value == -5
        assert tokens[1].type == TokenType.FLOAT
        assert tokens[1].value == -3.14
    
    def test_tokenize_escape_sequences(self):
        tokens = tokenize('"hello\\nworld"')
        assert tokens[0].type == TokenType.STRING
        assert tokens[0].value == "hello\nworld"
        
        tokens2 = tokenize('"tab\\there"')
        assert tokens2[0].value == "tab\there"
        
        tokens3 = tokenize('"quote\\""')
        assert tokens3[0].value == 'quote"'
    
    def test_tokenize_unquote_comma(self):
        tokens = tokenize("`(if ,x ,y nil)")
        assert tokens[0].type == TokenType.QUASIQUOTE
        assert tokens[1].type == TokenType.LPAREN
        assert tokens[2].type == TokenType.SYMBOL
        assert tokens[2].value == "if"
        assert tokens[3].type == TokenType.UNQUOTE
        assert tokens[3].value == ","
        assert tokens[4].type == TokenType.SYMBOL
        assert tokens[4].value == "x"
        assert tokens[5].type == TokenType.UNQUOTE
        assert tokens[5].value == ","
    
    def test_tokenize_unquote_tilde(self):
        tokens = tokenize("`(if ~x ~y nil)")
        assert tokens[0].type == TokenType.QUASIQUOTE
        assert tokens[3].type == TokenType.UNQUOTE
        assert tokens[3].value == "~"

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

class TestExamples:
    def test_hello_example(self):
        from generator import CodeGenerator
        with open('examples/hello.lisp') as f:
            source = f.read()
        ast = parse(source)
        gen = CodeGenerator()
        code = gen.generate(ast)
        assert 'lisp_print' in code
        assert 'lisp_make_string' in code
    
    def test_arithmetic_example(self):
        from generator import CodeGenerator
        with open('examples/arithmetic.lisp') as f:
            source = f.read()
        ast = parse(source)
        gen = CodeGenerator()
        code = gen.generate(ast)
        assert 'lisp_add' in code
        assert 'lisp_mul' in code
    
    def test_comparisons_example(self):
        from generator import CodeGenerator
        with open('examples/comparisons.lisp') as f:
            source = f.read()
        ast = parse(source)
        gen = CodeGenerator()
        code = gen.generate(ast)
        assert 'lisp_eq' in code
        assert 'lisp_lt' in code
        assert 'lisp_gt' in code
    
    def test_conditionals_example(self):
        from generator import CodeGenerator
        with open('examples/conditionals.lisp') as f:
            source = f.read()
        ast = parse(source)
        gen = CodeGenerator()
        code = gen.generate(ast)
        assert 'lisp_is_true' in code
        assert 'if' in code
    
    def test_lists_example(self):
        from generator import CodeGenerator
        with open('examples/lists.lisp') as f:
            source = f.read()
        ast = parse(source)
        assert len(ast) > 0
    
    def test_quotes_example(self):
        from generator import CodeGenerator
        with open('examples/quotes.lisp') as f:
            source = f.read()
        ast = parse(source)
        gen = CodeGenerator()
        code = gen.generate(ast)
        assert 'lisp_cons' in code or 'lisp_make_symbol' in code

if __name__ == "__main__":
    import pytest
    pytest.main([__file__, "-v"])