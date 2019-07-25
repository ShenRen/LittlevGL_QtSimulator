#ifndef QTSIMULATOR_H
#define QTSIMULATOR_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#ifdef LV_CONF_INCLUDE_SIMPLE
#include "lv_drv_conf.h"
#else
#include "../../lv_drv_conf.h"
#endif

#define USE_QTSIMULATOR 1

#if USE_QTSIMULATOR

#include <stdint.h>
#include <stdbool.h>
#include "lvgl/lv_hal/lv_hal_indev.h"
#include "lvgl/lv_misc/lv_color.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/


/**
 * @brief Initialize Qt simulator
 */
void qt_simulator_init();




/**
 * @brief Used when `LV_VDB_SIZE != 0` in lv_conf.h (buffered drawing)
 */
void qt_screen_flush(int32_t x1, int32_t y1, int32_t x2, int32_t y2, const lv_color_t *color_p);

/**
 * @brief Used when `LV_VDB_SIZE == 0` in lv_conf.h (unbuffered drawing)
 */
void qt_screen_fill(int32_t x1, int32_t y1, int32_t x2, int32_t y2, lv_color_t color);

/**
 * @brief Used when `LV_VDB_SIZE == 0` in lv_conf.h (unbuffered drawing)
 */
void qt_screen_map(int32_t x1, int32_t y1, int32_t x2, int32_t y2, const lv_color_t * color_p);




/**
 * Get the current position and state of the mouse
 * @param data store the mouse data here
 * @return false: because the points are not buffered, so no more data to be read
 */
bool qt_mouse_read(lv_indev_data_t * data);


/**********************
 *      MACROS
 **********************/

#endif //USE_QTSIMULATOR

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif // QTSIMULATOR_H
