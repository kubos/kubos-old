/* 

Main telemetry module example

Basic strategy from:
https://github.com/contiki-os/contiki/tree/master/examples/cc26xx/cc26xx-web-demo


*/

#include <stdlib.h>

#include "list.h"
#include "telemetry.h"

/* A cache of telemetry readings. */
LIST(telemetry_list);

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

static void telemetry_compare_and_update(telemetry_reading_t *reading)
{
  if(reading->last == reading->raw) {
    reading->changed = 0;
  } else {
    reading->last = reading->raw;
    reading->changed = 1;
  }
}

void init_telemetry_reading(telemetry_reading_t *reading)
{
  list_add(telemetry_list, &reading);
}

/* For right now data is hard coded as an int ptr */
telemetry_reading_status telemetry_reading_write(telemetry_reading_t *reading, int *data)
{
/* 
  if(reading == NULL || data == NULL)
        return TELEMETRY_READING_ERROR;

  telemetry_reading_status ret = TELEMETRY_READING_ERROR;
*/
    
/* Type check and limit check could go here*/

  reading->raw = *data;
  telemetry_compare_and_update(reading);
  return TELEMETRY_READING_OK;
}

/*

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

*/
