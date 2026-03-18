if (Test-Path tests/test_results.txt) { Remove-Item tests/test_results.txt }

docker build -t lisp-machine-stage1 .
docker run --rm -v ${PWD}/tests:/app/tests -v ${PWD}/output:/app/output lisp-machine-stage1

Write-Host ""
Write-Host "Test results saved to: tests/test_results.txt"
Write-Host "Compiled outputs saved to: output/"