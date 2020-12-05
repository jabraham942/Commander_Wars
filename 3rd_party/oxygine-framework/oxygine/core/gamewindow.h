#ifndef GAMEWINDOW_H
#define GAMEWINDOW_H

#include "../oxygine-include.h"
#include "../PointerState.h"

#include <qopenglwindow.h>
#include <qopenglfunctions.h>
#include <qmutex.h>
#include <qbasictimer.h>
#include <qthread.h>

#include "QKeyEvent"

namespace oxygine
{
    class GameWindow : public QOpenGLWindow, public QOpenGLFunctions
    {
        Q_OBJECT
    public:
        GameWindow();
        ~GameWindow();
        spEventDispatcher getDispatcher();

        static QOpenGLContext* getGLContext();
        static GameWindow*  getWindow();
        bool isReady2Render();
        /**
         * @brief quitGame quits this game
         */
        void quitGame()
        {
            m_quit = true;
        }
        void loadResAnim(oxygine::spResAnim pAnim, const QImage & image);

        virtual bool isWorker() = 0;
        /**
         * @brief isEvenScale
         * @param width1
         * @param width2
         * @return
         */
        static bool isEvenScale(qint32 width1, qint32 width2);
        /**
         * @brief pauseRendering
         */
        void pauseRendering()
        {
            Q_ASSERT(isWorker());
            if (m_pausedCounter == 0)
            {
                m_pauseMutex.lock();
            }
            ++m_pausedCounter;
        }
        /**
         * @brief continueRendering
         */
        void continueRendering()
        {
            Q_ASSERT(isWorker());
            --m_pausedCounter;
            if (m_pausedCounter == 0)
            {
                m_pauseMutex.unlock();
            }
        }


    signals:
        void sigLoadSingleResAnim(oxygine::spResAnim pAnim, const QImage & image);

        void sigMousePressEvent(oxygine::MouseButton button, qint32 x, qint32 y);
        void sigMouseReleaseEvent(oxygine::MouseButton button, qint32 x, qint32 y);
        void sigWheelEvent(qint32 x, qint32 y);
        void sigMouseMoveEvent(qint32 x, qint32 y);
    public slots:
        /**
         * @brief getBrightness
         * @return
         */
        float getBrightness() const;
        /**
         * @brief setBrightness
         * @param brightness
         */
        void setBrightness(float brightness);
        /**
         * @brief getGamma
         * @return
         */
        float getGamma() const;
        /**
         * @brief setGamma
         * @param gamma
         */
        void setGamma(float gamma);
    protected slots:
        void loadSingleResAnim(oxygine::spResAnim pAnim, const QImage & image);
    protected:
        virtual void initializeGL() override;
        virtual void registerResourceTypes();
        virtual void timerEvent(QTimerEvent *) override;
        virtual void paintGL() override;

        virtual void resizeGL(int w, int h) override;
        // input events
        virtual void mousePressEvent(QMouseEvent *event) override;
        virtual void mouseReleaseEvent(QMouseEvent *event) override;
        virtual void wheelEvent(QWheelEvent *event) override;
        virtual void mouseMoveEvent(QMouseEvent *event)override;

        virtual void loadRessources(){}
        void updateData();
        bool beginRendering();
        void swapDisplayBuffers();

        bool _renderEnabled = true;
        spEventDispatcher _dispatcher;

        bool m_quit{false};
        QBasicTimer m_Timer;

        QMutex m_pauseMutex;
        qint32 m_pausedCounter{0};

        static GameWindow* _window;
        float m_brightness{0.0f};
        float m_gamma{1.0f};
    };
}

#endif // GAMEWINDOW_H
