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

inline void appendModelData(QStringListModel *model, const QString data)
{
    int tail = model->rowCount();
    model->insertRow(tail);
    model->setData(model->index(tail), data);
}

class ChatContext : public QObject
{
    Q_OBJECT

public:
    explicit ChatContext(std::shared_ptr<sml::service> service, const std::string &id, const sml::address &addr, QObject *parent = nullptr);
    virtual ~ChatContext() {}

    const QString &id() const;
    void appendSystemMessage(const QString &msg);
    //void updateStringListModel(QStringListModel *model);
    QStringListModel *model();
    void feed(sml::recv_data *msg);
    const sml::address &addr() const;
    void appendMyMessage(const QString &msg);
    void startReceivingFile(const QString &path);
    bool fileToReceive() const;

signals:
    void fileIncoming();

private:
    void cmdHandler();
    void appendPeerMessage(const std::string &msg);
    void appendFile(const std::string &bytes);
    void setFileLength(size_t length);


    std::shared_ptr<sml::service> service_;

    QString myId_;
    QString id_;
    sml::address addr_;
    std::unique_ptr<QStringListModel> model_;

    enum FileState
    {
        NoneSet = 0x0,
        FileIncoming = 0x1,
        LengthSet = 0x2,
        PathSet = 0x4
    };
    int state_;
    std::string cmd_;
    size_t file_length_;
    std::string file_;
    QString filePath_;
};

#endif // CHATCONTEXT_H
