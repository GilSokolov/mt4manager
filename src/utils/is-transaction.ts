import { TradeRecord, TradeCommand } from "../types";

export function isTransactionTrade(trade: TradeRecord) {
  return (
    trade.cmd === TradeCommand.Balance || trade.cmd === TradeCommand.Credit
  );
}
