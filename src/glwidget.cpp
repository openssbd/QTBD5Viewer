#include <QMouseEvent>
#include <QCoreApplication>
#include <QColor>
#include <math.h>
#include "glwidget.h"
#include "BD5File.h"

GLWidget::GLWidget(QWidget *parent) : 
    QOpenGLWidget(parent)
{
    m_core = QSurfaceFormat::defaultFormat().profile() == QSurfaceFormat::CoreProfile;
}

GLWidget::~GLWidget()
{
    gluDeleteQuadric(qobj);
}

QSize GLWidget::minimumSizeHint() const
{
    return QSize(50, 50);
}

QSize GLWidget::sizeHint() const
{
    return QSize(400, 400);
}

static void qNormalizeAngle(int &angle)
{
    while (angle < 0)
        angle += 360 * 16;
    while (angle > 360 * 16)
        angle -= 360 * 16;
}

void GLWidget::setTimeToVisualize(int time)
{
    if (snapshots.empty())
        return;
    try
    {
        current_snapshot = snapshots.at(time);
        if (scales.TScale() > 0.0)
            emit snapshotTime( current_snapshot.Time() * scales.TScale() );
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
    try
    {
        BD5File file = BD5File(filePath.toStdString());
        snapshots = file.Read();
        scales = file.Scales();
        boundaries = file.GetBoundaries();
        boundaries.maxX = boundaries.maxX * scales.XScale();
        boundaries.maxY = boundaries.maxY * scales.YScale();
        boundaries.maxZ = boundaries.maxZ * scales.ZScale();
        boundaries.minX = boundaries.minX * scales.XScale();        
        boundaries.minY = boundaries.minY * scales.YScale();        
        boundaries.minZ = boundaries.minZ * scales.ZScale();
        gridSpacing = getGridSpacing(boundaries);
        initializeViewer();
        current_snapshot = snapshots.front();
        emit readTimeUnitsInfo( (snapshots.size() - 1), scales.TUnit() );
        update();
    } catch(...)
    {
        cout << "Review log file for errors." << '\n';
    }
}

void GLWidget::resetPosition()
{
    initializeViewer();
    update();
}

void GLWidget::setGeometryColor(QColor color)
{
    redColor = color.redF();
    greenColor = color.greenF();
    blueColor = color.blueF();
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
    if (scales.Type() == SpaceTime_t::ThreeDimension || scales.Type() == SpaceTime_t::ThreeDimensionAndTime)
        glRotatef(m_zRot, 0.0, 0.0, 1.0);
    glTranslatef(-boundaries.maxX/2, -boundaries.maxY/2, -boundaries.maxZ/2);

    for (auto& object : current_snapshot.GetObjects() )
    {
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
                    glColor3f(redColor, greenColor, blueColor);
                    for (auto &entity : entities)
                    {
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
                    glColor3f(redColor, greenColor, blueColor);
                    for (auto &entity : entities)
                    {
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
                    glColor3f(redColor, greenColor, blueColor);
                    for (auto &entity : entities)
                    {
                        glPushMatrix();
                        glTranslatef(scales.XScale() * entity.X(),
                                     scales.YScale() * entity.Y(),
                                     scales.ZScale() * entity.Z());
                        gluSphere(qobj, entity.Radius(), 15, 10);
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
    glFlush();
}


void GLWidget::resizeGL(int w, int h)
{
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, (GLfloat)w / (GLfloat)h, 0.01f, 10000.0);
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
        setXRotation(m_xRot + 8 * dy);
        setYRotation(m_yRot + 8 * dx);
    } else if (event->buttons() & Qt::RightButton) {
        // TODO: translation
    }
    m_lastPos = event->position().toPoint();
}

void GLWidget::wheelEvent(QWheelEvent *event)
{
    QPoint numPixels = event->pixelDelta();

    if (!numPixels.isNull()) {
        cameraZ += numPixels.y() * deltaZ;
    }
    event->accept();
    update();
}

void GLWidget::initializeViewer()
{
    m_xRot = 0.0;
    m_yRot = 0.0;
    m_zRot = 0.0;
    // cout << "x " << boundaries.minX << " X " << boundaries.maxX;
    // cout << " y " << boundaries.minY << " Y " << boundaries.maxY;
    // cout << " z " << boundaries.minZ << " Z " << boundaries.maxZ << endl;
    // cout << " Scales " << scales.XScale() << " " << scales.YScale() << " " << scales.ZScale() << endl;
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
