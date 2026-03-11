import sys
import argparse
from pathlib import Path

sys.path.insert(0, str(Path(__file__).parent))

from parser import parse
from generator import CodeGenerator
from analyzer import SemanticAnalyzer

def compile_file(input_path: str, output_path: str):
    with open(input_path, 'r', encoding='utf-8') as f:
        source = f.read()
    
    ast = parse(source)
    
    analyzer = SemanticAnalyzer()
    if not analyzer.analyze(ast):
        for error in analyzer.errors:
            print(f"Error: {error}", file=sys.stderr)
        sys.exit(1)
    for warning in analyzer.warnings:
        print(f"Warning: {warning}", file=sys.stderr)
    
    generator = CodeGenerator()
    c_code = generator.generate(ast)
    
    with open(output_path, 'w', encoding='utf-8') as f:
        f.write(c_code)

def main():
    parser = argparse.ArgumentParser(description='Lisp to C compiler')
    parser.add_argument('input', help='Input Lisp file')
    parser.add_argument('-o', '--output', default='output.c', help='Output C file')
    
    args = parser.parse_args()
    compile_file(args.input, args.output)

if __name__ == '__main__':
    main()