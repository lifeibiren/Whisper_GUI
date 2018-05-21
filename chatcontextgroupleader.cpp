#include "chatcontextgroupleader.h"

ChatContextGroupLeader::ChatContextGroupLeader(std::shared_ptr<sml::service> service, const std::string id, std::list<std::shared_ptr<ChatContext>> group, QObject *parent)
    : ChatContext(service, id, parent)
{
    for (auto it = group.begin(); it != group.end(); it ++)
    {
        ChatContextSingle *context = dynamic_cast<ChatContextSingle *>(it->get());
        if (context)
        {
            group_map_.insert(group_map_type::value_type(context->addr(), context->id()));
        }
    }
    sendGroupInit();
}

void ChatContextGroupLeader::sendGroupInit()
{
    YAML::Node node;
    node["group"]["id"] = id_;
    for (group_map_type::iterator it = group_map_.begin(); it != group_map_.end(); it ++)
    {
        sendYAMLNode(it->first, 0, node);
    }
}

void ChatContextGroupLeader::feed(sml::recv_data *msg)
{
    group_map_type::iterator from = group_map_.find(msg->addr());
    if (from == group_map_.end())
    {
        return;
    }

    for (group_map_type::iterator it = group_map_.begin(); it != group_map_.end(); it ++)
    {
        if (it->first != from->first)
        {
            sendMessage(it->first, from->second, msg->data());
        }
    }
    appendMessage(from->second, QString::fromStdString(msg->data()));
}

void ChatContextGroupLeader::appendMyMessage(const QString &msg)
{
    for (group_map_type::iterator it = group_map_.begin(); it != group_map_.end(); it ++)
    {
        sendMessage(it->first, myId_, msg.toStdString());
    }
    appendMessage(myId_, msg);
}

void ChatContextGroupLeader::sendMessage(const sml::address &addr, const QString &from, const std::string &msg)
{
    YAML::Node messageNode;
    messageNode[from] = msg;

    YAML::Node gNode;
    gNode["group"]["leader"] = myId_;
    gNode["group"]["message"] = messageNode;

    sendYAMLNode(addr, 3, gNode);
}
