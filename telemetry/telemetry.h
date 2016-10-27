/*

Telemetry header file example

Basic strategy from:
https://github.com/contiki-os/contiki/tree/master/examples/cc26xx/cc26xx-web-demo

*/

#include <stdlib.h>

/**
 * @brief The "objects" representing telemetry readings.
 */
#define CREATE_TELEMETRY_READING(name, type, units) \
  telemetry_reading_t name##_reading = \
  { NULL, 0, 0, units, type, 1, 1, 1 }

/* Telemetry reading types */
#define TELEMETRY_READING_HTU21D_TEMP          0
#define TELEMETRY_READING_HTU21D_HUMIDITY      1
#define TELEMETRY_READING_BNO055_POSITION_X    2
#define TELEMETRY_READING_BNO055_POSITION_Y    3
#define TELEMETRY_READING_BNO055_POSITION_Z    4
#define TELEMETRY_READING_BNO055_POSITION_W    5

/* Telemetry reading units */
#define TELEMETRY_UNIT_TEMP                    "C"
#define TELEMETRY_UNIT_HUMIDITY                "%RH"
#define TELEMETRY_UNIT_POSITION                "N/A"

/* Converted reading value length */
#define TELEMETRY_CONVERTED_LEN                 12

/* Platform-specific define to signify reading failure or out of 
 * limits.
 *  */
#define TELEMETRY_READING_ERROR                 0x80000000

/**
 * @brief A data type for sensor readings, internally stored in a linked 
 *        list. 
 */
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


/* Global telemetry configuration 
typedef struct telemetry_config_s {
  int len;
  uint32_t telemetry_bitmap;
} telemetry_config_t;

extern telemetry_config_t telemetry_config;

*/

/**
 * @brief Performs a lookup for a telemetry reading of a specific type.
 * @param reading_type i.e. TELEMETRY_READING_HTU21D_TEMP.
 * @retval A pointer to the telemetry reading data structure or NULL if 
 *         the reading isn't found.
 */
const telemetry_reading_t *telemetry_reading_lookup(int reading_type);

/**
 * @brief Returns the first available telemetry reading.
 * @retval A pointer to the reading data structure or NULL if no 
 *         telemetry readings existing.
 */
const telemetry_reading_t *telemetry_reading_first(void);

/**
 * @brief Compares the present and past values of the telemetry reading
 *        and sets the changed flag appropriately.
 * @param Instance of initialized telemetry_reading_t containing 
 *        telemetry readings to compare.
 */
void telemetry_compare_and_update(telemetry_reading_t *reading);

/**
 * @brief Adds a previously initialized telemetry reading to the
 *        telemetry linked list.
 * @param Instance of initialized telemetry_reading_t.
 */
void init_telemetry_reading(telemetry_reading_t *reading);


/* de_init_telemetry_reading(); */

/* telemetry_limit_checker(); */





