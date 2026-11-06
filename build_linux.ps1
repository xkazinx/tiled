# The server for Linux (the deployed one), built here in a Docker container of the
# server's own system (Ubuntu 26.04: its glibc and libmysqlclient), from
# src\server\CMakeLists.txt. Output src\build\linux\tiled_server (stripped; libstdc++
# inside, libmysqlclient.so.24 from the system).
#   powershell -ExecutionPolicy Bypass -File build_linux.ps1
$ErrorActionPreference = 'Stop'
$root = $PSScriptRoot
$base = 'kazilib-linux-build:26.04'
$image = 'tiled-linux-build:26.04'

if (-not (docker images -q $image)) {
	if (-not (docker images -q $base)) {
		# kazilib's compiler image (Ubuntu 26.04, g++, cmake, ninja), made once
		"FROM ubuntu:26.04`nRUN apt-get update -qq && DEBIAN_FRONTEND=noninteractive apt-get install -y -qq g++ cmake ninja-build >/dev/null && rm -rf /var/lib/apt/lists/*`n" |
			docker build -q -t $base - | Out-Null
		if ($LASTEXITCODE) { throw "docker build of $base failed" }
	}
	# + MySQL's client library, made once
	"FROM $base`nRUN apt-get update -qq && DEBIAN_FRONTEND=noninteractive apt-get install -y -qq libmysqlclient-dev >/dev/null && rm -rf /var/lib/apt/lists/*`n" |
		docker build -q -t $image - | Out-Null
	if ($LASTEXITCODE) { throw "docker build of $image failed" }
}

# Other builds share this PC: 8 jobs at most
$jobs = [Math]::Min(8, [Math]::Max(1, [Environment]::ProcessorCount - 1))
$src = ($root -replace '\\', '/')
docker run --rm -v "${src}:/tiled" $image bash -c "set -e; cmake -S /tiled/src/server -B /tmp/b -G Ninja -DCMAKE_BUILD_TYPE=Release >/dev/null; cmake --build /tmp/b -j $jobs; strip /tmp/b/tiled_server; mkdir -p /tiled/src/build/linux; cp /tmp/b/tiled_server /tiled/src/build/linux/"
if ($LASTEXITCODE) { throw 'Linux server build failed' }
Write-Host "Linux server: $(Join-Path $root 'src\build\linux\tiled_server')"
