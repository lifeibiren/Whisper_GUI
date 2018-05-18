#ifndef FILEBROWSER_H
#define FILEBROWSER_H

#include <QDialog>
#include <QStringListModel>
#include <QDir>

namespace Ui {
class FileBrowser;
}

class FileBrowser : public QDialog
{
    Q_OBJECT

public:
    explicit FileBrowser(QWidget *parent = 0);
    ~FileBrowser();

    const QString &getPath() const;
private slots:
    void on_pathListView_clicked(const QModelIndex &index);

    void on_pathListView_activated(const QModelIndex &index);

    void on_pathLineEdit_editingFinished();

private:
    Ui::FileBrowser *ui_;
    QDir dir;
    QStringListModel model_;
    QString path_;
};

#endif // FILEBROWSER_H
