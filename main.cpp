#include <QApplication>
#include "calculator.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    // 直接使用 calculator 類別
    calculator calc;
    calc.show();

    return app.exec();
}
