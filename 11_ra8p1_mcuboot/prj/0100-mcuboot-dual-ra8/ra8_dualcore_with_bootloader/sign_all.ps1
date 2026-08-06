$root   = "E:\RS_workspace\0100-mcuboot-dual-ra8\ra8_dualcore_with_bootloader"
$bl     = "$root\ra8p1_bootloader"
$objcpy = "D:\Program Files\LLVM\ATfE-21.1.1-Windows-x86_64\bin\llvm-objcopy.exe"
$imgtool = "$bl\ra\mcu-tools\MCUboot\scripts\imgtool.py"
$key    = "$bl\ra\mcu-tools\MCUboot\root-ec-p256.pem"
$ver    = "1.0.0"

$projects = @("ra8p1_primary_cpu0", "ra8p1_primary_cpu1", "ra8p1_secondary_cpu0", "ra8p1_secondary_cpu1")

foreach ($proj in $projects) {
    $d = "$root\$proj\Debug"
    $elf = "$d\$proj.elf"
    if (-not (Test-Path $elf)) { Write-Host "SKIP $proj (no .elf)"; continue }

    Write-Host "=== $proj ==="

    $bin = "$d\$proj.bin"
    & $objcpy -O binary $elf $bin
    if (-not $?) { Write-Host "objcopy FAILED"; continue }

    $signed = "$d\${proj}.bin.signed"
    python $imgtool sign --key $key --header-size 0x200 --align 8 --slot-size 0x30000 --pad-header --version $ver $bin $signed
    if (-not $?) { Write-Host "imgtool FAILED"; continue }

    Write-Host "  -> $signed"
}

Write-Host "`nDone."
