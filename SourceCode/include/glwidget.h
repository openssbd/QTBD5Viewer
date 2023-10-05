/**
 * @file    glwidget.h
 * @author  Riken. Center for Biosystems Dynamics Research. Laboratory for Developmental Dynamics
 * @brief   Qt class widget for handling and displaying OpenGL geometry 
 * @version 0.1
 * @date    2022-10-14
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#pragma once

#include <QtGamepad/QGamepad>
#include <QtCore/QList>
#include <QtCore/QDebug>
#define GL_SILENCE_DEPRECATION
#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <utility>
#include "BD5File.h"
#if __APPLE__
#include <GLUT/glut.h>
#elif _WIN32
#include <Windows.h>
#include <gl/GL.h>
#include <gl/GLU.h>
#else
#include <GL/glut.h>
#endif


using namespace BD5;
using namespace std;

QT_FORWARD_DECLARE_CLASS(QOpenGLShaderProgram)
QT_FORWARD_DECLARE_CLASS(QColor)

struct renderSnapshot 
{
    int index = 0;
    BD5::Snapshot snapshot;
    map<string, vector<float>> labelsColors;
};

class GLWidget : public QOpenGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT

public:
    GLWidget(QWidget *parent = nullptr);
    ~GLWidget();

    QSize minimumSizeHint() const override;
    QSize sizeHint() const override;

public slots:
    void setTimeToVisualize(int);
    void setXRotation(int);
    void setYRotation(int);
    void setZRotation(int);
    void setGeometry(QString);
    void resetPosition();
    void setAxesFlag(bool);
    void setGrid2DFlag(bool);
    void setGrid3DFlag(bool);
    void setTracksFlag(bool);
    void setObjectShowFlag(string, bool);
    void setLabelColor(string, std::vector<float>);
    void setLabelsOneColor(std::vector<float>);
    void setDefaultLabelsColors();

signals:
    void readTimeUnitsInfo(int, std::string);
    void snapshotTime(float);
    void moveToNextTime();
    void moveToPrevTime();
    void objectsNames(std::vector<std::string>);
    void labelsNames(std::vector<std::string>);

protected:
    void initializeGL() override;
    void paintGL() override;
    void resizeGL(int, int) override;
    void mousePressEvent(QMouseEvent*) override;
    void mouseMoveEvent(QMouseEvent*) override;
    void wheelEvent(QWheelEvent*) override;

private:
    void initializeViewer();
    void initializeGamepad(QWidget*);
    void lockGamepadJoystick(float, std::function<float()>);
    void lockGamepadButton(std::function<bool()>);
    void drawAxes();
    void drawPlaneGrid(pair<char, char>, int, float);
    void drawGrid2D();
    void drawGrid3D();
    int getGridSpacing(BD5::Boundaries);
    void drawTracks(int);
    void setCurrentSnapshot(int, BD5::Snapshot&, std::vector<std::string>);
    void setDefaultPaletteColors(std::vector<std::string>);   
    void defineGLColor(std::string);
    QGamepad *m_gamepad = nullptr;
    const int linesPerGrid = 10;
    const int pointSize = 5;
    bool m_core;
    int m_xRot = 0;
    int m_yRot = 0;
    int m_zRot = 0;
    float cameraZ = 0.0;
    float deltaZ = 0.0;

    bool showAxes = false;
    bool showGrid2D = false;
    bool showGrid3D = false;
    bool showTracks = false;
    int gridSpacing = 0;
    BD5::Boundaries boundaries;
    QPoint m_lastPos;
    BD5File file;

    vector<BD5::Snapshot> snapshots;
    map<string, bool> objsVisibility;
    renderSnapshot currentSnapshot;
    
    std::vector<std::vector<PointTrack>> tracks;
    BD5::ScaleUnit scales;
    GLUquadricObj *qobj;
};
