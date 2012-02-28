/********************************************************************/
/* Coldfire C Header File
 *
 *     Date      : 2009/04/30
 *     Revision  : 0.94
 *
 *     Copyright : 1997 - 2009 Freescale Semiconductor, Inc. All Rights Reserved.
 *
 *     http      : www.freescale.com
 *     mail      : support@freescale.com
 */

#ifndef __MCF5282_CANMB_H__
#define __MCF5282_CANMB_H__


/*********************************************************************
*
* Flex Controller Area Network Module (FlexCAN) message buffers
*
*********************************************************************/

/* Register read/write macros */
#define MCF_CANMB_TIME0                      (*(vuint8 *)(&__IPSBAR[0x1C0080]))
#define MCF_CANMB_CTRL0                      (*(vuint8 *)(&__IPSBAR[0x1C0081]))
#define MCF_CANMB_ID0                        (*(vuint32*)(&__IPSBAR[0x1C0082]))
#define MCF_CANMB_SID0                       (*(vuint16*)(&__IPSBAR[0x1C0082]))
#define MCF_CANMB_TIME16_0                   (*(vuint16*)(&__IPSBAR[0x1C0084]))
#define MCF_CANMB_DATA_WORD_1_0              (*(vuint16*)(&__IPSBAR[0x1C0086]))
#define MCF_CANMB_DATA_WORD_2_0              (*(vuint16*)(&__IPSBAR[0x1C0088]))
#define MCF_CANMB_DATA_WORD_3_0              (*(vuint16*)(&__IPSBAR[0x1C008A]))
#define MCF_CANMB_DATA_WORD_4_0              (*(vuint16*)(&__IPSBAR[0x1C008C]))
#define MCF_CANMB_TIME1                      (*(vuint8 *)(&__IPSBAR[0x1C0090]))
#define MCF_CANMB_CTRL1                      (*(vuint8 *)(&__IPSBAR[0x1C0091]))
#define MCF_CANMB_ID1                        (*(vuint32*)(&__IPSBAR[0x1C0092]))
#define MCF_CANMB_SID1                       (*(vuint16*)(&__IPSBAR[0x1C0092]))
#define MCF_CANMB_TIME16_1                   (*(vuint16*)(&__IPSBAR[0x1C0094]))
#define MCF_CANMB_DATA_WORD_1_1              (*(vuint16*)(&__IPSBAR[0x1C0096]))
#define MCF_CANMB_DATA_WORD_2_1              (*(vuint16*)(&__IPSBAR[0x1C0098]))
#define MCF_CANMB_DATA_WORD_3_1              (*(vuint16*)(&__IPSBAR[0x1C009A]))
#define MCF_CANMB_DATA_WORD_4_1              (*(vuint16*)(&__IPSBAR[0x1C009C]))
#define MCF_CANMB_TIME2                      (*(vuint8 *)(&__IPSBAR[0x1C00A0]))
#define MCF_CANMB_CTRL2                      (*(vuint8 *)(&__IPSBAR[0x1C00A1]))
#define MCF_CANMB_SID2                       (*(vuint16*)(&__IPSBAR[0x1C00A2]))
#define MCF_CANMB_ID2                        (*(vuint32*)(&__IPSBAR[0x1C00A2]))
#define MCF_CANMB_TIME16_2                   (*(vuint16*)(&__IPSBAR[0x1C00A4]))
#define MCF_CANMB_DATA_WORD_1_2              (*(vuint16*)(&__IPSBAR[0x1C00A6]))
#define MCF_CANMB_DATA_WORD_2_2              (*(vuint16*)(&__IPSBAR[0x1C00A8]))
#define MCF_CANMB_DATA_WORD_3_2              (*(vuint16*)(&__IPSBAR[0x1C00AA]))
#define MCF_CANMB_DATA_WORD_4_2              (*(vuint16*)(&__IPSBAR[0x1C00AC]))
#define MCF_CANMB_TIME3                      (*(vuint8 *)(&__IPSBAR[0x1C00B0]))
#define MCF_CANMB_CTRL3                      (*(vuint8 *)(&__IPSBAR[0x1C00B1]))
#define MCF_CANMB_ID3                        (*(vuint32*)(&__IPSBAR[0x1C00B2]))
#define MCF_CANMB_SID3                       (*(vuint16*)(&__IPSBAR[0x1C00B2]))
#define MCF_CANMB_TIME16_3                   (*(vuint16*)(&__IPSBAR[0x1C00B4]))
#define MCF_CANMB_DATA_WORD_1_3              (*(vuint16*)(&__IPSBAR[0x1C00B6]))
#define MCF_CANMB_DATA_WORD_2_3              (*(vuint16*)(&__IPSBAR[0x1C00B8]))
#define MCF_CANMB_DATA_WORD_3_3              (*(vuint16*)(&__IPSBAR[0x1C00BA]))
#define MCF_CANMB_DATA_WORD_4_3              (*(vuint16*)(&__IPSBAR[0x1C00BC]))
#define MCF_CANMB_TIME4                      (*(vuint8 *)(&__IPSBAR[0x1C00C0]))
#define MCF_CANMB_CTRL4                      (*(vuint8 *)(&__IPSBAR[0x1C00C1]))
#define MCF_CANMB_SID4                       (*(vuint16*)(&__IPSBAR[0x1C00C2]))
#define MCF_CANMB_ID4                        (*(vuint32*)(&__IPSBAR[0x1C00C2]))
#define MCF_CANMB_TIME16_4                   (*(vuint16*)(&__IPSBAR[0x1C00C4]))
#define MCF_CANMB_DATA_WORD_1_4              (*(vuint16*)(&__IPSBAR[0x1C00C6]))
#define MCF_CANMB_DATA_WORD_2_4              (*(vuint16*)(&__IPSBAR[0x1C00C8]))
#define MCF_CANMB_DATA_WORD_3_4              (*(vuint16*)(&__IPSBAR[0x1C00CA]))
#define MCF_CANMB_DATA_WORD_4_4              (*(vuint16*)(&__IPSBAR[0x1C00CC]))
#define MCF_CANMB_TIME5                      (*(vuint8 *)(&__IPSBAR[0x1C00D0]))
#define MCF_CANMB_CTRL5                      (*(vuint8 *)(&__IPSBAR[0x1C00D1]))
#define MCF_CANMB_ID5                        (*(vuint32*)(&__IPSBAR[0x1C00D2]))
#define MCF_CANMB_SID5                       (*(vuint16*)(&__IPSBAR[0x1C00D2]))
#define MCF_CANMB_TIME16_5                   (*(vuint16*)(&__IPSBAR[0x1C00D4]))
#define MCF_CANMB_DATA_WORD_1_5              (*(vuint16*)(&__IPSBAR[0x1C00D6]))
#define MCF_CANMB_DATA_WORD_2_5              (*(vuint16*)(&__IPSBAR[0x1C00D8]))
#define MCF_CANMB_DATA_WORD_3_5              (*(vuint16*)(&__IPSBAR[0x1C00DA]))
#define MCF_CANMB_DATA_WORD_4_5              (*(vuint16*)(&__IPSBAR[0x1C00DC]))
#define MCF_CANMB_TIME6                      (*(vuint8 *)(&__IPSBAR[0x1C00E0]))
#define MCF_CANMB_CTRL6                      (*(vuint8 *)(&__IPSBAR[0x1C00E1]))
#define MCF_CANMB_ID6                        (*(vuint32*)(&__IPSBAR[0x1C00E2]))
#define MCF_CANMB_SID6                       (*(vuint16*)(&__IPSBAR[0x1C00E2]))
#define MCF_CANMB_TIME16_6                   (*(vuint16*)(&__IPSBAR[0x1C00E4]))
#define MCF_CANMB_DATA_WORD_1_6              (*(vuint16*)(&__IPSBAR[0x1C00E6]))
#define MCF_CANMB_DATA_WORD_2_6              (*(vuint16*)(&__IPSBAR[0x1C00E8]))
#define MCF_CANMB_DATA_WORD_3_6              (*(vuint16*)(&__IPSBAR[0x1C00EA]))
#define MCF_CANMB_DATA_WORD_4_6              (*(vuint16*)(&__IPSBAR[0x1C00EC]))
#define MCF_CANMB_TIME7                      (*(vuint8 *)(&__IPSBAR[0x1C00F0]))
#define MCF_CANMB_CTRL7                      (*(vuint8 *)(&__IPSBAR[0x1C00F1]))
#define MCF_CANMB_SID7                       (*(vuint16*)(&__IPSBAR[0x1C00F2]))
#define MCF_CANMB_ID7                        (*(vuint32*)(&__IPSBAR[0x1C00F2]))
#define MCF_CANMB_TIME16_7                   (*(vuint16*)(&__IPSBAR[0x1C00F4]))
#define MCF_CANMB_DATA_WORD_1_7              (*(vuint16*)(&__IPSBAR[0x1C00F6]))
#define MCF_CANMB_DATA_WORD_2_7              (*(vuint16*)(&__IPSBAR[0x1C00F8]))
#define MCF_CANMB_DATA_WORD_3_7              (*(vuint16*)(&__IPSBAR[0x1C00FA]))
#define MCF_CANMB_DATA_WORD_4_7              (*(vuint16*)(&__IPSBAR[0x1C00FC]))
#define MCF_CANMB_TIME8                      (*(vuint8 *)(&__IPSBAR[0x1C0100]))
#define MCF_CANMB_CTRL8                      (*(vuint8 *)(&__IPSBAR[0x1C0101]))
#define MCF_CANMB_SID8                       (*(vuint16*)(&__IPSBAR[0x1C0102]))
#define MCF_CANMB_ID8                        (*(vuint32*)(&__IPSBAR[0x1C0102]))
#define MCF_CANMB_TIME16_8                   (*(vuint16*)(&__IPSBAR[0x1C0104]))
#define MCF_CANMB_DATA_WORD_1_8              (*(vuint16*)(&__IPSBAR[0x1C0106]))
#define MCF_CANMB_DATA_WORD_2_8              (*(vuint16*)(&__IPSBAR[0x1C0108]))
#define MCF_CANMB_DATA_WORD_3_8              (*(vuint16*)(&__IPSBAR[0x1C010A]))
#define MCF_CANMB_DATA_WORD_4_8              (*(vuint16*)(&__IPSBAR[0x1C010C]))
#define MCF_CANMB_TIME9                      (*(vuint8 *)(&__IPSBAR[0x1C0110]))
#define MCF_CANMB_CTRL9                      (*(vuint8 *)(&__IPSBAR[0x1C0111]))
#define MCF_CANMB_ID9                        (*(vuint32*)(&__IPSBAR[0x1C0112]))
#define MCF_CANMB_SID9                       (*(vuint16*)(&__IPSBAR[0x1C0112]))
#define MCF_CANMB_TIME16_9                   (*(vuint16*)(&__IPSBAR[0x1C0114]))
#define MCF_CANMB_DATA_WORD_1_9              (*(vuint16*)(&__IPSBAR[0x1C0116]))
#define MCF_CANMB_DATA_WORD_2_9              (*(vuint16*)(&__IPSBAR[0x1C0118]))
#define MCF_CANMB_DATA_WORD_3_9              (*(vuint16*)(&__IPSBAR[0x1C011A]))
#define MCF_CANMB_DATA_WORD_4_9              (*(vuint16*)(&__IPSBAR[0x1C011C]))
#define MCF_CANMB_TIME10                     (*(vuint8 *)(&__IPSBAR[0x1C0120]))
#define MCF_CANMB_CTRL10                     (*(vuint8 *)(&__IPSBAR[0x1C0121]))
#define MCF_CANMB_SID10                      (*(vuint16*)(&__IPSBAR[0x1C0122]))
#define MCF_CANMB_ID10                       (*(vuint32*)(&__IPSBAR[0x1C0122]))
#define MCF_CANMB_TIME16_10                  (*(vuint16*)(&__IPSBAR[0x1C0124]))
#define MCF_CANMB_DATA_WORD_1_10             (*(vuint16*)(&__IPSBAR[0x1C0126]))
#define MCF_CANMB_DATA_WORD_2_10             (*(vuint16*)(&__IPSBAR[0x1C0128]))
#define MCF_CANMB_DATA_WORD_3_10             (*(vuint16*)(&__IPSBAR[0x1C012A]))
#define MCF_CANMB_DATA_WORD_4_10             (*(vuint16*)(&__IPSBAR[0x1C012C]))
#define MCF_CANMB_TIME11                     (*(vuint8 *)(&__IPSBAR[0x1C0130]))
#define MCF_CANMB_CTRL11                     (*(vuint8 *)(&__IPSBAR[0x1C0131]))
#define MCF_CANMB_SID11                      (*(vuint16*)(&__IPSBAR[0x1C0132]))
#define MCF_CANMB_ID11                       (*(vuint32*)(&__IPSBAR[0x1C0132]))
#define MCF_CANMB_TIME16_11                  (*(vuint16*)(&__IPSBAR[0x1C0134]))
#define MCF_CANMB_DATA_WORD_1_11             (*(vuint16*)(&__IPSBAR[0x1C0136]))
#define MCF_CANMB_DATA_WORD_2_11             (*(vuint16*)(&__IPSBAR[0x1C0138]))
#define MCF_CANMB_DATA_WORD_3_11             (*(vuint16*)(&__IPSBAR[0x1C013A]))
#define MCF_CANMB_DATA_WORD_4_11             (*(vuint16*)(&__IPSBAR[0x1C013C]))
#define MCF_CANMB_TIME12                     (*(vuint8 *)(&__IPSBAR[0x1C0140]))
#define MCF_CANMB_CTRL12                     (*(vuint8 *)(&__IPSBAR[0x1C0141]))
#define MCF_CANMB_ID12                       (*(vuint32*)(&__IPSBAR[0x1C0142]))
#define MCF_CANMB_SID12                      (*(vuint16*)(&__IPSBAR[0x1C0142]))
#define MCF_CANMB_TIME16_12                  (*(vuint16*)(&__IPSBAR[0x1C0144]))
#define MCF_CANMB_DATA_WORD_1_               (*(vuint16*)(&__IPSBAR[0x1C0146]))
#define MCF_CANMB_DATA_WORD_2_12             (*(vuint16*)(&__IPSBAR[0x1C0148]))
#define MCF_CANMB_DATA_WORD_3_12             (*(vuint16*)(&__IPSBAR[0x1C014A]))
#define MCF_CANMB_DATA_WORD_4_12             (*(vuint16*)(&__IPSBAR[0x1C014C]))
#define MCF_CANMB_TIME13                     (*(vuint8 *)(&__IPSBAR[0x1C0150]))
#define MCF_CANMB_CTRL13                     (*(vuint8 *)(&__IPSBAR[0x1C0151]))
#define MCF_CANMB_SID13                      (*(vuint16*)(&__IPSBAR[0x1C0152]))
#define MCF_CANMB_ID13                       (*(vuint32*)(&__IPSBAR[0x1C0152]))
#define MCF_CANMB_TIME16_13                  (*(vuint16*)(&__IPSBAR[0x1C0154]))
#define MCF_CANMB_DATA_WORD_1_13             (*(vuint16*)(&__IPSBAR[0x1C0156]))
#define MCF_CANMB_DATA_WORD_2_13             (*(vuint16*)(&__IPSBAR[0x1C0158]))
#define MCF_CANMB_DATA_WORD_3_13             (*(vuint16*)(&__IPSBAR[0x1C015A]))
#define MCF_CANMB_DATA_WORD_4_13             (*(vuint16*)(&__IPSBAR[0x1C015C]))
#define MCF_CANMB_TIME14                     (*(vuint8 *)(&__IPSBAR[0x1C0160]))
#define MCF_CANMB_CTRL14                     (*(vuint8 *)(&__IPSBAR[0x1C0161]))
#define MCF_CANMB_SID14                      (*(vuint16*)(&__IPSBAR[0x1C0162]))
#define MCF_CANMB_ID14                       (*(vuint32*)(&__IPSBAR[0x1C0162]))
#define MCF_CANMB_TIME16_14                  (*(vuint16*)(&__IPSBAR[0x1C0164]))
#define MCF_CANMB_DATA_WORD_1_14             (*(vuint16*)(&__IPSBAR[0x1C0166]))
#define MCF_CANMB_DATA_WORD_2_14             (*(vuint16*)(&__IPSBAR[0x1C0168]))
#define MCF_CANMB_DATA_WORD_3_14             (*(vuint16*)(&__IPSBAR[0x1C016A]))
#define MCF_CANMB_DATA_WORD_4_14             (*(vuint16*)(&__IPSBAR[0x1C016C]))
#define MCF_CANMB_TIME15                     (*(vuint8 *)(&__IPSBAR[0x1C0170]))
#define MCF_CANMB_CTRL15                     (*(vuint8 *)(&__IPSBAR[0x1C0171]))
#define MCF_CANMB_ID15                       (*(vuint32*)(&__IPSBAR[0x1C0172]))
#define MCF_CANMB_SID15                      (*(vuint16*)(&__IPSBAR[0x1C0172]))
#define MCF_CANMB_TIME16_15                  (*(vuint16*)(&__IPSBAR[0x1C0174]))
#define MCF_CANMB_DATA_WORD_1_15             (*(vuint16*)(&__IPSBAR[0x1C0176]))
#define MCF_CANMB_DATA_WORD_2_15             (*(vuint16*)(&__IPSBAR[0x1C0178]))
#define MCF_CANMB_DATA_WORD_3_15             (*(vuint16*)(&__IPSBAR[0x1C017A]))
#define MCF_CANMB_DATA_WORD_4_15             (*(vuint16*)(&__IPSBAR[0x1C017C]))
#define MCF_CANMB_TIME(x)                    (*(vuint8 *)(&__IPSBAR[0x1C0080 + ((x)*0x10)]))
#define MCF_CANMB_CTRL(x)                    (*(vuint8 *)(&__IPSBAR[0x1C0081 + ((x)*0x10)]))
#define MCF_CANMB_ID(x)                      (*(vuint32*)(&__IPSBAR[0x1C0082 + ((x)*0x10)]))
#define MCF_CANMB_SID(x)                     (*(vuint16*)(&__IPSBAR[0x1C0082 + ((x)*0x10)]))
#define MCF_CANMB_TIME16(x)                  (*(vuint16*)(&__IPSBAR[0x1C0084 + ((x)*0x10)]))
#define MCF_CANMB_DATA_WORD_1(x)             (*(vuint16*)(&__IPSBAR[0x1C0086 + ((x)*0x10)]))
#define MCF_CANMB_DATA_WORD_2(x)             (*(vuint16*)(&__IPSBAR[0x1C0088 + ((x)*0x10)]))
#define MCF_CANMB_DATA_WORD_3(x)             (*(vuint16*)(&__IPSBAR[0x1C008A + ((x)*0x10)]))
#define MCF_CANMB_DATA_WORD_4(x)             (*(vuint16*)(&__IPSBAR[0x1C008C + ((x)*0x10)]))


