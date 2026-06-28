#ifndef PAL_COMM_H_
#define PAL_COMM_H_

/**
 * @file pal_comm.h
 * @brief PAL 通信外设回调接口。
 */

void pal_wireless_init(void);
void pal_wireless_rx_handler(void);
void pal_gnss_rx_callback(void);

#endif /* PAL_COMM_H_ */
