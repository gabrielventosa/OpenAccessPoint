#ifndef RFSIMPLICITI_H_
#define RFSIMPLICITI_H_

#define BM_SYNC_DATA_LENGTH                     (19u)

// Device data  (0)TYPE   (1) - (18) DATA 
#define SYNC_ED_TYPE_R2R                        (1u)
#define SYNC_ED_TYPE_MEMORY                     (2u)
#define SYNC_ED_TYPE_STATUS                     (3u)

// Host data    (0)CMD    (1) - (18) DATA 
#define SYNC_AP_CMD_NOP                         (1u)
#define SYNC_AP_CMD_GET_STATUS			(2u)
#define SYNC_AP_CMD_SET_WATCH                   (3u)
#define SYNC_AP_CMD_GET_MEMORY_BLOCKS_MODE_1   	(4u)
#define SYNC_AP_CMD_GET_MEMORY_BLOCKS_MODE_2   	(5u)
#define SYNC_AP_CMD_ERASE_MEMORY                (6u)
#define SYNC_AP_CMD_EXIT						(7u)

extern unsigned char simpliciti_data[SIMPLICITI_MAX_PAYLOAD_LENGTH];
extern unsigned char simpliciti_send_data_length;
extern unsigned char simpliciti_reply_count;

extern void simpliciti_sync_decode_ed_cmd_callback(void);
extern void simpliciti_sync_get_data_callback(unsigned int index);

#endif /*RFSIMPLICITI_H_*/
