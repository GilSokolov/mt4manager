import { PumpingOptions } from "../types";

export function normalizePumpingOptions(
  options: Partial<PumpingOptions> = {},
): PumpingOptions {
  return {
    ticks: options.ticks ?? false,
    news: options.news ?? false,
    fullNews: options.fullNews ?? false,
    mail: options.mail ?? false,
    onlineUsers: options.onlineUsers ?? false,
    users: options.users ?? true,
  };
}
