/********************************************
 * fbiad.h gpg3100 device driver
 *
 * version 1.00-01
 *
 * Copyright (C) 2000-2002 Interface Corporation
 *
 * ******************************************/
#include "asm/ioctl.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ======== gpg3100 constants ================== */

/*----------------------------------------------*
 *    sampling mode constants                   *
 *----------------------------------------------*/
#define FLAG_SYNC                 1
#define FLAG_ASYNC                2
#define FLAG_TRIGGER              4
#define FLAG_MASTER_MODE          5
#define FLAG_SLAVE_MODE           6

/*----------------------------------------------*
 *    file type constants                       *
 *----------------------------------------------*/
#define FLAG_BIN                  1
#define FLAG_CSV                  2

/*----------------------------------------------*
 *    data formate type constants               *
 *----------------------------------------------*/
#define CONV_BIN                  1
#define CONV_PHYS                 2

/*----------------------------------------------*
 *    sampling status constants                 *
 *----------------------------------------------*/
#define AD_STATUS_STOP_SAMPLING   1
#define AD_STATUS_WAIT_TRIGGER    2
#define AD_STATUS_NOW_SAMPLING    3

/*----------------------------------------------*
 *    sampling event constants                  *
 *----------------------------------------------*/
#define AD_EVENT_SMPLNUM          1
#define AD_EVENT_STOP_TRIGGER     2
#define AD_EVENT_STOP_FUNCTION    3
#define AD_EVENT_STOP_TIMEOUT     4
#define AD_EVENT_STOP_SAMPLING    5
#define AD_EVENT_STOP_SCER        6
#define AD_EVENT_STOP_ORER        7
#define AD_EVENT_SCER             8
#define AD_EVENT_ORER             9
#define AD_EVENT_STOP_LV_1       10
#define AD_EVENT_STOP_LV_2       11
#define AD_EVENT_STOP_LV_3       12
#define AD_EVENT_STOP_LV_4       13
#define AD_EVENT_RANGE           14
#define AD_EVENT_STOP_RANGE      15
#define AD_EVENT_OVPM            16
#define AD_EVENT_STOP_OVPM       17
#define AD_EVENT_DISCONNECTION   0x100

/*-----------------------------------------------*
 *    sampling method constants                  *
 *-----------------------------------------------*/
#define AD_INPUT_SINGLE           1
#define AD_INPUT_DIFF             2
#define AD_INPUT_GND              3
#define AD_INPUT_REFP5V           4
#define AD_INPUT_REFM5V           5
#define AD_INPUT_DACONV           6
#define AD_INPUT_REFP3V           7
#define AD_INPUT_REFM3V           8
#define AD_INPUT_DAC1             9
#define AD_INPUT_DAC2            10
#define AD_INPUT_DAC3            11
#define AD_INPUT_DAC4            12
#define AD_INPUT_DAC5            13
#define AD_INPUT_DAC6            14

/*------------------------------------------------*
 *    volume adjust constans                      *
 *------------------------------------------------*/
#define AD_ADJUST_BIOFFSET        1
#define AD_ADJUST_UNIOFFSET       2
#define AD_ADJUST_BIGAIN          3
#define AD_ADJUST_UNIGAIN         4

/*------------------------------------------------*
 *    voleme adjust constants                     *
 *------------------------------------------------*/
#define AD_ADJUST_UP              1
#define AD_ADJUST_DOWN            2
#define AD_ADJUST_STORE           3
#define AD_ADJUST_STANDBY         4
#define AD_ADJUST_NOT_STORE       5
#define AD_ADJUST_STORE_INITAREA  6

#define AD_ADJUST_READ_FACTORY		1
#define AD_ADJUST_READ_USER			2
/*------------------------------------------------*
 *    data conversion constants                   *
 *------------------------------------------------*/
