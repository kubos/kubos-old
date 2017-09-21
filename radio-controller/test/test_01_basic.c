#include <stdio.h>
#include "radio-controller/aos.h"

int main() {
  tAOSErr err = AOS_ERR_NOERR;
  tAOSParser parser;
  
  do {

    /* First, let's make sure we don't get errors from basic function calls */
    if( AOS_ERR_NOERR != ( err = AOS_Init( & parser ) ) ) {
      break;
    }

    if( AOS_ERR_NOERR != ( err = AOS_Register( & parser, AOS_VC_RETRANSMIT, NULL, NULL ) ) ) {
      break;
    }

    if( AOS_ERR_NOERR != ( err = AOS_Process( & parser, NULL, 0 ) ) ) {
      break;
    }
    
  } while( 0 );

  return( err );

}
