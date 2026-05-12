# mt4manager

TypeScript-friendly Node.js bindings for the MetaTrader 4 Manager API.

`mt4manager` lets you use the MT4 Manager API from JavaScript/TypeScript without writing C++ directly.

Built as a native Node.js addon using Node-API (`node-addon-api`), it provides a modern async API for brokerages, internal tools, and MT4 infrastructure services.

---

## Features

- Native Node.js addon powered by Node-API
- TypeScript-first developer experience
- Runtime DLL loading (`mtmanapi64.dll`)
- Async MT4 connection lifecycle
- Real-time pumping/event updates
- User management
- Symbol management
- Transactions API
- Positions API
- Event subscriptions
- One MT4 native client per manager instance
- Windows-focused MT4 integration

---

## Status

> Beta — APIs may change before `v1.0.0`.

This project is actively under development and intended for:

- brokerages
- prop firms
- internal tooling
- automation systems
- MT4 infrastructure services

---

## Installation

```bash
npm install mt4manager
```

---

## Requirements

### Operating System

- Windows x64

### Runtime

- Node.js `>=20`
- Recommended: Node.js `22`

### MetaTrader Manager DLL

This package does **not** include:

- `mtmanapi.dll`
- `mtmanapi64.dll`

You must provide your own licensed MT4 Manager API DLL.

Example:

```txt
./dll/mtmanapi64.dll
```

---

## Development Requirements

Building the native addon from source requires:

- Windows
- Node.js >= 20
- Python 3
- Visual Studio 2022 Build Tools
  - Desktop development with C++
  - MSVC v143 toolset
  - Windows SDK
- node-gyp

Install node-gyp:

```bash
npm install -g node-gyp
```

## Quick Start

```ts
import { createMT4Manager } from "mt4manager";

const manager = await createMT4Manager({
  dllPath: "./dll/mtmanapi64.dll",
  server: "server.com:443",
  login: 9707,
  password: "password",
  pump: true,
});

// listen for user updates
manager.users.on("update", (user) => {
  console.log("User updated:", user);
});

try {
  // get user
  const user = await manager.users.get(123456);

  // update user
  await manager.users.update(123456, {
    name: "New Name",
  });
} finally {
  // cleanup
  await manager.close();
}
```

---

# API Overview

## Users

### Get User

```ts
const user = await manager.users.get(123456);
```

### Update User

```ts
await manager.users.update(123456, {
  name: "Updated Name",
});
```

### Create User

```ts
const user = await manager.users.create({
  group: "demotest",
  name: "John Doe",
  email: "john@example.com",
});
```

### User Updates (Pump Events)

```ts
manager.users.on("update", (user) => {
  console.log(user);
});
```

---

## Symbols

### Get Symbol

```ts
const symbol = await manager.symbols.get("EURUSD");
```

### Get All Symbols

```ts
const symbols = await manager.symbols.getAll();
```

### Subscribe to Tick Updates

```ts
await manager.symbols.subscribe("EURUSD");

manager.symbols.watch("EURUSD", (symbol) => {
  console.log(symbol.bid, symbol.ask);
});
```

---

## Transactions

### Deposit

```ts
const tx = await manager.transactions.deposit({
  login: 123456,
  amount: 100,
  comment: "Initial deposit",
});
```

### Withdrawal

```ts
const tx = await manager.transactions.withdrawal({
  login: 123456,
  amount: 50,
  comment: "Client withdrawal",
});
```

### Credit In

```ts
const tx = await manager.transactions.creditIn({
  login: 123456,
  amount: 25,
  comment: "Bonus credit",
});
```

### Credit Out

```ts
const tx = await manager.transactions.creditOut({
  login: 123456,
  amount: 25,
  comment: "Remove bonus",
});
```

### Get Transaction

```ts
const tx = await manager.transactions.get(100000);
```

---

## Positions

### Open Market Position

