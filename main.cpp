#include "mainwindow.h"
#include <QApplication>

const char *default_conf_path = "./conf.yaml";

int main(int argc, char *argv[])
{
    std::string path;
    if (argc < 2)
    {
        path = default_conf_path;
    }
    else
    {
        path = argv[1];
        argv[1] = argv[0];
        argc --;
        argv ++;
    }
    QApplication a(argc, argv);
    MainWindow w(path);
    w.show();

    return a.exec();
}
