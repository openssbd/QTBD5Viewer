#include <QMouseEvent>
#include <QCoreApplication>
#include <QColor>
#include <QEventLoop>
#include <QTimer>
#include <math.h>
#include "glwidget.h"
#include "BD5File.h"
#include "utils.h"

GLWidget::GLWidget(QWidget *parent) : 
    QOpenGLWidget(parent)
{
    m_core = QSurfaceFormat::defaultFormat().profile() == QSurfaceFormat::CoreProfile;
    QList<int> gamepads = QGamepadManager::instance()->connectedGamepads();
    if (!gamepads.isEmpty()) {
        qDebug() << "Gamepad detected ";
        m_gamepad = new QGamepad(*gamepads.begin(), this);
        initializeGamepad(this);
    } else {
        qDebug() << "No gamepads detected ";
    }
}

GLWidget::~GLWidget()
{
    gluDeleteQuadric(qobj);
    if (m_gamepad != nullptr) {
        delete m_gamepad;
    }
}

QSize GLWidget::minimumSizeHint() const
{
    return QSize(200, 200);
}

QSize GLWidget::sizeHint() const
{
    return QSize(800, 600);
}

static void qNormalizeAngle(int &angle)
{
    while (angle < 0)
        angle += 360 * 16;
    while (angle > 360 * 16)
        angle -= 360 * 16;
}

/***
 * From here starts the Qt slots functions
*/
void GLWidget::setTimeToVisualize(int time)
{
    if (snapshots.empty())
        return;
    try
    {
        emit snapshotTime( snapshots[time].Time() * scales.TScale() );
        auto names = file.GetLabelsAtTime(time);
        // for (auto &obj: names) {
        //     for (auto &name: obj) {
        //         cout << "TimeToVisualize " << time << " " << name << " ";
        //     }
        // }
        // cout << endl;
        setCurrentSnapshot(time, snapshots[time], names);

        update();
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
}

void GLWidget::setXRotation(int angle)
{
    qNormalizeAngle(angle);
    if (angle != m_xRot) {
        m_xRot = angle;
        update();
    }
}

void GLWidget::setYRotation(int angle)
{
    qNormalizeAngle(angle);
    if (angle != m_yRot) {
        m_yRot = angle;
        update();
    }
}

void GLWidget::setZRotation(int angle)
{
    qNormalizeAngle(angle);
    if (angle != m_zRot) {
        m_zRot = angle;
        update();
    }
}

void GLWidget::setGeometry(QString filePath)
{
    snapshots.clear();
    try
    {
        snapshots = file.Read(filePath.toStdString());

        scales = file.Scales();
        boundaries = file.GetBoundaries();
        boundaries.maxX = boundaries.maxX * scales.XScale();
        boundaries.maxY = boundaries.maxY * scales.YScale();
        boundaries.maxZ = boundaries.maxZ * scales.ZScale();
        boundaries.minX = boundaries.minX * scales.XScale();        
        boundaries.minY = boundaries.minY * scales.YScale();        
        boundaries.minZ = boundaries.minZ * scales.ZScale();
        gridSpacing = getGridSpacing(boundaries);
        if (file.HasTracks()) {
            tracks = file.GetTracks();
        }
    } 
    catch (const std::out_of_range& oor) {
        cout << "Out of Range " << oor.what() << endl;
        return;
    }
    catch (const std::exception& e) {
        cout << "Error at reading file " << endl;
        cout << e.what() << endl;
        return;
    }

    initializeViewer();
    auto objNames = file.ObjectsNames();
    objsVisibility.clear();
    for (auto &obj : objNames)
    {
        objsVisibility[obj] = true;
    }

    emit objectsNames(file.ObjectsNames());

    auto labelNames = file.GetLabelsAtTime(0);
    // cout << "Labels on glWidget" << endl;
    // for (auto& obj: labelNames) {
    //     cout << "Obj ";
    //     for (auto& name: obj) {
    //         cout << name << " ";
    //     }
    //     cout << endl;
    // }

    setCurrentSnapshot(0, snapshots.front(), labelNames);

    emit readTimeUnitsInfo((snapshots.size() - 1), scales.TUnit());

    update();
}

void GLWidget::resetPosition()
{
    initializeViewer();
    update();
}

void GLWidget::setAxesFlag(bool flag)
{
    showAxes = flag;
    update();
}

void GLWidget::setGrid2DFlag(bool flag)
{
    showGrid2D = flag;
    update();
}

void GLWidget::setGrid3DFlag(bool flag)
{
    showGrid3D = flag;
    update();
}

void GLWidget::setTracksFlag(bool flag)
{
    showTracks = flag;
    update();
}

void GLWidget::setObjectShowFlag(string key, bool flag) {
    try {
        objsVisibility[key] = flag;
        update(); 
    }
    catch (const std::exception& e) {
        cout << "GLWidget. Object key not found. " << e.what() << endl;
    }
}

void GLWidget::setLabelColor(int objIndex, string key, vector<float> color) {
    try {
        currentSnapshot.labelsColors[objIndex][key] = color;
        update();
    }
    catch (const std::exception& e) {
        cout << "GLWidget. Color object key not found. " << e.what() << endl;
    }
}

void GLWidget::setLabelsOneColor(vector<float> color) {
    for (auto& obj: currentSnapshot.labelsColors)
    {
        for (auto& [key, val]: obj)
        {
            val = color;
        }
    }
    update();
}

void GLWidget::setDefaultLabelsColors()
{
    auto labels = file.GetLabelsAtTime(currentSnapshot.index);
    setDefaultPaletteColors(labels);
    update();
}
/**
 * End of Qt slots functions
*/

void GLWidget::initializeGL()
{
    initializeOpenGLFunctions();
    qobj = gluNewQuadric();
    GLfloat mat_ambient[] = { 0.5, 0.5, 0.5, 1.0 };
    GLfloat mat_specular[] = { 0.5, 0.5, 0.5, 1.0 };
    GLfloat mat_shininess[] = { 50.0 };
    GLfloat light_position[] = { 1.0, 1.0, 1.0, 0.0 };
    GLfloat model_ambient[] = { 0.5, 0.5, 0.5, 1.0 };
    glClearColor(0, 0, 0, 1);
    glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, model_ambient);
}

