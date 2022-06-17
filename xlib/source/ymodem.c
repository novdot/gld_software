/**
  ******************************************************************************
  * @file    IAP/src/ymodem.c 
  * @author  MCD Application Team
  * @version V3.3.0
  * @date    10/15/2010
  * @brief   This file provides all the software functions related to the ymodem 
  *          protocol.
  ******************************************************************************
  * @copy
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2010 STMicroelectronics</center></h2>
  */

/** @addtogroup IAP
  * @{
  */ 
  
#include "xlib/ymodem.h"

//#define ApplicationAddress    0x8003000
//#define PAGE_SIZE                         (0x400)    /* 1 Kbyte */
//#define FLASH_SIZE                        (0x20000)  /* 128 KBytes */

/* Compute the FLASH upload image size */  
//#define FLASH_IMAGE_SIZE (uint32_t) (FLASH_SIZE - (ApplicationAddress - 0x08000000))

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/
/* Common routines */
#define IS_AF(c)  ((c >= 'A') && (c <= 'F'))
#define IS_af(c)  ((c >= 'a') && (c <= 'f'))
#define IS_09(c)  ((c >= '0') && (c <= '9'))
#define ISVALIDHEX(c)  IS_AF(c) || IS_af(c) || IS_09(c)
#define ISVALIDDEC(c)  IS_09(c)
#define CONVERTDEC(c)  (c - '0')

#define CONVERTHEX_alpha(c)  (IS_AF(c) ? (c - 'A'+10) : (c - 'a'+10))
#define CONVERTHEX(c)   (IS_09(c) ? (c - '0') : CONVERTHEX_alpha(c))

#define SerialPutString(x) Serial_PutString((uint8_t*)(x))

//#define FLASH_IMAGE_SIZE (0)

/* Private variables ---------------------------------------------------------*/
x_uint8_t file_name[FILE_NAME_LENGTH];
//x_uint32_t FlashDestination = ApplicationAddress; /* Flash user program offset */
//x_uint16_t PageSize = PAGE_SIZE;
x_uint32_t EraseCounter = 0x0;
x_uint32_t NbrOfPage = 0;
//FLASH_Status FLASHStatus = FLASH_COMPLETE;
x_uint32_t RamSource;
//x_uint8_t tab_1024[1024];

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/


/******************************************************************************/
/**
  * @brief  Convert an Integer to a string
  * @param  str: The string
  * @param  intnum: The intger to be converted
  * @retval None
  */
void Int2Str(x_uint8_t* str, x_int32_t intnum)
{
  x_uint32_t i, Div = 1000000000, j = 0, Status = 0;

  for (i = 0; i < 10; i++) {
    str[j++] = (intnum / Div) + 48;

    intnum = intnum % Div;
    Div /= 10;
    if ((str[j-1] == '0') & (Status == 0))
    {
      j = 0;
    }
    else
    {
      Status++;
    }
  }
}

/******************************************************************************/
/**
  * @brief  Convert a string to an integer
  * @param  inputstr: The string to be converted
  * @param  intnum: The intger value
  * @retval 1: Correct
  *         0: Error
  */
x_uint32_t Str2Int(x_uint8_t *inputstr, x_int32_t *intnum)
{
    x_uint32_t i = 0, res = 0;
    x_uint32_t val = 0;

  if (inputstr[0] == '0' && (inputstr[1] == 'x' || inputstr[1] == 'X')) {
    if (inputstr[2] == '\0') {
      return 0;
    }
    for (i = 2; i < 11; i++) {
      if (inputstr[i] == '\0')
      {
        *intnum = val;
        /* return 1; */
        res = 1;
        break;
      }
      if (ISVALIDHEX(inputstr[i]))  {
        val = (val << 4) + CONVERTHEX(inputstr[i]);
      } else {
        /* return 0, Invalid input */
        res = 0;
        break;
      }
    }
    /* over 8 digit hex --invalid */
    if (i >= 11) {
      res = 0;
    }
  } else {
      /* max 10-digit decimal input */
    for (i = 0;i < 11;i++) {
      if (inputstr[i] == '\0') {
        *intnum = val;
        /* return 1 */
        res = 1;
        break;
      }
      else if ((inputstr[i] == 'k' || inputstr[i] == 'K') && (i > 0)) {
        val = val << 10;
        *intnum = val;
        res = 1;
        break;
      } else if ((inputstr[i] == 'm' || inputstr[i] == 'M') && (i > 0)) {
        val = val << 20;
        *intnum = val;
        res = 1;
        break;
      } else if (ISVALIDDEC(inputstr[i])) {
        val = val * 10 + CONVERTDEC(inputstr[i]);
      }
      else
      {
        /* return 0, Invalid input */
        res = 0;
        break;
      }
    }
    /* Over 10 digit decimal --invalid */
    if (i >= 11)
    {
      res = 0;
    }
  }

  return res;
}
/******************************************************************************/
/**
  * @brief  Receive byte from sender
  * @param  c: Character
  * @param  timeout: Timeout
  * @retval 0: Byte received
  *         -1: Timeout
  */
