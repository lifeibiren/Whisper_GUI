#ifndef CHATCONTEXTSINGLE_H
#define CHATCONTEXTSINGLE_H

#include "chatcontext.h"

class ChatContextSingle : public ChatContext
{
public:
    ChatContextSingle(std::shared_ptr<sml::service> service, const std::string &id, const sml::address &addr, QObject *parent = nullptr);

//    void startReceivingFile(const QString &path);
//    void sendFile(const QString &path);

    const sml::address &addr() const;
    virtual void feed(sml::recv_data *msg);
    virtual void appendMyMessage(const QString &msg);

protected:
    void cmdHandler();
    void sendCmd(const std::string &cmd);
    void appendPeerMessage(const std::string &msg);
    void startSendFile();

    sml::address addr_;

//    enum FileState
//    {
//        NoneSet = 0x0,
//        FileIncoming = 0x1,
//        LengthSet = 0x2,
//        PathSet = 0x4,
//        AllSet = FileIncoming | LengthSet | PathSet
//    };
//    int state_;
//    std::string cmd_;
//    size_t recvFileLength_;
//    std::string recvFile_;
//    QString recvFilePath_;

//    std::unique_ptr<QFile> sendFile_;
};

#endif // CHATCONTEXTSINGLE_H
