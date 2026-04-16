export class TradesService {
  constructor(private readonly native: unknown) {
    void this.native;
  }

  async get(_ticket: number): Promise<never> {
    throw new Error('trades.get is not implemented yet');
  }
}
