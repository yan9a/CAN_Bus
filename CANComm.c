//#############################################################################
//CAN communication
//File Name   : CANComm.c
//Programmer  : Yan Naing Aye
//Credit to: atmel.com
//#############################################################################
#include "CANComm.h"
//-----------------------------------------------------------------------------
unsigned char idata RB[13]={0,0,0,0,0,0,0,0,0,0,0,0,0};
//-----------------------------------------------------------------------------
void CanInit(void)
{
  //CANCLK uses 12 clk period as in x1 mode
  //CANx2 bit in CKCON0 has been set to use as in X1 mode
  //So, FCAN = XTAL_FREQ / 2
  //22118400/2= 11059200 in this case
  CKCON0|=0x80;

  CAN_CONTROLLER_RESET
  //Set General Reset (software reset) bit GRES in CAN General Control Register 
  //After a reset, the controller is disabled
    
  ECAN = 0;
  //Disable CAN interrupt bit(ECAN) in Interrupt Enable register1 (IEN1)    
  
  ETIM = 0;
  //Disable timer overrun interrupt bit in IEN1  

  //initialize all Mail boxes
  RazAllMailbox();

  //Set Baud rate prescaler
  CanSetBRP(BRP);

  //Set SJW  at Bit 5 and 6 of CANBT2
  CanSetSJW(SJW);

  //Set Prop segment at CANBT2.1 to CANBT2.3
  CanSetPRS(PRS);

  //Set PHS1 and PHS2 at CANBT3
  CanSetPHS1(PHS1);
  CanSetPHS2(PHS2);

  
  CAN_CONTROLLER_ENABLE;
  SetRxMailBoxes();  
}
//-----------------------------------------------------------------------------
void RazAllMailbox (void)
{
  unsigned char num_channel, num_data;

  for (num_channel = 0; num_channel < NB_CHANNEL; num_channel++)
  {
    //4 MSb of CANPAGE register is Message Object Number to select
    //Auto increment of the index to Data Bytes
    CANPAGE  = num_channel << 4; //select each Message object

    //CAN Mesage object control and DLC register
    CANCONCH = 0x10;//id 29 bits

    //clear CAN Message Object Status register
    CANSTCH  = 0;

    //Clear CAN ID Tag and Mask registers
    CANIDT1  = 0;
    CANIDT2  = 0;
    CANIDT3  = 0;
    CANIDT4  = 0;
    CANIDM1  = 0;
    CANIDM2  = 0;
    CANIDM3  = 0;
    CANIDM4  = 0;


    for (num_data = 0; num_data < NB_DATA_MAX; num_data++)
    {
      CANMSG = 0;
    }
  }

}
//-----------------------------------------------------------------------------
void CanSetBRP (unsigned char prescaler)
{
//Sys clk freq for CAN= FCAN/(BRP+1)
//Fscl=11.0592 in this case
  if ((prescaler <= CAN_PRESCALER_MAX) &&
      (prescaler >= CAN_PRESCALER_MIN)   )
  {
    CANBT1 = prescaler << 1;
  }
}
//-----------------------------------------------------------------------------
void CanSetSJW (unsigned char sjw)
{ 
  if ((sjw <= CAN_SJW_MAX) &&
      (sjw >= CAN_SJW_MIN)   )
  {
    CANBT2 &= ~MSK_CANBT2_SJW;	/* RAZ of field sjw.  */
    CANBT2 |= (sjw << 5);
  }
}
//-----------------------------------------------------------------------------
void CanSetPRS (unsigned char prs)
{

  if ((prs <= CAN_PRS_MAX) &&
      (prs >= CAN_PRS_MIN)   )
  {
    CANBT2 &= ~MSK_CANBT2_PRS;   /* RAZ of field prs.  */
    CANBT2 |= (prs << 1);
  }

}
//-----------------------------------------------------------------------------
void CanSetPHS2 (unsigned char phs2)
{
  if ((phs2 <= CAN_PHS2_MAX) &&
      (phs2 >= CAN_PHS2_MIN)   )
  {
    CANBT3 &= ~MSK_CANBT3_PHS2;  /* RAZ of field PHS2.  */
    CANBT3 |= (phs2 << 4);
  }

}
//-----------------------------------------------------------------------------
void CanSetPHS1 (unsigned char phs1)
{
  if ((phs1 <= CAN_PHS1_MAX) &&
      (phs1 >= CAN_PHS1_MIN)   )
  {
    CANBT3 &= ~MSK_CANBT3_PHS1;   /* RAZ field of phs1.  */
    CANBT3 |= (phs1 << 1);
  }

}
//-----------------------------------------------------------------------------
unsigned char CANTxString(unsigned char n,unsigned char s[])
{
    //Use 5 Message Objects (from 0 to 4) for Tx
    static unsigned char idata txChannel=0;
    unsigned char i;

    txChannel=(++txChannel)%5;
    
    switch(txChannel)
    {
        case 0: if(CANEN2 & 0x01) return 0; //if Message Object is in use
                break;
        case 1: if(CANEN2 & 0x02) return 0; //if Message Object is in use
                break;
        case 2: if(CANEN2 & 0x04) return 0; //if Message Object is in use
                break;
        case 3: if(CANEN2 & 0x08) return 0; //if Message Object is in use
                break;
        case 4: if(CANEN2 & 0x10) return 0; //if Message Object is in use
                break;
        default:  return 0; break;//if invalid channel
    }    
    //n is in the range of 0 to 12- (4 bytes for ID and 8 bytes for data)
    //n is string length rather than data length 
    if((n<CAN_MES_MIN_LEN) || (n>CAN_MES_MAX_LEN))
    {
        return 0;
    }
    else
    {
        CAN_SET_CHANNEL(txChannel);
        CANSTCH  = 0x00;
        CANCONCH = 0x10 |(n-4);        
        //CANCONCH
        //---------------------------------------------------------------------
        // BIT 7| BIT 6  |BIT 5 |BIT 4 |BIT 3    |BIT 2    |BIT 1    |BIT 0
        //CONCH1| CONCH1 | RPLV | IDE  |DLC.3    |DLC.2    |DLC.1    |DLC.0 
        //Notes
        //IDE =Id extension bit
        //DLC Data length code bit.
        //---------------------------------------------------------------------

        //ID
        CANIDT1 = s[0]; //ID1
        CANIDT2 = s[1]; //ID2
        CANIDT3 = s[2]; //ID3
        CANIDT4 = s[3]; //ID4

        //Data
        for (i=4; i<n; i++)
        {
            CANMSG = s[i];
        }
        ENABLE_CHANNEL_TX;
        //DisplayTx(n,s);
        return 1;
    }    
}
//-----------------------------------------------------------------------------
void CANRxCheck()
{
    //Use 10 Message Objects (from 5 to 14) for Rx Buffer
    unsigned char idata num_channel,n=0;
    char idata i;//signed char

    for (num_channel = 5; num_channel < NB_CHANNEL; num_channel++)
    {
        //4 MSb of CANPAGE register is Message Object Number to select
        //Auto increment of the index to Data Bytes
        CANPAGE  = num_channel << 4; //select each Message object
    
        if(CANSTCH & MSK_CANSTCH_RxOk)
        {
            n =(CANCONCH & 0x0F)+4; //get Data Length Code
             
            RB[0]=CANIDT1;
            RB[1]=CANIDT2;
            RB[2]=CANIDT3;
            RB[3]=CANIDT4;
            for (i = 4; i < n; i++)
            {
                RB[i]=CANMSG;
            }
            //clear CAN Message Object Status register
            CANSTCH  = 0;
            ENABLE_CHANNEL_BUFFER;
            //ProcessRx(RB);//Process receive data
        }        
    }
}
//-----------------------------------------------------------------------------
void SetRxMailBoxes()
{
    //Use 10 Message Objects (from 5 to 14) for Rx Buffer
    unsigned char num_channel, num_data;

    for (num_channel = 5; num_channel < NB_CHANNEL; num_channel++)
    {
        //4 MSb of CANPAGE register is Message Object Number to select
        //Auto increment of the index to Data Bytes
        CANPAGE  = num_channel << 4; //select each Message object
    
        //CAN Mesage object control and DLC register
        CANCONCH = 0x10;//id 29 bits
    
        //clear CAN Message Object Status register
        CANSTCH  = 0;
    
        //Clear CAN ID Tag and Mask registers
        CANIDT1  = 0;
        CANIDT2  = 0;
        CANIDT3  = 0;
        CANIDT4  = 0;//not used

        CANIDM1  = 0x00;
        CANIDM2  = 0x00;
        CANIDM3  = 0x00;
        CANIDM4  = 0x00;//not filtered
        
        for (num_data = 0; num_data < NB_DATA_MAX; num_data++)
        {
          CANMSG = 0;
        }
        ENABLE_CHANNEL_BUFFER;
    }
}
//-----------------------------------------------------------------------------
