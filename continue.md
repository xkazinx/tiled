# continue.md (tiled, master)

## State

- **kazilib** `src/dep/kazilib` at `a28bf1b` (ImGui in a browser; web builds).
- **tiled** on kazilib, SFML 3.1, FASTBuild (Release x86, v145, 64-bit-hosted tools); the
  server also on Linux (Docker build), the client also in a browser (Emscripten).
- **Live:** https://kazin.dev/projects/tiled/ - box `kazin-music`, service `tiled`
  (`/opt/tiled`), ENet UDP 7031, WebSocket gateway 127.0.0.1:7030 behind Caddy, MySQL 8.4
  database `tiled` (user `tiled@localhost`, password only in `/opt/tiled/bin/init.json`).

## Last work (2026-11-06)

- Port off the old kazilib (`kazin::`, LIBPATH, SFML 2.5, ENet 1.3.17): kazilib's
  `OPkt` / `IPkt` / `SocketHandler` / `BroadcasterBase` (wire `<NetOp><Op><fields>`),
  `lib::scene::Manager`, imgui-SFML, `initflags.h`, `log.h`; own `common::Point<T>`.
- Server: database settings from `init.json`; values escaped (`Sql::Quote`); salted
  SHA-256 passwords; connections closed; a disconnect frees the player's slot and tells
  the others (`UnitOp::Remove`, appended); `debug` flag for the per-packet traces.
- Client: the login's answer read (a refused login stays on the window with a message);
  `create` / `autologin` / `user=` / `pass=` / `walk=` flags; a click between two frames
  counts (browser).

## Tested

- Native (Windows): server + two clients, accounts made, login, both see each other, one
  walks (`walk=`), the other sees it; a closed client is removed from the other's map.
- Linux server in Docker against `mysql:8.4`: two native clients, same checks.
- `src\tests\web\run_web_test.ps1` local (server, a native client and two browser tabs in
  one game): passed. `-Url https://kazin.dev/projects/tiled/`: passed.

## Known gaps / next

- See `to-resolve.md` (native clients from the internet, original client quirks).
- No Testerino bridge yet (tictactoe has one); the web test drives the canvas.
