import sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).parent))

from src.parser import parse
from src.parser.ast_nodes import DefineNode
from src.generator import CodeGenerator

with open('examples/rest_param.lisp') as f:
    code = f.read()

ast = parse(source=code)

print('AST nodes:')
for i, node in enumerate(ast):
    if isinstance(node, DefineNode):
        print(f'{i}: DefineNode name={node.name} params={node.params} rest_param={node.rest_param}')
    else:
        print(f'{i}: {type(node).__name__}')

gen = CodeGenerator()
gen._collect_defines(ast)
print('\nfunc_names:', gen.func_names)
print('variadic_funcs:', gen.variadic_funcs)
print('functions count:', len(gen.functions))
