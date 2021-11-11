#ifndef IQ_RC_H
#define IQ_RC_H

#include <lib/mabaseprotocol.h>

class IQ_Rc : public MABaseProtocol
{
private:
    enum{
        BLE_FRAME_TYPE_SET_MODE_NO_SAVE_REQ	=	0x3230,
        BLE_FRAME_TYPE_GET_MODE_RESP        =   0x3231,
        BLE_FRAME_TYPE_GET_MODE_REQ         =   0x3232,
        BLE_FRAME_TYPE_SET_MODE_SAVE_REQ	=	0x3234,
        BLE_FRAME_TYPE_DEV_STATUS_REQ		=   0x3332,
        BLE_FRAME_TYPE_DEV_STATUS_RESP 		=	0x3331,
        BLE_FRAME_TYPE_SET_IND_AUTOSET_REQ  =   0x3530,
        BLE_FRAME_TYPE_GET_IND_AUTOSET_REQ  =   0x3532,
        BLE_FRAME_TYPE_GET_IND_AUTOSET_RESP =   0x3531,
    };

    typedef struct
    {
        uint16_t  PowerState            :1;
        uint16_t  SndIndicationState    :1;
        uint16_t  LEDIndicationState    :2;
        uint16_t  ChildLock             :1;
        uint16_t  reserved1             :2;
        uint16_t  LastComSource         :1;
        uint16_t  FilterWornout         :1;
        uint16_t  MagicAir              :1;
        uint16_t  MA_Auto               :1;
        uint16_t  ActiveTimer           :1;
        uint16_t  ABLayerExpire         :1;
        uint16_t  Reserved              :3;
    }IQ_Sys_Flags_t;

    typedef struct
    {
        uint16_t  PowerState            :1;
        uint16_t  SndIndicationState    :1;
        uint16_t  LEDIndicationState    :2;
        uint16_t  ChildLock             :1;
        uint16_t  reserved1             :2;
        uint16_t  LastComSource         :1;
        uint16_t  FactoryReset          :1;
        uint16_t  ErrorReset            :1;
        uint16_t  FilterReset           :1;
        uint16_t  MagicAir              :1;
        uint16_t  MA_Auto               :1;
        uint16_t  SleepTimerOp          :1;
        uint16_t  Reserved              :2;
    }IQ_Cmd_Flags_t;

    typedef struct
    {
      int32_t   HardwareCounter;
      int32_t   FanCounter;
      int32_t   FilterCounter;
      int32_t   AntibacterialLayerCounter;
    } IQ_sys_counters_t;

#pragma pack(push, 1)
    typedef struct _tagIQ_RC_RF_Response_t
    {
        uint32_t LastCmdId;

        IQ_Sys_Flags_t      Flags;
        int8_t              SpeedLimit;
        int8_t              FanSpeed;
        int8_t              SleepTimerValue;
        int16_t             PM25Val;
        int16_t             VOCVal;
        IQ_sys_counters_t   Counters;
        int16_t             FilterCCM;
        uint32_t            Errors;
    }IQ_RC_RF_Response_t;

    typedef struct _tagIQ_RC_RF_Cmd_t
    {
        uint32_t LastCmdId;

        IQ_Cmd_Flags_t      Flags;
        int8_t              FanSpeed;
        int8_t              SleepTimerValue;
        uint16_t            TimeToReplaceFilter;
        uint16_t            TimeToABExpire;
    }IQ_RC_RF_SetCmd_t;

    typedef struct
    {
        uint16_t	pm25_red_ind;
        uint16_t	pm25_yellow_ind;
        uint16_t	pm25_green_ind;
        uint16_t	pm25_motor_off;
        uint16_t	pm25_1_speed;
        uint16_t	pm25_2_speed;
        uint16_t	pm25_3_speed;
        uint16_t	pm25_4_speed;
        uint16_t	pm25_5_speed;
        uint16_t	pm25_6_speed;
        uint16_t	pm25_hesteresis_set;
        uint16_t	pm25_measerument_time_set;
        uint16_t	voc_red_ind;
        uint16_t	voc_yellow_ind;
        uint16_t	voc_green_ind;
        uint16_t	voc_motor_off;
        uint16_t	voc_1_speed;
        uint16_t	voc_2_speed;
        uint16_t	voc_3_speed;
        uint16_t	voc_4_speed;
        uint16_t	voc_5_speed;
        uint16_t	voc_6_speed;
    }	IQ_ind_autoset_t;

    typedef struct
    {
        uint32_t cmd_id;

        IQ_ind_autoset_t cmd;
    }   IQ_ind_autoset_pack_t;

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

    IQ_RC_RF_Response_t CurrentParams;
    IQ_ind_autoset_t    IndAutoset;

