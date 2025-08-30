#ifndef _SHTC3_DRV_H
#define _SHTC3_DRV_H

#include <stdint.h>

//#define SHTC3_ADDR (0X70)
//#define SHTC3_CMD_DEV_WRITE       (0xE0)
//#define SHTC3_CMD_DEV_READ        (0xE1)
//#define SHTC3_CMD_SOFT_RESET      0x805D
//#define SHTC3_CMD_READ_ID         0xEFC8
//#define SHTC3_CMD_WAKEUP          0x3517
//#define SHTC3_CMD_SLEEP           0xB098
//#define SHTC3_CMD_READ_TEMP_DATA  0x7866
//#define SHTC3_CMD_READ_HUM_DATA   0x5C24
 
void Shtc3_Read_Result(void);
float GetShtc3Data(uint8_t data);
void Shtc3Wakeup(void);
void Shtc3Hibernate(void);
 
#endif
