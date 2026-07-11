$ErrorActionPreference = "Stop"
$env:PATH = "C:\msys64\mingw64\bin;$env:PATH"
$env:TESSDATA_PREFIX = "C:\msys64\mingw64\share\tessdata"

$exe = Get-ChildItem -LiteralPath $PSScriptRoot -Recurse -Filter "comprehensive_demo.exe" |
    Select-Object -First 1
$sample = Get-ChildItem -LiteralPath $PSScriptRoot -Recurse -Filter "01_qr_id_card_like.png" |
    Select-Object -First 1
$converter = Get-ChildItem -LiteralPath $PSScriptRoot -Recurse -Filter "export_csv_to_xlsx.py" |
    Select-Object -First 1
$python = "C:\Users\86136\Anaconda3\python.exe"

if ($null -eq $exe -or $null -eq $sample -or $null -eq $converter) {
    throw "The comprehensive demo executable, samples, or Excel converter could not be found."
}
if (-not (Test-Path -LiteralPath $python)) {
    throw "The Python runtime for Excel export could not be found."
}

$work = "C:\vision_practice_runtime\2B\comprehensive_demo_work"
$input = Join-Path $work "input"
$output = Join-Path $work "output"
$result = Join-Path $work "result"
$csv = Join-Path $result "comprehensive_results.csv"
$xlsx = Join-Path $result "comprehensive_results.xlsx"

Remove-Item -LiteralPath $work -Recurse -Force -ErrorAction SilentlyContinue
New-Item -ItemType Directory -Force -Path $input, $output, $result | Out-Null
Get-ChildItem -LiteralPath $sample.DirectoryName -File -Filter "*.png" |
    Copy-Item -Destination $input

Write-Host "Running barcode and OCR comprehensive demo..."
& $exe.FullName $input $output $csv
if ($LASTEXITCODE -ne 0) {
    exit $LASTEXITCODE
}

& $python $converter.FullName $csv $xlsx
if ($LASTEXITCODE -ne 0) {
    exit $LASTEXITCODE
}

Write-Host ""
Write-Host "Results saved to: $work"
