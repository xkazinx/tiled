# The web client end to end: two headless browser tabs (kazilib web\tools\browser.js:
# Edge, WebGL2 on SwiftShader) each make an account and log in through the ImGui
# window, see each other, and one walks (the other sees it move). Locally it starts
# the Windows server (its WebSocket gateway serving deploy\server\web\, build_web.ps1)
# on ports 7035 / 7034 and a native client first, so the browsers and the native
# client share one game. The account names get a random number per run. Screenshots
# in testerino\web\. Exit code = failed steps.
#   powershell -ExecutionPolicy Bypass -File src\tests\web\run_web_test.ps1 [-Url https://kazin.dev/projects/tiled/]
# -Url  a deployed page instead (no server or native client started)
param([string]$Url = '')
$ErrorActionPreference = 'Stop'
$root = Resolve-Path (Join-Path $PSScriptRoot '..\..\..')
$shots = Join-Path $root 'testerino\web'
New-Item -ItemType Directory -Force $shots | Out-Null
$run = Get-Random -Minimum 1000 -Maximum 9999

$server = $null
$native = $null
if (-not $Url) {
	$deploy = Join-Path $root 'deploy'
	if (-not (Test-Path (Join-Path $deploy 'server\web\index.wasm'))) { throw 'no deploy\server\web\index.wasm: run build_web.ps1' }
	$server = Start-Process -FilePath (Join-Path $deploy 'server\server.exe') -ArgumentList 'port=7035', 'ws=7034' `
		-WorkingDirectory (Join-Path $deploy 'server') -WindowStyle Hidden -PassThru
	Start-Sleep -Milliseconds 800
	$nativeLog = Join-Path $shots 'native.txt'
	$native = Start-Process -FilePath (Join-Path $deploy 'client\client.exe') `
		-ArgumentList 'port=7035', 'create', 'autologin', "user=nat$run", "pass=pass$run" `
		-WorkingDirectory (Join-Path $deploy 'client') -PassThru -RedirectStandardOutput $nativeLog
	$until = (Get-Date).AddSeconds(15)
	while ((Get-Date) -lt $until -and -not (Select-String -Path $nativeLog -Pattern 'Entered the game' -Quiet -ErrorAction SilentlyContinue)) { Start-Sleep -Milliseconds 200 }
	$Url = 'http://127.0.0.1:7034/index.html'
}
# debug: the per-packet traces the steps wait for
$Url += if ($Url.Contains('?')) { '&debug' } else { '?debug' }

# Screenshot names in the steps land in testerino\web\; {RUN} is this run's number
$steps = Join-Path $shots 'two_players.json'
(Get-Content (Join-Path $PSScriptRoot 'two_players.json') -Raw) -replace '"shot": "', ('"shot": "' + ($shots -replace '\\', '/') + '/') -replace '\{RUN\}', $run |
	Set-Content -Encoding utf8 $steps
if ($native) {
	# The browsers also see the native client
	$list = Get-Content $steps -Raw | ConvertFrom-Json
	$extra = [pscustomobject]@{ page = 0; wait = "Added new unit\(nat$run\)"; timeout = 10; since = 'start' }
	$list = @($list[0..11]) + $extra + @($list[12..($list.Count - 1)])
	$list | ConvertTo-Json -Depth 5 | Set-Content -Encoding utf8 $steps
}

try {
	node (Join-Path $root 'src\dep\kazilib\web\tools\browser.js') --url $Url --pages 2 --steps $steps
	$failed = $LASTEXITCODE
	if ($native) {
		# The native client saw both browsers come in
		foreach ($name in "weba$run", "webb$run") {
			if (Select-String -Path $nativeLog -Pattern "Added new unit\($name\)" -Quiet) { Write-Host "[ok] native client sees $name" }
			else { Write-Host "[FAILED] native client does not see $name"; $failed++ }
		}
	}
} finally {
	if ($native) { Stop-Process -Id $native.Id -Force -ErrorAction SilentlyContinue }
	if ($server) { Stop-Process -Id $server.Id -Force -ErrorAction SilentlyContinue }
}
Write-Host ($(if ($failed) { "$failed failed" } else { 'web test passed' }))
exit $failed