/* Other macros */
#define MCF_CANMB_BYTE(x,y)                  (*(vuint8 *)(&__IPSBAR[((0x1C0086 + ((x)*0x10)+y))]))


/* Bit definitions and macros for MCF_CANMB_TIME */
#define MCF_CANMB_TIME_TIME_STAMP(x)         (((x)&0xFF)<<0)

/* Bit definitions and macros for MCF_CANMB_CTRL */
#define MCF_CANMB_CTRL_LENGTH(x)             (((x)&0xF)<<0)
#define MCF_CANMB_CTRL_CODE(x)               (((x)&0xF)<<0x4)

/* Bit definitions and macros for MCF_CANMB_ID */
#define MCF_CANMB_ID_RTR                     (0x1)
#define MCF_CANMB_ID_EXT(x)                  (((x)&0x3FFFF)<<0x1)
#define MCF_CANMB_ID_IDE                     (0x80000)
#define MCF_CANMB_ID_SRR                     (0x100000)
#define MCF_CANMB_ID_STD(x)                  (((x)&0x7FF)<<0x15)

/* Bit definitions and macros for MCF_CANMB_SID */
#define MCF_CANMB_SID_RTR                    (0x10)
#define MCF_CANMB_SID_STD(x)                 (((x)&0x7FF)<<0x5)

