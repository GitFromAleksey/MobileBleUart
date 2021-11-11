#include "iq_protocol.h"
#include "lib/crc_calculate.h"

IQ_Rc::IQ_Rc()
{

}

void IQ_Rc::frameRecieveCallback(ble_frame_t frame)
{
    switch(frame.type)
    {
        case BLE_FRAME_TYPE_GET_MODE_RESP:
        CurrentParams = iq_rem_get_params_from_frame(frame);
        break;
        case BLE_FRAME_TYPE_DEV_STATUS_RESP:
        firmVersion = brlt_rem_get_fv_from_frame(frame);
        break;
        case BLE_FRAME_TYPE_GET_IND_AUTOSET_RESP:
        IndAutoset = iq_rem_get_ind_autoset_from_frame(frame);
        break;
    }
}

/* Commands handlers */
IQ_Rc::IQ_RC_RF_Response_t
IQ_Rc::iq_rem_get_params_from_frame (ble_frame_t frame)
{
    IQ_RC_RF_Response_t ret;

    memcpy(&ret,frame.data, sizeof(IQ_RC_RF_Response_t));

    return ret;
}

IQ_Rc::IQ_ind_autoset_t
IQ_Rc::iq_rem_get_ind_autoset_from_frame(ble_frame_t frame)
{
    IQ_ind_autoset_t ret;
    IQ_ind_autoset_pack_t* p_data = (IQ_ind_autoset_pack_t*)frame.data;

    memcpy(&ret, &(p_data->cmd), sizeof(IQ_ind_autoset_t));
    return ret;
}

int IQ_Rc::brlt_rem_get_fv_from_frame (ble_frame_t frame)
{
    BLE_DevStatusFrame_TypeDef dev_st;
    memcpy(&dev_st, frame.data, sizeof (dev_st));
    return dev_st.SoftVer;
}

IQ_Rc::IQ_RC_RF_SetCmd_t
IQ_Rc::responseToCmd(IQ_RC_RF_Response_t* params)
{
    IQ_RC_RF_SetCmd_t cmd; // = {0};
    memset(&cmd, 0, sizeof cmd);

    cmd.LastCmdId = 1;

    cmd.TimeToReplaceFilter = params->Counters.FilterCounter;
    cmd.TimeToABExpire = params->Counters.AntibacterialLayerCounter;
    cmd.FanSpeed = params->FanSpeed;
    cmd.SleepTimerValue = params->SleepTimerValue;

    cmd.Flags.PowerState = params->Flags.PowerState;
    cmd.Flags.SndIndicationState = params->Flags.SndIndicationState;
    cmd.Flags.LEDIndicationState = params->Flags.LEDIndicationState;
    cmd.Flags.MA_Auto = params->Flags.MA_Auto;
    cmd.Flags.MagicAir = params->Flags.MagicAir;
    cmd.Flags.ChildLock = params->Flags.ChildLock;
    cmd.Flags.SleepTimerOp = params->Flags.ActiveTimer;

    return cmd;
}

IQ_Rc::IQ_ind_autoset_pack_t
IQ_Rc::IndAutosetToCmd(IQ_ind_autoset_t* params)
{
    IQ_ind_autoset_pack_t cmd;
    memcpy(&cmd.cmd, params, sizeof (IQ_ind_autoset_t));
    cmd.cmd_id = 1;
    return cmd;
}

IQ_Rc::ble_frame_t
IQ_Rc::cmdToFrame(IQ_RC_RF_SetCmd_t* cmd)
{
    ble_frame_t frame;
    frame.data = cmd;
    frame.data_size = sizeof(IQ_RC_RF_SetCmd_t);
    frame.type = BLE_FRAME_TYPE_SET_MODE_NO_SAVE_REQ;
    frame.request_id = 1;
    return frame;
};

void IQ_Rc::iq_rem_rc_send_get_params_request (void)
{
    ble_frame_t frame;
    memset(&frame, 0, sizeof frame);
    frame.request_id = 1;
    frame.type = BLE_FRAME_TYPE_GET_MODE_REQ;
    sendFrame(frame);
}

