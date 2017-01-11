/*
 * base16.h
 *
 *  Created on: Oct 3, 2016
 *      Author: johan
 */

#ifndef SLASH_SRC_BASE16_H_
#define SLASH_SRC_BASE16_H_

void base16_encode ( const uint8_t *raw, size_t len, char *encoded );
int base16_decode ( const char *encoded, uint8_t *raw );



#endif /* SLASH_SRC_BASE16_H_ */
