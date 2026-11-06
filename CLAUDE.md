# CLAUDE.md (tiled)

- **Public repo:** never commit secrets. The database password lives only in `init.json` beside the server exe (git-ignored; shape `src\server\init.json.example`), on the box `/opt/tiled/bin/init.json` (600, user `tiled`).
- **Build:** `powershell -ExecutionPolicy Bypass -File build.ps1` (`setup.ps1`: kazilib submodule, SFML 3.1, MySQL Connector/C 6.1 x86; then kazilib FASTBuild on `src\fastbuild.json`: kazilib, server, client). `build.ps1 server|client`, `-Regen`, `-Clean`, `-Local`; Debug only if asked. Output `deploy\server\`, `deploy\client\`. `setup.ps1`'s `submodule update` resets kazilib to the recorded pointer: stage a moved pointer first (`git add src/dep/kazilib`).
- **Web:** `build_web.ps1` (Emscripten, kazilib `web\`: `kazilib_web_target`, `kazilib_web_imgui`) → `deploy\server\web\`; `src\client\CMakeLists.txt` is the web build only. Browser-only code `#ifdef __EMSCRIPTEN__` (window, WebSocket URL), Windows-only `#ifdef _WIN32`.
- **Linux / deploy:** `build_linux.ps1` (Docker `tiled-linux-build:26.04` = kazilib's image + `libmysqlclient-dev`; `src\server\CMakeLists.txt`); `deploy_ubuntu.ps1` → box `kazin-music`: service `tiled`, `/opt/tiled`, ENet UDP 7031, gateway 127.0.0.1:7030, Caddy `kazin.dev/projects/tiled` (`deploy\ubuntu\`), MySQL database / user `tiled` (made once, kept).
- **Kazilib first:** packets (`OPkt` / `IPkt`, `BroadcasterBase`), sockets, gateway, scenes, ImGui (imgui-SFML), log (`Out` / `Info` / `Warn` / `Err`; per-packet traces `DebugLog`, flag `debug`), startup flags; reusable code goes there.
- **Protocol:** `src\common\netops.h`, `<NetOp><Op><fields>`, values only appended; writer and reader change together (`unit::Base` is written whole).
- **SQL:** values only through `Sql::Quote`; schema `src\server\sql\schema.sql` (idempotent).
- **Tests:** `src\tests\web\run_web_test.ps1` (local: server + native client + two browser tabs; `-Url https://kazin.dev/projects/tiled/` live), 0 failed, outside the command sandbox.
- **README:** the 2021 original stays at its end (`## Original README`).
- **Names:** members `_name`, functions / types `PascalCase`, `struct` (rules: `D:\code\CLAUDE.md`, `D:\code\hb\hbmillennium\CLAUDE.md`).
