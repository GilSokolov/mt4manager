import { TradeRecord } from "../types/trade-record";

export class Positions {
  constructor(private native: any) {}

  get(order: number): Promise<TradeRecord> {
    if (typeof order !== "number") {
      return Promise.reject("order must be number");
    }

    return this.native.get(order);
  }

  //   close(ticket: number) {
  //     if (typeof ticket !== "number") {
  //       return Promise.reject("ticket must be number");
  //     }

  //     return this.native.close(ticket);
  //   }

  //   closeAll(login?: number) {
  //     return this.native.closeAll(login);
  //   }
}
