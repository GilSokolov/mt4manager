export class MT4ManagerError extends Error {
  constructor(message: string, public readonly code?: number) {
    super(message);
    this.name = 'MT4ManagerError';
  }
}
