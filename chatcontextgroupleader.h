#ifndef CHATCONTEXTGROUPLEADER_H
#define CHATCONTEXTGROUPLEADER_H

#include "chatcontextgroup.h"

class ChatContextGroupLeader : public ChatContext
{
public:
    explicit ChatContextGroupLeader(std::shared_ptr<sml::service> service, const std::string id, std::list<std::shared_ptr<ChatContext>> group, QObject *parent = nullptr);
    virtual void feed(sml::recv_data *msg);
    virtual void appendMyMessage(const QString &msg);

protected:
    void sendGroupInit();
    void sendMessage(const sml::address &addr, const QString &from, const std::string &msg);
    typedef std::map<sml::address, QString> group_map_type;
    group_map_type group_map_;
};

#endif // CHATCONTEXTGROUPLEADER_H
