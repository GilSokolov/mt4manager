import { createMT4Manager } from "../src";
import { config } from "./config";

async function main() {
  const manager = await createMT4Manager({
    dllPath: config.dllPath,
    server: config.server,
    login: config.login,
    password: config.password,
    pump: {
      ticks: true,
    },
    // logLevel: 3,
  });

  await manager.symbols.subscribe("EURUSD.");

  manager.users.on("update", console.log);

  const done = new Promise<void>((resolve) => {
    setTimeout(() => {
      resolve();
    }, 60000);
  });

  await done;
}

main().catch((error) => {
  console.error(error);
  process.exit(1);
});