void IQ_Rc::iq_rem_rc_send_get_dev_param_request (void)
{
    ble_frame_t frame;
    memset(&frame, 0, sizeof frame);
    frame.request_id = 1;
    frame.type = BLE_FRAME_TYPE_DEV_STATUS_REQ;
    sendFrame(frame);
}

void IQ_Rc::iq_rem_rc_send_get_ind_autoset_request (void)
{
    ble_frame_t frame;
    memset(&frame, 0, sizeof frame);
    frame.request_id = 1;
    frame.type = BLE_FRAME_TYPE_GET_IND_AUTOSET_REQ;
    sendFrame(frame);
}

IQ_Rc::ble_frame_t IQ_Rc::IndAutosetToFrame(IQ_ind_autoset_pack_t* cmd)
{
    ble_frame_t frame;
    frame.data = cmd;
    frame.data_size = sizeof(IQ_ind_autoset_pack_t);
    frame.type = BLE_FRAME_TYPE_SET_IND_AUTOSET_REQ;
    frame.request_id = 1;
    return frame;
}

void IQ_Rc::iq_rem_send_set_params_cmd(IQ_RC_RF_Response_t* params,
                                        bool ft_reset, bool hard_reset, bool err_rst)
{
    IQ_RC_RF_SetCmd_t cmd = responseToCmd(params);

    if(ft_reset)
    {
        cmd.TimeToReplaceFilter = 180;
        cmd.Flags.FilterReset = 1;
    }

    if(hard_reset)
        cmd.Flags.FactoryReset = 1;

    if(err_rst)
        cmd.Flags.ErrorReset = 1;

    ble_frame_t frame = cmdToFrame(&cmd);

    sendFrame(frame);
}

void IQ_Rc::iq_rem_send_set_ind_autoset_cmd(IQ_ind_autoset_t* params)
{
    IQ_ind_autoset_pack_t cmd = IndAutosetToCmd(params);

    ble_frame_t frame = IndAutosetToFrame(&cmd);

    sendFrame(frame);
}

/* Setters */
IQ_Rc::RawDataList    IQ_Rc::DevParamRequest()
{
    resetDataForSend();
    iq_rem_rc_send_get_dev_param_request();
    return dataForSend;
}

IQ_Rc::RawDataList    IQ_Rc::AutosetIndRequest()
{
    resetDataForSend();
    iq_rem_rc_send_get_ind_autoset_request();
    return dataForSend;
}

IQ_Rc::RawDataList IQ_Rc::paramRequest()
{
    resetDataForSend();
    iq_rem_rc_send_get_params_request();
    return dataForSend;
}

IQ_Rc::RawDataList    IQ_Rc::setPowerState(bool val)
{
    resetDataForSend();
    CurrentParams.Flags.PowerState = val;
    iq_rem_send_set_params_cmd(&CurrentParams);
    return dataForSend;
}

IQ_Rc::RawDataList    IQ_Rc::setLEDInd(uint8_t val)
{
    resetDataForSend();
    CurrentParams.Flags.LEDIndicationState = val;
    iq_rem_send_set_params_cmd(&CurrentParams);
    return dataForSend;
}

IQ_Rc::RawDataList    IQ_Rc::setSoundInd(bool val)
{
    resetDataForSend();
    CurrentParams.Flags.SndIndicationState = val;
    iq_rem_send_set_params_cmd(&CurrentParams);
    return dataForSend;
}

IQ_Rc::RawDataList    IQ_Rc::setChildLock(bool val)
{
    resetDataForSend();
    CurrentParams.Flags.ChildLock = val;
    iq_rem_send_set_params_cmd(&CurrentParams);
    return dataForSend;
}

IQ_Rc::RawDataList    IQ_Rc::setMagicAir(bool val)
{
    resetDataForSend();
    CurrentParams.Flags.MagicAir = val;
    iq_rem_send_set_params_cmd(&CurrentParams);
    return dataForSend;
}

IQ_Rc::RawDataList    IQ_Rc::setMAAuto(bool val)
{
    resetDataForSend();
    CurrentParams.Flags.MA_Auto = val;
    iq_rem_send_set_params_cmd(&CurrentParams);
    return dataForSend;
}

IQ_Rc::RawDataList    IQ_Rc::setActiveTimer(bool val)
{
    resetDataForSend();
    CurrentParams.Flags.ActiveTimer = val;
    iq_rem_send_set_params_cmd(&CurrentParams);
    return dataForSend;
}