static x_int32_t Receive_Byte(x_ymodem_setups setups, x_uint8_t *c, x_uint32_t timeout)
{
    //x_uint8_t bInfFlag = 0;
    //if(timeout==0) bInfFlag = 1;
    while (timeout-- > 0)
    {
        /*if(bInfFlag==0) {
            timeout--;
            if(timeout<=0) goto end;
        }*/
        //if (SerialKeyPressed(c) == 1)
        if(setups.recieve_byte(c) == 1)
        {
            return 0;
        }
    }
end:;
    return -1;
}

/******************************************************************************/
/**
  * @brief  Send a byte
  * @param  c: Character
  * @retval 0: Byte sent
  */
static x_uint32_t Send_Byte(x_ymodem_setups setups, x_uint8_t c)
{
    setups.send_byte(c);
    //SerialPutChar(c);
    return 0;
}

/******************************************************************************/
/**
  * @brief  Receive a packet from sender
  * @param  data
  * @param  length
  * @param  timeout
  *     0: end of transmission
  *    -1: abort by sender
  *    >0: packet length
  * @retval 0: normally return
  *        -1: timeout or packet error
  *         1: abort by user
  */
static x_int32_t Receive_Packet (x_ymodem_setups setups,x_uint8_t *data, x_int32_t *length, x_uint32_t timeout)
{
    x_uint16_t i, packet_size;
    x_uint8_t c;
    *length = 0;
    if (Receive_Byte(setups,&c, timeout) != 0) {
        return -1;
    }
    switch (c) {
    case SOH:
      packet_size = PACKET_SIZE;
      break;
    case STX:
      packet_size = PACKET_1K_SIZE;
      break;
    case EOT:
      return 0;
    case CA:
      if ((Receive_Byte(setups,&c, timeout) == 0) && (c == CA))
      {
        *length = -1;
        return 0;
      }
      else
      {
        return -1;
      }
    case ABORT1:
    case ABORT2:
      return 1;
    default:
      return -1;
    }
    *data = c;
    for (i = 1; i < (packet_size + PACKET_OVERHEAD); i ++) {
        if (Receive_Byte(setups,data + i, timeout) != 0) {
            return -1;
        }
    }
    if (data[PACKET_SEQNO_INDEX] != ((data[PACKET_SEQNO_COMP_INDEX] ^ 0xff) & 0xff))
    {
    return -1;
    }
    *length = packet_size;
    return 0;
}

/******************************************************************************/
/**
  * @brief  Receive a file using the ymodem protocol
  * @param  buf: Address of the first byte
  * @retval The size of the file
  */
