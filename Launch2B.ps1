$env:PATH = "C:\msys64\mingw64\bin;$env:PATH"
$env:TESSDATA_PREFIX = "C:\msys64\mingw64\share\tessdata"

$exe = Get-ChildItem -LiteralPath $PSScriptRoot -Recurse -Filter "ocr_qt.exe" |
    Select-Object -First 1

if ($null -eq $exe) {
    Add-Type -AssemblyName PresentationFramework
    [System.Windows.MessageBox]::Show(
        "ocr_qt.exe was not found under:`n$PSScriptRoot",
        "2B Launcher",
        "OK",
        "Error"
    ) | Out-Null
    exit 1
}

Start-Process -FilePath $exe.FullName -WorkingDirectory $exe.DirectoryName