IQ_Rc::RawDataList    IQ_Rc::setFanSpeed(int8_t val)
{
    resetDataForSend();
    CurrentParams.FanSpeed = val;
    iq_rem_send_set_params_cmd(&CurrentParams);
    return dataForSend;
}

IQ_Rc::RawDataList    IQ_Rc::setSleepTimerValue(int8_t val)
{
    resetDataForSend();
    CurrentParams.SleepTimerValue = val;
    CurrentParams.Flags.ActiveTimer = 1;
    iq_rem_send_set_params_cmd(&CurrentParams);
    return dataForSend;
}

IQ_Rc::RawDataList    IQ_Rc::resetFilter()
{
    resetDataForSend();

    iq_rem_send_set_params_cmd(&CurrentParams, true, false, false);
    return dataForSend;
}

IQ_Rc::RawDataList    IQ_Rc::HardReset()
{
    resetDataForSend();
    iq_rem_send_set_params_cmd(&CurrentParams, false, true, false);
    return dataForSend;
}

IQ_Rc::RawDataList    IQ_Rc::resetErrCnt()
{
    resetDataForSend();
    iq_rem_send_set_params_cmd(&CurrentParams, false, false, true);
    return dataForSend;
}

IQ_Rc::RawDataList    IQ_Rc::setPM25_red_ind(uint16_t val)
{
    resetDataForSend();
    IndAutoset.pm25_red_ind = val;
    iq_rem_send_set_ind_autoset_cmd(&IndAutoset);
    return dataForSend;
}

IQ_Rc::RawDataList    IQ_Rc::setPM25_yellow_ind(uint16_t val)
{
    resetDataForSend();
    IndAutoset.pm25_yellow_ind = val;
    iq_rem_send_set_ind_autoset_cmd(&IndAutoset);
    return dataForSend;
}

IQ_Rc::RawDataList    IQ_Rc::setPM25_green_ind(uint16_t val)
{
    resetDataForSend();
    IndAutoset.pm25_green_ind = val;
    iq_rem_send_set_ind_autoset_cmd(&IndAutoset);
    return dataForSend;
}

IQ_Rc::RawDataList    IQ_Rc::setPM25_motor_off(uint16_t val)
{
    resetDataForSend();
    IndAutoset.pm25_motor_off = val;
    iq_rem_send_set_ind_autoset_cmd(&IndAutoset);
    return dataForSend;
}

IQ_Rc::RawDataList    IQ_Rc::setPM25_1_speed(uint16_t val)
{
    resetDataForSend();
    IndAutoset.pm25_1_speed = val;
    iq_rem_send_set_ind_autoset_cmd(&IndAutoset);
    return dataForSend;
}

IQ_Rc::RawDataList    IQ_Rc::setPM25_2_speed(uint16_t val)
{
    resetDataForSend();
    IndAutoset.pm25_2_speed = val;
    iq_rem_send_set_ind_autoset_cmd(&IndAutoset);
    return dataForSend;
}

IQ_Rc::RawDataList    IQ_Rc::setPM25_3_speed(uint16_t val)
{
    resetDataForSend();
    IndAutoset.pm25_3_speed = val;
    iq_rem_send_set_ind_autoset_cmd(&IndAutoset);
    return dataForSend;
}

IQ_Rc::RawDataList    IQ_Rc::setPM25_4_speed(uint16_t val)
{
    resetDataForSend();
    IndAutoset.pm25_4_speed = val;
    iq_rem_send_set_ind_autoset_cmd(&IndAutoset);
    return dataForSend;
}

IQ_Rc::RawDataList    IQ_Rc::setPM25_5_speed(uint16_t val)
{
    resetDataForSend();
    IndAutoset.pm25_5_speed = val;
    iq_rem_send_set_ind_autoset_cmd(&IndAutoset);
    return dataForSend;
}

IQ_Rc::RawDataList    IQ_Rc::setPM25_6_speed(uint16_t val)
{
    resetDataForSend();
    IndAutoset.pm25_6_speed = val;
    iq_rem_send_set_ind_autoset_cmd(&IndAutoset);
    return dataForSend;
}