#define AD_DATA_PHYSICAL          1
#define AD_DATA_BIN8              2
#define AD_DATA_BIN12             3
#define AD_DATA_BIN16             4
#define AD_DATA_BIN24             5
#define AD_DATA_BIN10             6

/*------------------------------------------------*
 *    data conversion constants                   *
 *------------------------------------------------*/
#define AD_CONV_SMOOTH            1
#define AD_CONV_AVERAGE1          2
#define AD_CONV_AVERAGE2          3

/*------------------------------------------------*
 *    sampling formula constants                  *
 *------------------------------------------------*/
#define AD_IO_SAMPLING            1
#define AD_FIFO_SAMPLING          2
#define AD_MEM_SAMPLING           4
#define AD_BM_SAMPLING            8

/*------------------------------------------------*
 *    trigger point constants                     *
 *------------------------------------------------*/
#define AD_TRIG_START             1
#define AD_TRIG_STOP              2
#define AD_TRIG_START_STOP        3

/*------------------------------------------------*
 *    trigger constants                           *
 *------------------------------------------------*/
#define AD_FREERUN                1
#define AD_EXTTRG                 2
#define AD_EXTTRG_DI              3
#define AD_LEVEL_P                4
#define AD_LEVEL_M                5
#define AD_LEVEL_D                6
#define AD_INRANGE                7
#define AD_OUTRANGE               8
#define AD_ETERNITY               9
#define AD_SMPLNUM                10

#define AD_START_SIGTIMER         11
#define AD_START_DA_START         12
#define AD_START_DA_STOP          13
#define AD_START_DA_IO            14
#define AD_START_DA_SMPLNUM       15
#define AD_STOP_SIGTIMER          11
#define AD_STOP_DA_START          12
#define AD_STOP_DA_STOP           13
#define AD_STOP_DA_IO             14
#define AD_STOP_DA_SMPLNUM        15

#define AD_START_P1               0x00000010
#define AD_START_M1               0x00000020
#define AD_START_D1               0x00000040
#define AD_START_P2               0x00000080
#define AD_START_M2               0x00000100
#define AD_START_D2               0x00000200
#define AD_STOP_P1                0x00000400
#define AD_STOP_M1                0x00000800
#define AD_STOP_D1                0x00001000
#define AD_STOP_P2                0x00002000
#define AD_STOP_M2                0x00004000
#define AD_STOP_D2                0x00008000
#define AD_ANALOG_FILTER          0x00010000
#define AD_START_CNT_EQ           0x00020000
#define AD_STOP_CNT_EQ            0x00040000
#define AD_START_DI_EQ            0x00080000
#define AD_STOP_DI_EQ             0x00100000
#define AD_STOP_SOFT              0x00200000
#define AD_START_Z_CLR            0x00400000
#define AD_STOP_Z_CLR             0x00800000
#define AD_START_SYNC             0x01000000
#define AD_STOP_SYNC              0x02000000
#define AD_START_SYNC1            0x10000000	// Start-trigger: Internal Sync1 trigger
#define AD_START_SYNC2            0x20000000	// Start-trigger: Internal Sync2 trigger
#define AD_STOP_SYNC1             0x40000000	// Stop-trigger: Internal Sync1 trigger
#define AD_STOP_SYNC2             0x80000000	// Stop-trigger: Internal Sync2 trigger

/*----------------------------------------------*
 *    trigger edge constants                    *
 *----------------------------------------------*/
#define AD_DOWN_EDGE              1
#define AD_UP_EDGE                2
#define AD_EXTRG_IN               3
#define AD_EXCLK_IN               4
#define AD_LOW_LEVEL              5
#define AD_HIGH_LEVEL             6

#define AD_EDGE_P1                0x0010
#define AD_EDGE_M1                0x0020
#define AD_EDGE_D1                0x0040
#define AD_EDGE_P2                0x0080
#define AD_EDGE_M2                0x0100
#define AD_EDGE_D2                0x0200
#define AD_DISABLE                0x80000000

