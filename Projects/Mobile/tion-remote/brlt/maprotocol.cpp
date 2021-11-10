#include "lib/crc_calculate.h"
#include "maprotocol.h"
#include <QDebug>

//**********************************************************
// RC Level
//**********************************************************


MaRc::MaRc() : firmVersion(0)
{
    memset(&currentParams, 0, sizeof currentParams);
}


MaRc::BRLT_RC_RF_Response_TypeDef
MaRc::brlt_rem_get_params_from_frame (ble_frame_t frame)
{
    BRLT_RC_RF_Response_TypeDef ret;
    //qDebug() << frame.data_size << sizeof (ret);

    memcpy(&ret,frame.data,sizeof (ret));
    return ret;
}

int MaRc::brlt_rem_get_fv_from_frame (ble_frame_t frame)
{
    BLE_DevStatusFrame_TypeDef dev_st;
    memcpy(&dev_st, frame.data, sizeof (dev_st));
    return dev_st.SoftVer;
}

void MaRc::brlt_rem_rc_send_get_params_request (void)
{
    ble_frame_t frame; //= {0};
    memset(&frame, 0, sizeof frame);
    frame.request_id = 1;
    frame.type = BLE_FRAME_TYPE_GET_MODE_REQ;
    sendFrame(frame);
}

void MaRc::brlt_rem_rc_send_dev_request (void)
{
    ble_frame_t frame; //= {0};
    memset(&frame, 0, sizeof frame);
    frame.request_id = 1;
    frame.type = BLE_FRAME_TYPE_DEV_STATUS_REQ;
    sendFrame(frame);
}


MaRc::BRLT_RC_RF_SetCmd_TypeDef
MaRc::responseToCmd(BRLT_RC_RF_Response_TypeDef* params)
{
    BRLT_RC_RF_SetCmd_TypeDef cmd; // = {0};
    memset(&cmd, 0, sizeof cmd);

    cmd.CmdID = 1;
    cmd.Command.FilterTimeDays = params->SysCounters.FilterTime;
    cmd.Command.ComSet.FanSpeed = params->UserSet.FanSpeed;
    cmd.Command.ComSet.GatePosition = params->UserSet.GatePosition;
    cmd.Command.ComSet.SetTemperature = params->UserSet.SetTemperature;

    cmd.Command.ComFlags.State = params->UserSet.SysFlags.State;
    cmd.Command.ComFlags.SndIndicationState = params->UserSet.SysFlags.SndIndicationState;
    cmd.Command.ComFlags.LEDIndicationState = params->UserSet.SysFlags.LEDIndicationState;
    cmd.Command.ComFlags.HeaterState = params->UserSet.SysFlags.HeaterState;

    cmd.Command.test_type = params->test_type;
    cmd.Command.button_presets = params->button_presets;

    return cmd;
}

MABaseProtocol::ble_frame_t
MaRc::cmdToFrame(BRLT_RC_RF_SetCmd_TypeDef* cmd)
{
    ble_frame_t frame;
    frame.data = cmd;
    frame.data_size = sizeof(BRLT_RC_RF_SetCmd_TypeDef);
    frame.type = BLE_FRAME_TYPE_SET_MODE_NO_SAVE_REQ;
    frame.request_id = 1;
    return frame;
};

void MaRc::brlt_rem_send_set_params_cmd(BRLT_RC_RF_Response_TypeDef* params,
                                        bool ft_reset, bool hard_reset)
{
    BRLT_RC_RF_SetCmd_TypeDef cmd = responseToCmd(params);

    if(ft_reset)
    {
        cmd.Command.FilterTimeDays = 0;
        cmd.Command.ComFlags.FilterReset = 1;
    }

    if(hard_reset)
        cmd.Command.ComFlags.FactoryReset = 1;

    ble_frame_t frame = cmdToFrame(&cmd);

    sendFrame(frame);
}

void MaRc::frameRecieveCallback(ble_frame_t frame)
{
    switch(frame.type)
    {
       case BLE_FRAME_TYPE_GET_MODE_RESP:
        currentParams = brlt_rem_get_params_from_frame(frame);
        break;
       case BLE_FRAME_TYPE_DEV_STATUS_RESP:
        firmVersion = brlt_rem_get_fv_from_frame(frame);
        break;
       default:
        break;
    }
}

