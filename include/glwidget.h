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
    void setGeometryColor(QColor);
    void setAxesFlag(bool);
    void setGrid2DFlag(bool);
    void setGrid3DFlag(bool);

signals:
    void readTimeUnitsInfo(int, std::string);
    void snapshotTime(float);

protected:
    void initializeGL() override;
    void paintGL() override;
    void resizeGL(int, int) override;
    void mousePressEvent(QMouseEvent*) override;
    void mouseMoveEvent(QMouseEvent*) override;
    void wheelEvent(QWheelEvent*) override;

private:
    void initializeViewer();
    void drawAxes();
    void drawPlaneGrid(pair<char, char>, int, float);
    void drawGrid2D();
    void drawGrid3D();
    const int linesPerGrid = 10;
    const int pointSize = 5;
    int getGridSpacing(BD5::Boundaries);
    bool m_core;
    int m_xRot = 0;
    int m_yRot = 0;
    int m_zRot = 0;
    float cameraZ = 0.0;
    float deltaZ = 0.0;
    
    float redColor = 0.8;
    float greenColor = 0.8;
    float blueColor = 0.8;
    bool showAxes = false;
    bool showGrid2D = false;
    bool showGrid3D = false;
    int gridSpacing = 0;
    BD5::Boundaries boundaries;
    QPoint m_lastPos;

    vector<BD5::Snapshot> snapshots;
    BD5::Snapshot current_snapshot;
    BD5::ScaleUnit scales;
    GLUquadricObj *qobj;
};