IQ_Rc::RawDataList    IQ_Rc::setPM25_hesteresis_set(uint16_t val)
{
    resetDataForSend();
    IndAutoset.pm25_hesteresis_set = val;
    iq_rem_send_set_ind_autoset_cmd(&IndAutoset);
    return dataForSend;
}

IQ_Rc::RawDataList    IQ_Rc::setPM25_measerument_time_set(uint16_t val)
{
    resetDataForSend();
    IndAutoset.pm25_measerument_time_set = val;
    iq_rem_send_set_ind_autoset_cmd(&IndAutoset);
    return dataForSend;
}

IQ_Rc::RawDataList    IQ_Rc::setVOC_red_ind(uint16_t val)
{
    resetDataForSend();
    IndAutoset.voc_red_ind = val;
    iq_rem_send_set_ind_autoset_cmd(&IndAutoset);
    return dataForSend;
}

IQ_Rc::RawDataList    IQ_Rc::setVOC_yellow_ind(uint16_t val)
{
    resetDataForSend();
    IndAutoset.voc_yellow_ind = val;
    iq_rem_send_set_ind_autoset_cmd(&IndAutoset);
    return dataForSend;
}

IQ_Rc::RawDataList    IQ_Rc::setVOC_green_ind(uint16_t val)
{
    resetDataForSend();
    IndAutoset.voc_green_ind = val;
    iq_rem_send_set_ind_autoset_cmd(&IndAutoset);
    return dataForSend;
}

IQ_Rc::RawDataList    IQ_Rc::setVOC_motor_off(uint16_t val)
{
    resetDataForSend();
    IndAutoset.voc_motor_off = val;
    iq_rem_send_set_ind_autoset_cmd(&IndAutoset);
    return dataForSend;
}

IQ_Rc::RawDataList    IQ_Rc::setVOC_1_speed(uint16_t val)
{
    resetDataForSend();
    IndAutoset.voc_1_speed = val;
    iq_rem_send_set_ind_autoset_cmd(&IndAutoset);
    return dataForSend;
}

IQ_Rc::RawDataList    IQ_Rc::setVOC_2_speed(uint16_t val)
{
    resetDataForSend();
    IndAutoset.voc_2_speed = val;
    iq_rem_send_set_ind_autoset_cmd(&IndAutoset);
    return dataForSend;
}

IQ_Rc::RawDataList    IQ_Rc::setVOC_3_speed(uint16_t val)
{
    resetDataForSend();
    IndAutoset.voc_3_speed = val;
    iq_rem_send_set_ind_autoset_cmd(&IndAutoset);
    return dataForSend;
}

IQ_Rc::RawDataList    IQ_Rc::setVOC_4_speed(uint16_t val)
{
    resetDataForSend();
    IndAutoset.voc_4_speed = val;
    iq_rem_send_set_ind_autoset_cmd(&IndAutoset);
    return dataForSend;
}

IQ_Rc::RawDataList    IQ_Rc::setVOC_5_speed(uint16_t val)
{
    resetDataForSend();
    IndAutoset.voc_5_speed = val;
    iq_rem_send_set_ind_autoset_cmd(&IndAutoset);
    return dataForSend;
}

IQ_Rc::RawDataList    IQ_Rc::setVOC_6_speed(uint16_t val)
{
    resetDataForSend();
    IndAutoset.voc_6_speed = val;
    iq_rem_send_set_ind_autoset_cmd(&IndAutoset);
    return dataForSend;
}

/***********/

/* Getters */
int     IQ_Rc::firmVer() const
{
    return firmVersion;
}

bool IQ_Rc::getPowerState() const
{
    return CurrentParams.Flags.PowerState;
}

uint8_t IQ_Rc::getLEDInd() const
{
    return CurrentParams.Flags.LEDIndicationState;
}

bool IQ_Rc::getSoundInd() const
{
    return CurrentParams.Flags.SndIndicationState;
}

bool IQ_Rc::getChildLock() const
{
    return CurrentParams.Flags.ChildLock;
}

bool IQ_Rc::getFilterWornout() const
{
    return CurrentParams.Flags.FilterWornout;
}

bool IQ_Rc::getMagicAir() const
{
    return CurrentParams.Flags.MagicAir;
}

