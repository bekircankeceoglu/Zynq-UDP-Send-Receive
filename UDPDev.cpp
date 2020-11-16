/*
 * UDPDev.cpp
 *
 *  Created on: 19 May 2020
 *      Author: Bekircan
 */

#include "UDPDev.h"

uint32_t rcvPort;

UDPDev::UDPDev() {
	// TODO Auto-generated constructor stub

}

UDPDev::~UDPDev() {
	// TODO Auto-generated destructor stub
}

int UDPDev::transfer_data(struct netif *netif) {
	xemacif_input(netif);
	return 0;
}

void UDPDev::print_app_header()
{
	xil_printf("\n\r\n\r-----lwIP UDP echo server ------\n\r");
	xil_printf("UDP packets sent to port 7 will be echoed back\n\r");
}

void UDPDev::udp_receive(void *arg, struct udp_pcb *pcb, struct pbuf *p, struct
					ip_addr *addr, u16_t port)
{
    if (p != NULL) {

		rcvPort = port;

    }
    pbuf_free(p);
}

void UDPDev::udp_send(uint8_t *tx_buffer, struct
					ip_addr *addr, u16_t port)
{
	txBuffer = pbuf_alloc(PBUF_TRANSPORT, sizeof(tx_buffer), PBUF_REF);
	txBuffer->payload = tx_buffer;
	txBuffer->len = sizeof(tx_buffer);
	/* send received packet back to sender */
	udp_sendto(this->pcb, txBuffer, addr, port);
	/* free the pbuf */
	pbuf_free(txBuffer);

}

int UDPDev::create_port(unsigned port)
{
	int err;

	/* create new UDP PCB structure */
	this->pcb = udp_new();
	if (!pcb) {
		xil_printf("Error creating PCB. Out of Memory\n\r");
		return -1;
	}

	/* bind to specified @port */
	err = udp_bind(pcb, IP_ADDR_ANY, port);
	if (err != 0) {
		xil_printf("Unable to bind to port %d: err = %d\n\r", port, err);
		return -2;
	}

	/* Receive data */
	udp_recv(this->pcb, udp_receive, NULL);


	xil_printf("UDP echo server started @ port %d\n\r", port);

	return 0;
}
