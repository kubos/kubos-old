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

/**
 * @brief Telemetry reading types.
 */
typedef enum {
        TELEMETRY_READING_HTU21D_TEMP = 0,
        TELEMETRY_READING_HTU21D_HUMIDITY,
        TELEMETRY_READING_BNO055_POSITION_X,
        TELEMETRY_READING_BNO055_POSITION_Y,
        TELEMETRY_READING_BNO055_POSITION_Z,
        TELEMETRY_READING_BNO055_POSITION_W
} telemetry_reading_type;

/**
 * @brief Telemetry reading status. Platform-specific define to 
 *        signify reading failure, out of limits.
 */
 typedef enum {
        TELEMETRY_READING_OK = 0,
        TELEMETRY_READING_ERROR,
        TELEMETRY_READING_TYPE_ERROR,
        TELEMETRY_READING_LIMIT_ERROR
} telemetry_reading_status;



/* Telemetry reading units */
#define TELEMETRY_UNIT_TEMP                    "C"
#define TELEMETRY_UNIT_HUMIDITY                "%RH"
#define TELEMETRY_UNIT_POSITION                "N/A"

/* Converted reading value length */
#define TELEMETRY_CONVERTED_LEN                 12


/**
 * @brief A union data type for telemetry data.
 */
typedef union telemetry_union_data {
  int i;
  float f;
} telemetry_union_data_t;

/**
 * @brief A data type for sensor readings that will be internally 
 *        stored in a linked list. 
 */
typedef struct telemetry_reading {
  struct telemetry_reading *next;
  telemetry_union_data_t raw;
  telemetry_union_data_t last;
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
 * @brief Adds a previously initialized telemetry reading to the
 *        telemetry linked list.
 * @param Instance of initialized telemetry_reading_t.
 */
void init_telemetry_reading(telemetry_reading_t *reading);

telemetry_reading_status telemetry_reading_write_int(telemetry_reading_t *reading, int *data);

telemetry_reading_status telemetry_reading_write_float(telemetry_reading_t *reading, float *data);

int telemetry_reading_read_int(telemetry_reading_t *reading);

float telemetry_reading_read_float(telemetry_reading_t *reading);

/* telemetry_limit_checker(); */





