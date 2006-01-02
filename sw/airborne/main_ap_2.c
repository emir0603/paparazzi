#include "main_ap.h"
#include "low_level_hw.h"
#include "sys_time.h"
#include "int.h"
#include "led.h"
#include "gps.h"

void init_ap( void ) {
  /* if AP is running in a separate MCU */
#ifndef FBW
  low_level_init();
  sys_time_init();
#ifdef LED
  led_init();
#endif /* LED */
#endif /* FBW */

#ifdef GPS
  gps_init();
  gps_configure();
#endif
  int_enable();
}

void periodic_task_ap( void ) {
  //  LED_TOGGLE(1);
  LED_TOGGLE(2);
}

void event_task_ap( void ) {
#ifdef GPS
  if (GpsBuffer()) {
    ReadGpsBuffer();
    if (gps_msg_received) {
      /* parse and use GPS messages */
      parse_gps_msg();
      gps_msg_received = FALSE;
      if (gps_pos_available) {
	LED_TOGGLE(1);
	use_gps_pos();
	gps_pos_available = FALSE;
      }
    }
  }
#endif /* GPS */
}
