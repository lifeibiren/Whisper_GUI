#ifndef CHATRECORD_H
#define CHATRECORD_H

#include <memory>
#include <vector>
#include <fstream>

#include <QDialog>
#include <QStringListModel>
#include <QStringList>

#include "yamlcpp-qt.h"

namespace Ui {
class ChatRecord;
}

class ChatRecord : public QDialog
{
    Q_OBJECT

public:
    explicit ChatRecord(const QString &path, QWidget *parent = 0);
    ~ChatRecord();
    void load();
    void refresh();
    void save() const;

public slots:
    void setPassword(const QString &passwd);
    void appendMessage(const QString &peer, const QString &speaker, const QString &content);

private slots:
    void on_peerListView_clicked(const QModelIndex &index);

private:
    QString filePath_;
    Ui::ChatRecord *ui_;
    bool encrypted_;
    QString passwd_;
    YAML::Node record;

    QStringListModel peerStringListModel_;
    std::vector<std::unique_ptr<QStringListModel>> messageStringListModel_;
//    QStringList peerStringList_;
//    std::vector<std::unique_ptr<QStringList>> messageStringList_;
};

#endif // CHATRECORD_H
