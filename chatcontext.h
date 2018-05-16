#ifndef CHATCONTEXT_H
#define CHATCONTEXT_H

#include <QString>
#include <QStringList>
#include <QStringListModel>
#include <address.h>

inline void appendModelData(QStringListModel *model, const QString data)
{
    int tail = model->rowCount();
    model->insertRow(tail);
    model->setData(model->index(tail), data);
}

class ChatContext
{
public:
    ChatContext(const QString &myId, const std::string &id, const sml::address &addr);
    const QString &id() const;
    //void updateStringListModel(QStringListModel *model);
    QStringListModel *model();
    void appendPeerMessage(const QString &msg);
    void appendMyMessage(const QString &msg);
    const sml::address &addr() const;
private:
    QString myId_;
    QString id_;
    sml::address addr_;
    std::unique_ptr<QStringListModel> model_;
    //QStringList msgList_;
};

#endif // CHATCONTEXT_H
