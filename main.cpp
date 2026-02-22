#include "reviewapp.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    ReviewApp window;
    window.show();
    return app.exec();
}
