#include "glwidget.h"
#include "window.h"
#include "mainwindow.h"
#include <QSlider>
#include <QLabel>
#include <QString>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QKeyEvent>
#include <QPushButton>
#include <QApplication>
#include <QMessageBox>

Window::Window(MainWindow *mw)
    : mainWindow(mw)
{
    glWidget = new GLWidget;
    tSlider = createSlider();
    minTLabel = new QLabel;
    maxTLabel = new QLabel;
    indexTLabel = new QLabel;
    timeLabel = new QLabel;
    minTLabel->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed));
    maxTLabel->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed));
    indexTLabel->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed));
    timeLabel->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed));
    minTLabel->setAlignment(Qt::AlignLeft);
    maxTLabel->setAlignment(Qt::AlignRight);
    indexTLabel->setAlignment(Qt::AlignCenter);
    timeLabel->setAlignment(Qt::AlignCenter);

    connect(mw, &MainWindow::fileOpened, glWidget, &GLWidget::setGeometry);    
    connect(mw, &MainWindow::resetRequested, glWidget, &GLWidget::resetPosition);
    connect(mw, &MainWindow::colorChanged, glWidget, &GLWidget::setGeometryColor);
    connect(mw, &MainWindow::axesChanged, glWidget, &GLWidget::setAxesFlag);
    connect(mw, &MainWindow::grid2DChanged, glWidget, &GLWidget::setGrid2DFlag);
    connect(mw, &MainWindow::grid3DChanged, glWidget, &GLWidget::setGrid3DFlag);
    connect(tSlider, &QSlider::valueChanged, glWidget, &GLWidget::setTimeToVisualize);
    connect(tSlider, &QSlider::valueChanged, this, &Window::setCurrentTime);
    connect(glWidget, &GLWidget::readTimeUnitsInfo, this, &Window::setNumTimeMarks);
    connect(glWidget, &GLWidget::snapshotTime, this, &Window::setTimeToDisplay);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(glWidget);

    mainLayout->addWidget(tSlider);
    QHBoxLayout *labels = new QHBoxLayout;
    labels->addWidget(minTLabel);
    labels->addWidget(indexTLabel);
    labels->addWidget(timeLabel);
    labels->addWidget(maxTLabel);
    mainLayout->addLayout(labels);
    setLayout(mainLayout);

    setWindowTitle(tr("BD5Viewer"));
}

QSlider *Window::createSlider()
{
    QSlider *slider = new QSlider(Qt::Horizontal);
    slider->setRange(0, 100);
    slider->setSingleStep(1);
    slider->setPageStep(10);
    slider->setTickInterval(10);
    slider->setTickPosition(QSlider::TicksRight);
    slider->setValue(0);
    return slider;
}

void Window::setCurrentTime(int time)
{
    QString text = "Time index: " + QString::number(time);
    indexTLabel->setText(text);
}

void Window::keyPressEvent(QKeyEvent *e)
{
    if (e->key() == Qt::Key_Escape)
        close();
    else
        QWidget::keyPressEvent(e);
}

void Window::setNumTimeMarks(int time, string tUnit)
{
    timeUnit = QString::fromStdString(tUnit);
    tSlider->setRange(0, time);
    tSlider->setValue(0);
    minTLabel->setText("0");
    maxTLabel->setText(QString::number(time)); 
    QString text = "Time index: 0";
    indexTLabel->setText(text);
    text = "(" + timeUnit + ")";
    timeLabel->setText(text);
}

void Window::setTimeToDisplay(float time)
{
    QString text = "Time " + QString::number(time) + " (" + timeUnit + ")";
    timeLabel->setText(text);
}