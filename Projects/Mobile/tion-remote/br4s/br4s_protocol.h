#ifndef BR4S_PROTOCOL_H
#define BR4S_PROTOCOL_H

#include <QByteArray>
#include <QList>
#include <QTimer>
#include <QDateTime>
#include <lib/mabaseprotocol.h>

class BR4S_Rc : public MABaseProtocol
{
private:
    enum{
        BLE_FRAME_TYPE_SET_MODE_NO_SAVE_REQ	=	0x3230,
        BLE_FRAME_TYPE_GET_MODE_RESP		=	0x3231,
        BLE_FRAME_TYPE_GET_MODE_REQ         =	0x3232,
        BLE_FRAME_TYPE_SET_MODE_SAVE_REQ	=	0x3234,
        BLE_FRAME_TYPE_DEV_STATUS_REQ		=   0x3332,
        BLE_FRAME_TYPE_DEV_STATUS_RESP 		=	0x3331,
        BLE_FRAME_TYPE_TEST_REQ             =   0x3132,
        BLE_FRAME_TYPE_TEST_RESP            =   0x3131,
        BLE_FRAME_TYPE_SET_TIMER_REQ        =   0x3430,
        BLE_FRAME_TYPE_GET_TIMER_REQ        =   0x3432,
        BLE_FRAME_TYPE_GET_TIMER_RESP       =   0x3431,
        BLE_FRAME_TYPE_GET_TIMERS_STATE_REQ =   0x3532,
        BLE_FRAME_TYPE_GET_TIMERS_STATE_RESP=   0x3531,
        BLE_FRAME_TYPE_SET_TIME_REQ         =   0x3630,
        BLE_FRAME_TYPE_GET_TIME_REQ         =   0x3632,
        BLE_FRAME_TYPE_GET_TIME_RESP        =   0x3631,
        BLE_FRAME_TYPE_GET_ERR_CNT_REQ      =   0x3732,
        BLE_FRAME_TYPE_GET_ERR_CNT_RESP     =   0x3731,
        BLE_FRAME_TYPE_SET_TEST_REQ         =   0x3830,
        BLE_FRAME_TYPE_GET_CURR_TEST_REQ    =   0x3832,
        BLE_FRAME_TYPE_GET_CURR_TEST_RESP   =   0x3831,
        BLE_FRAME_TYPE_START_TURBO			=   0x4130,
        BLE_FRAME_TYPE_GET_TURBO_PARAM_REQ	=   0x4132,
        BLE_FRAME_TYPE_GET_TURBO_PARAM_RESP =   0x4131,
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
      INFLOW_SUBSTATE,
      RECIRCULATION_SUBSTATE,
    };

    enum
    {
        ERROR_TYPE_NUMBER = 32
    };

    typedef struct
    {
        uint8_t max_fan_speed;
    } brlt_settings_limits_t;


    typedef struct _tagBRLT_ComSet_TypeDef
    {
        uint8_t  Substate;
         int8_t  SetTemperature;
        uint8_t  FanSpeed;
    }BRLT_ComSet_TypeDef;

    typedef struct
    {
        uint16_t  State			:1;
        uint16_t  SndIndicationState	:1;
        uint16_t  LEDIndicationState	:1;
        uint16_t  HeaterMode		:1;
        uint16_t  LastComSource     :1;
        uint16_t  FactoryReset		:1;
        uint16_t  ErrorReset		:1;
        uint16_t  FilterReset		:1;
        uint16_t  MagicAir          :1;
        uint16_t  MA_Auto           :1;
        uint16_t  Reserved          :6;
    }BRLT_Com_Flags_TypeDef;

