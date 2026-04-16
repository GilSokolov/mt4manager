import path from "node:path";
import MT4Manager from "../src";

async function main() {
  const dllPath = path.resolve("dll", "mtmanapi64.dll");
  console.log("dllPath =", dllPath);

  console.log("before constructor");

  let manager: MT4Manager;
  try {
    manager = new MT4Manager(dllPath);
    console.log("after constructor");
  } catch (error) {
    console.error("constructor threw:", error);
    process.exit(1);
    return;
  }

  try {
    console.log("before connect");
    await manager.connect("mt2.leverate.com:443");
    console.log("after connect");

    console.log("before close");
    await manager.close();
    console.log("after close");
  } catch (error) {
    console.error("runtime failed:", error);
    process.exit(1);
  }
}

main().catch((error) => {
  console.error("main failed:", error);
  process.exit(1);
});
