#ifndef CHATCONTEXT_H
#define CHATCONTEXT_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <QStringListModel>
#include <QFile>
#include <address.h>
#include <message.h>
#include <service.h>

#include "yamlcpp-qt.h"

inline void appendModelData(QStringListModel *model, const QString data)
{
    int tail = model->rowCount();
    model->insertRow(tail);
    model->setData(model->index(tail), data);
}

class ChatContextGroup;

class ChatContext : public QObject
{
    Q_OBJECT

public:
    explicit ChatContext(std::shared_ptr<sml::service> service, const std::string &id, QObject *parent = nullptr);
//    explicit ChatContext(std::shared_ptr<sml::service> service, const std::string &id, const sml::address &addr, QObject *parent = nullptr);
    virtual ~ChatContext() {}

    const QString &id() const;
    void appendSystemMessage(const QString &msg);
    //void updateStringListModel(QStringListModel *model);
    QStringListModel *model();

    void startReceivingFile(const QString &path);
    bool fileToReceive() const;
    void sendFile(const QString &path);

    virtual void feed(sml::recv_data *msg) = 0;
    virtual void appendMyMessage(const QString &msg) = 0;

//    static std::shared_ptr<ChatContext> create_group(const std::list<std::shared_ptr<ChatContext>> peerList);
signals:
    void fileIncoming();
    void newMessage(const QString &peerId, const QString &speaker, const QString &content);
    void newGroup(std::shared_ptr<ChatContext> group);

protected:
    void sendYAMLNode(const sml::address &addr, sml::datagram::id_type id, const YAML::Node &node);
    YAML::Node extractYAMLNode(std::string &str);

    virtual void sendCmd(const std::string &) {};
    void appendMessage(const QString &from, const QString &msg);
//    void appendPeerMessage(const std::string &msg);
    void appendFile(const std::string &bytes);
    void setFileLength(size_t length);
//    void startSendFile();

    std::shared_ptr<sml::service> service_;

    QString myId_;
    QString id_;
    std::unique_ptr<QStringListModel> model_;

    enum FileState
    {
        NoneSet = 0x0,
        FileIncoming = 0x1,
        LengthSet = 0x2,
        PathSet = 0x4,
        AllSet = FileIncoming | LengthSet | PathSet
    };
    int state_;
    std::string cmd_;
    size_t recvFileLength_;
    std::string recvFile_;
    QString recvFilePath_;

    std::unique_ptr<QFile> sendFile_;
};

#endif // CHATCONTEXT_H