void GLWidget::paintGL()
{
    if (snapshots.empty()) 
        return;

    try {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glEnable(GL_LIGHTING);

    glEnable(GL_LIGHT0);
    glEnable(GL_SMOOTH);
    glEnable(GL_COLOR_MATERIAL);
    glLoadIdentity();
    glTranslatef( 0.0, 0.0, -cameraZ);
    glRotatef(m_xRot, 1.0, 0.0, 0.0);
    glRotatef(m_yRot, 0.0, 1.0, 0.0);
    if (scales.Type() == SpaceTime_t::ThreeDimension || scales.Type() == SpaceTime_t::ThreeDimensionAndTime) {
        glRotatef(m_zRot, 0.0, 0.0, 1.0);
    }
    float cameraX = boundaries.minX+(boundaries.maxX-boundaries.minX)/2;
    float cameraY = boundaries.minY+(boundaries.maxY-boundaries.minY)/2;
    float cameraZ = boundaries.maxZ/2;

    glTranslatef(-cameraX, -cameraY, -cameraZ);

    auto& current_snapshot = currentSnapshot.snapshot;
    int objIndex = 0;
    for (auto& object : current_snapshot.GetObjects() )
    {
        // Make the object invisible according to the flag
        auto key = file.ObjectsNames().at(objIndex);
        objIndex++;
        if (objsVisibility[key] == false) 
        {
            continue;
        }

        for (auto& subObject : object.GetAllSubObjects() )
        {
            EntityType subObjectType = subObject.first;
            switch (subObjectType)
            {
            case EntityType::Line:
                glDisable(GL_LIGHTING);
                glDisable(GL_LIGHT0);
            case EntityType::Face:
                {
                auto &entitiesBySID = subObject.second;
                for (auto &entities : entitiesBySID)
                {
                    if (subObjectType == EntityType::Line)
                        glBegin(GL_LINE_STRIP);
                    if (subObjectType == EntityType::Face)
                        glBegin(GL_POLYGON);
                    for (auto &entity : entities)
                    {
                        // Eliminate last element used for storing the polyline box center
                        if (&entity == &entities.back()) {
                            continue;
                        } 

                        defineGLColor(entity.Label);
                        switch (scales.Type())
                        {
                            case SpaceTime_t::TwoDimension:
                            case SpaceTime_t::TwoDimensionAndTime:
                                glVertex2f(scales.XScale() * entity.X(),
                                           scales.YScale() * entity.Y() );
                                break;
                            case SpaceTime_t::ThreeDimension:
                            case SpaceTime_t::ThreeDimensionAndTime:
                                glVertex3f(scales.XScale() * entity.X(),
                                           scales.YScale() * entity.Y(),
                                           scales.ZScale() * entity.Z());
                                break;
                            default:
                                break;
                        }
                    }
                    glEnd();
                }
                }
            break;
            case EntityType::Point:
                {
                auto &entitiesBySID = subObject.second;
                {
                    auto &entities = entitiesBySID.front();
                    glPointSize(pointSize);
                    glBegin(GL_POINTS);

                    for (auto &entity : entities)
                    {
                        defineGLColor(entity.Label);
                        switch(scales.Type())
                        {
                            case SpaceTime_t::TwoDimension:
                            case SpaceTime_t::TwoDimensionAndTime:
                                glVertex2f(scales.XScale() * entity.X(),
                                           scales.YScale() * entity.Y());
                                break;
                            case SpaceTime_t::ThreeDimension:
                            case SpaceTime_t::ThreeDimensionAndTime:
                                glVertex3f(scales.XScale() * entity.X(),
                                           scales.YScale() * entity.Y(),
                                           scales.ZScale() * entity.Z());
                                break;
                            default:
                                break;
                        }

                    }
                    glEnd();
                }
                }
                break;
            case EntityType::Circle:
                break;
            case EntityType::Sphere:
                {
                gluQuadricDrawStyle(qobj, GLU_FILL);
                gluQuadricNormals(qobj, GLU_SMOOTH);
                auto &entitiesBySID = subObject.second;
                {
                    auto &entities = entitiesBySID.front();
                    for (auto &entity : entities)
                    {
                        defineGLColor(entity.Label);
                        glPushMatrix();
                        glTranslatef(scales.XScale() * entity.X(),
                                     scales.YScale() * entity.Y(),
                                     scales.ZScale() * entity.Z());
                        // gluSphere(qobj, entity.Radius(), 15, 10);
                        gluSphere(qobj, entity.Radius(), 6, 4);
                        glPopMatrix();
                    }
                }
                }
                break;
            case EntityType::Undefined:
                break;
            }
        }
    }

    if (showAxes)
        drawAxes();
    if (showGrid2D)
        drawGrid2D();
    if (showGrid3D)
        drawGrid3D();
    if (showTracks && (tracks.size() > 0) ) {
        drawTracks(currentSnapshot.index);
    }
    glFlush();
    } catch (const std::exception& e) {
            cout << e.what() << endl;
    }

}

