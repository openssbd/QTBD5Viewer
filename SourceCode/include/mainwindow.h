/**
 * @file    mainwindow.h
 * @author  Riken. Center for Biosystems Dynamics Research. Laboratory for Developmental Dynamics
 * @brief   Qt class for handling the application main window (Menues, status bar)
 * @version 0.1
 * @date    2022-10-14
 * 
 * @copyright Copyright (c) 2022
 * 
 */
# pragma once

#include <QMainWindow>
QT_BEGIN_NAMESPACE
class QStatusBar;
class QLabel;
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow();

private slots:
    void onOpenFile();

signals:
    void fileOpened(QString);
    void resetRequested();
    void colorChanged(QColor);
    void axesChanged(bool);
    void grid2DChanged(bool);
    void grid3DChanged(bool);
    void tracksChanged(bool);
private:
    QStatusBar *statusBar;
    QLabel *statusFileName;
};


