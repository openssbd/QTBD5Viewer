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
class QLabel;
class QVBoxLayout;
class QCheckBox;
class QColor;
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
    void createObjsGroupBox(std::vector<std::string>);
    void createLabelsGroupBox(std::vector<std::string>);
    void removeObjectsNames();
    void removeLabelNames();
    void setColorToButtons(QColor);
    void setDefaultColorsToButtons();
    void resetOneColorCheck();
    void removeOneColorButton();

public slots:
    void setNumTimeMarks(int, std::string);
    void setTimeToDisplay(float);
    void moveToNextTime();
    void moveToPrevTime();
    void setObjectsNames(std::vector<std::string>);
    void setLabelsNames(std::vector<std::string>);
    void oneColorStateChanged(bool);

signals:
    void showObjectCheckChanged(std::string, bool);
    void labelColorChanged(std::string, std::vector<float>);
    void labelsOneColorChanged(std::vector<float>);
    void setDefaultLabelsColors();

private:
    QSlider *createSlider();
    void setCurrentTime(int);
    QString timeUnit;
    GLWidget *glWidget;
    QSlider *tSlider;
    QLabel *minTLabel;
    QLabel *maxTLabel;
    QLabel *indexTLabel;
    QLabel *timeLabel;
    QVBoxLayout *objectsLayout;
    QVBoxLayout *labelsLayout;
    QVBoxLayout *oneColorLayout;
    MainWindow *mainWindow;
};