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
  await manager.startPumping({ ticks: false, users: true });
  console.log("[mt4] pumping started");

  const users = [];

  manager.users.on("update", (user) => {
    console.log(user);
  });

  await new Promise((resolve) => setTimeout(resolve, 30000));

  await manager.stopPumping();

  console.log("users.length", users.length);

  await manager.close();
}

main().catch((error) => {
  console.error(error);
  process.exit(1);
});
