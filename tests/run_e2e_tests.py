import json
import subprocess
import sys
from pathlib import Path


def run_test(test: dict, project_root: Path) -> dict:
    lisp_file = project_root / test["file"]
    expected = test["expected_output"]
    
    stem = lisp_file.stem
    output_dir = project_root / "output" / stem
    output_dir.mkdir(parents=True, exist_ok=True)
    
    c_file = output_dir / f"{stem}.c"
    import os
    binary = output_dir / (f"{stem}.exe" if os.name == "nt" else stem)
    output_file = output_dir / f"{stem}.output"
    
    result = {
        "file": test["file"],
        "status": "pass",
        "message": ""
    }
    
    try:
        subprocess.run(
            ["python3", "src/main.py", str(lisp_file), "-o", str(c_file)],
            cwd=project_root,
            capture_output=True,
            text=True,
            check=True
        )
    except subprocess.CalledProcessError as e:
        result["status"] = "fail"
        result["message"] = f"Compilation failed: {e.stderr}"
        return result
    
    try:
        subprocess.run(
            ["gcc", str(c_file), "build/lisp.o", "-I.", "-o", str(binary)],
            cwd=project_root,
            capture_output=True,
            text=True,
            check=True
        )
    except subprocess.CalledProcessError as e:
        result["status"] = "fail"
        result["message"] = f"C compilation failed: {e.stderr}"
        return result
    
    try:
        proc = subprocess.run(
            [str(binary)],
            cwd=project_root,
            capture_output=True,
            text=True,
            timeout=10
        )
        actual_output = proc.stdout
    except subprocess.TimeoutExpired:
        result["status"] = "fail"
        result["message"] = "Timeout (10s)"
        return result
    except Exception as e:
        result["status"] = "fail"
        result["message"] = f"Execution failed: {e}"
        return result
    
    output_file.write_text(actual_output)
    
    if actual_output == expected:
        result["message"] = "OK"
    else:
        result["status"] = "fail"
        result["message"] = f"Output mismatch:\n  Expected: {repr(expected)}\n  Actual:   {repr(actual_output)}"
    
    return result


def main():
    project_root = Path(__file__).parent.parent
    
    output_dir = project_root / "output"
    output_dir.mkdir(exist_ok=True)
    
    manifest_path = project_root / "tests" / "expected_results.json"
    with open(manifest_path) as f:
        manifest = json.load(f)
    
    results = []
    passed = 0
    failed = 0
    
    for test in manifest["tests"]:
        result = run_test(test, project_root)
        results.append(result)
        
        if result["status"] == "pass":
            passed += 1
            print(f"  PASS: {test['file']}")
        else:
            failed += 1
            print(f"  FAIL: {test['file']}")
            print(f"        {result['message']}")
    
    print("=" * 60)
    print(f"Results: {passed} passed, {failed} failed")
    print("=" * 60)
    
    results_file = output_dir / "test_results.json"
    with open(results_file, "w") as f:
        json.dump({"results": results, "passed": passed, "failed": failed}, f, indent=2)
    
    sys.exit(0 if failed == 0 else 1)


if __name__ == "__main__":
    main()