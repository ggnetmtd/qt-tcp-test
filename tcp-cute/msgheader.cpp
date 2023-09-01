#include "msgheader.h"

MessageHeader::MessageHeader(MessageType msgType) : m_msgType(msgType)
{

}


char MessageHeader::getByte() const
{
    return (char) this->m_msgType;
}

bool MessageHeader::castByte(char byte, MessageType *mType)
{
    switch(byte)
    {
        case Unknown:
            *mType = Unknown;
            return true;
        case MathMessage:
            *mType = MathMessage;
            return true;
        default:
            return false;
    }

    return false;
}