    typedef struct
    {
        uint16_t  State                 :1;
        uint16_t  SndIndicationState    :1;
        uint16_t  LEDIndicationState    :1;
        uint16_t  HeaterState           :1;
        uint16_t  HeaterMode            :1;
        uint16_t  LastComSource         :1;
        uint16_t  FilterWornout         :1;
        uint16_t  HeaterPresent         :3;
        uint16_t  MagicAir              :1;
        uint16_t  MA_Auto               :1;
        uint16_t  ActiveTimer           :1;
        uint16_t  Reserved		:3;
    }BRLT_Sys_Flags_TypeDef;

#pragma pack(push, 1)
    typedef struct  _tagBRLT_ComParams_TypeDef
    {
        BRLT_Com_Flags_TypeDef		ComFlags;
        BRLT_ComSet_TypeDef		ComSet;
        uint32_t 	FilterTimeDays;

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
        int8_t T_ctrlb;
        int8_t T_pwrb;
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
        uint8_t		Monday : 1;
        uint8_t		Tuesday : 1;
        uint8_t		Wednesday : 1;
        uint8_t		Thursday : 1;
        uint8_t		Friday : 1;
        uint8_t		Saturday : 1;
        uint8_t		Sunday : 1;
        uint8_t		reserved : 1;
        uint8_t		hours;
        uint8_t		minutes;
    }   br4s_timer_time_t;

    typedef struct
    {  
        uint8_t power_state : 1;
        uint8_t sound_ind_state : 1;
        uint8_t led_ind_state : 1;
        uint8_t heater_mode : 1; // 0 - temperature maintenance, 1 - heating
        uint8_t	timer_state : 1; /* on/off */
        uint8_t reserved : 3;
    }   br4s_timer_flags_t;



#pragma pack(push, 1)
    typedef struct
    {
        br4s_timer_time_t		timer_time;
        br4s_timer_flags_t		timer_flags;
         int8_t					target_temp;
        uint8_t					fan_state;
        uint8_t					device_mode;
    }   br4s_timer_settings_t;

    typedef struct
    {
        int64_t		unix_time;
    }	br4s_time_format_t;


    #define NUM_OF_TIMERS	6
    typedef struct
    {
        bool timer_state[NUM_OF_TIMERS];
    }	br4s_timers_state_t;


    typedef struct
    {
        int32_t 			CmdID;
        BRLT_ErrorCnt_TypeDef	 	cnt;
    }brlt_error_data;

    typedef struct _tagBRLT_UserParams_TypeDef
    {
        BRLT_Sys_Flags_TypeDef		SysFlags;

        uint8_t  Substate;
         int8_t  SetTemperature;
        uint8_t  FanSpeed;
    }BRLT_UserParams_TypeDef;


