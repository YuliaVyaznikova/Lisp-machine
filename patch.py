import json
from pathlib import Path

def fix_json():
    json_path = Path("tests/expected_results.json")
    with open(json_path, "r", encoding="utf-8") as f:
        data = json.load(f)

    # Используем обычные переносы строк \n, json.dump сам сделает всё красиво
    correct_output = (
        '"=== when macro ==="\n'
        '"true branch"\n'
        'nil\n'
        '"=== unless macro ==="\n'
        '"condition was false"\n'
        'nil\n'
        '"=== cond macro ==="\n'
        '"one equals one"\n'
        '"fallback"\n'
        '"=== and macro ==="\n'
        'true\n'
        'false\n'
        'true\n'
        'true\n'
        '"=== or macro ==="\n'
        'true\n'
        'true\n'
        'true\n'
        'true\n'
        '"=== let macro ==="\n'
        '15\n'
        '40\n'
        '"=== swap macro ==="\n'
        '(2 1)\n'
        '"=== inc macro ==="\n'
        '6\n'
        '12\n'
        '"=== dec macro ==="\n'
        '4\n'
        '8\n'
        '"=== assert macro ==="\n'
        '("PASS:" . "1 equals 1")\n'
        '("PASS:" . "1 is less than 2")\n'
        '"=== not-nil? macro ==="\n'
        'true\n'
        'false\n'
    )

    for test in data["tests"]:
        if test["file"] == "examples/macros.lisp":
            test["expected_output"] = correct_output
            break

    with open(json_path, "w", encoding="utf-8") as f:
        json.dump(data, f, indent=2)
        
    print("[-] tests/expected_results.json исправлен (убрано лишнее экранирование).")

if __name__ == "__main__":
    fix_json()