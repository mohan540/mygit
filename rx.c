
#include "contiki.h"
#include "cc2520.h"
#include <stdio.h> /* For printf() */
#include "net/netstack.h"
#include <stdint.h>
#include<string.h>
#include "dev/leds.h"
#include "net/packetbuf.h"

#define CHANNEL			15
#define PAN_ID			0xCDAC
#define SRC_NWK_ADDR		0x007A
#define DEST_NWK_ADDR		0x007B
#define MAX_PAYLOAD_SIZE	80
#define BASIC_MAC_HEADER_SIZE	9
uint8_t ieee_addr[] = {0x02,0x00,0x11,0x25,0x20,0x0C,0xAC,0xCD};

typedef struct {
	uint8_t fcf[2];
	uint8_t seq_no;
	uint8_t pan_id[2];
	uint8_t dest_nwk_addr[2];
	uint8_t src_nwk_addr[2];
	uint8_t payload[MAX_PAYLOAD_SIZE];
} basicMACDataPkt_t;
uint8_t str[MAX_PAYLOAD_SIZE-20];
static basicMACDataPkt_t basicMACDataPkt;

static void set_rf_parameters(uint16_t pan_id, uint16_t nwk_addr, uint8_t *extd_addr, uint8_t channel);
/*---------------------------------------------------------------------------*/
PROCESS(cmote_rx_process, "C-Mote RX process");
AUTOSTART_PROCESSES(&cmote_rx_process);
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(cmote_rx_process, ev, data)
{
  PROCESS_BEGIN();
	static uint8_t length,l,rssi,correlation;
	static uint8_t  *buff;
	set_rf_parameters(PAN_ID, DEST_NWK_ADDR, ieee_addr, CHANNEL);
	NETSTACK_RADIO.on();
	do{
		while(!(is_packet_received()));
		leds_toggle(LEDS_ALL);
		clear_packet_received();
		buff = (uint8_t *)packetbuf_dataptr();
		length = buff[0];
		rssi = packetbuf_attr(PACKETBUF_ATTR_RSSI);
		correlation = packetbuf_attr(PACKETBUF_ATTR_LINK_QUALITY);
		printf("Received Packet length	: 0x%02X\n", length);
		printf("RSSI value		: 0x%02X\n",rssi);
		printf("Correlation value	: 0x%02X\n", correlation);
		printf("PAN ID			: 0x%04X\n",(((uint16_t)(buff[5]))<<8)|(buff[4]));
		printf("DESTINATION NWK Addr	: 0x%04X\n",(((uint16_t)(buff[7]))<<8)|(buff[6]));
		printf("SOURCE NWK Addr		: 0x%04X\n",(((uint16_t)(buff[9]))<<8)|(buff[8]));
		printf("Packet Data		: ");
		for(l=BASIC_MAC_HEADER_SIZE+1;l<(length - 2);l++){
			printf("0x%02X ",buff[l]);
		}
		printf("\n");
	} while(1);
	
  PROCESS_END();
}

static void set_rf_parameters(uint16_t pan_id, uint16_t nwk_addr, uint8_t *extd_addr, uint8_t channel){

	cc2520_set_channel(channel);
	cc2520_set_pan_addr(pan_id, nwk_addr,extd_addr);
}

/*---------------------------------------------------------------------------*/
