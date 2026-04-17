import MT4Manager from "../src";
import { config } from "./config";

export function createManager() {
  const manager = new MT4Manager({ dllPath: config.dllPath });
  let closed = false;

  return {
    manager,
    close() {
      if (closed) return;
      closed = true;
      manager.close();
    },
  };
}