#include "hardware/hardware.h"
x_int32_t x_Ymodem_Receive (x_ymodem_setups setups, x_uint8_t *buf)
{
    int i_=0;
    char dbg[64];
    //x_uint32_t timeout = NAK_TIMEOUT;
    x_uint32_t buffer_length = 0;
    x_uint8_t buf_data[1024];
    
    x_uint8_t packet_data[PACKET_1K_SIZE + PACKET_OVERHEAD], file_size[FILE_SIZE_LENGTH], *file_ptr, *buf_ptr;
    x_int32_t i, j, packet_length, session_done, file_done, packets_received, errors, session_begin, size = 0;

    /* Initialize FlashDestination variable */
    //FlashDestination = ApplicationAddress;

    for (session_done = 0, errors = 0, session_begin = 0; ;) {
        for (packets_received = 0, file_done = 0, buf_ptr = buf; ;) {
            switch (Receive_Packet(setups, packet_data, &packet_length, NAK_TIMEOUT)) {
            /* normally return */
            case 0:
                errors = 0;
                switch (packet_length)  {
                /* Abort by sender */
                case - 1:
                    Send_Byte(setups,ACK);
                    return 0;
                /* End of transmission */ 
                case 0:
                    Send_Byte(setups,ACK);
                    file_done = 1;
                    buffer_length = 1;
                    if(
                        setups.mem_write(
                            file_name
                            ,buffer_length
                            ,buf_data
                            , 0
                            )==_x_true){
                        //Send_Byte(setups,ACK);
                    }else{
                        // End session
                        //Send_Byte(setups,CA);
                        //Send_Byte(setups,CA);
                        return -2;
                    }
                    break;
                /* Normal packet */
                default:
                    if ((packet_data[PACKET_SEQNO_INDEX] & 0xff) != (packets_received & 0xff)){
                        Send_Byte(setups,NAK);
                    }else {
                        //Recieve #0
                        if (packets_received == 0){
                            //timeout = 0;
                            /* Filename packet */
                            if (packet_data[PACKET_HEADER] != 0){
                                /* Filename packet has valid data */
                                for (i = 0, file_ptr = packet_data + PACKET_HEADER; (*file_ptr != 0) && (i < FILE_NAME_LENGTH);){
                                    file_name[i++] = *file_ptr++;
                                }
                                file_name[i++] = '\0';
                                for (i = 0, file_ptr ++; (*file_ptr != ' ') && (i < FILE_SIZE_LENGTH);) {
                                    file_size[i++] = *file_ptr++;
                                }
                                file_size[i++] = '\0';
                                Str2Int(file_size, &size);
                                
                                //buf_data = (x_uint8_t*)malloc(size);

                                /* Test the size of the image to be sent */
                                /* Image size is greater than Flash size *
                                if (size > (FLASH_SIZE - 1)){
                                    /* End session *
                                    Send_Byte(setups,CA);
                                    Send_Byte(setups,CA);
                                    return -1;
                                }

                                /* Erase the needed pages where the user application will be loaded */
                                /* Define the number of page to be erased */
                                //NbrOfPage = FLASH_PagesMask(size);

                                /* Erase the FLASH pages *
                                for (EraseCounter = 0; (EraseCounter < NbrOfPage) && (FLASHStatus == FLASH_COMPLETE); EraseCounter++){
                                    FLASHStatus = FLASH_ErasePage(FlashDestination + (PageSize * EraseCounter));
                                }*/
                                
                                if(setups.mem_erase(file_name,size)==_x_true){
                                    Send_Byte(setups,ACK);
                                    Send_Byte(setups,CRC16);
                                    //return 255;
                                }else{
                                    // End session
                                    Send_Byte(setups,CA);
                                    Send_Byte(setups,CA);
                                    return -1;
                                }
                            }else{
                                /* Filename packet is empty, end session */
                                Send_Byte(setups,ACK);
                                file_done = 1;
                                session_done = 1;
                                break;
                            }
                        }
                        //Recieve #(packet_data[PACKET_SEQNO_INDEX] & 0xff) 
                        else{
                            /* Data packet */
                            memcpy(buf_ptr, packet_data + PACKET_HEADER, packet_length);
                            buffer_length +=packet_length;
                            memcpy(buf_data + buffer_length - packet_length, buf_ptr, packet_length);
                            
                            Send_Byte(setups,ACK);
                            /*
                            RamSource = (x_uint32_t)buf;
                            for (j = 0;(j < packet_length) && (FlashDestination <  ApplicationAddress + size);j += 4) {
                                /* Program the data received into STM32F10x Flash */
                                /*FLASH_ProgramWord(FlashDestination, *(x_uint32_t*)RamSource);

                                if (*(x_uint32_t*)FlashDestination != *(x_uint32_t*)RamSource) {
                                    // End session *
                                    Send_Byte(setups,CA);
                                    Send_Byte(setups,CA);
                                    return -2;
                                }
                                FlashDestination += 4;
                                RamSource += 4;
                            }
                            */
                            /*if(
                                setups.mem_write(
                                    file_name
                                    ,packet_length
                                    ,buf_ptr
                                    , 0
                                    )==_x_true){
                                Send_Byte(setups,ACK);
                            }else{
                                // End session
                                Send_Byte(setups,CA);
                                Send_Byte(setups,CA);
                                return -2;
                            }*/
                        }
                        packets_received ++;
                        session_begin = 1;
                    }
                }
                break;
             
            /* abort by user */   
            case 1:
                Send_Byte(setups,CA);
                Send_Byte(setups,CA);
                return -3;
            
            /* timeout or packet error */ 
            default:
                if (session_begin > 0) {
                    errors ++;
                }
                if (errors > MAX_ERRORS) {
                    Send_Byte(setups,CA);
                    Send_Byte(setups,CA);
                    return 0;
                }
                Send_Byte(setups,CRC16);
                //return 255;
                break;
            }
            if (file_done != 0) {
                break;
            }
        }
        if (session_done != 0) {
            break;
        }
    }
    return (x_int32_t)size;
}

