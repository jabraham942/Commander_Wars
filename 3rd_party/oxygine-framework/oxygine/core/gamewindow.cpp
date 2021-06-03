#include "3rd_party/oxygine-framework/oxygine/core/gamewindow.h"
#include "3rd_party/oxygine-framework/oxygine/core/oxygine.h"
#include "3rd_party/oxygine-framework/oxygine/core/gl/VideoDriverGLES20.h"

#include "3rd_party/oxygine-framework/oxygine/KeyEvent.h"
#include "3rd_party/oxygine-framework/oxygine/actor/Stage.h"
#include "3rd_party/oxygine-framework/oxygine/STDRenderDelegate.h"
#include "3rd_party/oxygine-framework/oxygine/PostProcess.h"
#include "3rd_party/oxygine-framework/oxygine/MaterialCache.h"
#include "3rd_party/oxygine-framework/oxygine/PostProcess.h"
#include "3rd_party/oxygine-framework/oxygine/STDRenderDelegate.h"
#include "3rd_party/oxygine-framework/oxygine/actor/Stage.h"
#include "3rd_party/oxygine-framework/oxygine/actor/TextField.h"
#include "3rd_party/oxygine-framework/oxygine/res/CreateResourceContext.h"
#include "3rd_party/oxygine-framework/oxygine/res/ResAtlas.h"
#include "3rd_party/oxygine-framework/oxygine/res/ResBuffer.h"
#include "3rd_party/oxygine-framework/oxygine/res/ResFontBM.h"
#include "3rd_party/oxygine-framework/oxygine/res/Resources.h"
#include "3rd_party/oxygine-framework/oxygine/STDRenderer.h"
#include "3rd_party/oxygine-framework/oxygine/Clock.h"
#include "3rd_party/oxygine-framework/oxygine/STDRenderDelegate.h"

#include <QMouseEvent>
#include <QTimerEvent>
#include <QMutexLocker>
#include <qapplication.h>

#include "coreengine/console.h"

namespace oxygine
{
    GameWindow* GameWindow::_window = nullptr;

    GameWindow::GameWindow()
        : m_dispatcher(spEventDispatcher::create())
    {
        setObjectName("GameWindow");
        QSurfaceFormat newFormat = format();
        newFormat.setProfile(QSurfaceFormat::CoreProfile);
        newFormat.setSamples(2);    // Set the number of samples used for multisampling
        setFormat(newFormat);
        _window = this;
        m_mainHandle = QThread::currentThreadId();
        connect(this, &GameWindow::sigLoadSingleResAnim, this, &GameWindow::loadSingleResAnim, Qt::BlockingQueuedConnection);
        connect(this, &GameWindow::sigLoadRessources, this, &GameWindow::loadRessources, Qt::QueuedConnection);
    }

    GameWindow::~GameWindow()
    {
        m_dispatcher->removeAllEventListeners();

        rsCache().reset();
        rsCache().setDriver(nullptr);

        clearPostProcessItems();
        PostProcess::freeShaders();

        MaterialCache::mc().clear();

        STDRenderer::release();
        STDRenderDelegate::instance = nullptr;
        IVideoDriver::instance = nullptr;

        Input::instance.cleanup();

        if (Stage::instance)
        {
            Stage::instance->cleanup();
        }
        Stage::instance = nullptr;

        Resources::unregisterResourceType("atlas");
        Resources::unregisterResourceType("buffer");
        Resources::unregisterResourceType("font");
        Resources::unregisterResourceType("bmfc_font");
        Resources::unregisterResourceType("sdfont");
    }

    void GameWindow::timerEvent(QTimerEvent *)
    {
        // Request an update
        update();
    }

    void GameWindow::updateData()
    {
        timeMS duration = IVideoDriver::_stats.duration;
        IVideoDriver::_stats = IVideoDriver::Stats();
        IVideoDriver::_stats.duration = duration;
    }

    void GameWindow::paintGL()
    {
        updateData();
        if (m_pauseMutex.tryLock())
        {
            oxygine::getStage()->updateStage();
            if (beginRendering())
            {
                QColor clearColor(0, 0, 0, 255);
                QSize size = oxygine::GameWindow::getWindow()->size();
                oxygine::Rect viewport(oxygine::Point(0, 0), oxygine::Point(size.width(), size.height()));
                // Render all actors inside the stage. Actor::render will also be called for all its children
                oxygine::getStage()->renderStage(clearColor, viewport);
                swapDisplayBuffers();
            }
            m_pauseMutex.unlock();
        }
        // check for termination
        if (m_quit)
        {
            QApplication::exit();
        }
    }

    bool GameWindow::beginRendering()
    {
        if (!m_renderEnabled)
        {
            return false;
        }

        bool ready = STDRenderer::isReady();
        if (ready)
        {
            rsCache().reset();
            IVideoDriver::_stats.start = Clock::getTimeMS();
            updatePortProcessItems();
            rsCache().reset();
        }
        else
        {
            Console::print("!ready", Console::eDEBUG);
        }

        return ready;
    }