bool IQ_Rc::getMAAuto() const
{
    return CurrentParams.Flags.MA_Auto;
}

bool IQ_Rc::getActiveTimer() const
{
    return CurrentParams.Flags.ActiveTimer;
}

bool IQ_Rc::getABLayerExpire() const
{
    return CurrentParams.Flags.ABLayerExpire;
}

int32_t IQ_Rc::getHWCounter() const
{
    return CurrentParams.Counters.HardwareCounter;
}

int32_t IQ_Rc::getFanCounter() const
{
    return CurrentParams.Counters.FanCounter;
}

int32_t IQ_Rc::getFilterCounter() const
{
    return CurrentParams.Counters.FilterCounter;
}

int32_t IQ_Rc::getABLayerCounter() const
{
    return CurrentParams.Counters.AntibacterialLayerCounter;
}

int8_t IQ_Rc::getSpeedLimit() const
{
    return CurrentParams.SpeedLimit;
}

int8_t IQ_Rc::getFanSpeed() const
{
    return CurrentParams.FanSpeed;
}

int8_t IQ_Rc::getSleepTimerValue() const
{
    return CurrentParams.SleepTimerValue;
}

int16_t IQ_Rc::getPM25() const
{
    return CurrentParams.PM25Val;
}

int16_t IQ_Rc::getVOC() const
{
    return CurrentParams.VOCVal;
}

int16_t IQ_Rc::getFilterCCM() const
{
    return CurrentParams.FilterCCM;
}

uint32_t IQ_Rc::getError(int i) const
{
    return CurrentParams.Errors  & (1 << i);
}

uint16_t IQ_Rc::getPM25_red_ind() const
{
    return IndAutoset.pm25_red_ind;
}

uint16_t IQ_Rc::getPM25_yellow_ind() const
{
    return IndAutoset.pm25_yellow_ind;
}

uint16_t IQ_Rc::getPM25_green_ind() const
{
    return IndAutoset.pm25_green_ind;
}

uint16_t IQ_Rc::getPM25_motor_off() const
{
    return IndAutoset.pm25_motor_off;
}

uint16_t IQ_Rc::getPM25_1_speed() const
{
    return IndAutoset.pm25_1_speed;
}

uint16_t IQ_Rc::getPM25_2_speed() const
{
    return IndAutoset.pm25_2_speed;
}

uint16_t IQ_Rc::getPM25_3_speed() const
{
    return IndAutoset.pm25_3_speed;
}

uint16_t IQ_Rc::getPM25_4_speed() const
{
    return IndAutoset.pm25_4_speed;
}

uint16_t IQ_Rc::getPM25_5_speed() const
{
    return IndAutoset.pm25_5_speed;
}

uint16_t IQ_Rc::getPM25_6_speed() const
{
    return IndAutoset.pm25_6_speed;
}

uint16_t IQ_Rc::getPM25_hesteresis_set() const
{
    return IndAutoset.pm25_hesteresis_set;
}

uint16_t IQ_Rc::getPM25_measerument_time_set() const
{
    return IndAutoset.pm25_measerument_time_set;
}

uint16_t IQ_Rc::getVOC_red_ind() const
{
    return IndAutoset.voc_red_ind;
}

uint16_t IQ_Rc::getVOC_yellow_ind() const
{
    return IndAutoset.voc_yellow_ind;
}

uint16_t IQ_Rc::getVOC_green_ind() const
{
    return IndAutoset.voc_green_ind;
}

uint16_t IQ_Rc::getVOC_motor_off() const
{
    return IndAutoset.voc_motor_off;
}

uint16_t IQ_Rc::getVOC_1_speed() const
{
    return IndAutoset.voc_1_speed;
}

uint16_t IQ_Rc::getVOC_2_speed() const
{
    return IndAutoset.voc_2_speed;
}

uint16_t IQ_Rc::getVOC_3_speed() const
{
    return IndAutoset.voc_3_speed;
}

uint16_t IQ_Rc::getVOC_4_speed() const
{
    return IndAutoset.voc_4_speed;
}

uint16_t IQ_Rc::getVOC_5_speed() const
{
    return IndAutoset.voc_5_speed;
}

uint16_t IQ_Rc::getVOC_6_speed() const
{
    return IndAutoset.voc_6_speed;
}

/***********/