#define	AD_TRIG_MODE              2
#define	AD_BUSY_MODE              3
#define	AD_POST_MODE              4
#define	AD_ENABLE                 5
#define	AD_SMP1_MODE              6
#define	AD_SMP2_MODE              7
#define	AD_ATRIG_MODE             8

/*------------------------------------------------*
 *    analog trigger edge constants               *
 *------------------------------------------------*/
#define AD_LOW_PULSE              1
#define AD_HIGH_PULSE             2

/*------------------------------------------------*
 *    fast mode constants                         *
 *------------------------------------------------*/
#define AD_NORMAL_MODE            1
#define AD_FAST_MODE              2

/*------------------------------------------------*
 *    status                                      *
 *------------------------------------------------*/
#define AD_NO_STATUS              1
#define AD_ADD_STATUS             2

/*------------------------------------------------*
 *    error of busmaster clock                    *
 *------------------------------------------------*/
#define AD_STOP_SCER              2
#define AD_STOP_ORER              4

/*------------------------------------------------*
 *    write way of busmaster data                 *
 *------------------------------------------------*/
#define AD_APPEND                 1
#define AD_OVERWRITE              2

/*------------------------------------------------*
 *    Degital Filter                              *
 *------------------------------------------------*/
#define AD_DF_8						0	// 8 (default setting)
#define AD_DF_16					1	// 16
#define AD_DF_32					2	// 32
#define AD_DF_64					3	// 64
#define AD_DF_128					4	// 128
#define AD_DF_256					5	// 256

/*------------------------------------------------*
 *    sampling range constants                    *
 *------------------------------------------------*/
#define AD_0_1V                   0x00000001
#define AD_0_2P5V                 0x00000002
#define AD_0_5V                   0x00000004
#define AD_0_10V                  0x00000008
#define AD_1_5V                   0x00000010
#define AD_0_2V                   0x00000020
#define AD_0_0P125V               0x00000040
#define AD_0_1P25V                0x00000080
#define AD_0_0P625V               0x00000100
#define AD_0_0P156V               0x00000200
#define AD_0_20mA                 0x00001000
#define AD_4_20mA                 0x00002000
#define AD_20mA                   0x00004000
#define AD_1V                     0x00010000
#define AD_2P5V                   0x00020000
#define AD_5V                     0x00040000
#define AD_10V                    0x00080000
#define AD_20V                    0x00100000
#define AD_50V                    0x00200000
#define AD_0P125V                 0x00400000
#define AD_1P25V                  0x00800000
#define AD_0P625V                 0x01000000
#define AD_0P156V                 0x02000000
#define AD_1P25V_AC               0x04000000
#define AD_0P625V_AC              0x08000000
#define AD_0P156V_AC              0x10000000
#define AD_AC_COUPLING            0x40000000
#define AD_GND                    0x80000000

#define AD_TYPE_K                 0x00000001
#define AD_TYPE_B                 0x00000002
#define AD_TYPE_R                 0x00000004
#define AD_TYPE_S                 0x00000008
#define AD_TYPE_N                 0x00000010
#define AD_TYPE_E                 0x00000020
#define AD_TYPE_J                 0x00000040
#define AD_TYPE_T                 0x00000080
#define AD_TYPE_AD                0x80000000

/*-------------------------------------------------*
 *    isolation constants                          *
 *-------------------------------------------------*/
#define AD_ISOLATION              1
#define AD_NOT_ISOLATION          2

/*-------------------------------------------------*
 *    sync sampling constants                      *
 *-------------------------------------------------*/
#define AD_MASTER_MODE            1
#define AD_SLAVE_MODE             2

/*-------------------------------------------------*
 *    sync Number constants                        *
 *-------------------------------------------------*/
#define AD_SYNC_NUM_1            0x0100
#define AD_SYNC_NUM_2            0x0200
#define AD_SYNC_NUM_3            0x0400
#define AD_SYNC_NUM_4            0x0800
#define AD_SYNC_NUM_5            0x1000
#define AD_SYNC_NUM_6            0x2000
#define AD_SYNC_NUM_7            0x4000

