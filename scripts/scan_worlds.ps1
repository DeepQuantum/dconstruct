$py = "C:\Users\damix\Documents\GitHub\TLOU2Modding\.venv\Scripts\python.exe"
if (-not (Test-Path $py)) { $py = "python" }
$m = "C:\Program Files (x86)\Steam\steamapps\common\The Last of Us Part II\build\pc\main"
$tmp = "C:\Users\damix\Documents\GitHub\TLOU2Modding\tlou2_pc\_scan_tmp"
$names = "C:\Users\damix\Documents\GitHub\TLOU2Modding\dconstruct\scripts\anim_names.txt"
$scanner = "C:\Users\damix\Documents\GitHub\TLOU2Modding\dconstruct\scripts\pak_sid_scan.py"
$results = "C:\Users\damix\Documents\GitHub\TLOU2Modding\dconstruct\scripts\world_scan_results.txt"
"scan started $(Get-Date)" | Set-Content $results

$worlds = @("world-rogue") + (Get-ChildItem $m -Filter "world-*.psarc" | Where-Object { $_.BaseName -notin "world-rogue","world-watchtower","world-patrol" } | ForEach-Object BaseName)
foreach ($w in $worlds) {
    if (Test-Path $tmp) { Remove-Item $tmp -Recurse -Force }
    & ndarc -e "$m\$w.psarc" -o $tmp 2>&1 | Out-Null
    $hits = & $py $scanner $names $tmp 2>&1 | Select-String -NotMatch "^sid" | ForEach-Object { $_.Line }
    if ($hits) {
        Add-Content $results "=== $w ==="
        $hits | Add-Content $results
    } else {
        Add-Content $results "$w : no hits"
    }
}
if (Test-Path $tmp) { Remove-Item $tmp -Recurse -Force }
Add-Content $results "scan finished $(Get-Date)"
