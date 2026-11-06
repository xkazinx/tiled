# The libraries the client and server build against, x86 (Win32):
#   kazilib  src\dep\kazilib  (submodule: enet, the packet layer, std names, startup
#            flags, imgui; private: building needs access to github.com/xkazinx/kazilib)
#   SFML     src\dep\SFML-3.1.0  (a junction to D:\code\kazilib\SFML-3.1.0 when that
#            shared SDK is there; else the official vc17 32-bit release, downloaded)
#   MySQL    src\dep\mysql-connector-c-6.1.11-win32  (the server's client library,
#            32-bit, downloaded from MySQL's archive)
# Every build uses the 64-bit-hosted tools (host=x64) and the v145 toolset.
$ErrorActionPreference = 'Stop'
$root = $PSScriptRoot
$dep = Join-Path $root 'src\dep'

git -C $root submodule update --init src/dep/kazilib
if ($LASTEXITCODE) { throw 'submodule update failed' }

# SFML 3.1.0
$sfml = Join-Path $dep 'SFML-3.1.0'
if (-not (Test-Path (Join-Path $sfml 'include\SFML\Graphics.hpp'))) {
    $shared = 'D:\code\kazilib\SFML-3.1.0'
    if (Test-Path (Join-Path $shared 'include\SFML\Graphics.hpp')) {
        New-Item -ItemType Junction -Path $sfml -Target $shared | Out-Null
        Write-Host "SFML: linked to $shared"
    } else {
        $zip = Join-Path $env:TEMP 'SFML-3.1.0-windows-vc17-32-bit.zip'
        Write-Host 'SFML: downloading 3.1.0 (vc17 32-bit)'
        Invoke-WebRequest 'https://github.com/SFML/SFML/releases/download/3.1.0/SFML-3.1.0-windows-vc17-32-bit.zip' -OutFile $zip
        Expand-Archive $zip -DestinationPath $dep -Force
        Remove-Item $zip
    }
}

# MySQL Connector/C 6.1.11, 32-bit (the original README's connector; libmysql.dll)
$mysql = Join-Path $dep 'mysql-connector-c-6.1.11-win32'
if (-not (Test-Path (Join-Path $mysql 'lib\libmysql.lib'))) {
    $zip = Join-Path $env:TEMP 'mysql-connector-c-6.1.11-win32.zip'
    Write-Host 'MySQL: downloading Connector/C 6.1.11 (32-bit)'
    Invoke-WebRequest 'https://cdn.mysql.com/archives/mysql-connector-c/mysql-connector-c-6.1.11-win32.zip' -OutFile $zip -UserAgent 'Mozilla/5.0'
    Expand-Archive $zip -DestinationPath $dep -Force
    Remove-Item $zip
}

Write-Host 'Dependencies ready'