/*-------------------------------------------------*
 *    calibration                                  *
 *-------------------------------------------------*/
#define AD_SELF_CALIBRATION       1
#define AD_ZEROSCALE_CALIBRATION  2
#define AD_FULLSCALE_CALIBRATION  3

/*-------------------------------------------------*
 *    over/under range status                      *
 *-------------------------------------------------*/
#define AD_STATUS_UNDER_RANGE     1
#define AD_STATUS_OVER_RANGE      2

/*-------------------------------------------------*
 *    compare data status                          *
 *-------------------------------------------------*/
#define AD_STATUS_UP_DATA         1
#define AD_STATUS_DOWN_DATA       2

/*-------------------------------------------------*
 *    PCI-3525 CN3,4 function                      *
 *-------------------------------------------------*/
#define AD_CN_FREE                   0
#define AD_CN_EXTRG_IN               1
#define AD_CN_EXTRG_OUT              2
#define AD_CN_EXCLK_IN               3
#define AD_CN_EXCLK_OUT              4
#define AD_CN_EXINT_IN               5
#define AD_CN_ATRG_OUT               6
#define AD_CN_DI                     7
#define AD_CN_DO                     8
#define AD_CN_EXSMP1_OUT            12
#define AD_CN_EXSMP2_OUT            13
#define AD_CN_DIO					1	// DIO
#define AD_CN_CONTROL				2	// Control used
#define AD_CN_CNT					3	// Counter used

/*-------------------------------------------------*
 *    CPZ-360810 function                          *
 *-------------------------------------------------*/
#define AD_EX_DIO1                1
#define AD_EX_DIO2                2
#define AD_EX_DIO3                3
#define AD_EX_DIO4                4
#define AD_EX_DIO5                5
#define AD_EX_DIO6                6
#define AD_EX_DIO7                7
#define AD_EX_DIO8                8

/*-------------------------------------------------*
 *    temperture                                   *
 *-------------------------------------------------*/
#define AD_GET_CURRENT_TEMPERATURE				1
#define AD_LOAD_TEMPERATURE						2
#define AD_LOAD_FACTORY_SETTING_TEMPERATURE		3
#define AD_SAVE_TEMPERATURE_USER				4

/* ======== structure define ================== */
/***************************************/
/* Structure of AD board specification */
/***************************************/
typedef struct _ADBOARDSPEC {
  unsigned long ulBoardType;
  unsigned long ulBoardID;
  unsigned long ulSamplingMode;
  unsigned long ulChCountS;
  unsigned long ulChCountD;
  unsigned long ulResolution;
  unsigned long ulRange;
  unsigned long ulIsolation;
  unsigned long ulDI;
  unsigned long ulDO;
} ADBOARDSPEC, *PADBOARDSPEC;

/******************************************/
/* Structure of ch Sampling Configuration */
/******************************************/
typedef struct _ADSMPLCHREQ {
  unsigned long ulChNo;
  unsigned long ulRange;
} ADSMPLCHREQ, *PADSMPLCHREQ;

/****************************************/
/* Structure of Sampling Configuration  */
/****************************************/
typedef struct _ADSMPLREQ {
  unsigned long ulChCount;
  ADSMPLCHREQ   SmplChReq[256];
  unsigned long ulSamplingMode;
  unsigned long ulSingleDiff;
  unsigned long ulSmplNum;
  unsigned long ulSmplEventNum;
  float         fSmplFreq;
  unsigned long ulTrigPoint;
  unsigned long ulTrigMode;
  long          lTrigDelay;
  unsigned long ulTrigCh;
  float         fTriglevel1;
  float         fTriglevel2;
  unsigned long ulEClkEdge;
  unsigned long ulATrgPulse;
  unsigned long ulTrigEdge;
  unsigned long ulTrigDI;
  unsigned long ulFastMode;
} ADSMPLREQ, *PADSMPLREQ;