void GLWidget::resizeGL(int w, int h)
{
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, (GLfloat)w / (GLfloat)h, 0.01f, 100000.0);
    // glOrtho(-10.0, 70.0, -10.0, 60.0, 0, 10000);
    glMatrixMode(GL_MODELVIEW);
}

void GLWidget::mousePressEvent(QMouseEvent *event)
{
    m_lastPos = event->position().toPoint();
}

void GLWidget::mouseMoveEvent(QMouseEvent *event)
{
    int dx = event->position().toPoint().x() - m_lastPos.x();
    int dy = event->position().toPoint().y() - m_lastPos.y();

    if (event->buttons() & Qt::LeftButton) {
        setXRotation(m_xRot + 4 * dy);
        setYRotation(m_yRot + 4 * dx);
    } else if (event->buttons() & Qt::RightButton) {

    }
    m_lastPos = event->position().toPoint();
}

void GLWidget::wheelEvent(QWheelEvent *event)
{
    QPoint numPixels = event->angleDelta();

    if (!numPixels.isNull()) {
        cameraZ += numPixels.y() * deltaZ;
    }
    event->accept();
    update();
}

void GLWidget::initializeViewer()
{
    // TODO translate the camera, we are moving in Z axis.
    m_xRot = 0.0;
    m_yRot = 0.0;
    m_zRot = 0.0;
    cout << "x " << boundaries.minX << " X " << boundaries.maxX;
    cout << " y " << boundaries.minY << " Y " << boundaries.maxY;
    cout << " z " << boundaries.minZ << " Z " << boundaries.maxZ << endl;
    cout << " Scales " << scales.XScale() << " " << scales.YScale() << " " << scales.ZScale() << endl;
    if ((boundaries.maxZ == boundaries.minZ) && (boundaries.maxZ == 0.0)) {
        cameraZ = boundaries.maxX / 2;
        deltaZ = 1.0;
    }
    else
    {
        cameraZ = (boundaries.maxZ - boundaries.minZ) * 4.0;
        deltaZ = (boundaries.maxZ - boundaries.minZ) / 100.0;
    }
}

