#include <QApplication>
#include <QSurfaceFormat>
#include <QScreen>
#include <QCommandLineParser>
#include <QCommandLineOption>
#include <QtGamepad/QGamepadManager>
#include <QWindow>

#include "glwidget.h"
#include "mainwindow.h"


int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QCoreApplication::setApplicationName("BD5Viewer");
    QCoreApplication::setOrganizationName("Riken");
    QCoreApplication::setApplicationVersion(QT_VERSION_STR);
    QCommandLineParser parser;
    parser.setApplicationDescription(QCoreApplication::applicationName());
    parser.addHelpOption();
    parser.addVersionOption();

    QCommandLineOption withGamepad("g", QCoreApplication::translate("main", "Connect gamepad"));
    parser.addOption(withGamepad);

    parser.process(app);

    if (parser.isSet(withGamepad)) {
        do
        {
            app.processEvents();
            cout << "Waiting Gamepad ";
        } while (QGamepadManager::instance()->connectedGamepads().isEmpty());
    }

    MainWindow mainWindow;

    mainWindow.resize(mainWindow.sizeHint());
    int desktopArea = QGuiApplication::primaryScreen()->size().width() *
                      QGuiApplication::primaryScreen()->size().height();
    int widgetArea = mainWindow.width() * mainWindow.height();
    if (((float)widgetArea / (float)desktopArea) < 0.75f)
        mainWindow.show();
    else
        mainWindow.showMaximized();
    return app.exec();
}
