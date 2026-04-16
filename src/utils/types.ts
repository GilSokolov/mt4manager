export interface MT4User {
  login: number;
  name?: string;
  balance?: number;
}

export interface MT4Trade {
  ticket: number;
  login?: number;
  symbol?: string;
  profit?: number;
}
