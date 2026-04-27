import { createMT4Manager } from "../src";
import { config } from "./config";

async function main() {
  const manager = await createMT4Manager(config);

  

  await manager.close();
  console.log("closed");
}

main().catch((error) => {
  console.error(error);
  process.exit(1);
});
