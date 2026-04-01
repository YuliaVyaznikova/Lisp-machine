import sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).parent))

from src.parser import parse
from src.parser.ast_nodes import DefineNode
from src.generator import CodeGenerator

with open('examples/rest_param.lisp') as f:
    code = f.read()

ast = parse(source=code)
gen = CodeGenerator()

# Step by step
print('=== After _collect_defines ===')
gen._collect_defines(ast)
print('func_names:', gen.func_names)
print('variadic_funcs:', gen.variadic_funcs)
print('functions:')
for i, f in enumerate(gen.functions):
    print(f'--- Function {i} ---')
    print(f)

print('\n=== Full generate ===')
gen2 = CodeGenerator()
result = gen2.generate(ast)
print('func_names:', gen2.func_names)
print('variadic_funcs:', gen2.variadic_funcs)
print('functions count:', len(gen2.functions))
