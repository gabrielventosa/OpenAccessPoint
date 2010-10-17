#include "simpliciti.h"

unsigned char simpliciti_data[SIMPLICITI_MAX_PAYLOAD_LENGTH];
unsigned char simpliciti_reply_count;
unsigned char simpliciti_send_data_length;

// *************************************************************************************************
// @fn          simpliciti_sync_decode_ap_cmd_callback
// @brief       For SYNC mode only: Decode command from end device and trigger actions. 
// @param       none
// @return      none
// *************************************************************************************************
void simpliciti_sync_decode_ed_cmd_callback(void)
{
        simpliciti_reply_count = 0;
	// Default behaviour is to send no reply packets
	//simpliciti_reply_count = 0;

	switch (simpliciti_data[0])
	{
		case SYNC_ED_TYPE_R2R:		
                  simpliciti_data[0]= SYNC_AP_CMD_SET_WATCH;
                  simpliciti_reply_count = 1;
                  break;

                case SYNC_ED_TYPE_MEMORY:	        break;

                case SYNC_ED_TYPE_STATUS:               break;
								
	}

}

// *************************************************************************************************
// @fn          simpliciti_sync_get_data_callback
// @brief       For SYNC mode only: Access point has requested data. Copy this data into the TX buffer now. 
// @param       u16 index		Index used for memory requests
// @return      none
// *************************************************************************************************

void simpliciti_sync_get_data_callback(unsigned int index)
{
	unsigned int i;
	
	switch (simpliciti_data[0])
	{
		case SYNC_AP_CMD_NOP:	
                  simpliciti_send_data_length = 1;
                  break;

		case SYNC_AP_CMD_GET_STATUS:            break;

		case SYNC_AP_CMD_SET_WATCH:
                  simpliciti_data[1] = 0x97;
                  simpliciti_data[2] = 0x16;
                  simpliciti_data[3] = 0x2c;
                  simpliciti_data[4] = 0x07;
                  simpliciti_data[5] = 0xda;
                  simpliciti_data[6] = 0x0a;
                  simpliciti_data[7] = 0x0e;
                  simpliciti_data[8] = 0x06;
                  simpliciti_data[9] = 0x1e;
                  simpliciti_data[10] = 00;
                  simpliciti_data[11] = 0xdc;
                  simpliciti_data[12] = 01;
                  simpliciti_data[13] = 0xf4;
                  simpliciti_send_data_length = 19;
                  break;
                                      
                case SYNC_AP_CMD_GET_MEMORY_BLOCKS_MODE_1: break;

		case SYNC_AP_CMD_GET_MEMORY_BLOCKS_MODE_2: break;
		
		case SYNC_AP_CMD_ERASE_MEMORY:	            break;
										
                case SYNC_AP_CMD_EXIT:                  break;										
	}
	
}


