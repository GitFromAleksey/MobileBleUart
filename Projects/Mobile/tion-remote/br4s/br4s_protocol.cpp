#include "br4s_protocol.h"
#include "lib/crc_calculate.h"
#include <QDebug>


//**********************************************************
// RC Level
//**********************************************************


BR4S_Rc::BR4S_Rc()
{
    memset(&currentParams, 0, sizeof currentParams);
    memset(timer, 0, sizeof(br4s_timer_settings_t) * NUM_OF_TIMERS);
}


BR4S_Rc::BRLT_RC_RF_Response_TypeDef
BR4S_Rc::brlt_rem_get_params_from_frame (ble_frame_t frame)
{
    BRLT_RC_RF_Response_TypeDef ret;
    //qDebug() << frame.data_size << sizeof (ret);

    memcpy(&ret,frame.data,/*frame.data_size*/ sizeof(BRLT_RC_RF_Response_TypeDef));
    return ret;
}

int BR4S_Rc::brlt_rem_get_fv_from_frame (ble_frame_t frame)
{
    BLE_DevStatusFrame_TypeDef dev_st;
    memcpy(&dev_st, frame.data, sizeof (dev_st));
    return dev_st.SoftVer;
}

void BR4S_Rc::br4s_rem_get_err_cnt_from_frame(ble_frame_t frame)
{

    brlt_error_data err_data;
    memcpy(&err_data, frame.data, frame.data_size);
    memcpy(err_cnt.er, err_data.cnt.er, sizeof(uint8_t)*ERROR_TYPE_NUMBER);
}

void BR4S_Rc::br4s_rem_get_timer_settings_from_frame(ble_frame_t frame)
{
    BRLT_RC_RF_TimerSettings_Resp_TypeDef b_timer_set;
    memcpy(&b_timer_set, frame.data, sizeof(b_timer_set));
    timer[b_timer_set.timer_id] = b_timer_set.timer;
}

void BR4S_Rc::br4s_rem_get_time_from_frame(ble_frame_t frame)
{
    br4s_rc_time_pack_t b_time_pack;

    memcpy(&b_time_pack, frame.data, sizeof(b_time_pack));
    currDateTime = QDateTime::fromSecsSinceEpoch(b_time_pack.time.unix_time, Qt::UTC);
}

void BR4S_Rc::br4s_rem_get_turbo_state_from_frame(ble_frame_t frame)
{
    br4s_rc_turbo_state_pack_t b_turbo_pack;

    memcpy(&(b_turbo_pack), frame.data, sizeof(b_turbo_pack));
    turbo_state = b_turbo_pack.turbo_state;
}

void BR4S_Rc::brlt_rem_rc_send_get_params_request (void)
{
    ble_frame_t frame; //= {0};
    memset(&frame, 0, sizeof frame);
    frame.request_id = 1;
    frame.type = BLE_FRAME_TYPE_GET_MODE_REQ;
    sendFrame(frame);
}

void BR4S_Rc::brlt_rem_rc_send_dev_request (void)
{
    ble_frame_t frame; //= {0};
    memset(&frame, 0, sizeof frame);
    frame.request_id = 1;
    frame.type = BLE_FRAME_TYPE_DEV_STATUS_REQ;
    sendFrame(frame);
}

void BR4S_Rc::br4s_rem_rc_send_err_cnt_request(void)
{
    ble_frame_t frame; //= {0};
    memset(&frame, 0, sizeof frame);
    frame.request_id = 1;
    frame.type = BLE_FRAME_TYPE_GET_ERR_CNT_REQ;
    sendFrame(frame);
}

void BR4S_Rc::br4s_rem_rc_send_start_test_request(uint32_t test_type)
{
    uint32_t test = test_type;
    ble_frame_t frame; //= {0};
    memset(&frame, 0, sizeof frame);
    frame.request_id = 1;
    frame.type = BLE_FRAME_TYPE_SET_TEST_REQ;
    frame.data = &test;
    frame.data_size = sizeof(uint32_t);
    sendFrame(frame);
}

