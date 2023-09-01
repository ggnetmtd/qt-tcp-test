#ifndef MSGHEADER_H
#define MSGHEADER_H

#include <cinttypes>

enum MessageType
{
    Unknown = 0,
    MathMessage = 1
};

class MessageHeader
{
public:
    const MessageType m_msgType;

public:
    MessageHeader(MessageType msgType);

    char getByte() const;
    static bool castByte(char byte, MessageType* mType);
};

#endif // MSGHEADER_H
