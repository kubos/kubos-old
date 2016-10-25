/*

Telemetry header file example

Basic strategy from:
https://github.com/contiki-os/contiki/tree/master/examples/cc26xx/cc26xx-web-demo

*/

#include <stdlib.h>


/* Telemetry reading types */
#define TELEMETRY_READING_TEMP   0
#define TELEMETRY_READING_VOLT   1

/* Telemetry reading units */
#define TELEMETRY_UNIT_TEMP     "C"
#define TELEMETRY_UNIT_VOLT     "mV"


/* Converted reading value length */
#define TELEMETRY_CONVERTED_LEN        12


typedef struct telemetry_reading {
  struct telemetry_reading *next;
  int raw;
  int last;
  char *units;
  uint8_t type;
  uint8_t beacon;
  uint8_t changed;
  char converted[TELEMETRY_CONVERTED_LEN];
} telemetry_reading_t;


/* Global telemetry configuration */
typedef struct telemetry_config_s {
  int len;
  uint32_t telemetry_bitmap;
} telemetry_config_t;

//extern telemetry_config_t telemetry_config;


/**
 * \brief Performs a lookup for a reading of a specific type
 * \param reading_type TELEMETRY_READING_TEMP...
 * \return A pointer to the reading data structure or NULL
 */
const telemetry_reading_t *telemetry_reading_lookup(int reading_type);


/**
 * \brief Returns the first available telemetry reading
 * \return A pointer to the reading data structure or NULL
 */
const telemetry_reading_t *telemetry_reading_first(void);










