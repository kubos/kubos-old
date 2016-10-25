/* 

Main telemetry module example

Basic strategy from:
https://github.com/contiki-os/contiki/tree/master/examples/cc26xx/cc26xx-web-demo


*/

#include <stdio.h>
#include <stdlib.h>

#include "list.h"

/* The "objects" representing telemetry readings */
#define GENERIC_TELEMETRY_READING(name, type, units) \
  telemetry_reading name##_reading = \
  { NULL, 0, 0, units, type, 1, 1 }
  

/* A cache of telemetry readings. */
LIST(telemetry_list);


GENERIC_TELEMETRY_READING(telemetry_temp, TELEMETRY_READING_TEMP,
            TELEMETRY_UNIT_TEMP);

GENERIC_TELEMETRY_READING(telemetry_volt, TELEMETRY_READING_VOLT,
            TELEMETRY_UNIT_VOLT);


const telemetry_reading_t *telemetry_reading_lookup(int reading_type)
{
  telemetry_reading_t *reading = NULL;

  for(reading = list_head(telemetry_list);
      reading != NULL;
      reading = list_item_next(reading)) {
    if(reading->type == reading_type) {
      return reading;
    }
  }

  return NULL;
}


const telemetry_reading_t *telemetry_reading_first()
{
  return list_head(telemetry_list);
}


static void compare_and_update(telemetry_reading_t *reading)
{
  if(reading->last == reading->raw) {
    reading->changed = 0;
  } else {
    reading->last = reading->raw;
    reading->changed = 1;
  }
}


static void init_telemetry_readings(void)
{

  list_add(telemetry_list, &telemetry_temp_reading);
  list_add(telemetry_list, &telemetry_volt_reading);

}


static void test_print_telemetry_beacon(void)
{

telemetry_reading_t *reading = NULL;

  for(reading = list_head(telemetry_list);
      reading != NULL;
      reading = list_item_next(reading)) {
    if(reading->beacon) {
      //Print here
    }
  }
}


static void test_dump_telemetry(void)
{

telemetry_reading_t *reading = NULL;

  for(reading = list_head(telemetry_list);
      reading != NULL;
      reading = list_item_next(reading)) {
      //Print here
  }
}


static void test_log_telemetry(void)
{

telemetry_reading_t *reading = NULL;

  for(reading = list_head(telemetry_list);
      reading != NULL;
      reading = list_item_next(reading)) {
    if(reading->changed) {
      //Log it
    }
  }
}


static void save_telemetry_config()
{

telemetry_reading_t *reading = NULL;

  for(reading = list_head(telemetry_list);
    reading != NULL;
    reading = list_item_next(reading)) {
      if(reading->beacon) {
        telemetry_config_t.telemetry_bitmap |= (1 << reading->type);
      }
    }
    
    //Save telemetry_config_t to non-volatile memory
}


static void load_telemetry_config()
{

}

