#ifndef MAPROTOCOL_H
#define MAPROTOCOL_H

#include <QByteArray>
#include <QList>
#include <lib/firmware.h>
#include <lib/mabaseprotocol.h>
#include <lib/maupdater.h>

class MaRc : public MABaseProtocol
{
public:
    MaRc();
    RawDataList paramRequest();
    RawDataList devRequest();
    RawDataList setTemp(int t);
    RawDataList setSpeed(int sp);
    RawDataList setHeaterState(bool st);
    RawDataList setPowerState(bool st);
    RawDataList setNoTest();
    RawDataList setGateTest();
    RawDataList setHeaterTest();
    RawDataList resetFilter();
    RawDataList resetHard();
    RawDataList switchSound();

    int8_t temp() const;
    uint8_t speed() const;
    bool powerState() const;
    bool heaterState() const;
    bool noTest() const;
    bool gateTest() const;
    bool heaterTest() const;
    int tempIn() const;
    int tempOut() const;
    int heaterVar() const;
    int filterCounter() const;
    int elecCounter() const;
    int motorCounter() const;
    int errorCnt(int numb) const;
    bool error(int numb) const;
    int airFlowCnt() const;
    int firmVer() const;
    int speedLimit() const;

private:
    enum{
        BLE_FRAME_TYPE_SET_MODE_NO_SAVE_REQ	=	0x1230,
        BLE_FRAME_TYPE_GET_MODE_RESP		=	0x1231,
        BLE_FRAME_TYPE_GET_MODE_REQ         =	0x1232,
        BLE_FRAME_TYPE_SET_MODE_SAVE_REQ	=	0x1234,
        BLE_FRAME_TYPE_DEV_STATUS_REQ		=   0x4009,
        BLE_FRAME_TYPE_DEV_STATUS_RESP 		=	0x400A,
    };

    //***************************************************
    // RC level
    //***************************************************

    enum
    {
        NO_TEST,
        DEFAULT_TEST,
        GATE_TEST,
        HEATER_TEST
    };

    enum
    {
        ERROR_TYPE_NUMBER = 20
    };

    enum
    {
        PRESET_NUMBER = 3
    };

    typedef struct
    {
         int8_t temp[PRESET_NUMBER];
        uint8_t fan_speed[PRESET_NUMBER];
    } brlt_settings_presets_t;

    typedef struct
    {
        uint8_t max_fan_speed;
    } brlt_settings_limits_t;


    typedef struct _tagBRLT_ComSet_TypeDef
    {
        uint8_t  GatePosition;
         int8_t  SetTemperature;
        uint8_t  FanSpeed;
    }BRLT_ComSet_TypeDef;

    typedef struct
    {
        uint16_t  State			:1;
        uint16_t  SndIndicationState	:1;
        uint16_t  LEDIndicationState	:1;
        uint16_t  AutoCO2		:1;
        uint16_t  HeaterState		:1;
        uint16_t  LastComSource     :1;
        uint16_t  FactoryReset		:1;
        uint16_t  ErrorReset		:1;
        uint16_t  FilterReset		:1;
        uint16_t  Reserved          :7;
    }BRLT_Com_Flags_TypeDef;

    typedef struct
    {
        uint16_t  State			:1;
        uint16_t  SndIndicationState	:1;
        uint16_t  LEDIndicationState	:1;
        uint16_t  LastComSource		:1;
        uint16_t  FilterWornout		:1;
        uint16_t  AutoCO2		:1;
        uint16_t  HeaterState		:1;
        uint16_t  HeaterPresent		:1;
        uint16_t  Reserved		:8;
    }BRLT_Sys_Flags_TypeDef;

#pragma pack(push, 1)
    typedef struct  _tagBRLT_ComParams_TypeDef
    {
        BRLT_Com_Flags_TypeDef		ComFlags;
        BRLT_ComSet_TypeDef		ComSet;
        brlt_settings_presets_t	button_presets;
        uint16_t 	FilterTimeDays;
        uint8_t		test_type;

    }BRLT_ComParams_TypeDef;

