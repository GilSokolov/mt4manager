export interface User {
  // --- common settings
  login: number;
  group: string;
  password: string;

  // --- access flags (MT4 uses int flags, so keep number)
  enable: number;
  enableChangePassword: number;
  enableReadOnly: number;
  enableOtp: number;

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
  regdate: number;
  lastdate: number;
  timestamp: number;

  // --- trade settings
  leverage: number;
  agentAccount: number;
  lastIp: number;

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