/**********************************/
/* Structure of AdDeviceFind Data */
/**********************************/
typedef struct _AD_FIND_DEV {
  int type;
  int rsw;
  int subsys;
}AD_FIND_DEV, *PAD_FIND_DEV;

/***************************************************/
/* Structure of Analog Trigger Config in BusMaster */
/***************************************************/
typedef struct _ADTRIGCHREQ {
  unsigned long ulChNo;
  float         fTrigLevel;
  float         fHysteresis;
} ADTRIGCHREQ, *PADTRGCHREQ;

/*****************************************************/
/* Structure of Sampling Configuration  in BusMaster */
/*****************************************************/
typedef struct _ADBMSMPLREQ {
  unsigned long ulChCount;
  ADSMPLCHREQ   SmplChReq[256];
  unsigned long ulSingleDiff;
  unsigned long ulSmplNum;
  unsigned long ulSmplEventNum;
  unsigned long ulSmplRepeat;
  unsigned long ulBufferMode;
  float         fSmplFreq;
  float         fScanFreq;
  unsigned long ulStartMode;
  unsigned long ulStopMode;
  unsigned long ulPreTrigDelay;
  unsigned long ulPostTrigDelay;
  ADTRIGCHREQ   TrigChReq[2];
  unsigned long ulATrgMode;
  unsigned long ulATrgPulse;
  unsigned long ulStartTrigEdge;
  unsigned long ulStopTrigEdge;
  unsigned long ulTrigDI;
  unsigned long ulEClkEdge;
  unsigned long ulFastMode;
  unsigned long ulStatusMode;
  unsigned long ulErrCtrl;
} ADBMSMPLREQ, *PADBMSMPLREQ;

/*****************************************************/
/* Structure of Sampling Configuration  in Memory    */
/*****************************************************/
typedef struct _ADMEMSMPLREQ {
  unsigned long ulChCount;
  ADSMPLCHREQ   SmplChReq[256];
  unsigned long ulSingleDiff;
  float         fSmplFreq;
  unsigned long ulStopMode;
  unsigned long ulPreTrigDelay;
  unsigned long ulPostTrigDelay;
  ADTRIGCHREQ   TrigChReq[2];
  unsigned long ulATrgMode;
  unsigned long ulATrgPulse;
  unsigned long ulStopTrigEdge;
  unsigned long ulEClkEdge;
  unsigned long ulFastMode;
  unsigned long ulStatusMode;
  unsigned long ulErrCtrl;
} ADMEMSMPLREQ, *PADMEMSMPLREQ;

typedef void (*PLPADCALLBACK)(int);
typedef void (*PADCONVPROC)(int wCh, unsigned long dwCount, void *pDat);

/* ======== function prototype ================== */
int AdOpen(int nDevice);
int AdClose(int nDevice);

int AdGetDeviceInfo(int nDevice, PADBOARDSPEC pBoardSpec);

int AdSetBoardConfig(int nDevice, int reserved, PLPADCALLBACK pEventProc, int uData);
int AdGetBoardConfig(int nDevice, unsigned long *ulAdSmplEventFactor);

int AdSetSamplingConfig(int nDevice, PADSMPLREQ pSmplConfig);
int AdGetSamplingConfig(int nDevice, PADSMPLREQ pSmplConfig);

int AdGetSamplingData(int nDevice, void *pSmplData, unsigned long *ulSmplNum);
int AdFifoGetSamplingData(int nDevice, void *pSmplData, void *pDiData, unsigned long *ulSmplNum);
int AdReadSamplingBuffer(int nDevice, long lOffset, unsigned long *ulSmplNum, void *pSmplData);
int AdClearSamplingData(int nDevice);