```ts
import { TradeCommand } from "mt4manager";

const trade = await manager.positions.open({
  login: 123456,
  symbol: "EURUSD",
  cmd: TradeCommand.Buy,
  volume: 1,
  comment: "open-position",
  price: 1.17,
});
```

### Close Position

```ts
await manager.positions.close({
  id: trade.id,
  volume: trade.volume,
  price: trade.openPrice,
});
```

### Modify Position

```ts
await manager.positions.modify({
  id: trade.id,
  sl: 1.1,
  tp: 1.2,
  price: 1.17,
});
```

### Modify Position Comment

```ts
await manager.positions.modifyComment(trade.id, "updated-comment");
```

### Open Pending Order

```ts
const pending = await manager.positions.open({
  login: 123456,
  symbol: "EURUSD",
  cmd: TradeCommand.BuyLimit,
  volume: 1,
  comment: "buy-limit",
  price: 1.0,
});
```

### Cancel Pending Order

```ts
await manager.positions.cancel(pending.id, pending.cmd);
```

### Close Opposite Positions By Ticket

```ts
const buy = await manager.positions.open({
  login: 123456,
  symbol: "EURUSD",
  cmd: TradeCommand.Buy,
  volume: 1,
  price: 1.17,
});

const sell = await manager.positions.open({
  login: 123456,
  symbol: "EURUSD",
  cmd: TradeCommand.Sell,
  volume: 1,
  price: 1.17,
});

await manager.positions.closeBy(buy.id, sell.id);
```

### Close Multiple Opposite Positions

```ts
await manager.positions.closeMultipleBy(123456, "EURUSD");
```

### Listen For Pumped Trade Events

```ts
manager.positions.on("add", (trade) => {
  console.log("Position opened:", trade);
});

manager.positions.on("update", (trade) => {
  console.log("Position updated:", trade);
});

manager.positions.on("delete", (trade) => {
  console.log("Position closed:", trade);
});
```

---

# Pumping

Pumping mode enables real-time updates from the MT4 server.

Example:

```ts
const manager = await createMT4Manager({
  pump: {
    ticks: true,
  },
});
```

Real-time updates are automatically emitted through module event handlers.

---

# Environment Variables

Only needed for local development and tests.
Consumers can pass config directly in code.

Example `.env`:

```env
MT4_SERVER=server.com:443
MT4_LOGIN=9707
MT4_PASSWORD=abc123
MT4_DLL_PATH=./dll/mtmanapi64.dll
MT4_USER_LOGIN=100010
MT4_TEST_GROUP=demotest
```

---

# Development

## Install Dependencies

```bash
npm install
```

## Build

```bash
npm run build
```

## Rebuild

```bash
npm run rebuild
```

## Run Tests

```bash
npm test
```

---

# Project Structure

```txt
src/
native/
include/
build/
dist/
test/
```

---

# Native Architecture

`mt4manager` is built using:

- Node-API (`node-addon-api`)
- C++
- TypeScript
- `node-gyp`

The project uses:

- thread-safe JS callback bridges
- native MT4 client wrappers
- async-safe event forwarding
- runtime DLL loading
- modular MT4 service wrappers

Example native wrapper:

```cpp
bridge_->CallJs(UserPayload{*user, type}, BuildUserArgs);
```

This architecture allows MT4 pump events to safely propagate from native C++ threads into JavaScript event handlers.

---

# Important Notes

## MT4 Licensing

Review MetaQuotes licensing terms before deploying or redistributing MT4 Manager API components.

## Live Trading Warning

This package can perform:

- deposits
- withdrawals
- credits
- user modifications
- position operations

Use carefully on production MT4 servers.

---

# Publishing

Before publishing:

```bash
npm run clean
npm run build
npm test
npm run pack:check
npm publish
```

---

# Current Coverage

- Users
- Symbols
- Transactions
- Positions
- Pumping/event updates
- Native async wrappers
- TypeScript support

---

# License

MIT

---

# Repository

GitHub:

```txt
https://github.com/GilSokolov/mt4manager
```
