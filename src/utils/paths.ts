import { ManagerConfig, NativeMT4Manager } from "../types";

export function loadBinding(): {
  MT4Manager: new (config: ManagerConfig) => NativeMT4Manager;
} {
  // eslint-disable-next-line @typescript-eslint/no-var-requires
  return require("bindings")("mt4manager");
}
