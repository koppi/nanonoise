#include <QApplication>
#include "MidiKorgNanoKONTROL.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    MidiKorgNanoKONTROL window;
    window.show();

    return app.exec();
}
