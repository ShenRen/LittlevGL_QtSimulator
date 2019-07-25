
/**
 * @file main
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <lvtask.hpp>
#include <lvlang.hpp>
#include "lvgl/lvgl.h"

#include <QApplication>

#include "qtsimulator.h"
#include "qtsimulator.hpp"
#include "../marlinsimulator.h"

///////////PC FILE //////////////////
#include <pcfiles.h>


///////////CPP_PORT /////////////////
#include <lvapplication.h>
#include <themes/lvtheme.h>


/////////// MARLIN UI /////////////////
#include <QTimer>
#include <marlinui.h>
#include <screen.h>

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
void memory_monitor(void * param);
void startmemory_monitor(void * param);

//事件函数
void onIdle();

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void marlinUiInit(void){}
void marlinUiRun(void){}

int arduinoSetup(void);
int arduinoLoop(void);

class ArduinoLoopTimer : public QTimer
{
public:
    ArduinoLoopTimer()
        :QTimer()
    {
        connect(this,&ArduinoLoopTimer::timeout,this,&ArduinoLoopTimer::doArduinoLoop);
    }

public Q_SLOTS:
    void doArduinoLoop()
    {
        arduinoLoop();
    }
};

#define NO_MARLIN_RUN


int main(int argc, char ** argv)
{
    QApplication app(argc,argv);

    LVApplication lvapp(qt_simulator_init);

    arduinoSetup();

    MarlinUi * marlinUi = MarlinUi::getInstance(LV_HOR_RES, LV_VER_RES, LV_DPI);
    marlinUi->initMarlinUI();
    //marlinUi->showSplashScreen();
    marlinUi->showHomeScreen();
    //marlinUi->showMotionScreen();
    //marlinUi->showExtrudeScreen();
    //marlinUi->showFileScreen();
    //marlinUi->showNumberInputScreen();
    //marlinUi->showTemperatureScreen();
    //marlinUi->showPrintingScreen();
    //marlinUi->showCheckScreen();

    //测试信号和槽////////////////////////////////////////////
    //虚拟事件触发
    //LVTask virtualIdleTask(1000);
    //virtualIdleTask.setTaskFunc([]()
    //{
    //    LV_LOG_INFO("onIdle();");
    //    //ExtUI::onPrinterKilled("onPrinterKilled");
    //    ExtUI::onIdle();
    //});
    //virtualIdleTask.start();

    //创建槽对象
    //LVSlot idleSlot;
    //idleSlot.setSlotFunc([](LVSignal* signal)
    //{
    //    LV_LOG_INFO("idleSlot");
    //});
    ////关联信号和槽对象
    //connect(&marlinUi->getSignalIdle(),&idleSlot);//,Connection::QueueConnect


    //测试动态语言切换///////////////////////////////
    //LVTask virtualLangChangTask(8000);
    //virtualLangChangTask.setTaskFunc([]()
    //{
    //    uint16_t langId = LVLang::langActived();
    //    LVLang::langSet((langId + 1) % 2 );
    //    langId = LVLang::langActived();
    //    LV_LOG_INFO("LangChang :");
    //    LV_LOG_INFO((langId == 0 ? "chinese" : "english"));
    //    Screen::showBubble((langId == 0 ? "chinese" : "english"));
    //});
    //virtualLangChangTask.start();


    //测试内存泄露///////////////////////////////
    //LVTask memoryLeakCheckTask(1000);
    //memoryLeakCheckTask.setTaskFunc([]()
    //{
    //    if(Screen::CurrScreen())
    //        Screen::CurrScreen()->hide();
    //    else if(Screen::LastScreen())
    //        Screen::LastScreen()->show();
    //});
    //memoryLeakCheckTask.start();


    //lvapp.exec();

#ifndef NO_MARLIN_RUN
    MarlinSimulator marlin;
    LittlevGlSimulator::getInstance()->setParent(&marlin);
    LittlevGlSimulator::getInstance()->move(15,10);
    marlin.show();

    ArduinoLoopTimer arduinoLoopTimer;
    arduinoLoopTimer.start(0);

#endif

    app.exec();

    delete marlinUi;
    //MarlinUi::getInstance() = nullptr;

    return 0;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/


