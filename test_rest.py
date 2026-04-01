import sys
sys.path.insert(0, 'src')
from parser.parser import parse

code = '(define (sum-all &rest nums) 0)'
ast = parse(code)
node = ast[0]
print("name:", node.name)
print("params:", node.params)
print("rest_param:", node.rest_param)
