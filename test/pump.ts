import MT4Manager from "../src";
import { config } from "./config";

async function main() {
  const manager = new MT4Manager(config.dllPath);

  console.log("[mt4] connecting...");
  await manager.connect(config.server);
  console.log("[mt4] connected");

  console.log("[mt4] logging in...");
  await manager.login(config.login, config.password);
  console.log("[mt4] logged in");

  console.log("[mt4] starting pumping...");
  await manager.startPumping();
  console.log("[mt4] pumping started");

  await new Promise((resolve) => setTimeout(resolve, 3000));

  manager.stopPumping();

  await new Promise((resolve) => setTimeout(resolve, 3000));
  // console.log("[mt4] closing...");
  // await manager.close();
  // console.log("[mt4] closed");
}

main().catch((error) => {
  console.error(error);
  process.exit(1);
});
