/*
 * Empty C++ Application
 */
#include "FreeRTOS.h"
#include "task.h"
#include "xparameters.h"

#include <stdio.h>
#include "xparameters.h"
#include "platform_config.h"
#include "xil_printf.h"
#include "xscugic.h"
#include "xil_types.h"
#include "xtime_l.h"
#include "xil_io.h"
#include "xpseudo_asm.h"
#include "xil_mmu.h"

#include "netif/xadapter.h"
#include "lwip/udp.h"
#include "lwip/err.h"
#include "lwip/init.h"
#include "sleep.h"
#include "UDPDev.h"

#define INTC_DEVICE_ID XPAR_PS7_SCUGIC_0_DEVICE_ID

/************************** Function Prototypes ******************************/
void TaskEthernet1(void *pvParameters);
void TaskUDP(void *pvParameters);
void platform_enable_interrupts();
void init_platform();
int start_application();
int transfer_data();
int initUDP();

/************************** Class Objects ******************************/
UDPDev udpObj;


/***************** Macros (Inline Functions) Definitions *********************/

TaskHandle_t UDPTask;
TaskHandle_t EthernetTask1;

/************************** Network Structs ******************************/
static struct netif server_netif;
struct  netif *echo_netif1;
struct ip_addr ipaddr, netmask, gw, RemoteAddr;

extern XScuGic xInterruptController;
extern uint32_t rcvPort;

int InterruptSystemSetup( XScuGic *xInterruptControllerancePtr)
{
       Xil_ExceptionInit();
       Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_INT,(Xil_ExceptionHandler)XScuGic_InterruptHandler,xInterruptControllerancePtr);

       Xil_ExceptionEnable();

       return XST_SUCCESS;
}

static int IntcInitFunction()
{
	XScuGic_Config *IntcConfig;
	IntcConfig = XScuGic_LookupConfig(INTC_DEVICE_ID);
	if(IntcConfig == NULL)
	{
		return XST_FAILURE;
	}
	int32_t status = XScuGic_CfgInitialize(&xInterruptController,IntcConfig,IntcConfig->CpuBaseAddress);
	if(status != XST_SUCCESS)
	{
		return XST_FAILURE;
	}

	return XST_SUCCESS;
}

int main()
{
	initUDP(); // init ip addresses
	udpObj.create_port(4001); // listen port

	xTaskCreate( TaskUDP, 				/* The function that implements the task. */
			 ( const char * ) "1", 			/* Text name for the task, provided to assist debugging only. */
			  4096, 						/* The stack allocated to the task. */
			  NULL, 						/* The task parameter is not used, so set to NULL. */
			  tskIDLE_PRIORITY+3,			/* The task runs at the idle priority. */
			  &UDPTask);


	xTaskCreate( TaskEthernet1, 		/* The function that implements the task. */
		 ( const char * ) "2", 			/* Text name for the task, provided to assist debugging only. */
		  4096, 						/* The stack allocated to the task. */
		  NULL, 						/* The task parameter is not used, so set to NULL. */
		  tskIDLE_PRIORITY+3,			/* The task runs at the idle priority. */
		  &EthernetTask1);




	vTaskStartScheduler();
	while (1)
	{

	}

	return 0;
}



void TaskEthernet1(void *pvParameters)
{
	uint8_t array[4] = {0x31, 0x32, 0x33, 0x34};

	while(1)
	{

		if(rcvPort==4001)
		{
			udpObj.udp_send(array,&RemoteAddr, 4001);
			rcvPort = 0;
		}

	}
}

void TaskUDP(void *pvParameters)
{
	// UDP HEART BEAT
	while(1)
	{
		udpObj.transfer_data(echo_netif1);
	}
}


int initUDP()
{
	init_platform();

	/* the mac address of the board. this should be unique per board */
	unsigned char mac_ethernet_address[] =
	{ 0x00, 0x0a, 0x35, 0x00, 0x01, 0x02 };

	int32_t Status = InterruptSystemSetup( &xInterruptController);
	if(Status != XST_SUCCESS)
	{
		return XST_FAILURE;
	}
	// free rtos interrupt'i ile calismasi icin eklendi
	Status = IntcInitFunction();
	if(Status != XST_SUCCESS)
	{
		return XST_FAILURE;
	}

	echo_netif1 = &server_netif;

	/* initliaze IP addresses to be used */
	IP4_ADDR(&ipaddr,  192, 168,   1, 10);
	IP4_ADDR(&netmask, 255, 255, 255,  0);
	IP4_ADDR(&gw,      192, 168,   1,  1);
	IP4_ADDR(&RemoteAddr,  192, 168,   1, 100);

	lwip_init();


	/* Add network interface to the netif_list, and set it as default */
	if (!xemac_add(echo_netif1, &ipaddr, &netmask,
						&gw, mac_ethernet_address,
						PLATFORM_EMAC_BASEADDR)) {
		xil_printf("Error adding N/W interface\n\r");
		return -1;
	}
	netif_set_default(echo_netif1);

	/* Create a new DHCP client for this interface.
	 * Note: you must call dhcp_fine_tmr() and dhcp_coarse_tmr() at
	 * the predefined regular intervals after starting the client.
	 */
	/* dhcp_start(echo_netif); */

	/* specify that the network if is up */
	netif_set_up(echo_netif1);

	return 0;
}
