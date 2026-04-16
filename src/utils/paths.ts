export function loadBinding(): { MT4Manager: new (dllPath: string) => unknown } {
  // eslint-disable-next-line @typescript-eslint/no-var-requires
  return require('bindings')('mt4manager');
}
