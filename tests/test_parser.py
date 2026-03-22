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

class TestMacros:
    def test_parse_defmacro_simple(self):
        ast = parse('(defmacro inc (n) `(+ ,n 1))')
        assert len(ast) == 1
        node = ast[0]
        assert isinstance(node, DefmacroNode)
        assert node.name == 'inc'
        assert node.params == ['n']
    
    def test_parse_defmacro_multi_params(self):
        ast = parse('(defmacro when (condition body) `(if ,condition ,body nil))')
        assert len(ast) == 1
        node = ast[0]
        assert node.name == 'when'
        assert node.params == ['condition', 'body']
    
    def test_macro_expansion_when(self):
        from macro.expander import expand_macros
        source = '''
        (defmacro when (condition body) `(if ,condition ,body nil))
        (when (= 1 1) "yes")
        '''
        ast = parse(source)
        expanded = expand_macros(ast)
        assert len(expanded) == 1
        call = expanded[0]
        assert isinstance(call, IfNode) or (isinstance(call, ListNode) and 
                                            isinstance(call.elements[0], SymbolNode) and 
                                            call.elements[0].name == 'if')
    
    def test_macro_expansion_and(self):
        from macro.expander import expand_macros
        source = '''
        (defmacro and (a b) `(if ,a ,b nil))
        (and true false)
        '''
        ast = parse(source)
        expanded = expand_macros(ast)
        assert len(expanded) == 1
    
    def test_macro_expansion_or(self):
        from macro.expander import expand_macros
        source = '''
        (defmacro or (a b) `(if ,a true ,b))
        (or false true)
        '''
        ast = parse(source)
        expanded = expand_macros(ast)
        assert len(expanded) == 1
    
    def test_macro_expansion_cond(self):
        from macro.expander import expand_macros
        source = '''
        (defmacro cond (p1 p2 p3) 
          `(if ,(first p1) ,(first (rest p1))
               (if ,(first p2) ,(first (rest p2))
                   (if ,(first p3) ,(first (rest p3)) nil))))
        (cond ((= 1 2) "a") ((= 1 1) "b") (true "c"))
        '''
        ast = parse(source)
        expanded = expand_macros(ast)
        assert len(expanded) == 1
    
    def test_nested_macro_calls(self):
        from macro.expander import expand_macros
        source = '''
        (defmacro inc (n) `(+ ,n 1))
        (inc (inc 5))
        '''
        ast = parse(source)
        expanded = expand_macros(ast)
        assert len(expanded) == 1
    
    def test_multiple_macro_calls(self):
        from macro.expander import expand_macros
        source = '''
        (defmacro inc (n) `(+ ,n 1))
        (inc 5)
        (inc 10)
        '''
        ast = parse(source)
        expanded = expand_macros(ast)
        assert len(expanded) == 2

if __name__ == "__main__":
    import pytest
    pytest.main([__file__, "-v"])