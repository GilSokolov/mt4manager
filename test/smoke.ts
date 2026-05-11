import { createMT4Manager } from "../src";
import { config } from "./config";

async function main() {
  const manager = await createMT4Manager({
    dllPath: config.dllPath + "asfsdfd",
    server: config.server,
    login: config.login,
    password: config.password,
    pump: {
      ticks: true,
    },
    // logLevel: 3,
  });


}

main().catch((error) => {
  console.error(error);
  process.exit(1);
});
