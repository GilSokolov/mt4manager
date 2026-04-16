import MT4Manager from "../src";
import { config } from "./config";

async function main() {
  const manager = new MT4Manager(config.dllPath);

  console.log("[mt4] connecting...");
  await manager.connect(config.server);
  console.log("connected");

  console.log("[mt4] logging in...");
  await manager.login(config.login, config.password);
  console.log("logged in");

  await manager.close();
  console.log("closed");
}

main().catch((error) => {
  console.error(error);
  process.exit(1);
});
