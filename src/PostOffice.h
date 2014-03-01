/**
 * @file:   PostOffice.h
 * @brief:  Manages incoming messages from processes
 */

#ifndef _POST_OFFICE_
#define _POST_OFFICE_

#include "Utilities/MessageQueue.h"

void initializePostOffice(void);
int k_delayed_send(int process_id, void *message_envelope, int delay); // message sent to process_id after delay

#endif /*_POST_OFFICE_*/