void GLWidget::initializeGamepad(QWidget* w)
{
    if (m_gamepad == nullptr) {
        cout << "Gamepad null pointer";
        return;
    }
    connect(m_gamepad, &QGamepad::axisLeftXChanged, w, 
            [&](double value) { 
                // qDebug() << "Jockstick Left X" << value;
                cameraZ -= deltaZ * value;
                update();
                if ((value >= 1.0) || (value <= -0.99))
                {
                    auto eval = [game=m_gamepad, this]() -> float
                    {
                        float newValue = game->axisLeftX();
                        cameraZ -= deltaZ * newValue;
                        update();
                        return newValue;
                    };
                    lockGamepadJoystick(value, eval);
                }
            });
    connect(m_gamepad, &QGamepad::axisLeftYChanged, w, 
            [&](double value) { 
                // qDebug() << "Jockstick Left Y" << value; 
                cameraZ += deltaZ * value;
                update();
                if ((value >= 1.0) || (value <= -0.99))
                {
                    auto eval = [game=m_gamepad, this]() -> float
                    {
                        float newValue = game->axisLeftY();
                        cameraZ += deltaZ * newValue;
                        update();
                        return newValue;
                    };
                    lockGamepadJoystick(value, eval);
                }                
            });
    connect(m_gamepad, &QGamepad::axisRightXChanged, w, 
            [&](double value) {          
                // qDebug() << "Jockstick Right X" << value;
                setYRotation(m_yRot + 4 * value);
                if ((value >= 1.0) || (value <= -0.99))
                {
                    auto eval = [game=m_gamepad, this]() -> float
                    {
                        float newValue = game->axisRightX();
                        setYRotation(m_yRot + 4 * newValue);
                        return newValue;
                    };
                    lockGamepadJoystick(value, eval);
                }
            });
    connect(m_gamepad, &QGamepad::axisRightYChanged, w, 
            [&](double value) { 
                // qDebug() << "Jocsktick Right Y" << value;
                setXRotation(m_xRot + 4 * value);
                if ((value >= 1.0) || (value <= -0.99))
                {
                    auto eval = [game=m_gamepad, this]() -> float
                    {
                        float newValue = game->axisRightY();
                        setXRotation(m_xRot + 4 * newValue);
                        return newValue;
                    };
                    lockGamepadJoystick(value, eval);
                }
            });
    // connect(m_gamepad, &QGamepad::buttonAChanged, w, [](bool pressed)
    //         { 
    //             qDebug() << "Button A" << pressed; 
    //         });
    // connect(m_gamepad, &QGamepad::buttonBChanged, w, [](bool pressed)
    //         { 
    //             qDebug() << "Button B" << pressed; 
    //         });
    // connect(m_gamepad, &QGamepad::buttonCenterChanged, w, [](bool pressed)
    //         { 
    //             qDebug() << "Button Center" << pressed;
    //         });
    // connect(m_gamepad, &QGamepad::buttonXChanged, w, [](bool pressed)
    //         { 
    //             qDebug() << "Button X" << pressed; 
    //         });
    // connect(m_gamepad, &QGamepad::buttonYChanged, w, [](bool pressed)
    //         { 
    //             qDebug() << "Button Y" << pressed; 
    //         });
    // connect(m_gamepad, &QGamepad::buttonL1Changed, w, [](bool pressed)
    //         { 
    //             qDebug() << "Button L1" << pressed; 
    //         });
    // connect(m_gamepad, &QGamepad::buttonR1Changed, w, [](bool pressed)
    //         { 
    //             qDebug() << "Button R1" << pressed;
    //         });
    // connect(m_gamepad, &QGamepad::buttonL2Changed, w, [](double value)
    //         { 
    //             qDebug() << "Button L2: " << value;
    //         });
    // connect(m_gamepad, &QGamepad::buttonR2Changed, w, [](double value)
    //         { 
    //             qDebug() << "Button R2: " << value;
    //         });
    connect(m_gamepad, &QGamepad::buttonUpChanged, w,
            [&](bool pressed) {
                // qDebug() << "Button Up " << pressed;
                if (pressed) {
                    emit moveToNextTime();
                    if (m_gamepad->buttonR1())
                    {
                        auto eval = [game = m_gamepad, this]() -> bool
                        {
                            bool button = game->buttonUp();
                            if (button)
                            {
                                emit moveToNextTime();
                            }
                            return button;
                        };
                        lockGamepadButton(eval);
                    }
                }                
            });
    connect(m_gamepad, &QGamepad::buttonDownChanged, w,
            [&](bool pressed) { 
                // qDebug() << "Button Down" << pressed;
                if (pressed) {
                    emit moveToPrevTime();
                    if (m_gamepad->buttonR1())
                    {
                        auto eval = [game = m_gamepad, this]() -> bool
                        {
                            bool button = game->buttonDown();
                            if (button)
                            {
                                emit moveToPrevTime();
                            }
                            return button;
                        };
                        lockGamepadButton(eval);
                    }
                }  
            });
    connect(m_gamepad, &QGamepad::buttonLeftChanged, w, 
            [&](bool pressed) { 
                // qDebug() << "Button Left: " << pressed;
                if (pressed) {
                    emit moveToPrevTime();
                    if (m_gamepad->buttonR1())
                    {
                        auto eval = [game = m_gamepad, this]() -> bool
                        {
                            bool button = game->buttonLeft();
                            if (button)
                            {
                                emit moveToPrevTime();
                            }
                            return button;
                        };
                        lockGamepadButton(eval);
                    }
                }
            });
    connect(m_gamepad, &QGamepad::buttonRightChanged, w, [&](bool pressed)
            { 
                // qDebug() << "Button Right: " << pressed;
                if (pressed) {
                    emit moveToNextTime();
                    if (m_gamepad->buttonR1())
                    {
                        auto eval = [game = m_gamepad, this]() -> bool
                        {
                            bool button = game->buttonRight();
                            if (button)
                            {
                                emit moveToNextTime();
                            }
                            return button;
                        };
                        lockGamepadButton(eval);
                    }
                }
            });
    // connect(m_gamepad, &QGamepad::buttonSelectChanged, w, [](bool pressed)
    //         { 
    //             qDebug() << "Button Select" << pressed;
    //         });
    // connect(m_gamepad, &QGamepad::buttonStartChanged, w,
    //         [this](bool pressed) {
    //             qDebug() << "Button Start" << pressed;
    //             resetPosition();
    //         });
    // connect(m_gamepad, &QGamepad::buttonGuideChanged, w, [](bool pressed)
    //         { 
    //             qDebug() << "Button Guide" << pressed;
    //         });
}

