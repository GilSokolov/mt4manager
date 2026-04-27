import { MT4Manager } from "./manager";
import type { MT4ManagerFactoryConfig } from "./types";

export async function createMT4Manager(config: MT4ManagerFactoryConfig) {
  const manager = new MT4Manager(config.dllPath);

  try {
    await manager.connect(config.server);
    await manager.login(config.login, config.password);

    if (config.pump) {
      const options = config.pump === true ? undefined : config.pump;
      await manager.startPumping(options);
    }

    return manager;
  } catch (err) {
    await manager.close().catch(() => {});
    throw err;
  }
}

export { MT4Manager };
export default MT4Manager;
export * from "./types";
