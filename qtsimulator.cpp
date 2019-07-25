#include "qtsimulator.hpp"
#include <QMouseEvent>
#include <lvgl.h>
#include "lv_misc/lv_log.h"
#include "pcfiles.h"
#include <QDebug>
#include <QPointer>

//global variable
LittlevGlSimulator * qtSimulator = nullptr;

/**
 * Print the memory usage periodically
 * @param param
 */
void memory_monitor(void * param)
{
    (void) param; /*Unused*/

    lv_mem_monitor_t mon;
    lv_mem_monitor(&mon);
    printf("used: %6d (%3d %%), frag: %3d %%, biggest free: %6d\n",
           (int)mon.total_size - mon.free_size,
           mon.used_pct,
           mon.frag_pct,
           (int)mon.free_biggest_size);
}

/**
 * Add a log
 * @param level the level of log. (From `lv_log_level_t` enum)
 * @param file name of the file when the log added
 * @param line line number in the source code where the log added
 * @param dsc description of the log
 */
void log_printf(lv_log_level_t level, const char * file, uint32_t line, const char * dsc)
{
    static const char * lvl_prefix[] = {"Trace", "Info", "Warn", "Error"};
    static QTextStream stdoutSteam(stdout);

    QString logStr;
    logStr.sprintf("%s: %s \t(%s #%d)\n", lvl_prefix[level], dsc,  file, line);
    stdoutSteam<<logStr;
    stdoutSteam.flush();
}

void qt_simulator_init()
{
    if(qtSimulator == nullptr)
    {
        // Instance Qt Simulator
        qtSimulator = LittlevGlSimulator::getInstance();
        qtSimulator->show();
    }

    /* Add a display
     * Use the 'monitor' driver which creates window on PC's monitor to simulate a display*/
    lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);            /*Basic initialization*/
    disp_drv.disp_flush = qt_screen_flush;    /*Used when `LV_VDB_SIZE != 0` in lv_conf.h (buffered drawing)*/
    disp_drv.disp_fill = qt_screen_fill;      /*Used when `LV_VDB_SIZE == 0` in lv_conf.h (unbuffered drawing)*/
    disp_drv.disp_map = qt_screen_map;        /*Used when `LV_VDB_SIZE == 0` in lv_conf.h (unbuffered drawing)*/
    lv_disp_drv_register(&disp_drv);

    /* Add the mouse as input device
     * Use the 'mouse' driver which reads the PC's mouse*/
    lv_indev_drv_t indev_drv;
    lv_indev_drv_init(&indev_drv);          /*Basic initialization*/
    indev_drv.type = LV_INDEV_TYPE_POINTER;
    indev_drv.read = qt_mouse_read;         /*This function will be called periodically (by the library) to get the mouse position and state*/
    lv_indev_t * mouse_indev = lv_indev_drv_register(&indev_drv);

    //NOTE: cursor image is dispensable
    /*Set a cursor for the mouse*/
    //LV_IMG_DECLARE(mouse_cursor_icon);                          /*Declare the image file.*/
    //lv_obj_t * cursor_obj =  lv_img_create(lv_scr_act(), NULL); /*Create an image object for the cursor */
    //lv_img_set_src(cursor_obj, &mouse_cursor_icon);             /*Set the image source*/
    //lv_indev_set_cursor(mouse_indev, cursor_obj);               /*Connect the image  object to the driver*/

    /* Register custom log print */
#if LV_LOG_PRINTF == 0
    lv_log_register_print(log_printf);
#endif


    /* init pc file swvice */
    initPcFiledrive('P');

    /* Optional:
     * Create a memory monitor task which prints the memory usage in periodically.*/
    //lv_task_create(memory_monitor, 2000, LV_TASK_PRIO_LOWEST, NULL);
}


void qt_screen_flush(int32_t x1, int32_t y1, int32_t x2, int32_t y2, const lv_color_t *color_p)
{
    if(qtSimulator == nullptr)
    {
        lv_flush_ready();
        return;
    }

    /*Return if the area is out the screen*/
    if(x2 < 0 || y2 < 0 || x1 > LV_HOR_RES - 1 || y1 > LV_VER_RES - 1) {
        lv_flush_ready();
        return;
    }

    for(int32_t y = y1; y <= y2; y++) {
        for(int32_t x = x1; x <= x2; x++) {
            qtSimulator->setScreenPixel(x,y,lv_color_to32(*color_p));
            color_p++;
        }
    }

    /*IMPORTANT! It must be called to tell the system the flush is ready*/
    lv_flush_ready();
}

/**
 * Fill out the marked area with a color
 * @param x1 left coordinate
 * @param y1 top coordinate
 * @param x2 right coordinate
 * @param y2 bottom coordinate
 * @param color fill color
 */