void GLWidget::lockGamepadJoystick(float value, std::function<float()> evalFunction)
{
    int timeLapse = 10;

    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, [value, timer, evalFunction]() {
        float newValue = evalFunction();
        if (newValue != value) {
            timer->stop();
        }
    });
    timer->start(timeLapse);
}

void GLWidget::lockGamepadButton(std::function<bool()> evalFunction)
{
    int timeLapse = 100;

    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, [timer, evalFunction]() {
        if (!evalFunction())
        {
            timer->stop();
        }
    });
    timer->start(timeLapse);
}

void GLWidget::drawAxes()
{
    gluQuadricDrawStyle(qobj, GLU_FILL);
    gluQuadricNormals(qobj, GLU_SMOOTH);
    float height = (boundaries.maxX - boundaries.minX)/2;
    float radius = height / 50.0;
    // z axis
    glColor4f(0, 0, 1, 0.5);
    gluCylinder(qobj, radius, radius, height, 15, 10);
    glPushMatrix();
    glTranslatef(0.0, 0.0, height);
    gluCylinder(qobj, radius*2.0, 0, height/10.0, 15, 10);
    glPopMatrix();

    // x axis
    glColor4f(1, 0, 0, 0.5);
    glPushMatrix();
    glRotatef(90.0, 0, 1, 0);
    gluCylinder(qobj, radius, radius, height, 15, 10);
    glTranslatef(0.0, 0.0, height);
    gluCylinder(qobj, radius*2.0, 0, height/10.0, 15, 10);
    glPopMatrix();

    // y axis
    glColor4f(0, 1, 0, 0.5);
    glPushMatrix();
    glRotatef(-90.0, 1, 0, 0);
    gluCylinder(qobj, radius, radius, height, 15, 10);
    glTranslatef(0.0, 0.0, height);
    gluCylinder(qobj, radius*2.0, 0, height/10.0, 15, 10);
    glPopMatrix();
}

