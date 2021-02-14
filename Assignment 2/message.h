#ifndef MESSAGE_H
#define MESSAGE_H

struct Message {
    int message_type;
    char sender[10];
    char receiver[10];
    char message[100];
};

struct Register {
    char name[20];
    char number[10];
};

#endif /* !MESSAGE_H */