void qt_screen_fill(int32_t x1, int32_t y1, int32_t x2, int32_t y2, lv_color_t color)
{
    if(qtSimulator == nullptr) return;

    /*Return if the area is out the screen*/
    if(x2 < 0) return;
    if(y2 < 0) return;
    if(x1 > LV_HOR_RES - 1) return;
    if(y1 > LV_VER_RES - 1) return;

    /*Truncate the area to the screen*/
    int32_t act_x1 = x1 < 0 ? 0 : x1;
    int32_t act_y1 = y1 < 0 ? 0 : y1;
    int32_t act_x2 = x2 > LV_HOR_RES - 1 ? LV_HOR_RES - 1 : x2;
    int32_t act_y2 = y2 > LV_VER_RES - 1 ? LV_VER_RES - 1 : y2;

    int32_t x;
    int32_t y;
    uint32_t color32 = lv_color_to32(color);

    for(x = act_x1; x <= act_x2; x++) {
        for(y = act_y1; y <= act_y2; y++) {
            qtSimulator->setScreenPixel(x,y,color32);
        }
    }
}

/**
 * Put a color map to the marked area
 * @param x1 left coordinate
 * @param y1 top coordinate
 * @param x2 right coordinate
 * @param y2 bottom coordinate
 * @param color_p an array of colors
 */
void qt_screen_map(int32_t x1, int32_t y1, int32_t x2, int32_t y2, const lv_color_t * color_p)
{

    if(qtSimulator == nullptr) return;

    /*Return if the area is out the screen*/
    if(x2 < 0) return;
    if(y2 < 0) return;
    if(x1 > LV_HOR_RES - 1) return;
    if(y1 > LV_VER_RES - 1) return;

    /*Truncate the area to the screen*/
    int32_t act_x1 = x1 < 0 ? 0 : x1;
    int32_t act_y1 = y1 < 0 ? 0 : y1;
    int32_t act_x2 = x2 > LV_HOR_RES - 1 ? LV_HOR_RES - 1 : x2;
    int32_t act_y2 = y2 > LV_VER_RES - 1 ? LV_VER_RES - 1 : y2;

    int32_t x;
    int32_t y;

    for(y = act_y1; y <= act_y2; y++) {
        for(x = act_x1; x <= act_x2; x++) {
            qtSimulator->setScreenPixel(x,y,lv_color_to32(*color_p));
            color_p++;
        }

        color_p += x2 - act_x2;
    }
}


/**
 * Get the current position and state of the mouse
 * @param data store the mouse data here
 * @return false: because the points are not buffered, so no more data to be read
 */
bool qt_mouse_read(lv_indev_data_t * data)
{
    /*Store the collected data*/
    data->point.x = qtSimulator->last_x();
    data->point.y = qtSimulator->last_y();
    data->state = qtSimulator->left_button_down() ? LV_INDEV_STATE_PR : LV_INDEV_STATE_REL;

    return false;
}

bool LittlevGlSimulator::screenChanged() const
{
    return m_screenChanged;
}

void LittlevGlSimulator::setScreenChanged(bool screenChanged)
{
    m_screenChanged = screenChanged;
}

QImage &LittlevGlSimulator::screenBuffer()
{
    return m_screenBuffer;
}

int16_t LittlevGlSimulator::last_x() const
{
    return m_last_x;
}

int16_t LittlevGlSimulator::last_y() const
{
    return m_last_y;
}

bool LittlevGlSimulator::left_button_down() const
{
    return m_left_button_down;
}

LittlevGlSimulator::LittlevGlSimulator(QWidget *parent)
    :QLabel(parent)
    ,m_screenBuffer(LV_HOR_RES,LV_VER_RES,QImage::Format_RGB32) //getImageFormat(LV_COLOR_DEPTH)
{
    //set simulator screen size
    setFixedSize(QSize(LV_HOR_RES,LV_VER_RES));

    //Tracking mouse while moving
    setMouseTracking(true);

    //start screen fresh 30fps
    startTimer(1000/30);

    m_elapsedTimer.start();
}

LittlevGlSimulator * LittlevGlSimulator::getInstance()
{
    static QPointer<LittlevGlSimulator> instance;
    if(instance.isNull())
    {
        instance = new LittlevGlSimulator();
    }
    return instance.data();
}

void LittlevGlSimulator::updateScreen()
{
    //update while screen has change
    if(screenChanged())
    {
        setPixmap(QPixmap::fromImage(m_screenBuffer));
        setScreenChanged(false);
    }
}

void LittlevGlSimulator::setScreenPixel(int32_t x, int32_t y , uint32_t color)
{
    //The performance is low
    //maybe we shoule use bits() to direct operation pixel data
    m_screenBuffer.setPixel(x,y,color);
    setScreenChanged(true);
}


void LittlevGlSimulator::mousePressEvent(QMouseEvent *event)
{
    m_last_x = event->pos().x();
    m_last_y = event->pos().y();
    m_left_button_down = true;
}

void LittlevGlSimulator::mouseReleaseEvent(QMouseEvent *event)
{
    m_last_x = event->pos().x();
    m_last_y = event->pos().y();
    m_left_button_down = false;
}

void LittlevGlSimulator::mouseMoveEvent(QMouseEvent *event)
{
    m_last_x = event->pos().x();
    m_last_y = event->pos().y();
}

void LittlevGlSimulator::timerEvent(QTimerEvent *event)
{

    /*Tell LittelvGL that how many milliseconds were elapsed*/
    lv_tick_inc(m_elapsedTimer.restart());
    //call lvgl task handel
    lv_task_handler();
    //update simulator screen
    updateScreen();
}
