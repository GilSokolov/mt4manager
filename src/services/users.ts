export class UsersService {
  constructor(private readonly native: unknown) {
    void this.native;
  }

  async get(_login: number): Promise<never> {
    throw new Error('users.get is not implemented yet');
  }
}
