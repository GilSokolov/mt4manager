import { run } from "node:test";
import { spec } from "node:test/reporters";
import { readdir } from "node:fs/promises";
import { join } from "node:path";

async function getTestFiles(dir: string): Promise<string[]> {
  const entries = await readdir(dir, { withFileTypes: true, recursive: true });
  return entries
    .filter((e) => e.isFile() && e.name.endsWith(".test.js"))
    .map((e) => join(e.parentPath, e.name));
}

(async () => {
  const files = await getTestFiles("dist/test");

  console.log("Found test files:", files);

  const stream = run({ files, concurrency: 1, isolation: "none" });
  stream.compose(spec()).pipe(process.stdout);

  stream.on("test:fail", () => {
    process.exitCode = 1;
  });
})();
