import "dotenv/config";
import path from "node:path";
function requireEnv(name: string): string {
  const val = process.env[name];
  if (!val) throw new Error(`Missing env: ${name}`);
  return val;
}

export const config = {
  server: requireEnv("MT4_SERVER"),
  login: Number(requireEnv("MT4_LOGIN")),
  password: requireEnv("MT4_PASSWORD"),
  dllPath: path.resolve(requireEnv("MT4_DLL_PATH")),
  runSlowTests: process.env.MT4_SLOW_TESTS === "1",
  userLogin: Number(process.env.MT4_USER_LOGIN!),
};
