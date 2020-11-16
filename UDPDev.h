/*
 * UDPDev.h
 *
 *  Created on: 19 May 2020
 *      Author: Bekircan
 */

#ifndef SRC_UDPDEV_H_
#define SRC_UDPDEV_H_

#include <stdio.h>
#include <string.h>

#include <stdio.h>
#include "xparameters.h"

#include "netif/xadapter.h"
#include "platform_config.h"
#include "xil_printf.h"
#include "lwip/udp.h"
#include "xil_cache.h"
#include "xuartps.h"
#include "sleep.h"
#include "lwip/udp.h"
#include "lwip/err.h"
#include "lwip/init.h"

class UDPDev {
public:
	UDPDev();
	virtual ~UDPDev();
	int create_port(unsigned port);
	static void udp_receive(void *arg, struct udp_pcb *pcb, struct pbuf *p, struct
						ip_addr *addr, u16_t port);
	void udp_send(uint8_t *tx_buffer, struct
						ip_addr *addr, u16_t port);
	void print_app_header();
	int transfer_data(struct netif *netif);

	uint8_t rcv_buffer[512];
	struct pbuf *txBuffer;
	struct udp_pcb *pcb;
};

#endif /* SRC_UDPDEV_H_ */