void GLWidget::drawPlaneGrid(pair<char, char>axes, int numLines, float freeAxis)
{
    if (axes.first == 'x' && axes.second == 'y')
    {
        for (int i = 0; i <= numLines; i++)
        {
            float val = i * pow(10.0, gridSpacing);
            glVertex3f(val, 0.0, freeAxis * pow(10.0, gridSpacing) );
            glVertex3f(val, numLines * pow(10.0, gridSpacing), freeAxis * pow(10.0, gridSpacing) );
            glVertex3f(0.0, val, freeAxis * pow(10.0, gridSpacing) );
            glVertex3f(numLines * pow(10.0, gridSpacing), val, freeAxis * pow(10.0, gridSpacing) );
        }
    }

    if (axes.first == 'y' && axes.second == 'z')
    {
        for (int i = 0; i <= numLines; i++)
        {
            float val = i * pow(10.0, gridSpacing);
            glVertex3f(freeAxis * pow(10.0, gridSpacing), val, 0.0);
            glVertex3f(freeAxis * pow(10.0, gridSpacing), val, numLines * pow(10.0, gridSpacing));
            glVertex3f(freeAxis * pow(10.0, gridSpacing), 0.0, val);
            glVertex3f(freeAxis * pow(10.0, gridSpacing), numLines * pow(10.0, gridSpacing), val);
        }
    }
}

