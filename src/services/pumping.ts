import { EventEmitter } from 'events';

export class PumpingService extends EventEmitter {
  constructor(private readonly native: EventEmitter) {
    super();
    void this.native;
  }

  enable(): void {
    throw new Error('pumping.enable is not implemented yet');
  }

  disable(): void {
    throw new Error('pumping.disable is not implemented yet');
  }
}