/******************************************************************************/
/**
  * @brief  check response using the ymodem protocol
  * @param  buf: Address of the first byte
  * @retval The size of the file
  */
x_int32_t Ymodem_CheckResponse(x_uint8_t c)
{
  return 0;
}

/******************************************************************************/
/**
  * @brief  Prepare the first block
  * @param  timeout
  *     0: end of transmission
  */
void Ymodem_PrepareIntialPacket(x_uint8_t *data, const x_uint8_t* fileName, x_uint32_t *length)
{
  x_uint16_t i, j;
  x_uint8_t file_ptr[10];
  
  /* Make first three packet */
  data[0] = SOH;
  data[1] = 0x00;
  data[2] = 0xff;
  
  /* Filename packet has valid data */
  for (i = 0; (fileName[i] != '\0') && (i < FILE_NAME_LENGTH);i++)
  {
     data[i + PACKET_HEADER] = fileName[i];
  }

  data[i + PACKET_HEADER] = 0x00;
  
  Int2Str (file_ptr, *length);
  for (j =0, i = i + PACKET_HEADER + 1; file_ptr[j] != '\0' ; )
  {
     data[i++] = file_ptr[j++];
  }
  
  for (j = i; j < PACKET_SIZE + PACKET_HEADER; j++)
  {
    data[j] = 0;
  }
}

/******************************************************************************/
/**
  * @brief  Prepare the data packet
  * @param  timeout
  *     0: end of transmission
  */
void Ymodem_PreparePacket(x_uint8_t *SourceBuf, x_uint8_t *data, x_uint8_t pktNo, x_uint32_t sizeBlk)
{
  x_uint16_t i, size, packetSize;
  x_uint8_t* file_ptr;
  
  /* Make first three packet */
  packetSize = sizeBlk >= PACKET_1K_SIZE ? PACKET_1K_SIZE : PACKET_SIZE;
  size = sizeBlk < packetSize ? sizeBlk :packetSize;
  if (packetSize == PACKET_1K_SIZE)
  {
     data[0] = STX;
  }
  else
  {
     data[0] = SOH;
  }
  data[1] = pktNo;
  data[2] = (~pktNo);
  file_ptr = SourceBuf;
  
  /* Filename packet has valid data */
  for (i = PACKET_HEADER; i < size + PACKET_HEADER;i++) {
     data[i] = *file_ptr++;
  }
  if ( size  <= packetSize) {
    for (i = size + PACKET_HEADER; i < packetSize + PACKET_HEADER; i++) {
      data[i] = 0x1A; /* EOF (0x1A) or 0x00 */
    }
  }
}

/******************************************************************************/
/**
  * @brief  Update CRC16 for input byte
  * @param  CRC input value 
  * @param  input byte
   * @retval None
  */
