export type Listener<T> = (user: T) => void;

export type PumpingOptions = {
  ticks: boolean;
  news: boolean;
  mail: boolean;
  onlineUsers: boolean;
  users: boolean;
  fullNews: boolean;
};
