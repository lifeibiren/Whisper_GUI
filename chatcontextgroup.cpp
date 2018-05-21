#include "chatcontextgroup.h"

ChatContextGroup::ChatContextGroup(std::shared_ptr<sml::service> service, const std::string &id, const sml::address &addr, QObject *parent)
    : ChatContextSingle(service, id, addr, parent)
{

}

void ChatContextGroup::feed(sml::recv_data *msg)
{
    if (msg->addr() != addr_)
    {
        return;
    }

    switch (msg->id())
    {
        case 3:
        {
            yaml_str += msg->data();
            YAML::Node node;
            while (1)
            {
                node = extractYAMLNode(yaml_str);

                if (node["group"])
                {
                    YAML::Node gNode = node["group"];
                    YAML::Node mNode = gNode["message"];
                    for (YAML::Node::const_iterator it = mNode.begin(); it != mNode.end(); it ++)
                    {
                        appendMessage(it->first.as<QString>(), it->second.as<QString>());
                    }
                }
                else
                {
                    break;
                }
            }
            break;
        }
        default:
            break;
    }
}

void ChatContextGroup::appendMyMessage(const QString &msg)
{
    service_->post(boost::make_shared<sml::send_data>(addr_, 3, msg.toStdString()));
    appendMessage(myId_, msg);
}
