# to-resolve.md (tiled)

## Port to kazilib, Linux, web, kazin.dev (2026-11-06)

### Manual checks

- **Play in a real browser** (why: the tests use headless Edge on SwiftShader). Steps: open
  https://kazin.dev/projects/tiled/ in two tabs or two PCs, Create Account + Login in each,
  left click tiles to walk, right click to turn; each sees the other move.
- **Hold-to-walk feel** (why: input feel). Steps: native client and browser, hold the left
  button and sweep: the player follows the cursor tile by tile.

### Open questions

- **Native clients from the internet**: the box's ufw lets only ssh / http(s) / mail in, so
  ENet's UDP 7031 is not reachable from outside (tictactoe 6987 / tileship 7021 the same).
  Browsers work (through Caddy). Open `7031/udp` in ufw if native clients should play live?
- **Original client quirks kept** (behaviour unchanged): the client never reads a tile's
  type (blocked tiles are not drawn; only the map's edge band is red and unwalkable); a
  walking unit's previous tile can stay green on the other players' maps; spawns are
  `80 + n` on one row, so a freed slot can put two players on one tile; a second login of
  the same account is not refused (the `#todo`). Fix any of them?
- **Wire changed** from the 2021 build (kazilib's packet layer; `unit::Action` now u8;
  `UnitOp::Remove` appended): old 2021 clients cannot join (none deployed).