    typedef struct _tagBRLT_RC_RF_SetCmd_TypeDef
    {
        uint32_t CmdID;

        BRLT_ComParams_TypeDef	Command;
    }BRLT_RC_RF_SetCmd_TypeDef;

    typedef struct _tagBRLT_RC_RF_SetPreset_TypeDef
    {
    /*Copy from Breezer 3S		*/
        uint32_t CmdID;
    }BRLT_RC_RF_SetPreset_TypeDef;

    typedef struct _tagBRLT_RC_RF_Command_TypeDef
    {
        uint32_t FrameID;
        uint32_t RequestID;
        union{
        void *Data;
        BRLT_RC_RF_SetCmd_TypeDef *SetCmd;
        BRLT_RC_RF_SetPreset_TypeDef *PresetCmd;
        };
        uint32_t DataSize;
    }BRLT_RC_RF_Command_TypeDef;

 #pragma pack(pop)

    typedef struct
    {
        int8_t T_in;
        int8_t T_out;
        int8_t PCB_Temp;
    }BRLT_Sensor_State_TypeDef;

    typedef struct
    {
        uint32_t WorkTime;
        uint32_t FanTime;
        uint32_t FilterTime;
        uint32_t airflow_counter;
    }BRLT_Counters_TypeDef;

    typedef struct
    {
        uint8_t er[ERROR_TYPE_NUMBER];
    }BRLT_ErrorCnt_TypeDef;

    typedef struct
    {
        uint32_t 			reg;
        BRLT_ErrorCnt_TypeDef	 	cnt;
    }brlt_error_data;

#pragma pack(push, 1)
    typedef struct _tagBRLT_UserParams_TypeDef
    {
        BRLT_Sys_Flags_TypeDef		SysFlags;

        uint8_t  GatePosition;
         int8_t  SetTemperature;
        uint8_t  FanSpeed;
    }BRLT_UserParams_TypeDef;


    typedef struct _tagBRLT_RC_RF_Response_TypeDef
    {
        uint32_t LastCmdId;

        BRLT_UserParams_TypeDef		UserSet;
        BRLT_Sensor_State_TypeDef	SensorState;
        BRLT_Counters_TypeDef		SysCounters;
        brlt_error_data errors;

        brlt_settings_presets_t	button_presets;
        brlt_settings_limits_t	limits;
        uint8_t					heater_var;
        uint8_t					test_type;
    }BRLT_RC_RF_Response_TypeDef;

    typedef struct _tagBRLT_RC_RF_FrameResponseErrorLog_TypeDef
    {
      uint8_t  sysErrorLogBuf[100];
      uint8_t  erNum;
      uint8_t  erLeft;
    }BRLT_RC_RF_FrameResponseErrorLog_TypeDef;

    typedef struct _tagBLE_DevStatusFrame_TypeDef
    {
      uint8_t  WorkMode;
      uint16_t DevType;
      uint16_t DevSType; //subtype
      uint16_t SoftVer;
      uint16_t HardVer;
      uint8_t  Reserved[16];
    }BLE_DevStatusFrame_TypeDef;
#pragma pack(pop)

    BRLT_RC_RF_Response_TypeDef currentParams;
    int firmVersion;

    void brlt_rem_rc_send_get_params_request();
    void brlt_rem_send_set_params_cmd(BRLT_RC_RF_Response_TypeDef *params,
                                      bool ft_reset = false, bool hard_reset = false);

    RawDataList setTestState(int type);
    BRLT_RC_RF_SetCmd_TypeDef responseToCmd(BRLT_RC_RF_Response_TypeDef *params);
    ble_frame_t cmdToFrame(BRLT_RC_RF_SetCmd_TypeDef *cmd);
    BRLT_RC_RF_Response_TypeDef brlt_rem_get_params_from_frame(ble_frame_t frame);
    void frameRecieveCallback(ble_frame_t frame);
    void brlt_rem_rc_send_dev_request();
    int brlt_rem_get_fv_from_frame(ble_frame_t frame);
};
#endif // MAPROTOCOL_H