    bool GameWindow::isReady2Render()
    {
        if (!m_renderEnabled)
        {
            return false;
        }
        return STDRenderer::isReady();
    }

    void GameWindow::swapDisplayBuffers()
    {
        IVideoDriver::_stats.duration = Clock::getTimeMS() - IVideoDriver::_stats.start;
    }

    float GameWindow::getGamma() const
    {
        return m_gamma;
    }

    void GameWindow::setGamma(float gamma)
    {
        m_gamma = gamma;
    }

    float GameWindow::getBrightness() const
    {
        return m_brightness;
    }

    void GameWindow::setBrightness(float brightness)
    {
        m_brightness = brightness / 100.0f;
    }

    void GameWindow::registerResourceTypes()
    {
        Resources::registerResourceType(ResAtlas::create, "atlas");
        Resources::registerResourceType(ResBuffer::create, "buffer");
        Resources::registerResourceType(ResFontBM::create, "font");
        Resources::registerResourceType(ResFontBM::createBM, "bmfc_font");
        Resources::registerResourceType(ResFontBM::createSD, "sdfont");
    }

    void GameWindow::initializeGL()
    {
        initializeOpenGLFunctions();
        if (!hasOpenGLFeature(QOpenGLFunctions::Shaders))
        {
            Console::print("Shaders are not supported by open gl. This may result in a black screen.", Console::eWARNING);
        }
        if (!hasOpenGLFeature(QOpenGLFunctions::Multitexture))
        {
            Console::print("Multitextures are not supported by open gl. This may result in a black screen.", Console::eWARNING);
        }
        // init oxygine engine
        Console::print("initialize oxygine", Console::eDEBUG);
        IVideoDriver::instance = spVideoDriverGLES20::create();

        IVideoDriver::instance->setDefaultSettings();

        rsCache().setDriver(IVideoDriver::instance.get());

        STDRenderer::initialize();

        registerResourceTypes();

        STDRenderer::instance = spSTDRenderer::create();
        STDRenderDelegate::instance = spSTDRenderDelegate::create();
        Material::null       = spNullMaterialX::create();
        Material::current = Material::null;

        STDRenderer::current = STDRenderer::instance;

        // Create the stage. Stage is a root node for all updateable and drawable objects
        oxygine::Stage::instance = oxygine::spStage::create();
        emit sigLoadRessources();
    }

    void GameWindow::resizeGL(qint32 w, qint32 h)
    {
        Console::print("core::restore()", Console::eDEBUG);
        IVideoDriver::instance->restore();
        STDRenderer::restore();
        Restorable::restoreAll();
        oxygine::Stage::instance->setSize(w, h);
        Console::print("core::restore() done", Console::eDEBUG);
    }

    void GameWindow::loadResAnim(oxygine::spResAnim pAnim, QImage & image, qint32 columns, qint32 rows, float scaleFactor, bool addTransparentBorder)
    {
        if (QThread::currentThreadId() == m_mainHandle)
        {
            loadSingleResAnim(pAnim, image, columns, rows, scaleFactor, addTransparentBorder);
        }
        else
        {
            emit sigLoadSingleResAnim(pAnim, image, columns, rows, scaleFactor, addTransparentBorder);
        }
    }

    void GameWindow::loadSingleResAnim(oxygine::spResAnim pAnim, QImage & image, qint32 columns, qint32 rows, float scaleFactor, bool addTransparentBorder)
    {
        if (pAnim.get() != nullptr)
        {
            pAnim->init(image, columns, rows, scaleFactor, addTransparentBorder);
        }
    }

    void GameWindow::mousePressEvent(QMouseEvent *event)
    {
        MouseButton b = MouseButton_Left;
        switch (event->button())
        {
            case Qt::MouseButton::LeftButton:
            {
                b = MouseButton_Left;
                break;
            }
            case Qt::MouseButton::MiddleButton:
            {
                b = MouseButton_Middle;
                break;
            }
            case Qt::MouseButton::RightButton:
            {
                b = MouseButton_Right;
                break;
            }
            default:
            {
                // do nothing
            }
        }
        emit sigMousePressEvent(b, event->position().x(), event->position().y());
        m_pressDownTime.start();
        m_pressDownTimeRunning = true;
    }

    void GameWindow::mouseReleaseEvent(QMouseEvent *event)
    {
        MouseButton b = MouseButton_Left;
        switch (event->button())
        {
            case Qt::MouseButton::LeftButton:
            {
                b = MouseButton_Left;
                break;
            }
            case Qt::MouseButton::MiddleButton:
            {
                b = MouseButton_Middle;
                break;
            }
            case Qt::MouseButton::RightButton:
            {
                b = MouseButton_Right;
                break;
            }
            default:
            {
                // do nothing
            }
        }
        emit sigMouseReleaseEvent(b, event->position().x(), event->position().y());
        std::chrono::milliseconds time = std::chrono::milliseconds(m_pressDownTime.elapsed());
        if (time > std::chrono::milliseconds(500))
        {
            emit sigMouseLongPressEvent(b, event->position().x(), event->position().y());
        }
        m_pressDownTimeRunning = false;
    }