/* Bit definitions and macros for MCF_CANMB_TIME16 */
#define MCF_CANMB_TIME16_TIMESTAMP16(x)      (((x)&0xFFFF)<<0)

/* Bit definitions and macros for MCF_CANMB_DATA_WORD_1 */
#define MCF_CANMB_DATA_WORD_1_DATA_BYTE_1(x) (((x)&0xFF)<<0)
#define MCF_CANMB_DATA_WORD_1_DATA_BYTE_0(x) (((x)&0xFF)<<0x8)

/* Bit definitions and macros for MCF_CANMB_DATA_WORD_2 */
#define MCF_CANMB_DATA_WORD_2_DATA_BYTE_3(x) (((x)&0xFF)<<0)
#define MCF_CANMB_DATA_WORD_2_DATA_BYTE_2(x) (((x)&0xFF)<<0x8)

/* Bit definitions and macros for MCF_CANMB_DATA_WORD_3 */
#define MCF_CANMB_DATA_WORD_3_DATA_BYTE_5(x) (((x)&0xFF)<<0)
#define MCF_CANMB_DATA_WORD_3_DATA_BYTE_4(x) (((x)&0xFF)<<0x8)

/* Bit definitions and macros for MCF_CANMB_DATA_WORD_4 */
#define MCF_CANMB_DATA_WORD_4_DATA_BYTE_7(x) (((x)&0xFF)<<0)
#define MCF_CANMB_DATA_WORD_4_DATA_BYTE_6(x) (((x)&0xFF)<<0x8)


#endif /* __MCF5282_CANMB_H__ */
