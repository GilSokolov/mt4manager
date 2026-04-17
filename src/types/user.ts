export interface User {
  login: number;
  group: string;
  name?: string;
  email?: string;
  leverage?: number;
}

export interface CreateUserInput {
  login: number;
  group: string;
  name?: string;
  email?: string;
  leverage?: number;
  password?: string;
  investorPassword?: string;
}

export interface UpdateUserInput {
  login: number;
  group?: string;
  name?: string;
  email?: string;
  leverage?: number;
}

export interface ChangeUserPasswordInput {
  login: number;
  password: string;
  investor?: boolean;
}
