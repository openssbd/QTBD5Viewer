#include "glwidget.h"
#include "window.h"
#include "mainwindow.h"
#include "utils.h"
#include <QSlider>
#include <QLabel>
#include <QString>
#include <QCheckBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QGridLayout>
#include <QKeyEvent>
#include <QPushButton>
#include <QColor>
#include <QColorDialog>
#include <QApplication>
#include <QMessageBox>
#include <QSize>
#include <QScrollArea>
#include <QtGamepad/QGamepadManager>

Window::Window(MainWindow *mw)
    : mainWindow(mw)
{
    glWidget = new GLWidget;
    tSlider = createSlider();
    minTLabel = new QLabel;
    maxTLabel = new QLabel;
    indexTLabel = new QLabel;
    timeLabel = new QLabel;
    QVBoxLayout* objsAndLabels(new QVBoxLayout);

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
    connect(mw, &MainWindow::axesChanged, glWidget, &GLWidget::setAxesFlag);
    connect(mw, &MainWindow::grid2DChanged, glWidget, &GLWidget::setGrid2DFlag);
    connect(mw, &MainWindow::grid3DChanged, glWidget, &GLWidget::setGrid3DFlag);
    connect(mw, &MainWindow::tracksChanged, glWidget, &GLWidget::setTracksFlag);
    connect(this, &Window::showObjectCheckChanged, glWidget, &GLWidget::setObjectShowFlag);
    connect(this, &Window::labelColorChanged, glWidget, &GLWidget::setLabelColor);
    connect(this, &Window::labelsOneColorChanged, glWidget, &GLWidget::setLabelsOneColor);
    connect(this, &Window::setDefaultLabelsColors, glWidget, &GLWidget::setDefaultLabelsColors);
    connect(tSlider, &QSlider::valueChanged, glWidget, &GLWidget::setTimeToVisualize);
    connect(tSlider, &QSlider::valueChanged, this, &Window::setCurrentTime);
    connect(glWidget, &GLWidget::readTimeUnitsInfo, this, &Window::setNumTimeMarks);
    connect(glWidget, &GLWidget::snapshotTime, this, &Window::setTimeToDisplay);
    connect(glWidget, &GLWidget::moveToNextTime, this, &Window::moveToNextTime);
    connect(glWidget, &GLWidget::moveToPrevTime, this, &Window::moveToPrevTime);
    connect(glWidget, &GLWidget::objectsNames, this, &Window::setObjectsNames);
    connect(glWidget, &GLWidget::labelsNames, this, &Window::setLabelsNames);

    QString objectsName = QString("Ob&jects");
    QGroupBox* objectsGroupBox(new QGroupBox(objectsName));
    objectsGroupBox->setObjectName(objectsName);
    objectsGroupBox->setFlat(true);
    objectsLayout = new QVBoxLayout;
    objectsLayout->setSizeConstraint(QLayout::SetMaximumSize);
    objectsLayout->setAlignment(Qt::AlignTop | Qt::AlignLeft);
    objectsGroupBox->setLayout(objectsLayout);
    // objsAndLabels->setAlignment(objectsGroupBox, Qt::AlignTop);
    objsAndLabels->addWidget(objectsGroupBox);

    QString oneColorName = QString("Labels &colors");
    QGroupBox *oneColorGroupBox(new QGroupBox(oneColorName));
    oneColorGroupBox->setFlat(true);
    oneColorLayout = new QVBoxLayout(oneColorGroupBox);
    oneColorGroupBox->setLayout(oneColorLayout);
    objsAndLabels->addWidget(oneColorGroupBox);

    QString labelsName = QString("La&bels");
    QGroupBox *labelsGroupBox(new QGroupBox(labelsName));
    labelsGroupBox->setFlat(true);
    labelsGroupBox->setObjectName(labelsName);
    labelsGroupBox->setMinimumWidth(120);  // TODO define a flexible size

    labelsLayout = new QVBoxLayout(labelsGroupBox);
    labelsLayout->setObjectName("VerticalBox");
    labelsLayout->setSizeConstraint(QLayout::SetMinimumSize);
    labelsGroupBox->setLayout(labelsLayout);

    QScrollArea *scroll = new QScrollArea;
    scroll->setMaximumWidth(200);  // TODO define a flexible size
    scroll->setWidget(labelsGroupBox);
    scroll->setWidgetResizable(true);
    objsAndLabels->addWidget(scroll);
    objsAndLabels->setSizeConstraint(QLayout::SetMinimumSize);

    QGridLayout *mainLayout(new QGridLayout);
    mainLayout->addWidget(glWidget, 0, 0);
    mainLayout->addLayout(objsAndLabels, 0, 1);
    mainLayout->addWidget(tSlider, 1, 0);

    QHBoxLayout *sliderLabels(new QHBoxLayout);
    sliderLabels->addWidget(minTLabel);
    sliderLabels->addWidget(indexTLabel);
    sliderLabels->addWidget(timeLabel);
    sliderLabels->addWidget(maxTLabel);
    mainLayout->addLayout(sliderLabels, 2, 0);
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

void Window::createObjsGroupBox(vector<string> names) 
{
    for (auto& name: names) {
        QString title = QString::fromStdString(name);
        QCheckBox *nameCheck = new QCheckBox(title, this);
        nameCheck->setObjectName(title);
        nameCheck->setChecked(true);
        objectsLayout->addWidget(nameCheck);
        connect(nameCheck, &QCheckBox::clicked, this, [objName = name, this](bool flag)
        {
            emit showObjectCheckChanged(objName, flag);
        });
    }
}

void Window::createLabelsGroupBox(vector<string> names) 
{
    int i = 0;
    for (auto& name : names) {
        QHBoxLayout* hbox = new QHBoxLayout;
        hbox->setObjectName("HorizontalBox");
        hbox->setSizeConstraint(QLayout::SetMinimumSize);
        QPushButton* labelColor = new QPushButton;
        labelColor->setObjectName(tr("colorButton"));
        // labelColor->setMaximumWidth(20);
        // labelColor->setMinimumWidth(20);
        labelColor->setMaximumSize( QSize(20,20) );
        QColor bColor;
        ColorPalette palette;
        auto utilColor = palette.GetColorAt(i);
        bColor.setRgbF(utilColor[0], utilColor[1], utilColor[2]);
        i++;
        QString style = "QPushButton { background: %1; }";
        labelColor->setStyleSheet( style.arg(bColor.name()));
        hbox->addWidget(labelColor);
        connect(labelColor, &QPushButton::clicked, this, [labelName = name, button = labelColor, this]()
        {
            QColor qColor = QColorDialog::getColor(Qt::lightGray, this, "Label entity color", QColorDialog::DontUseNativeDialog);
            if (qColor.isValid())
            {
                vector<float> color { qColor.redF(), qColor.greenF(), qColor.blueF() };
                emit labelColorChanged(labelName, color);
                this->resetOneColorCheck();
                QString selectedColor = qColor.name();
                QString style = "QPushButton { background: %1; }";
                button->setStyleSheet( style.arg(selectedColor) );
            }
        });
        QLabel* labelName = new QLabel( QString::fromStdString(name), this);
        hbox->addWidget(labelName);
        labelsLayout->addLayout(hbox);
    }
}

void Window::removeObjectsNames() 
{
    QLayoutItem *child;
    while ((child = objectsLayout->takeAt(0)) != nullptr)
    {
        delete child->widget();
        delete child;
    }
}

void Window::removeLabelNames() 
{
    QLayoutItem *child;
    QLayoutItem *innerChild;
    QLayout *hLayout;
    while ((child = labelsLayout->takeAt(0)) != nullptr)
    {
        hLayout = child->layout();
        if (hLayout == nullptr)
            cout << "Null layout";
        while ((innerChild = hLayout->takeAt(0)) != nullptr)
        {
            delete innerChild->widget();
            delete innerChild;
        }

        delete child->widget();
        delete child;
    }

    // Common color for labels
    QLayoutItem *commonColor;
    while ((commonColor = oneColorLayout->takeAt(0)) != nullptr)
    {
        delete commonColor->widget();
        delete commonColor;
    }    
}


void Window::setColorToButtons(QColor color)
{
    QString selectedColor = color.name();
    QString style = "QPushButton { background: %1; }";
    for (qsizetype i = 0; i < labelsLayout->children().size(); i++)
    {
        try {
            QObject* child = labelsLayout->children()[i];
            QHBoxLayout *hbox = qobject_cast<QHBoxLayout *>(child);
            if (hbox == nullptr) {
                cout << "Color Horizontal layout not found " << endl;
            }
            QLayoutItem* item = hbox->itemAt(0);
            QPushButton* buttonWidget = qobject_cast<QPushButton *>(item->widget());
            buttonWidget->setStyleSheet( style.arg(selectedColor) );            
        }
        catch (std::exception& ex) {
            cout << "qt interface " << ex.what() << endl;
        }
    }
}

void Window::setDefaultColorsToButtons()
{
    QString style = "QPushButton { background: %1; }";
    for (qsizetype i = 0; i < labelsLayout->children().size(); i++)
    {
        try {
            QColor bColor;
            ColorPalette palette;
            auto utilColor = palette.GetColorAt(i);
            bColor.setRgbF(utilColor[0], utilColor[1], utilColor[2]);
            QString selectedColor = bColor.name();

            QObject* child = labelsLayout->children()[i];
            QHBoxLayout *hbox = qobject_cast<QHBoxLayout *>(child);
            if (hbox == nullptr) {
                cout << "Color Horizontal layout not found " << endl;
            }
            QLayoutItem* item = hbox->itemAt(0);
            QPushButton* buttonWidget = qobject_cast<QPushButton *>(item->widget());
            buttonWidget->setStyleSheet( style.arg(selectedColor) );            
        }
        catch (std::exception& ex) {
            cout << "qt interface " << ex.what() << endl;
        }
    }
}

void Window::resetOneColorCheck()
{
    QLayoutItem *child = oneColorLayout->itemAt(0);
    if (child != nullptr)
    {
        QCheckBox *checkOneColor = qobject_cast<QCheckBox *>(child->widget());
        if (checkOneColor == nullptr) {
            cout << "CheckBox widget not found " << endl;
        }
        checkOneColor->setChecked(false);
        removeOneColorButton();
    }
}

void Window::removeOneColorButton()
{
    if (oneColorLayout->count() < 2) 
        return;
    QLayoutItem *child = oneColorLayout->takeAt(1);
    if (child != nullptr)
    {
        delete child->widget();
        delete child;
    }
}

/**
 * From here starts the Qt slots functions
*/
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

void Window::moveToNextTime() 
{
    int curValue = tSlider->value();
    if (curValue < tSlider->maximum())
    {
        tSlider->setValue(curValue + 1);
    }
}
    
void Window::moveToPrevTime()
{
    int curValue = tSlider->value();
    if (curValue > tSlider->minimum())
    {
        tSlider->setValue(curValue - 1);
    }
}

void Window::setObjectsNames(vector<string> names)
{
    removeObjectsNames();
    createObjsGroupBox(names);
}

void Window::setLabelsNames(vector<string> names) 
{
    removeLabelNames();
    if (names.empty())
        return;

    createLabelsGroupBox(names);

    // Common color for labels
    QCheckBox *checkOneColor = new QCheckBox(tr("One color?"));
    connect(checkOneColor, &QCheckBox::clicked, this, &Window::oneColorStateChanged);
    checkOneColor->setObjectName(tr("OneColorCheckBox"));
    oneColorLayout->addWidget(checkOneColor);
}

void Window::oneColorStateChanged(bool checked)
{
    if (checked)
    {
        QPushButton* labelColor = new QPushButton;
        labelColor->setMaximumSize( QSize(20, 20) );
        labelColor->setObjectName(tr("CommonColorButton"));
        connect(labelColor, &QPushButton::clicked, this, [button = labelColor, this]()
        {
            QColor qColor = QColorDialog::getColor(Qt::lightGray, this, "Label entity color", QColorDialog::DontUseNativeDialog);
            if (qColor.isValid())
            {
                vector<float> color { qColor.redF(), qColor.greenF(), qColor.blueF() };
                emit labelsOneColorChanged(color);
                QString selectedColor = qColor.name();
                QString style = "QPushButton { background: %1; }";
                button->setStyleSheet( style.arg(selectedColor) );
                setColorToButtons(qColor);
            }
        });

        // Initialize with a default color
        oneColorLayout->addWidget(labelColor);
        ColorPalette palette;
        auto defaultColor = palette.GetColorAt(1);
        emit labelsOneColorChanged(defaultColor);
        QColor qDefaultColor;
        qDefaultColor.setRgbF(defaultColor[0], defaultColor[1], defaultColor[2]);
        QString colorStr = qDefaultColor.name();
        QString style = "QPushButton { background: %1; }";
        labelColor->setStyleSheet( style.arg(colorStr) );
        setColorToButtons(qDefaultColor);
    }
    else
    {
        removeOneColorButton();
        emit setDefaultLabelsColors();
        setDefaultColorsToButtons();                
    }
}