MABaseProtocol::RawDataList MaRc::paramRequest()
{
    resetDataForSend();
    brlt_rem_rc_send_get_params_request();
    return dataForSend;
}

MABaseProtocol::RawDataList MaRc::devRequest()
{
    resetDataForSend();
    brlt_rem_rc_send_dev_request();
    return dataForSend;
}

MABaseProtocol::RawDataList MaRc::setTemp(int t)
{
    resetDataForSend();

    currentParams.UserSet.SetTemperature = t;
    brlt_rem_send_set_params_cmd(&currentParams);

    return dataForSend;
}

MABaseProtocol::RawDataList MaRc::setSpeed(int sp)
{
    resetDataForSend();

    currentParams.UserSet.FanSpeed = sp;
    brlt_rem_send_set_params_cmd(&currentParams);

    return dataForSend;
}

MABaseProtocol::RawDataList MaRc::setHeaterState(bool st)
{
    resetDataForSend();

    currentParams.UserSet.SysFlags.HeaterState = st;
    brlt_rem_send_set_params_cmd(&currentParams);

    return dataForSend;
}

MABaseProtocol::RawDataList MaRc::setPowerState(bool st)
{
    resetDataForSend();

    currentParams.UserSet.SysFlags.State = st;
    brlt_rem_send_set_params_cmd(&currentParams);

    return dataForSend;
}

MABaseProtocol::RawDataList MaRc::setTestState(int type)
{
    resetDataForSend();

    currentParams.test_type = type;
    brlt_rem_send_set_params_cmd(&currentParams);

    return dataForSend;
}

MABaseProtocol::RawDataList MaRc::resetFilter()
{
    resetDataForSend();

    brlt_rem_send_set_params_cmd(&currentParams, true);

    return dataForSend;
}

MABaseProtocol::RawDataList MaRc::resetHard()
{
    resetDataForSend();

    brlt_rem_send_set_params_cmd(&currentParams, false, true);

    return dataForSend;
}

MABaseProtocol::RawDataList MaRc::switchSound()
{
    resetDataForSend();

    currentParams.UserSet.SysFlags.SndIndicationState =
            !currentParams.UserSet.SysFlags.SndIndicationState;

    brlt_rem_send_set_params_cmd(&currentParams);

    return dataForSend;
}

MABaseProtocol::RawDataList MaRc::setNoTest(void)
{
    return setTestState(NO_TEST);
}

MABaseProtocol::RawDataList MaRc::setGateTest(void)
{
    return setTestState(GATE_TEST);
}

MABaseProtocol::RawDataList MaRc::setHeaterTest(void)
{
    return setTestState(HEATER_TEST);
}

int8_t MaRc::temp() const
{
    return currentParams.UserSet.SetTemperature;
}

uint8_t MaRc::speed() const
{
    return currentParams.UserSet.FanSpeed;
}

bool MaRc::powerState() const
{
    return currentParams.UserSet.SysFlags.State;
}

bool MaRc::heaterState() const
{
    return currentParams.UserSet.SysFlags.HeaterState;
}

bool MaRc::noTest() const
{
    return currentParams.test_type == NO_TEST;
}

bool MaRc::gateTest() const
{
    return currentParams.test_type == GATE_TEST;
}

bool MaRc::heaterTest() const
{
    return currentParams.test_type == HEATER_TEST;
}

int MaRc::tempIn () const
{
    return currentParams.SensorState.T_in;
}

int MaRc::tempOut () const
{
    return currentParams.SensorState.T_out;
}

int MaRc::heaterVar () const
{
    return currentParams.heater_var;
}

int MaRc::filterCounter () const
{
    return currentParams.SysCounters.FilterTime;
}

int MaRc::elecCounter () const
{
    return currentParams.SysCounters.WorkTime;
}

int MaRc::motorCounter () const
{
    return currentParams.SysCounters.FanTime;
}

bool MaRc::error(int numb) const
{
    return currentParams.errors.reg & (1 << numb);
}

int MaRc::errorCnt(int numb) const
{
    return currentParams.errors.cnt.er[numb];
}

int MaRc::airFlowCnt() const
{
    return currentParams.SysCounters.airflow_counter;
}

int MaRc::firmVer() const
{
    return firmVersion;
}

int MaRc::speedLimit() const
{
    return currentParams.limits.max_fan_speed;
}
