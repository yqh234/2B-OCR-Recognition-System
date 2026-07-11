$ErrorActionPreference = "Stop"
$env:PATH = "C:\msys64\mingw64\bin;$env:PATH"
$env:TESSDATA_PREFIX = "C:\msys64\mingw64\share\tessdata"

$exe = Get-ChildItem -LiteralPath $PSScriptRoot -Recurse -Filter "ocr_cli.exe" |
    Select-Object -First 1
$sample = Get-ChildItem -LiteralPath $PSScriptRoot -Recurse -Filter "01_chinese_simple.png" |
    Select-Object -First 1

if ($null -eq $exe -or $null -eq $sample) {
    throw "The OCR executable or test images could not be found."
}

$work = "C:\vision_practice_runtime\2B\cli_test_work"
$input = Join-Path $work "input"
$output = Join-Path $work "output"
$csv = Join-Path $work "ocr_results.csv"

Remove-Item -LiteralPath $work -Recurse -Force -ErrorAction SilentlyContinue
New-Item -ItemType Directory -Force -Path $input, $output | Out-Null
Get-ChildItem -LiteralPath $sample.DirectoryName -File -Filter "*.png" |
    Copy-Item -Destination $input

Write-Host "Running OCR CLI test..."
& $exe.FullName $input $output $csv
if ($LASTEXITCODE -ne 0) {
    exit $LASTEXITCODE
}

Write-Host ""
Write-Host "Results saved to: $work"
