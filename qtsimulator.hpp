#ifndef QTSIMULATOR_HPP
#define QTSIMULATOR_HPP

#include <QtCore/qglobal.h>
#include <QLabel>
#include <QImage>
#include <QElapsedTimer>

#include "qtsimulator.h"

/**
 * @brief The LittlevGl Simulator class
 */
class LittlevGlSimulator : public QLabel
{
protected:
    QImage m_screenBuffer; //!< screen buffer where lvgl draw pixel;
    bool m_screenChanged = false; //!< using to mark screen buffer change
    QElapsedTimer m_elapsedTimer;

    int16_t m_last_x = 0;
    int16_t m_last_y = 0;
    bool m_left_button_down = false;

    LittlevGlSimulator(QWidget * parent = nullptr);
public:

    static LittlevGlSimulator * getInstance();

    /**
     * @brief updateScreen
     * NOTE: it will not immediate update screen untill
     * paintEvent() called by Qt.
     */
    void updateScreen();

    /**
     * @brief Set screen pixel
     */
    void setScreenPixel(int32_t x, int32_t y, uint32_t color);

    bool screenChanged() const;
    void setScreenChanged(bool screenChanged);

    QImage& screenBuffer();

    int16_t last_x() const;
    int16_t last_y() const;
    bool left_button_down() const;

    // QWidget interface

protected:
    virtual void mousePressEvent(QMouseEvent *event) override;
    virtual void mouseReleaseEvent(QMouseEvent *event) override;
    virtual void mouseMoveEvent(QMouseEvent *event) override;
    virtual void timerEvent(QTimerEvent *event) override;
};

#endif // QTSIMULATOR_HPP