x_uint16_t UpdateCRC16(x_uint16_t crcIn, x_uint8_t byte)
{
    x_uint32_t crc = crcIn;
    x_uint32_t in = byte|0x100;
    do {
        crc <<= 1;
        in <<= 1;
        if(in&0x100)
        ++crc;
        if(crc&0x10000)
        crc ^= 0x1021;
    }
    while(!(in&0x10000));
    return crc&0xffffu;
}

/******************************************************************************/
/**
  * @brief  Cal CRC16 for YModem Packet
  * @param  data
  * @param  length
   * @retval None
  */
x_uint16_t Cal_CRC16(const x_uint8_t* data, x_uint32_t size)
{
    x_uint32_t crc = 0;
    const x_uint8_t* dataEnd = data+size;
    
    while(data<dataEnd)
        crc = UpdateCRC16(crc,*data++);

    crc = UpdateCRC16(crc,0);
    crc = UpdateCRC16(crc,0);
    return crc&0xffffu;
}

/******************************************************************************/
/**
  * @brief  Cal Check sum for YModem Packet
  * @param  data
  * @param  length
   * @retval None
  */
x_uint8_t CalChecksum(const x_uint8_t* data, x_uint32_t size)
{
    x_uint32_t sum = 0;
    const x_uint8_t* dataEnd = data+size;
    while(data < dataEnd )
    sum += *data++;
    return sum&0xffu;
}

/******************************************************************************/
/**
  * @brief  Transmit a data packet using the ymodem protocol
  * @param  data
  * @param  length
   * @retval None
  */
void Ymodem_SendPacket(x_ymodem_setups setups, x_uint8_t *data, x_uint16_t length)
{
    x_uint16_t i;
    i = 0;
    while (i < length) {
        Send_Byte(setups,data[i]);
        i++;
    }
}

/******************************************************************************/
/**
  * @brief  Transmit a file using the ymodem protocol
  * @param  buf: Address of the first byte
  * @retval The size of the file
  */
