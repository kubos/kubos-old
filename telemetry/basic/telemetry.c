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

static void telemetry_compare_and_update(telemetry_reading_t *reading)
{
  if(reading->last.i == reading->raw.i) {
    reading->changed = 0;
  } else {
    reading->last.i = reading->raw.i;
    reading->changed = 1;
  }
}

const telemetry_reading_t *telemetry_reading_first()
{
  return list_head(telemetry_list);
}

void init_telemetry_reading(telemetry_reading_t *reading)
{
  list_add(telemetry_list, reading);
}

telemetry_reading_status telemetry_reading_write_int(telemetry_reading_t *reading, int *data)
{
/* 
  if(reading == NULL || data == NULL)
        return TELEMETRY_READING_ERROR;

  telemetry_reading_status ret = TELEMETRY_READING_ERROR;
*/
    
/* Type check and limit check could go here*/

  reading->raw.i = *data;
  return TELEMETRY_READING_OK;
}

int telemetry_reading_read_int(telemetry_reading_t *reading)
{
  return reading->raw.i;
}

telemetry_reading_status telemetry_reading_write_float(telemetry_reading_t *reading, float *data)
{
  reading->raw.f = *data;
  return TELEMETRY_READING_OK;
}

float telemetry_reading_read_float(telemetry_reading_t *reading)
{
  return reading->raw.f;
}
/*

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