    virtual void frameRecieveCallback(ble_frame_t frame) override;
    IQ_Rc::ble_frame_t cmdToFrame(IQ_RC_RF_SetCmd_t* cmd);
    IQ_Rc::ble_frame_t IndAutosetToFrame(IQ_ind_autoset_pack_t* cmd);
    IQ_RC_RF_Response_t iq_rem_get_params_from_frame(ble_frame_t frame);
    IQ_ind_autoset_t    iq_rem_get_ind_autoset_from_frame(ble_frame_t frame);
    IQ_RC_RF_SetCmd_t responseToCmd(IQ_RC_RF_Response_t* params);
    IQ_ind_autoset_pack_t IndAutosetToCmd(IQ_ind_autoset_t* params);
    void iq_rem_rc_send_get_params_request (void);
    void iq_rem_rc_send_get_dev_param_request (void);
    void iq_rem_rc_send_get_ind_autoset_request (void);
    int brlt_rem_get_fv_from_frame (ble_frame_t frame);
    void iq_rem_send_set_params_cmd(IQ_RC_RF_Response_t* params,
                                  bool ft_reset = false, bool hard_reset = false, bool err_rst = false);
    void iq_rem_send_set_ind_autoset_cmd(IQ_ind_autoset_t* params);
public:
    IQ_Rc();
    /* Setters */
    RawDataList     paramRequest();

    RawDataList    setPowerState(bool);
    RawDataList    setLEDInd(uint8_t);
    RawDataList    setSoundInd(bool);
    RawDataList    setChildLock(bool);
    RawDataList    setMagicAir(bool);
    RawDataList    setMAAuto(bool);
    RawDataList    setActiveTimer(bool);
    RawDataList    setFanSpeed(int8_t);
    RawDataList    setSleepTimerValue(int8_t);
    RawDataList    resetFilter();
    RawDataList    HardReset();
    RawDataList    resetErrCnt();
    RawDataList    setPM25_red_ind(uint16_t);
    RawDataList    setPM25_yellow_ind(uint16_t);
    RawDataList    setPM25_green_ind(uint16_t);
    RawDataList    setPM25_motor_off(uint16_t);
    RawDataList    setPM25_1_speed(uint16_t);
    RawDataList    setPM25_2_speed(uint16_t);
    RawDataList    setPM25_3_speed(uint16_t);
    RawDataList    setPM25_4_speed(uint16_t);
    RawDataList    setPM25_5_speed(uint16_t);
    RawDataList    setPM25_6_speed(uint16_t);
    RawDataList    setPM25_hesteresis_set(uint16_t);
    RawDataList    setPM25_measerument_time_set(uint16_t);
    RawDataList    setVOC_red_ind(uint16_t);
    RawDataList    setVOC_yellow_ind(uint16_t);
    RawDataList    setVOC_green_ind(uint16_t);
    RawDataList    setVOC_motor_off(uint16_t);
    RawDataList    setVOC_1_speed(uint16_t);
    RawDataList    setVOC_2_speed(uint16_t);
    RawDataList    setVOC_3_speed(uint16_t);
    RawDataList    setVOC_4_speed(uint16_t);
    RawDataList    setVOC_5_speed(uint16_t);
    RawDataList    setVOC_6_speed(uint16_t);

    RawDataList    DevParamRequest();
    RawDataList    AutosetIndRequest();
    /***********/

    /* Getters */
    int     firmVer() const;
    bool    getPowerState() const;
    uint8_t    getLEDInd() const;
    bool getSoundInd() const;
    bool    getChildLock() const;
    bool    getFilterWornout() const;
    bool    getMagicAir() const;
    bool    getMAAuto() const;
    bool    getActiveTimer() const;
    bool    getABLayerExpire() const;
    int32_t     getHWCounter() const;
    int32_t     getFanCounter() const;
    int32_t     getFilterCounter() const;
    int32_t     getABLayerCounter() const;
    int8_t      getSpeedLimit() const;
    int8_t      getFanSpeed() const;
    int8_t      getSleepTimerValue() const;
    int16_t     getPM25() const;
    int16_t     getVOC() const;
    int16_t     getFilterCCM() const;
    uint32_t    getError(int i) const;
    uint16_t    getPM25_red_ind() const;
    uint16_t	getPM25_yellow_ind() const;
    uint16_t	getPM25_green_ind() const;
    uint16_t	getPM25_motor_off() const;
    uint16_t	getPM25_1_speed() const;
    uint16_t	getPM25_2_speed() const;
    uint16_t	getPM25_3_speed() const;
    uint16_t	getPM25_4_speed() const;
    uint16_t	getPM25_5_speed() const;
    uint16_t	getPM25_6_speed() const;
    uint16_t	getPM25_hesteresis_set() const;
    uint16_t	getPM25_measerument_time_set() const;
    uint16_t	getVOC_red_ind() const;
    uint16_t	getVOC_yellow_ind() const;
    uint16_t	getVOC_green_ind() const;
    uint16_t	getVOC_motor_off() const;
    uint16_t	getVOC_1_speed() const;
    uint16_t	getVOC_2_speed() const;
    uint16_t	getVOC_3_speed() const;
    uint16_t	getVOC_4_speed() const;
    uint16_t	getVOC_5_speed() const;
    uint16_t	getVOC_6_speed() const;
    /***********/
};

#endif // IQ_RC_H
