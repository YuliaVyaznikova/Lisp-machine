from pathlib import Path

file_path = Path("tests/run_e2e_tests.py")
code = file_path.read_text(encoding="utf-8")

old_line = 'binary = output_dir / stem'
new_line = 'import os\n    binary = output_dir / (f"{stem}.exe" if os.name == "nt" else stem)'

if old_line in code:
    file_path.write_text(code.replace(old_line, new_line), encoding="utf-8")
    print("Успешно: скрипт тестов адаптирован под Windows!")
else:
    print("Скрипт уже пропатчен или строка не найдена.")