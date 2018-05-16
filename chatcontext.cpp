#include "chatcontext.h"

ChatContext::ChatContext(const QString &myId, const std::string &id, const sml::address &addr)
    : myId_(myId), id_(QString::fromStdString(id)), addr_(addr), model_(std::make_unique<QStringListModel>())
{
}

const QString &ChatContext::id() const
{
    return id_;
}

void ChatContext::appendPeerMessage(const QString &msg)
{
    appendModelData(model_.get(), id_ + ":" + msg);
}

void ChatContext::appendMyMessage(const QString &msg)
{
    appendModelData(model_.get(), myId_ + ":" + msg);
}

QStringListModel *ChatContext::model()
{
    return model_.get();
}

const sml::address &ChatContext::addr() const
{
    return addr_;
}
