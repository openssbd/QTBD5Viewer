/**
 * @file    window.h
 * @author  Riken. Center for Biosystems Dynamics Research. Laboratory for Developmental Dynamics
 * @brief   Qt class handling the window containing the glwidget to visualize OpenGL geometry
 * @version 0.1
 * @date    2022-10-14
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#pragma once

#include <QWidget>

QT_BEGIN_NAMESPACE
class QSlider;
class QPushButton;
class QLabel;
QT_END_NAMESPACE

class GLWidget;
class MainWindow;

class Window : public QWidget
{
    Q_OBJECT

public:
    Window(MainWindow *mw);

protected:
    void keyPressEvent(QKeyEvent*) override;

public slots:
    void setNumTimeMarks(int, std::string);
    void setTimeToDisplay(float);

private:
    QString timeUnit;
    QSlider *createSlider();
    void setCurrentTime(int);
    GLWidget *glWidget;
    QSlider *tSlider;
    QLabel *minTLabel;
    QLabel *maxTLabel;
    QLabel *indexTLabel;
    QLabel *timeLabel;
    MainWindow *mainWindow;
};
