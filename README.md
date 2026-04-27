# mt4manager

Starter repository for a Node.js native addon around the MetaTrader 4 Manager API.

## Scope of v0.1.0

This starter is intentionally minimal. It gives you:

- Node-API / `node-addon-api` based native addon
- runtime DLL path loading
- one native client per JS instance
- async `connect`, `login`, `disconnect`, `close`
- per-instance mutex protection
- split TS structure with `services` and `utils`
- placeholders for `users`, `trades`, and `pumping`
- GitHub Actions for CI and publish

## Important

- This package is designed for **Windows-focused MT4 usage**.
- You should review the licensing terms for `mtmanapi.dll` / `mtmanapi64.dll` before redistribution.
- The package is structured so users can pass their own DLL path at runtime.

## Planned JS API

```ts
import { createMT4Manager } from "mt4manager";

const manager = await createMT4Manager({
  dllPath: ".../mtmanapi64.dll",
  server: "your.server:443",
  login: 123456,
  password: "password",
  pump: true,
});

// listen to updates
manager.users.on("update", (user) => {
  console.log("User updated:", user);
});

// perform action
await manager.users.update(123456, {
  name: "New Name",
});

await manager.close();
```

## Repository layout

```txt
include/MT4ManagerAPI.h      Vendor header
native/                      C++ addon implementation
src/manager.ts               Public class wrapper
src/services/                Future domain services
src/utils/                   Shared helpers and types
test/                        Node tests
```

## Install

```bash
npm install
npm run build
```

## First local run

1. Place `MT4ManagerAPI.h` in `include/`.
2. Put your DLL somewhere local, for example `dll/mtmanapi64.dll`.
3. Replace the stubbed MT4 API integration points in `native/mt4_client.cc` with the real calls from your SDK.
4. Build:

```bash
npm run build
```

## First GitHub push

```bash
git init
git add .
git commit -m "chore: initialize mt4manager starter"
git branch -M main
git remote add origin https://github.com/YOUR_USER/mt4manager.git
git push -u origin main
```

## First npm publish

### Manual publish

```bash
npm login
npm pack --dry-run
npm publish --access public
```

### Recommended later

Use GitHub Actions with npm trusted publishing after the package is working end-to-end.

## Suggested next commits

1. Wire the real MT4 API initialization using `MT4ManagerAPI.h`
2. Finish `connect`
3. Finish `login`
4. Add lifecycle tests
5. Add `users.get`
6. Add `trades.get`
7. Add pumping bridge with `ThreadSafeFunction`
