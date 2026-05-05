import { EventType } from "./events-types";

export interface User {
  // --- common settings
  login: number;
  group: string;
  password: string;

  // --- access flags (MT4 uses int flags, so keep number)
  enable: boolean;
  enableChangePassword: boolean;
  enableReadOnly: boolean;
  enableOtp: boolean;

  // --- passwords
  passwordInvestor: string;
  passwordPhone: string;

  // --- personal info
  name: string;
  country: string;
  city: string;
  state: string;
  zipcode: string;
  address: string;
  leadSource: string;
  phone: string;
  email: string;
  comment: string;
  id: string;
  status: string;

  // --- dates (MT4 timestamps = number)
  regdate: Date;
  lastdate: Date;
  timestamp: Date;

  // --- trade settings
  leverage: number;
  agentAccount: number;
  lastIp: string;

  balance: number;
  prevMonthBalance: number;
  prevBalance: number;
  credit: number;
  interestRate: number;
  taxes: number;
  prevMonthEquity: number;
  prevEquity: number;

  // --- security
  otpSecret: string;

  // --- misc
  sendReports: number;
  mqid: number;
  userColor: number;

  // --- api
  apiData: string;
}

export type CreateUserInput = Partial<User> & Pick<User, "group">;

export type UpdateUserInput = Partial<Omit<User, "login">>;

export interface ChangeUserPasswordInput {
  login: number;
  password: string;
  investor?: boolean;
}

export interface NativeUsersApi {
  get(login: number): Promise<User>;
  create(input: CreateUserInput): Promise<User>;
  update(login: number, input: UpdateUserInput): Promise<User>;

  _setUpdateHandler(handler: (user: User, type: EventType) => void): void;
}
