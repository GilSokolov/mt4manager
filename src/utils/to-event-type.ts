import { EventTypeName, EventType } from "../types";

const EventTypeMap: Record<EventType, EventTypeName> = {
  [EventType.Add]: "add",
  [EventType.Delete]: "delete",
  [EventType.Update]: "update",
  [EventType.ChangeGroup]: "change_group",
};

export function toEventType(type: EventType): EventTypeName | "unknown" {
  return EventTypeMap[type] ?? "unknown";
}
