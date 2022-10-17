#include "mainwindow.h"
#include "window.h"
#include <iostream>
#include <QMenuBar>
#include <QMenu>
#include <QMessageBox>
#include <QFileDialog>
#include <QColorDialog>
#include <QColor>
#include <QStatusBar>
#include <QCheckBox>
#include <QFileInfo>
#include <QLabel>
#include <QApplication>

MainWindow::MainWindow()
{
    QMenuBar *menuBar = new QMenuBar;
    QMenu *menuFile = menuBar->addMenu(tr("&File"));
    QMenu *menuView = menuBar->addMenu(tr("&View"));

    QAction *openFile = new QAction(menuFile);
    openFile->setText(tr("&Open"));
    menuFile->addAction(openFile);
    connect(openFile, &QAction::triggered, this, &MainWindow::onOpenFile);

    QAction *exit = new QAction(menuFile);
    exit->setText(tr("E&xit"));
    menuFile->addAction(exit);
    connect(exit, &QAction::triggered, this, &MainWindow::onExit);

    QAction *resetPos = new QAction(menuView);
    resetPos->setText(tr("&Reset Position"));
    menuView->addAction(resetPos);
    connect(resetPos, &QAction::triggered, this, &MainWindow::onResetPosition);

    QAction *selectColor = new QAction(menuView);
    selectColor->setText(tr("Select &Color"));
    menuView->addAction(selectColor);
    connect(selectColor, &QAction::triggered, this, &MainWindow::onSelectColor);

    setMenuBar(menuBar);
    statusBar = new QStatusBar(this);
    QCheckBox *axes = new QCheckBox("Axes", this);
    QCheckBox *grid2D = new QCheckBox("2D Grid", this);
    QCheckBox *grid3D = new QCheckBox("3D Grid", this);
    statusFileName = new QLabel(this);

    axes->setCheckState(Qt::Unchecked);
    grid2D->setCheckState(Qt::Unchecked);
    statusBar->addPermanentWidget(axes);
    statusBar->addPermanentWidget(grid2D);
    statusBar->addPermanentWidget(grid3D);
    statusBar->insertPermanentWidget(0, statusFileName, 10);   
    connect(axes, &QCheckBox::clicked, this, &MainWindow::onChangeAxesFlag);
    connect(grid2D, &QCheckBox::clicked, this, &MainWindow::onChangeGrid2DFlag);
    connect(grid3D, &QCheckBox::clicked, this, &MainWindow::onChangeGrid3DFlag);
    setStatusBar(statusBar);

    setCentralWidget(new Window(this));
}

void MainWindow::onOpenFile()
{
    QString fileName = QFileDialog::getOpenFileName(this,
        tr("Open BD5 file"), "",
        tr("BD5 file (*.h5)"));
    if (fileName.isEmpty())
        return;
    else
    {
        QFileInfo info(fileName);
        statusFileName->setText(info.fileName());
        emit fileOpened(fileName);
    }
}

void MainWindow::onResetPosition()
{
    emit resetRequested();
}

void MainWindow::onSelectColor()
{
    QColor color = QColorDialog::getColor(Qt::lightGray, this, "Geometry color", QColorDialog::DontUseNativeDialog);
    if (color.isValid())
    {
        emit colorChanged(color);
    }
}

void MainWindow::onExit()
{
    QApplication::quit();
}

void MainWindow::onChangeAxesFlag(bool flag)
{
    emit axesChanged(flag);
}

void MainWindow::onChangeGrid2DFlag(bool flag)
{
    emit grid2DChanged(flag);
}

void MainWindow::onChangeGrid3DFlag(bool flag)
{
    emit grid3DChanged(flag);
}