void BR4S_Rc::br4s_rem_rc_send_curr_test_request(void)
{
    ble_frame_t frame; //= {0};
    memset(&frame, 0, sizeof frame);
    frame.request_id = 1;
    frame.type = BLE_FRAME_TYPE_GET_CURR_TEST_REQ;
    sendFrame(frame);
}

void BR4S_Rc::br4s_rem_rc_send_timer_settings_request(uint8_t timer_id)
{
    BRLT_RC_RF_TimerSettings_Req_TypeDef tim_req;
    ble_frame_t frame; //= {0};
    memset(&frame, 0, sizeof frame);

    tim_req.cmd_id = 1;
    tim_req.timer_id = timer_id;

    frame.data = &tim_req;
    frame.data_size = sizeof(tim_req);
    frame.request_id = 1;
    frame.type = BLE_FRAME_TYPE_GET_TIMER_REQ;
    sendFrame(frame);
}

void BR4S_Rc::br4s_rem_rc_send_set_timer_settings_request(br4s_timer_settings_t tim_set, uint8_t timer_id)
{
    BRLT_RC_RF_TimerSettings_Resp_TypeDef b_tim_set_pack;
    ble_frame_t frame; //= {0};
    memset(&frame, 0, sizeof frame);

    b_tim_set_pack.cmd_id = 1;
    b_tim_set_pack.timer_id = timer_id;
    b_tim_set_pack.timer = tim_set;

    frame.data = &b_tim_set_pack;
    frame.data_size = sizeof(b_tim_set_pack);
    frame.request_id = 1;
    frame.type = BLE_FRAME_TYPE_SET_TIMER_REQ;
    sendFrame(frame);
}

void BR4S_Rc::br4s_rem_rc_send_timer_state_request()
{
    ble_frame_t frame; //= {0};
    memset(&frame, 0, sizeof frame);
    frame.request_id = 1;
    frame.type = BLE_FRAME_TYPE_GET_TIMERS_STATE_REQ;
    sendFrame(frame);
}

void BR4S_Rc::br4s_rem_rc_send_time_set_request(br4s_time_format_t time)
{
    br4s_rc_time_pack_t b_time_pack;
    ble_frame_t frame; //= {0};
    memset(&frame, 0, sizeof frame);

    b_time_pack.cmd_id = 1;
    b_time_pack.time = time;

    frame.data = &b_time_pack;
    frame.data_size = sizeof(b_time_pack);
    frame.request_id = 1;
    frame.type = BLE_FRAME_TYPE_SET_TIME_REQ;
    sendFrame(frame);
}

void BR4S_Rc::br4s_rem_rc_send_time_request()
{
    ble_frame_t frame; //= {0};
    memset(&frame, 0, sizeof (frame));
    frame.request_id = 1;
    frame.type = BLE_FRAME_TYPE_GET_TIME_REQ;
    sendFrame(frame);
}

void BR4S_Rc::br4s_rem_rc_send_turbo_start_request(uint16_t time)
{
    ble_frame_t frame; //= {0};
    br4s_rc_turbo_state_pack_req_t b_turbo = {.time = time, .err_code = 0x00};
    memset(&frame, 0, sizeof (frame));

    frame.data = &b_turbo;
    frame.data_size = sizeof(br4s_rc_turbo_state_pack_req_t);
    frame.request_id = 1;
    frame.type = BLE_FRAME_TYPE_START_TURBO;
    sendFrame(frame);
}

void BR4S_Rc::br4s_rem_rc_send_turbo_err_reset_request()
{
    ble_frame_t frame; //= {0};
    br4s_rc_turbo_state_pack_req_t b_turbo = {.time = 0, .err_code = 0x00};
    memset(&frame, 0, sizeof (frame));

    frame.data = &b_turbo;
    frame.data_size = sizeof(br4s_rc_turbo_state_pack_req_t);
    frame.request_id = 1;
    frame.type = BLE_FRAME_TYPE_START_TURBO;
    sendFrame(frame);
}