    typedef struct _tagBRLT_RC_RF_Response_TypeDef
    {
        uint32_t LastCmdId;

        BRLT_UserParams_TypeDef		UserSet;
        BRLT_Sensor_State_TypeDef	SensorState;
        BRLT_Counters_TypeDef		SysCounters;
        uint32_t errors;

        brlt_settings_limits_t	limits;
        uint8_t					heater_var;
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

    typedef struct _tagBR4S_RC_RF_FrameTimerSettingsResponse_TypeDef
    {
        uint32_t 				cmd_id;

        uint8_t					timer_id;
        br4s_timer_settings_t	timer;
    }BRLT_RC_RF_TimerSettings_Resp_TypeDef;

    typedef struct _tagBR4S_RC_RF_FrameTimerSettingsRequest_TypeDef
    {
        uint32_t 				cmd_id;
        uint8_t					timer_id;
    }BRLT_RC_RF_TimerSettings_Req_TypeDef;

    typedef struct _tagBR4S_RC_RF_FrameTimerSateResponse_TypeDef
    {
        uint32_t LastCmdId;

        br4s_timers_state_t		timers_state;
    }BRLT_RC_RF_TimerState_Resp_TypeDef;

    typedef struct _tagBR4S_RC_time_Resp_TypeDef
    {
        uint32_t 				cmd_id;
        br4s_time_format_t		time;
    }	br4s_rc_time_pack_t;

    typedef struct _tagbr4s_turbo_state_pack_resp_t
    {
        uint32_t			cmd_id;
        uint16_t			time;
        uint8_t             err_code;
    }	br4s_rc_turbo_state_pack_req_t;

    typedef struct _tagbr4s_turbo_state_t
    {
        uint8_t		is_active;
        uint16_t	turbo_time;
        uint8_t     err_code;
    }	br4s_turbo_state_t;

    typedef struct _tagbr4s_turbo_state_pack_t
    {
        uint32_t			cmd_id;
        br4s_turbo_state_t	turbo_state;
    }	br4s_rc_turbo_state_pack_t;
#pragma pack(pop)


    uint32_t    test_type;
    BRLT_ErrorCnt_TypeDef err_cnt;
    br4s_timer_settings_t timer[NUM_OF_TIMERS];
    QDateTime currDateTime;
    br4s_turbo_state_t  turbo_state = {0, 0, 0};
    BRLT_RC_RF_Response_TypeDef currentParams;

    /* Set of Sending functions. */

    void brlt_rem_send_set_params_cmd(BRLT_RC_RF_Response_TypeDef *params,
                                      bool ft_reset = false, bool hard_reset = false);

    void brlt_rem_rc_send_get_params_request();
    void brlt_rem_rc_send_dev_request();
    //void br4s_rem_rc_send_set_timer
    void br4s_rem_rc_send_err_cnt_request();
    void br4s_rem_rc_send_start_test_request(uint32_t test_type);
    void br4s_rem_rc_send_curr_test_request();
    void br4s_rem_rc_send_timer_settings_request(uint8_t timer_id);
    void br4s_rem_rc_send_set_timer_settings_request(br4s_timer_settings_t tim_set, uint8_t timer_id);
    void br4s_rem_rc_send_timer_state_request();
    void br4s_rem_rc_send_time_set_request(br4s_time_format_t time);
    void br4s_rem_rc_send_time_request();
    void br4s_rem_rc_send_turbo_start_request(uint16_t time);
    void br4s_rem_rc_send_turbo_state_request();
    void br4s_rem_rc_send_turbo_err_reset_request();

    /******************************/
    RawDataList setTestState(int type);
    BRLT_RC_RF_SetCmd_TypeDef responseToCmd(BRLT_RC_RF_Response_TypeDef *params);
    ble_frame_t cmdToFrame(BRLT_RC_RF_SetCmd_TypeDef *cmd);
    BRLT_RC_RF_Response_TypeDef brlt_rem_get_params_from_frame(ble_frame_t frame);
    virtual void frameRecieveCallback(ble_frame_t frame) override;
    int brlt_rem_get_fv_from_frame(ble_frame_t frame);
    void br4s_rem_get_err_cnt_from_frame(ble_frame_t frame);
    void br4s_rem_get_curr_test_from_frame(ble_frame_t frame);
    void br4s_rem_get_timer_settings_from_frame(ble_frame_t frame);
    void br4s_rem_get_time_from_frame(ble_frame_t frame);
    void br4s_rem_get_turbo_state_from_frame(ble_frame_t frame);

public:
    BR4S_Rc();
    /* Setters. Returns the raw packet to send via BLE. */
    RawDataList CurrTestRequest();
    RawDataList paramRequest();
    RawDataList devRequest();
    RawDataList setSubstate(uint8_t subst);
    RawDataList setTemp(int t);
    RawDataList setSpeed(int sp);
    RawDataList setPowerState(bool st);
    RawDataList setNoTest();
    RawDataList setGateTest();
    RawDataList setHeaterTest();
    RawDataList setDefaultTest();
    RawDataList resetFilter();
    RawDataList resetHard();
    RawDataList switchSound(bool);
    RawDataList switchLED(bool);
    RawDataList switchMA(bool);
    RawDataList switchMA_auto(bool);
    RawDataList setHeaterMode(bool mode);
    RawDataList ErrCntRequest();
    RawDataList setTime (int64_t time,  int64_t timezone_offset);
    RawDataList timerSettingsRequest(uint8_t timer_id);
    RawDataList timeRequest();
    RawDataList StartTurbo(int16_t time);
    RawDataList TurboReq();
    RawDataList ResetTurboErr();

    /* Timers set-functions section */
    RawDataList setTimerEnable(bool state, uint8_t timer_id);
    RawDataList setTimerTime(QTime time, uint8_t timer_id);
    RawDataList setTimerDuration(uint16_t timer_duration, uint8_t timer_id);
    RawDataList setTimerMonday(bool state, uint8_t timer_id);
    RawDataList setTimerTuesday(bool state, uint8_t timer_id);
    RawDataList setTimerWednesday(bool state, uint8_t timer_id);
    RawDataList setTimerThursday(bool state, uint8_t timer_id);
    RawDataList setTimerFriday(bool state, uint8_t timer_id);
    RawDataList setTimerSaturday(bool state, uint8_t timer_id);
    RawDataList setTimerSunday(bool state, uint8_t timer_id);
    RawDataList setTimerSubstate(uint8_t substate, uint8_t timer_id);
    RawDataList setTimerHeaterMode(bool heater_mode, uint8_t timer_id);
    RawDataList setTimerPowerState(bool power_state, uint8_t timer_id);
    RawDataList setTimerSndIndState(bool snd_ind_state, uint8_t timer_id);
    RawDataList setTimerLEDIndState(bool led_ind_state, uint8_t timer_id);
    RawDataList setTimerTargetSpeed(uint8_t speed, uint8_t timer_id);
    RawDataList setTimerTargetTemp(int8_t temp, uint8_t timer_id);
    /****/

    /* Getters. Returns value of parameter keeped in protocol class. */
    int8_t temp() const;
    uint8_t speed() const;
    bool powerState() const;
    bool heaterState() const;
    bool heaterMode() const;
    bool noTest() const;
    bool gateTest() const;
    bool heaterTest() const;
    bool defaultTest() const;
    int tempIn() const;
    int tempOut() const;
    int tempPwrb () const;
    int tempCtrlb () const;
    int heaterVar() const;
    uint32_t filterCounter() const;
    uint32_t elecCounter() const;
    uint32_t motorCounter() const;
    uint8_t errorCnt(int numb) const;
    bool error(int numb) const;
    uint32_t airFlowCnt() const;
    int firmVer() const;
    uint32_t speedLimit() const;
    bool maControl() const;
    bool maAutoControl() const;
    int substate() const;
    bool LEDstate() const;
    bool SoundState() const;
    int heaterPresent() const;
    QDateTime getDateTime() const;
    br4s_turbo_state_t getTurboState() const;
    /* Timers section get-functions */
    bool getTimerEnable(uint8_t timer_id) const;
    QTime getTimerTime(uint8_t timer_id) const;
    uint16_t getTimerDuration(uint8_t timer_id) const;
    bool getTimerMonday(uint8_t timer_id) const;
    bool getTimerTuesday(uint8_t timer_id) const;
    bool getTimerWednesday(uint8_t timer_id) const;
    bool getTimerThursday(uint8_t timer_id) const;
    bool getTimerFriday(uint8_t timer_id) const;
    bool getTimerSaturday(uint8_t timer_id) const;
    bool getTimerSunday(uint8_t timer_id) const;
    uint8_t getTimerSubstate(uint8_t timer_id) const;
    bool getTimerHeaterMode(uint8_t timer_id) const;
    bool getTimerPowerState(uint8_t timer_id) const;
    bool getTimerSndIndState(uint8_t timer_id) const;
    bool getTimerLEDIndState(uint8_t timer_id) const;
    uint8_t getTimerTargetSpeed(uint8_t timer_id) const;
    int8_t getTimerTargetTemp(uint8_t timer_id) const;
    /****/

};
#endif // BR4S_PROTOCOL_H
