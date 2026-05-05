export type ExpirationMode = "none" | "optional" | "required";

function toExpirationSeconds(expiration?: number | Date): number | undefined {
  if (expiration === undefined) return undefined;

  if (typeof expiration === "number") {
    return expiration;
  }

  return Math.floor(expiration.getTime() / 1000);
}

export function resolveExpiration(
  mode: ExpirationMode,
  expiration?: number | Date,
): number {
  const value = toExpirationSeconds(expiration);

  if (mode === "none") {
    return 0;
  }

  if (mode === "optional") {
    return value ?? 0;
  }

  return value ?? Math.floor(Date.now() / 1000) + 86400 * 30;
}
