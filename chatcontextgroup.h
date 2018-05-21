#ifndef CHATCONTEXTGROUP_H
#define CHATCONTEXTGROUP_H

#include "chatcontextsingle.h"

class ChatContextGroup : public ChatContextSingle
{
public:
    explicit ChatContextGroup(std::shared_ptr<sml::service> service, const std::string &id, const sml::address &addr, QObject *parent = nullptr);
    virtual ~ChatContextGroup() {}
    virtual void feed(sml::recv_data *msg);
    virtual void appendMyMessage(const QString &msg);
protected:
    std::string yaml_str;
};

#endif // CHATCONTEXTGROUP_H
