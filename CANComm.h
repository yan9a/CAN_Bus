//#############################################################################
//CAN communication
//File Name   : CANComm.h
//Programmer  : Yan Naing Aye
//Credit to: atmel.com
//#############################################################################
#ifndef	CANCOMM_H
	#define	CANCOMM_H
//-----------------------------------------------------------------------------
	#define CAN_MES_MIN_LEN   4
	#define CAN_MES_MAX_LEN   12
//-----------------------------------------------------------------------------
	#define NB_CHANNEL   15
	#define NB_DATA_MAX   8
	//-----------------------------------------------------------------------------
	//#define CAN_BAUD_RATE   1000000 //1M
	#define CAN_BAUD_RATE   500000 //500k
	//#define CAN_BAUD_RATE   100000 //100k
	//#define CAN_BAUD_RATE   40000 //40k
//-----------------------------------------------------------------------------
#define MSK_CANGIT_CANIT	0x80
#define MSK_CANGIT_OVRTIM   0x20
#define MSK_CANGIT_OVRBUF	0x10	
#define MSK_CANGIT_SERG		0x08
#define MSK_CANGIT_CERG		0x04
#define MSK_CANGIT_FERG		0x02
#define MSK_CANGIT_AERG		0x01
#define MSK_CANGSTA_OVFG	0x40
#define MSK_CANGSTA_TBSY	0x10
#define MSK_CANGSTA_RBSY	0x08
#define MSK_CANGSTA_ENFG	0x04
#define MSK_CANGSTA_BOFF	0x02
#define MSK_CANGSTA_ERRP	0x01
#define MSK_CANGCON_ABRQ   	0x80
#define MSK_CANGCON_OVRQ   	0x40
#define MSK_CANGCON_TTC   	0x20
#define MSK_CANGCON_SYNCTTC 0x10
#define TTC_EOF             0x10
#define TTC_SOF		        0x00
#define MSK_CANGCON_AUTBAUD 0x08
#define MSK_CANGCON_ENA  	0x02
#define MSK_CANGCON_GRES 	0x01
#define MSK_CANSTCH_DLCW  	0x80
#define MSK_CANSTCH_TxOk  	0x40
#define MSK_CANSTCH_RxOk  	0x20
#define MSK_CANSTCH_BERR  	0x10
#define MSK_CANSTCH_SERR  	0x08
#define MSK_CANSTCH_CERR  	0x04
#define MSK_CANSTCH_FERR  	0x02
#define MSK_CANSTCH_AERR  	0x01
#define MSK_CANCONCH_IDE  	0x10
#define MSK_CANCONCH_DLC  	0x0F
#define MSK_CANCONCH_CONF 	0xC0
#define DLC_MAX    8
#define CH_DISABLE 0x00
#define CH_RxENA   0x80
#define CH_TxENA   0x40
#define CH_RxBENA  0xC0
#define CAN_PRESCALER_MIN  0
#define CAN_PRESCALER_MAX  63
#define MSK_CANBT2_SJW  0x60
#define MSK_CANBT2_PRS  0x0E
#define CAN_SJW_MIN  0
#define CAN_SJW_MAX  3
#define CAN_PRS_MIN  0
#define CAN_PRS_MAX  7
#define MSK_CANBT3_PHS2 0x70
#define MSK_CANBT3_PHS1 0x0E
#define CAN_PHS2_MIN 0
#define CAN_PHS2_MAX 7
#define CAN_PHS1_MIN 0
#define CAN_PHS1_MAX 7
#define MSK_CANIDT4_RTRTAG 	0x04
#define MSK_CANGIE_ENRX    	0x20
#define MSK_CANGIE_ENTX    	0x10
#define MSK_CANGIE_ENERCH  	0x08
#define MSK_CANGIE_ENBUF   	0x04
#define MSK_CANGIE_ENERG   	0x02
#define MSK_CANIDM4_RTRMSK 	0x04
#define MSK_CANIDM4_IDEMSK 	0x01
//-----------------------------------------------------------------------------
//CANx2 bit in CKCON0 has been set to use as in X1 mode
//FCAN = XTAL_FREQ / 2 ------>(22118400/2= 11059200 in this case)
//Fscl = FCAN/(BRP+1)  ------>For BRP=0, 11.0592 in this case
//For 1 MHz can bus, there are 11 pulses for each CAN bit
//Fscl/ CANFreq = NTQ (Number of Time Quanta) = 11

//Tbit = Tsyns + Tprs + Tphs1 + Tphs2 = 11TQ
//Tsyns is always 1
//
#if XTAL_FREQ==22118400
	#if CAN_BAUD_RATE ==1000000
		#define BRP 0x00
		#define SJW 0x00
		#define PRS 0x03
		#define PHS1 0x02
		#define PHS2 0x02
	#elif CAN_BAUD_RATE ==500000
		#define BRP 0x00
		#define SJW 0x00
		#define PRS 0x04
		#define PHS1 0x07
		#define PHS2 0x07
	#elif CAN_BAUD_RATE ==100000
		#define BRP 0x04
		#define SJW 0x00
		#define PRS 0x04
		#define PHS1 0x07
		#define PHS2 0x07
	#elif CAN_BAUD_RATE ==40000
		#define BRP 0x0A
		#define SJW 0x00
		#define PRS 0x07
		#define PHS1 0x07
		#define PHS2 0x07
	#endif
#elif XTAL_FREQ==12000000	
	#if CAN_BAUD_RATE ==500000
		#define BRP 0x00
		#define SJW 0x00
		#define PRS 0x02
		#define PHS1 0x03
		#define PHS2 0x03
	#elif CAN_BAUD_RATE ==100000
		#define BRP 0x05
		#define SJW 0x00
		#define PRS 0x01
		#define PHS1 0x03
		#define PHS2 0x02
	#elif CAN_BAUD_RATE ==40000
		#define BRP 0x05
		#define SJW 0x00
		#define PRS 0x07
		#define PHS1 0x07
		#define PHS2 0x07
	#endif
#endif
//-----------------------------------------------------------------------------
//Macro
#define CAN_CONTROLLER_ENABLE    { CANGCON |= MSK_CANGCON_ENA;  }
#define CAN_CONTROLLER_DISABLE   { CANGCON &= ~MSK_CANGCON_ENA; }
#define CAN_CONTROLLER_RESET     { CANGCON |= MSK_CANGCON_GRES; }
#define CAN_SET_CHANNEL(channel) (CANPAGE = (channel << 4))
#define CAN_GET_CHANNEL          (CANPAGE >> 4)

#define ENABLE_CHANNEL_RX      (CANCONCH |= (MSK_CANCONCH_CONF & CH_RxENA))
#define ENABLE_CHANNEL_BUFFER  (CANCONCH |= (MSK_CANCONCH_CONF & CH_RxBENA))
#define ENABLE_CHANNEL_TX      (CANCONCH |= (MSK_CANCONCH_CONF & CH_TxENA))
#define DISABLE_CHANNEL    (CANCONCH &= ~MSK_CANCONCH_CONF)
//-----------------------------------------------------------------------------
	void CanInit(void);
	void RazAllMailbox (void);
	void CanSetBRP (unsigned char prescaler);
	void CanSetSJW (unsigned char sjw);
	void CanSetPRS (unsigned char prs);
	void CanSetPHS2 (unsigned char phs2);
	void CanSetPHS1 (unsigned char phs1);
	unsigned char CANTxString(unsigned char n,unsigned char s[]);
	void CANRxCheck();
	void SetRxMailBoxes();
//-----------------------------------------------------------------------------	
#endif