void GLWidget::drawGrid2D()
{
    pair xyAxis('x', 'y');
    glColor3f(0.5, 0.5, 0.5);
    glBegin(GL_LINES);    
        drawPlaneGrid(xyAxis, linesPerGrid, 0);
    glEnd();
}


void GLWidget::drawGrid3D()
{
    pair xyAxis('x', 'y');
    pair yzAxis('y', 'z');
    glColor3f(0.5, 0.5, 0.5);
    glBegin(GL_LINES);
        if (scales.Type() == SpaceTime_t::ThreeDimension || scales.Type() == SpaceTime_t::ThreeDimensionAndTime)
        {
            for (int i = 0; i <= linesPerGrid; i++) {
                drawPlaneGrid(xyAxis, linesPerGrid, i);
                drawPlaneGrid(yzAxis, linesPerGrid, i);
            }
        }
    glEnd();
}

int GLWidget::getGridSpacing(Boundaries box)
{
    for (int i = 0; i < 10; i++)
    {
        int positive = static_cast<int> (box.maxX - box.minX) / (pow(10.0, i));
        if (positive > 0 && positive < 10)
            return i;
        int negative = static_cast<int> (box.maxX - box.minX) / (pow(10.0, -i));
        if (negative > 0 && negative < 10)
            return -i;
    }
    return 1;
}

void GLWidget::drawTracks(int t)
{
    vector<vector<PointTrack>> filteredTracks;
    for (auto& track : tracks) {
        vector<PointTrack> fTrack (track.size() );
        auto it = std::copy_if(track.begin(), track.end(), fTrack.begin(),
                    [&t](PointTrack& pT){ return (pT.id <= t); });
        fTrack.resize(std::distance(fTrack.begin(), it));
        if (fTrack.size() > 0) {
            filteredTracks.push_back(fTrack);
        }
    }

    int colorIndex = 0;
    ColorPalette palette;

    for (auto& track : filteredTracks ) {
        vector<GLfloat> color = palette.GetColorAt(colorIndex);
        glColor3f(color.at(0), color.at(1), color.at(2));
        colorIndex++;
        glBegin(GL_LINE_STRIP);
        for (auto& line : track) {
            glVertex3f(scales.XScale() * line.x,
                        scales.YScale() * line.y,
                        scales.ZScale() * line.z);
        }
        glEnd();
    }
}

void GLWidget::setCurrentSnapshot(int index, BD5::Snapshot& snapshot, vector<vector<string>> labels)
{
    currentSnapshot.index = index;
    currentSnapshot.snapshot = snapshot;
    setDefaultPaletteColors(labels);
    emit labelsNames(file.ObjectsNames(), labels);
}

void GLWidget::setDefaultPaletteColors(vector<vector<string>> objLabels)
{
    currentSnapshot.labelsColors.clear();
    int i = 0;
    ColorPalette palette;
    for (auto &obj: objLabels)
    {
        map<string, vector<float>> objLabelsColors;
        for (auto &label : obj)
        {
            objLabelsColors[label] = palette.GetColorAt(i);
            i++;
        }
        currentSnapshot.labelsColors.push_back(objLabelsColors);
    }
}

void GLWidget::defineGLColor(string label)
{
    // Default color
    glColor3f(1.0, 1.0, 1.0);

    // Change entity color base on the label color
    if (!label.empty() && !currentSnapshot.labelsColors.empty())
    {
        try
        {
            for (auto &obj: currentSnapshot.labelsColors)
            {
                auto it = obj.find(label);
                if (it != obj.end())
                {
                    auto color = it->second;
                    glColor3fv(color.data());
                }
            }
        }
        catch (const std::out_of_range &ofr)
        {
            cout << "No in range " << ofr.what() << " " << label << " ";
        }
        catch (const std::exception &e)
        {
            cout << "On defineGLColor: " << e.what() << endl;
        }
    }
}