#ifndef GUI_EVENTHANDLER_H
#define GUI_EVENTHANDLER_H

typedef int EventType;

class EventHandler {
public:
    static const EventType KEYDOWN_EVENT = 0;
    static const EventType KEYUP_EVENT = 1;
    static const EventType MOUSEMOVE_EVENT = 2;
    static const EventType RESIZE_EVENT = 3;
    static const EventType QUIT_EVENT = 4;

    static void HandleEvent(EventType type, unsigned long p1, unsigned long p2);
};


#endif //GUI_EVENTHANDLER_H