void BR4S_Rc::br4s_rem_rc_send_turbo_state_request()
{
    ble_frame_t frame; //= {0};
    memset(&frame, 0, sizeof (frame));

    frame.request_id = 1;
    frame.type = BLE_FRAME_TYPE_GET_TURBO_PARAM_REQ;
    sendFrame(frame);
}

BR4S_Rc::BRLT_RC_RF_SetCmd_TypeDef
BR4S_Rc::responseToCmd(BRLT_RC_RF_Response_TypeDef* params)
{
    BRLT_RC_RF_SetCmd_TypeDef cmd; // = {0};
    memset(&cmd, 0, sizeof cmd);

    cmd.CmdID = 1;

    cmd.Command.FilterTimeDays = params->SysCounters.FilterTime;
    cmd.Command.ComSet.FanSpeed = params->UserSet.FanSpeed;
    cmd.Command.ComSet.Substate = params->UserSet.Substate;
    cmd.Command.ComSet.SetTemperature = params->UserSet.SetTemperature;

    cmd.Command.ComFlags.State = params->UserSet.SysFlags.State;
    cmd.Command.ComFlags.SndIndicationState = params->UserSet.SysFlags.SndIndicationState;
    cmd.Command.ComFlags.LEDIndicationState = params->UserSet.SysFlags.LEDIndicationState;
    cmd.Command.ComFlags.MA_Auto = params->UserSet.SysFlags.MA_Auto;
    cmd.Command.ComFlags.MagicAir = params->UserSet.SysFlags.MagicAir;
    cmd.Command.ComFlags.HeaterMode = params->UserSet.SysFlags.HeaterMode;



    return cmd;
}

BR4S_Rc::ble_frame_t
BR4S_Rc::cmdToFrame(BRLT_RC_RF_SetCmd_TypeDef* cmd)
{
    ble_frame_t frame;
    frame.data = cmd;
    frame.data_size = sizeof(BRLT_RC_RF_SetCmd_TypeDef);
    frame.type = BLE_FRAME_TYPE_SET_MODE_NO_SAVE_REQ;
    frame.request_id = 1;
    return frame;
};