    void GameWindow::wheelEvent(QWheelEvent *event)
    {
        emit sigWheelEvent(event->angleDelta().x(), event->angleDelta().y());
    }
    void GameWindow::mouseMoveEvent(QMouseEvent *event)
    {
        emit sigMouseMoveEvent(event->position().x(), event->position().y());
    }

    void GameWindow::touchEvent(QTouchEvent *event)
    {
        QList<QTouchEvent::TouchPoint> touchPoints = event->points();
        switch (event->type())
        {
            case QEvent::TouchBegin:
            {
                if (touchPoints.count() == 1 )
                {
                    const QTouchEvent::TouchPoint &touchPoint0 = touchPoints.first();
                    emit sigMousePressEvent(MouseButton_Left, touchPoint0.position().x(), touchPoint0.position().y());
                    m_pressDownTime.start();
                    m_pressDownTimeRunning = true;
                    m_touchMousePressSent = true;
                }
            }
            case QEvent::TouchUpdate:
            {
                handleZoomGesture(touchPoints);
                if (touchPoints.count() == 1 )
                {
                    const QTouchEvent::TouchPoint &touchPoint0 = touchPoints.first();
                    std::chrono::milliseconds time = std::chrono::milliseconds(m_pressDownTime.elapsed());
                    if (m_pressDownTimeRunning &&
                        touchPoint0.pressPosition() == touchPoint0.position() &&
                        time > std::chrono::milliseconds(500))
                    {
                        emit sigMouseLongPressEvent(MouseButton_Left, touchPoint0.position().x(), touchPoint0.position().y());
                        m_longPressSent = true;
                    }
                    else
                    {
                        emit sigMouseMoveEvent(touchPoint0.position().x(), touchPoint0.position().y());
                    }
                }
                break;
            }
            case QEvent::TouchEnd:
            {
                if (touchPoints.count() == 1)
                {
                    const QTouchEvent::TouchPoint &touchPoint0 = touchPoints.first();
                    std::chrono::milliseconds time = std::chrono::milliseconds(m_pressDownTime.elapsed());
                    if (!m_longPressSent)
                    {
                        if (time > std::chrono::milliseconds(500) && m_pressDownTimeRunning)
                        {
                            emit sigMouseLongPressEvent(MouseButton_Left, touchPoint0.position().x(), touchPoint0.position().y());
                        }
                    }
                }
                if (m_touchMousePressSent)
                {
                    const QTouchEvent::TouchPoint &touchPoint0 = touchPoints.first();
                    emit sigMouseReleaseEvent(MouseButton_Left, touchPoint0.position().x(), touchPoint0.position().y());

                }
                m_pressDownTimeRunning = false;
                m_touchMousePressSent = false;
                m_longPressSent = false;
                m_lastZoomValue = 1.0f;
            }
            default:
                break;
        }
    }

    void GameWindow::handleZoomGesture(QList<QTouchEvent::TouchPoint> & touchPoints)
    {
        if (touchPoints.count() == 2)
        {
            constexpr float minZoomOutGesture = 0.3f;
            constexpr float minZoomInGesture = 0.3f;
            // determine scale factor
            const QTouchEvent::TouchPoint &touchPoint0 = touchPoints.first();
            const QTouchEvent::TouchPoint &touchPoint1 = touchPoints.last();
            qreal scale = QLineF(touchPoint0.position(), touchPoint1.position()).length() /
                          QLineF(touchPoint0.pressPosition(), touchPoint1.pressPosition()).length();
            if (scale > m_lastZoomValue + 1 / (1 - minZoomInGesture))
            {
                m_lastZoomValue = scale;
                emit sigWheelEvent(1, 1);
            }
            else if (scale < m_lastZoomValue - minZoomOutGesture)
            {
                m_lastZoomValue = scale;
                emit sigWheelEvent(-1, -1);
            }
        }
    }

    spEventDispatcher GameWindow::getDispatcher()
    {
        return m_dispatcher;
    }

    QOpenGLContext* GameWindow::getGLContext()
    {
        return _window->context();
    }

    GameWindow* GameWindow::getWindow()
    {
        return _window;
    }

    bool GameWindow::isEvenScale(qint32 width1, qint32 width2)
    {
        float scale1 = static_cast<float>(width1) / static_cast<float>(width2);
        float scale2 = static_cast<float>(width2) / static_cast<float>(width1);
        if ((fmodf(scale1, 2.0f) == 0.0f) ||
            (fmodf(scale2, 2.0f) == 0.0f))
        {
            return true;
        }
        return false;
    }

    bool GameWindow::hasCursor()
    {
        QPoint position = cursor().pos();
        if (position.x() < x() || position.y() < y() ||
            position.x() > x() + width() || position.y() > y() + height())
        {
            return false;
        }
        return true;
    }
}
