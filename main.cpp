#include "reviewapp.h"
#include <QtWidgets/QApplication>
#include <QFile>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QFile file(":/themes/lighter.qss");   // ×ÊÔ´Â·¾¶
    if (file.open(QFile::ReadOnly | QFile::Text))
    {
        QString styleSheet = file.readAll();
        app.setStyleSheet(styleSheet);
        file.close();
    }
    else
    {
        qDebug() << "Failed to load QSS file";
    }

    ReviewApp window;
    window.show();
    return app.exec();
}