int AdStartSampling(int nDevice, int uSyncFlag);
int AdTriggerSampling(int nDevice, unsigned long ulChNo, unsigned long ulRange, unsigned long ulSingleDiff, unsigned long ulTriggerMode, unsigned long ulTrigEdge, unsigned long ulSmplNum);
int AdMemTriggerSampling(int nDevice, unsigned long ulChCount, PADSMPLCHREQ SmplChReq, unsigned long ulSmplNum, unsigned long ulRepeatCount, unsigned long ulTrigEdge, float fSmplFreq, unsigned long ulEClkEdge, unsigned long ulFastMode);
int AdSyncSampling(int nDevice, unsigned long ulMode);

int AdStopSampling(int nDevice);
int AdGetStatus(int uDevice, unsigned long *ulAdSmplStatus, unsigned long *ulAdSMplCount, unsigned long *ulAdAvailCount);
int AdInputAD(int nDevice, unsigned long uCh, unsigned long ulSingleDiff, PADSMPLCHREQ pSmplChReq, void *pData);

int AdInputDI(int nDevice, unsigned long *uData);
int AdOutputDO(int nDevice, unsigned long uData);

int AdDataConv(int nSrcFormCode, void *pSrcData, int nSrcSmplDataNum, PADSMPLREQ pSrcSmplReq, int nDestFormCode, void *pDestData, int *pnDestSmplDataNum, PADSMPLREQ pDestSmplReq, int nEffect, int nCount, PADCONVPROC lpfnConv);

int AdDeviceFind(PAD_FIND_DEV pDevTbl);

int AdSetRangeEvent(int nDevice, unsigned long ulEventMask, unsigned long ulStopMode);
int AdResetRangeEvent(int nDevice);
int AdGetRangeEventStatus(int nDevice, unsigned long *ulEventChNo, unsigned long *ulEventStatus);

int AdLvSetSamplingConfig(int nDevice, unsigned long ulChNo, unsigned long ulSmplNum, float fSmplFreq, unsigned long ulRange, int reserved1, PLPADCALLBACK pEventProc, int reserved2);
int AdLvGetSamplingConfig(int nDevice, unsigned long ulChNo, unsigned long *ulSmplNum, float *fSmplFreq, unsigned long *ulRange);
int AdLvCalibration(int nDevice, unsigned long ulChNo, unsigned long ulCalibration);
int AdLvGetSamplingData(int nDevice, unsigned long ulChNo, void *pSmplData, unsigned long *ulSmplDataNum);
int AdLvStartSampling(int nDevice, unsigned long ulCnNo);
int AdLvStopSampling(int nDevice, unsigned long ulCnNo);
int AdLvGetStatus(int nDevice, unsigned long ulChNo, unsigned long *ulAdSmplStatus, unsigned long *ulAdSmplCount, unsigned long *ulAdAvailCount);
int AdMeasureTemperature(int nDevice, float *fTemperature);

int AdBmSetSamplingConfig(int, PADBMSMPLREQ);
int AdBmGetSamplingConfig(int, PADBMSMPLREQ);
int AdBmGetSamplingData(int, void*, unsigned long*);

int AdGetOverRangeChStatus(int, unsigned long*);
int AdResetOverRangeCh(int);

int AdSetInterval(int, unsigned long);
int AdGetInterval(int, unsigned long*);
int AdSetFunction(int, unsigned int, unsigned long);
int AdGetFunction(int, unsigned int, unsigned long*);

int AdSetFilter(int, unsigned long);
int AdGetFilter(int, unsigned long*);

int AdCommonGetPciDeviceInfo(int, unsigned long*, unsigned long*, unsigned long*, unsigned long*, unsigned long*, unsigned long*, unsigned long*, unsigned long*, unsigned long*, unsigned long*, unsigned long*, unsigned long*, unsigned long*, unsigned long*);

