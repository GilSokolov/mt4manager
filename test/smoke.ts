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
    logLevel: 3,
  });

  console.log("[debug] create manager passed");

  console.log("[debug] closing manager");

  try {
    await manager.close();
    console.log("[debug] manager closed");
  } catch (err) {
    console.error("[debug] close failed:", err);
    throw err;
  }
}

main().catch((error) => {
  console.error(error);
  process.exit(1);
});
