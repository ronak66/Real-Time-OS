struct Message {
    int message_type;
    char sender_name[20];
    char sender[10];
    char receiver[10];
    uint8_t message[1024];
};

struct Data {
    char name[20];
    char number[10];
};