int AdMemSetSamplingConfig(int, PADMEMSMPLREQ);
int AdMemGetSamplingConfig(int, PADMEMSMPLREQ);
int AdSetOutMode(int, unsigned long, unsigned long);
int AdGetOutMode(int, unsigned long*, unsigned long*);
int AdMemSetDiPattern(int, unsigned long, unsigned long*);
int AdMemGetDiPattern(int, unsigned long*);

int AdOpenEx(unsigned short wType, unsigned short wSubSystemID, unsigned char bRSW1, int *pnDevice);


#ifdef __RTL__
  int AdSetInputADEx(int nDevice, unsigned long uCh, unsigned long ulSingleDiff, PADSMPLCHREQ pSmplChReq, int uMode);
  int AdInputADEx(int nDevice, void *pData);
  int AdSetSamplingBuffer(int nDevice, void* pSmplBuffer);
  int AdSetDmaBuffer(int nDevice, void* pDmaBuffer, unsigned long ulBufferSize);
  int AdSetDiBuffer(int nDevice, void* pSmplBuffer);
  int AdStartSamplingEx(int nDevice);
#else
  int AdSetInputADEx(int nDevice, unsigned long uCh, unsigned long ulSingleDiff, PADSMPLCHREQ pSmplChReq);
  int AdInputADEx(int nDevice, void *pData);
  int AdStartFileSampling(int nDevice, char *szPathName, unsigned long ulFileFlag);
  int AdReadFile(char *PathName, void *pSmplData, unsigned long uFormCode);
  int AdBmStartFileSampling(int nDevice, char *szPathName, unsigned long uFileFlag);
  int AdStartSamplingEx(int nDevice, void *pBuffer, unsigned long BufferSize);
  int AdOutputSync(int uDevice, unsigned long Line, unsigned long TrgMode);
  int AdCalibration(int uDevice, unsigned long ulChannel, unsigned long ulRange, unsigned long ulSingleDiff);
  int AdSetDisConnction(int uDevice, unsigned int Channel);
  int AdGetDisConnction(int uDevice, unsigned int *Channel);
  int AdGetDisConnctionStatus(int uDevice, unsigned int *Status);
#endif

/* ======== error code ======================== */
#define AD_ERROR_SUCCESS                         0
#define AD_ERROR_NOT_DEVICE                      0xC0000001
#define AD_ERROR_NOT_OPEN                        0xC0000002
#define AD_ERROR_INVALID_DEVICENO                0xC0000003
#define AD_ERROR_ALREADY_OPEN                    0xC0000004
#define AD_ERROR_INVALID_DEVICE_NUMBER           0xC0000005
#define AD_ERROR_NOT_SUPPORTED                   0xC0000009
#define AD_ERROR_NOW_SAMPLING                    0xC0001001
#define AD_ERROR_STOP_SAMPLING                   0xC0001002
#define AD_ERROR_INVALID_PARAMETER               0xC0001021
#define AD_ERROR_NULL_POINTER                    0xC0001023
#define AD_ERROR_GET_DATA                        0xC0001024
#define AD_ERROR_USED_DA                         0xC0001025
#define AD_ERROR_FILE_OPEN                       0xC0001041
#define AD_ERROR_FILE_CLOSE                      0xC0001042
#define AD_ERROR_FILE_READ                       0xC0001043
#define AD_ERROR_FILE_WRITE                      0xC0001044
#define AD_ERROR_INVALID_DATA_FORMAT             0xC0001061
#define AD_ERROR_INVALID_AVERAGE_OR_SMOOTHING    0xC0001062
#define AD_ERROR_INVALID_SOURCE_DATA             0xC0001063
#define AD_ERROR_NOT_ALLOCATE_MEMORY             0xC0001081
#define AD_ERROR_CALIBRATION                     0xC0001084

#define AD_ERROR_USBIO_FAILED                    0xC0001085
#define AD_ERROR_USBIO_TIMEOUT                   0xC0001086
#define AD_ERROR_USBLIB_LOAD_FAILED              0xC0001087

#ifdef __cplusplus
}
#endif

/* ============================================ */
