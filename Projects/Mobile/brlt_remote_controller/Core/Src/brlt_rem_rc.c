#include "brlt_rem_rc.h"
#include "string.h"

void brlt_rem_rc_send_get_params_request (void)
{
	ma_ble_frame_t frame = {0};
	frame.request_id = 1;
	frame.type = BLE_FRAME_TYPE_GET_MODE_REQ;
	ma_ble_put_frame_in_queue(frame);
}
void brlt_rem_rc_send_get_dev_status_request (void)
{
	ma_ble_frame_t frame = {0};
	frame.request_id = 1;
	frame.type = BLE_FRAME_TYPE_DEV_STATUS_REQ;
	ma_ble_put_frame_in_queue(frame);
}

uint32_t brlt_rem_rc_get_fv_ver (ma_ble_frame_t frame)
{
  	BLE_DevStatusFrame_TypeDef dev_st;
	memcpy(&dev_st,frame.data,frame.data_size);
	return dev_st.SoftVer;
}

BRLT_RC_RF_Response_TypeDef brlt_rem_get_params_from_frame(ma_ble_frame_t frame)
{
	BRLT_RC_RF_Response_TypeDef ret;
	memcpy(&ret,frame.data,frame.data_size);
	return ret;
}
	
void brlt_rem_send_set_params_cmd(BRLT_RC_RF_Response_TypeDef* params)
{
	BRLT_RC_RF_SetCmd_TypeDef cmd = {0};
	
	cmd.CmdID = 1;
	cmd.Command.filter_time_in_days = params->SysCounters.FilterTime;
	cmd.Command.ComSet.FanSpeed = params->UserSet.FanSpeed;
	cmd.Command.ComSet.GatePosition = params->UserSet.GatePosition;
	cmd.Command.ComSet.SetTemperature = params->UserSet.SetTemperature;
	
	cmd.Command.ComFlags.State = params->UserSet.SysFlags.State;
	cmd.Command.ComFlags.SndIndicationState = params->UserSet.SysFlags.SndIndicationState;
	cmd.Command.ComFlags.LEDIndicationState = params->UserSet.SysFlags.LEDIndicationState;
	cmd.Command.ComFlags.HeaterState = params->UserSet.SysFlags.HeaterState;
	
	cmd.Command.test_type = params->test_type;
	cmd.Command.button_presets = params->button_presets;
	
	ma_ble_frame_t frame = 
	{.data = &cmd,
	 .data_size = sizeof(BRLT_RC_RF_SetCmd_TypeDef),
	 .type = BLE_FRAME_TYPE_SET_MODE_NO_SAVE_REQ,
	 .request_id = 1
	};
	
	ma_ble_put_frame_in_queue(frame);
}

void brlt_rem_send_test_cmd (void)
{
	BRLT_RC_RF_SetCmd_TypeDef cmd = {0};
	
	cmd.CmdID = 1;
	cmd.Command.filter_time_in_days = 365;
	cmd.Command.ComSet.FanSpeed = 2;
	cmd.Command.ComSet.GatePosition = 1;;
	cmd.Command.ComSet.SetTemperature = 20;
	
	cmd.Command.ComFlags.State = 1;
	cmd.Command.ComFlags.SndIndicationState = 1;
	cmd.Command.ComFlags.LEDIndicationState = 1;
	cmd.Command.ComFlags.HeaterState = 1;
	
	ma_ble_frame_t frame = 
	{.data = &cmd,
	 .data_size = sizeof(BRLT_RC_RF_SetCmd_TypeDef),
	 .type = BLE_FRAME_TYPE_SET_MODE_NO_SAVE_REQ,
	 .request_id = 1
	};
	
	ma_ble_put_frame_in_queue(frame);	
	
	
}