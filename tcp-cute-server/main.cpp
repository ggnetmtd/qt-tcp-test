#include <QCoreApplication>

#include "server.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    Server server = Server("127.0.0.1", 8080, Q_NULLPTR);
    server.init();
    server.run();

    return a.exec();
}