x_uint8_t x_Ymodem_Transmit (x_ymodem_setups setups, x_uint8_t *buf, const x_uint8_t* sendFileName, x_uint32_t sizeFile)
{
    x_uint8_t packet_data[PACKET_1K_SIZE + PACKET_OVERHEAD];
    x_uint8_t FileName[FILE_NAME_LENGTH];
    x_uint8_t *buf_ptr, tempCheckSum ;
    x_uint16_t tempCRC, blkNumber;
    x_uint8_t receivedC[2], CRC16_F = 0, i;
    x_uint32_t errors, ackReceived, size = 0, pktSize;

    errors = 0;
    ackReceived = 0;
    for (i = 0; i < (FILE_NAME_LENGTH - 1); i++) {
        FileName[i] = sendFileName[i];
    }
    CRC16_F = 1;       

    /* Prepare first block */
    Ymodem_PrepareIntialPacket(&packet_data[0], FileName, &sizeFile);

    do  {
        /* Send Packet */
        Ymodem_SendPacket(setups, packet_data, PACKET_SIZE + PACKET_HEADER);
        /* Send CRC or Check Sum based on CRC16_F */
        if (CRC16_F) {
            tempCRC = Cal_CRC16(&packet_data[3], PACKET_SIZE);
            Send_Byte(setups,tempCRC >> 8);
            Send_Byte(setups,tempCRC & 0xFF);
        } else {
            tempCheckSum = CalChecksum (&packet_data[3], PACKET_SIZE);
            Send_Byte(setups,tempCheckSum);
        }

        /* Wait for Ack and 'C' */
        if (Receive_Byte(setups,&receivedC[0], 10000) == 0) {
            if (receivedC[0] == ACK) { 
                /* Packet transfered correctly */
                ackReceived = 1;
            }
        } else {
            errors++;
        }
    }while (!ackReceived && (errors < 0x0A));

    if (errors >=  0x0A) {
        return errors;
    }
    buf_ptr = buf;
    size = sizeFile;
    blkNumber = 0x01;
    /* Here 1024 bytes package is used to send the packets */

    /* Resend packet if NAK  for a count of 10 else end of commuincation */
    while (size)
    {
        /* Prepare next packet */
        Ymodem_PreparePacket(buf_ptr, &packet_data[0], blkNumber, size);
        ackReceived = 0;
        receivedC[0]= 0;
        errors = 0;
        do {
            /* Send next packet */
            if (size >= PACKET_1K_SIZE)
            {
            pktSize = PACKET_1K_SIZE;

            }
            else
            {
                pktSize = PACKET_SIZE;
            }
            Ymodem_SendPacket(setups, packet_data, pktSize + PACKET_HEADER);
            /* Send CRC or Check Sum based on CRC16_F */
            /* Send CRC or Check Sum based on CRC16_F */
            if (CRC16_F)
            {
             tempCRC = Cal_CRC16(&packet_data[3], pktSize);
             Send_Byte(setups,tempCRC >> 8);
             Send_Byte(setups,tempCRC & 0xFF);
            }
            else
            {
            tempCheckSum = CalChecksum (&packet_data[3], pktSize);
            Send_Byte(setups,tempCheckSum);
            }

            /* Wait for Ack TODO**************
            if ((Receive_Byte(setups,&receivedC[0], 100000) == 0)  && (receivedC[0] == ACK)) {
                ackReceived = 1;  
                if (size > pktSize)
                {
                   buf_ptr += pktSize;  
                   size -= pktSize;
                   if (blkNumber == (FLASH_IMAGE_SIZE/1024)) {
                     return 0xFF; /*  error *
                   } else {
                      blkNumber++;
                   }
                } else {
                  buf_ptr += pktSize;
                  size = 0;
                }
            }
            else {
                errors++;
            }*********/
        }while(!ackReceived && (errors < 0x0A));
        /* Resend packet if NAK  for a count of 10 else end of commuincation */

        if (errors >=  0x0A)
        {
          return errors;
        }

    }//while size
    ackReceived = 0;
    receivedC[0] = 0x00;
    errors = 0;
    do 
    {
        Send_Byte(setups,EOT);
        /* Send (EOT); */
        /* Wait for Ack */
        if ((Receive_Byte(setups,&receivedC[0], 10000) == 0)  && receivedC[0] == ACK)
        {
            ackReceived = 1;  
        }
        else
        {
            errors++;
        }
    }while (!ackReceived && (errors < 0x0A));

    if (errors >=  0x0A)
    {
    return errors;
    }

    /* Last packet preparation */
    ackReceived = 0;
    receivedC[0] = 0x00;
    errors = 0;

    packet_data[0] = SOH;
    packet_data[1] = 0;
    packet_data [2] = 0xFF;

    for (i = PACKET_HEADER; i < (PACKET_SIZE + PACKET_HEADER); i++)
    {
     packet_data [i] = 0x00;
    }

    do 
    {
    /* Send Packet */
    Ymodem_SendPacket(setups, packet_data, PACKET_SIZE + PACKET_HEADER);
    /* Send CRC or Check Sum based on CRC16_F */
    tempCRC = Cal_CRC16(&packet_data[3], PACKET_SIZE);
    Send_Byte(setups,tempCRC >> 8);
    Send_Byte(setups,tempCRC & 0xFF);

    /* Wait for Ack and 'C' */
    if (Receive_Byte(setups,&receivedC[0], 10000) == 0)  
    {
      if (receivedC[0] == ACK)
      { 
        /* Packet transfered correctly */
        ackReceived = 1;
      }
    }
    else
    {
        errors++;
    }

    }while (!ackReceived && (errors < 0x0A));
    /* Resend packet if NAK  for a count of 10  else end of commuincation */
    if (errors >=  0x0A)
    {
        return errors;
    }  

    do 
    {
        Send_Byte(setups,EOT);
        /* Send (EOT); */
        /* Wait for Ack */
        if ((Receive_Byte(setups,&receivedC[0], 10000) == 0)  && receivedC[0] == ACK)
        {
            ackReceived = 1;  
        }
        else
        {
            errors++;
        }
    }while (!ackReceived && (errors < 0x0A));

    if (errors >=  0x0A)
    {
        return errors;
    }
    return 0; /* file trasmitted successfully */
}

/**
  * @}
  */