void BR4S_Rc::brlt_rem_send_set_params_cmd(BRLT_RC_RF_Response_TypeDef* params,
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

void BR4S_Rc::frameRecieveCallback(ble_frame_t frame)
{
    switch(frame.type)
    {
       case BLE_FRAME_TYPE_GET_MODE_RESP:
        currentParams = brlt_rem_get_params_from_frame(frame);
        break;
       case BLE_FRAME_TYPE_DEV_STATUS_RESP:
        firmVersion = brlt_rem_get_fv_from_frame(frame);
        break;
       case BLE_FRAME_TYPE_GET_ERR_CNT_RESP:
        br4s_rem_get_err_cnt_from_frame(frame);
        break;
       case BLE_FRAME_TYPE_GET_CURR_TEST_REQ:
         memcpy(&test_type, frame.data, sizeof(test_type));
        break;
       case BLE_FRAME_TYPE_GET_TIMER_RESP:
        br4s_rem_get_timer_settings_from_frame(frame);
        break;
       case BLE_FRAME_TYPE_GET_TIMERS_STATE_RESP:

        break;
       case BLE_FRAME_TYPE_GET_TIME_RESP:
        br4s_rem_get_time_from_frame(frame);
        break;
       case BLE_FRAME_TYPE_GET_TURBO_PARAM_RESP:
        br4s_rem_get_turbo_state_from_frame(frame);
       default:
        break;
    }
}

BR4S_Rc::RawDataList BR4S_Rc::CurrTestRequest()
{
    resetDataForSend();
    br4s_rem_rc_send_curr_test_request();
    return dataForSend;
}

BR4S_Rc::RawDataList BR4S_Rc::ErrCntRequest()
{
    resetDataForSend();
    br4s_rem_rc_send_err_cnt_request();
    return dataForSend;
}

BR4S_Rc::RawDataList BR4S_Rc::paramRequest()
{
    resetDataForSend();
    brlt_rem_rc_send_get_params_request();
    return dataForSend;
}

BR4S_Rc::RawDataList BR4S_Rc::devRequest()
{
    resetDataForSend();
    brlt_rem_rc_send_dev_request();
    return dataForSend;
}

BR4S_Rc::RawDataList BR4S_Rc::setSubstate(uint8_t subst)
{
    resetDataForSend();

    currentParams.UserSet.Substate = subst;
    brlt_rem_send_set_params_cmd(&currentParams);

    return dataForSend;
}

BR4S_Rc::RawDataList BR4S_Rc::setTemp(int t)
{
    resetDataForSend();

    currentParams.UserSet.SetTemperature = static_cast<int8_t>(t);
    brlt_rem_send_set_params_cmd(&currentParams);

    return dataForSend;
}

BR4S_Rc::RawDataList BR4S_Rc::setSpeed(int sp)
{
    resetDataForSend();

    currentParams.UserSet.FanSpeed = static_cast<uint8_t>(sp);
    brlt_rem_send_set_params_cmd(&currentParams);

    return dataForSend;
}

BR4S_Rc::RawDataList BR4S_Rc::setPowerState(bool st)
{
    resetDataForSend();

    currentParams.UserSet.SysFlags.State = st;
    brlt_rem_send_set_params_cmd(&currentParams);

    return dataForSend;
}

BR4S_Rc::RawDataList BR4S_Rc::setTestState(int type)
{
    resetDataForSend();

    test_type = static_cast<uint32_t>(type);
    br4s_rem_rc_send_start_test_request(test_type);

    return dataForSend;
}

BR4S_Rc::RawDataList BR4S_Rc::resetFilter()
{
    resetDataForSend();

    brlt_rem_send_set_params_cmd(&currentParams, true);

    return dataForSend;
}

BR4S_Rc::RawDataList BR4S_Rc::resetHard()
{
    resetDataForSend();

    brlt_rem_send_set_params_cmd(&currentParams, false, true);

    return dataForSend;
}

BR4S_Rc::RawDataList BR4S_Rc::switchSound(bool st)
{
    resetDataForSend();

    currentParams.UserSet.SysFlags.SndIndicationState = st;

    brlt_rem_send_set_params_cmd(&currentParams);

    return dataForSend;
}

BR4S_Rc::RawDataList BR4S_Rc::switchLED(bool st)
{
    resetDataForSend();

    currentParams.UserSet.SysFlags.LEDIndicationState = st;

    brlt_rem_send_set_params_cmd(&currentParams);

    return dataForSend;
}

BR4S_Rc::RawDataList BR4S_Rc::switchMA(bool st)
{
    resetDataForSend();

    currentParams.UserSet.SysFlags.MagicAir = st;

    brlt_rem_send_set_params_cmd(&currentParams);

    return dataForSend;
}

BR4S_Rc::RawDataList BR4S_Rc::switchMA_auto(bool st)
{
    resetDataForSend();

    currentParams.UserSet.SysFlags.MA_Auto = st;

    brlt_rem_send_set_params_cmd(&currentParams);

    return dataForSend;
}

BR4S_Rc::RawDataList BR4S_Rc::setHeaterMode(bool mode)
{
    resetDataForSend();

    currentParams.UserSet.SysFlags.HeaterMode = mode;
    brlt_rem_send_set_params_cmd(&currentParams);

    return dataForSend;
}

BR4S_Rc::RawDataList BR4S_Rc::setNoTest(void)
{
    return setTestState(NO_TEST);
}

BR4S_Rc::RawDataList BR4S_Rc::setGateTest(void)
{
    return setTestState(GATE_TEST);
}

BR4S_Rc::RawDataList BR4S_Rc::setHeaterTest(void)
{
    return setTestState(HEATER_TEST);
}

BR4S_Rc::RawDataList BR4S_Rc::setDefaultTest(void)
{
    return setTestState(DEFAULT_TEST);
}

BR4S_Rc::RawDataList BR4S_Rc::setTime (int64_t unix_time, int64_t timezone_offset)
{
    resetDataForSend();
    br4s_time_format_t b_time;
    b_time.unix_time = unix_time + timezone_offset;

    br4s_rem_rc_send_time_set_request(b_time);
    return dataForSend;
}

BR4S_Rc::RawDataList BR4S_Rc::timerSettingsRequest(uint8_t timer_id)
{
    resetDataForSend();
    br4s_rem_rc_send_timer_settings_request(timer_id);
    return dataForSend;
}

BR4S_Rc::RawDataList BR4S_Rc::timeRequest()
{
    resetDataForSend();
    br4s_rem_rc_send_time_request();
    return dataForSend;
}

BR4S_Rc::RawDataList BR4S_Rc::setTimerEnable(bool state, uint8_t timer_id)
{
    resetDataForSend();
    timer[timer_id].timer_flags.timer_state = state;
    br4s_rem_rc_send_set_timer_settings_request(timer[timer_id],timer_id);
    return dataForSend;
}

BR4S_Rc::RawDataList BR4S_Rc::setTimerTime(QTime time, uint8_t timer_id)
{
    resetDataForSend();
    timer[timer_id].timer_time.hours = static_cast<uint8_t>(time.hour());
    timer[timer_id].timer_time.minutes = static_cast<uint8_t>(time.minute());
    br4s_rem_rc_send_set_timer_settings_request(timer[timer_id],timer_id);
    return dataForSend;
}

BR4S_Rc::RawDataList BR4S_Rc::setTimerMonday(bool state, uint8_t timer_id)
{
    resetDataForSend();
    timer[timer_id].timer_time.Monday = state;
    br4s_rem_rc_send_set_timer_settings_request(timer[timer_id],timer_id);
    return dataForSend;
}

BR4S_Rc::RawDataList BR4S_Rc::setTimerTuesday(bool state, uint8_t timer_id)
{
    resetDataForSend();
    timer[timer_id].timer_time.Tuesday = state;
    br4s_rem_rc_send_set_timer_settings_request(timer[timer_id],timer_id);
    return dataForSend;
}

BR4S_Rc::RawDataList BR4S_Rc::setTimerWednesday(bool state, uint8_t timer_id)
{
    resetDataForSend();
    timer[timer_id].timer_time.Wednesday = state;
    br4s_rem_rc_send_set_timer_settings_request(timer[timer_id],timer_id);
    return dataForSend;
}

BR4S_Rc::RawDataList BR4S_Rc::setTimerThursday(bool state, uint8_t timer_id)
{
    resetDataForSend();
    timer[timer_id].timer_time.Thursday = state;
    br4s_rem_rc_send_set_timer_settings_request(timer[timer_id],timer_id);
    return dataForSend;
}

BR4S_Rc::RawDataList BR4S_Rc::setTimerFriday(bool state, uint8_t timer_id)
{
    resetDataForSend();
    timer[timer_id].timer_time.Friday = state;
    br4s_rem_rc_send_set_timer_settings_request(timer[timer_id],timer_id);
    return dataForSend;
}

BR4S_Rc::RawDataList BR4S_Rc::setTimerSaturday(bool state, uint8_t timer_id)
{
    resetDataForSend();
    timer[timer_id].timer_time.Saturday = state;
    br4s_rem_rc_send_set_timer_settings_request(timer[timer_id],timer_id);
    return dataForSend;
}

BR4S_Rc::RawDataList BR4S_Rc::setTimerSunday(bool state, uint8_t timer_id)
{
    resetDataForSend();
    timer[timer_id].timer_time.Sunday = state;
    br4s_rem_rc_send_set_timer_settings_request(timer[timer_id],timer_id);
    return dataForSend;
}

BR4S_Rc::RawDataList BR4S_Rc::setTimerSubstate(uint8_t substate, uint8_t timer_id)
{
    resetDataForSend();
    timer[timer_id].device_mode = substate;
    br4s_rem_rc_send_set_timer_settings_request(timer[timer_id],timer_id);
    return dataForSend;
}

BR4S_Rc::RawDataList BR4S_Rc::setTimerHeaterMode(bool heater_mode, uint8_t timer_id)
{
    resetDataForSend();
    timer[timer_id].timer_flags.heater_mode = heater_mode;
    br4s_rem_rc_send_set_timer_settings_request(timer[timer_id],timer_id);
    return dataForSend;
}

BR4S_Rc::RawDataList BR4S_Rc::setTimerPowerState(bool power_state, uint8_t timer_id)
{
    resetDataForSend();
    timer[timer_id].timer_flags.power_state = power_state;
    br4s_rem_rc_send_set_timer_settings_request(timer[timer_id],timer_id);
    return dataForSend;
}

BR4S_Rc::RawDataList BR4S_Rc::setTimerSndIndState(bool snd_ind_state, uint8_t timer_id)
{
    resetDataForSend();
    timer[timer_id].timer_flags.sound_ind_state = snd_ind_state;
    br4s_rem_rc_send_set_timer_settings_request(timer[timer_id],timer_id);
    return dataForSend;
}

BR4S_Rc::RawDataList BR4S_Rc::setTimerLEDIndState(bool led_ind_state, uint8_t timer_id)
{
    resetDataForSend();
    timer[timer_id].timer_flags.led_ind_state = led_ind_state;
    br4s_rem_rc_send_set_timer_settings_request(timer[timer_id],timer_id);
    return dataForSend;
}

BR4S_Rc::RawDataList BR4S_Rc::setTimerTargetSpeed(uint8_t speed, uint8_t timer_id)
{
    resetDataForSend();
    timer[timer_id].fan_state = speed;
    br4s_rem_rc_send_set_timer_settings_request(timer[timer_id],timer_id);
    return dataForSend;
}

BR4S_Rc::RawDataList BR4S_Rc::setTimerTargetTemp(int8_t temp, uint8_t timer_id)
{
    resetDataForSend();
    timer[timer_id].target_temp = temp;
    br4s_rem_rc_send_set_timer_settings_request(timer[timer_id],timer_id);
    return dataForSend;
}

BR4S_Rc::RawDataList BR4S_Rc::StartTurbo(int16_t time)
{
    resetDataForSend();
    br4s_rem_rc_send_turbo_start_request(static_cast<uint16_t>(time));
    return dataForSend;
}

BR4S_Rc::RawDataList BR4S_Rc::ResetTurboErr()
{
    resetDataForSend();
    br4s_rem_rc_send_turbo_err_reset_request();
    return dataForSend;
}

BR4S_Rc::RawDataList BR4S_Rc::TurboReq()
{
    resetDataForSend();
    br4s_rem_rc_send_turbo_state_request();
    return dataForSend;
}

int8_t BR4S_Rc::temp() const
{
    return currentParams.UserSet.SetTemperature;
}

uint8_t BR4S_Rc::speed() const
{
    return currentParams.UserSet.FanSpeed;
}

bool BR4S_Rc::powerState() const
{
    return currentParams.UserSet.SysFlags.State;
}

bool BR4S_Rc::heaterState() const
{
    return currentParams.UserSet.SysFlags.HeaterState;
}

bool BR4S_Rc::heaterMode() const
{
    return currentParams.UserSet.SysFlags.HeaterMode;
}


bool BR4S_Rc::noTest() const
{
    return test_type == NO_TEST;
}

bool BR4S_Rc::gateTest() const
{
    return test_type == GATE_TEST;
}

bool BR4S_Rc::heaterTest() const
{
    return test_type == HEATER_TEST;
}

bool BR4S_Rc::defaultTest() const
{
    return test_type == DEFAULT_TEST;
}

int BR4S_Rc::tempIn () const
{
    return currentParams.SensorState.T_in;
}

int BR4S_Rc::tempOut () const
{
    return currentParams.SensorState.T_out;
}

int BR4S_Rc::tempPwrb () const
{
    return currentParams.SensorState.T_pwrb;
}

int BR4S_Rc::tempCtrlb () const
{
    return currentParams.SensorState.T_ctrlb;
}

int BR4S_Rc::heaterVar () const
{
    return currentParams.heater_var;
}

uint32_t BR4S_Rc::filterCounter () const
{
    return currentParams.SysCounters.FilterTime;
}

uint32_t BR4S_Rc::elecCounter () const
{
    return currentParams.SysCounters.WorkTime;
}

uint32_t BR4S_Rc::motorCounter () const
{
    return currentParams.SysCounters.FanTime;
}

bool BR4S_Rc::error(int numb) const
{
    return currentParams.errors & (1 << numb);
}

uint8_t BR4S_Rc::errorCnt (int numb) const
{
    return err_cnt.er[numb];
}

uint32_t BR4S_Rc::airFlowCnt() const
{
    return currentParams.SysCounters.airflow_counter;
}

int BR4S_Rc::firmVer() const
{
    return firmVersion;
}

uint32_t BR4S_Rc::speedLimit() const
{
    return currentParams.limits.max_fan_speed;
}

bool BR4S_Rc::maControl() const
{
    return currentParams.UserSet.SysFlags.MagicAir;
}

bool BR4S_Rc::maAutoControl() const
{
    return currentParams.UserSet.SysFlags.MA_Auto;
}

bool BR4S_Rc::LEDstate() const
{
    return currentParams.UserSet.SysFlags.LEDIndicationState;
}

bool BR4S_Rc::SoundState() const
{
    return currentParams.UserSet.SysFlags.SndIndicationState;
}

int BR4S_Rc::substate() const
{
    return currentParams.UserSet.Substate;
}

int BR4S_Rc::heaterPresent() const
{
    return currentParams.UserSet.SysFlags.HeaterPresent;
}

QDateTime BR4S_Rc::getDateTime() const
{
    return currDateTime;
}

BR4S_Rc::br4s_turbo_state_t BR4S_Rc::getTurboState() const
{
    return turbo_state;
}

bool BR4S_Rc::getTimerEnable(uint8_t timer_id) const
{
    return timer[timer_id].timer_flags.timer_state;
}

QTime BR4S_Rc::getTimerTime(uint8_t timer_id) const
{
    return QTime(timer[timer_id].timer_time.hours, timer[timer_id].timer_time.minutes);
}

bool BR4S_Rc::getTimerMonday(uint8_t timer_id) const
{
    return timer[timer_id].timer_time.Monday;
}

bool BR4S_Rc::getTimerTuesday(uint8_t timer_id) const
{
    return timer[timer_id].timer_time.Tuesday;
}

bool BR4S_Rc::getTimerWednesday(uint8_t timer_id) const
{
    return timer[timer_id].timer_time.Wednesday;
}

bool BR4S_Rc::getTimerThursday(uint8_t timer_id) const
{
    return timer[timer_id].timer_time.Thursday;
}

bool BR4S_Rc::getTimerFriday(uint8_t timer_id) const
{
    return timer[timer_id].timer_time.Friday;
}

bool BR4S_Rc::getTimerSaturday(uint8_t timer_id) const
{
    return timer[timer_id].timer_time.Saturday;
}

bool BR4S_Rc::getTimerSunday(uint8_t timer_id) const
{
    return timer[timer_id].timer_time.Sunday;
}

uint8_t BR4S_Rc::getTimerSubstate(uint8_t timer_id) const
{
    return timer[timer_id].device_mode;
}

bool BR4S_Rc::getTimerHeaterMode(uint8_t timer_id) const
{
    return timer[timer_id].timer_flags.heater_mode;
}

bool BR4S_Rc::getTimerPowerState(uint8_t timer_id) const
{
    return timer[timer_id].timer_flags.power_state;
}

bool BR4S_Rc::getTimerSndIndState(uint8_t timer_id) const
{
    return timer[timer_id].timer_flags.sound_ind_state;
}

bool BR4S_Rc::getTimerLEDIndState(uint8_t timer_id) const
{
    return timer[timer_id].timer_flags.led_ind_state;
}

uint8_t BR4S_Rc::getTimerTargetSpeed(uint8_t timer_id) const
{
    return timer[timer_id].fan_state;
}

int8_t BR4S_Rc::getTimerTargetTemp(uint8_t timer_id) const
{
    return timer[timer_id].target_temp;
}
