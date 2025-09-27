
#pragma once
#include <cstdint>

namespace BQ25820
{
    // Register addresses and acronyms from Table 7-7 (BQ25820)
    enum class BQ25820_Register : uint8_t
    {
        REG0x00_Charge_Voltage_limit                   = 0x00,       // Charge Voltage Limit
        REG0x02_Charge_Current_Limit                   = 0x2,
        REG0x06_Input_Current_DPM_Limit                = 0x6,
        REG0x08_Input_Voltage_DPM_Limit                = 0x8,
        REG0x0A_Reverse_Mode_Input_Current_Limit       = 0xA,
        REG0x0C_Reverse_Mode_System_Voltage_Limit      = 0xC,
        REG0x10_Precharge_Current_Limit                = 0x10,
        REG0x12_Termination_Current_Limit              = 0x12,
        REG0x14_Precharge_and_Termination_Control      = 0x14,
        REG0x15_Timer_Control                          = 0x15,
        REG0x16_Three_Stage_Charge_Control             = 0x16,
        REG0x17_Charger_Control                        = 0x17,
        REG0x18_Pin_Control                            = 0x18,
        REG0x19_Power_Path_and_Reverse_Mode_Control    = 0x19,
        REG0x1A_MPPT_Control                           = 0x1A,
        REG0x1B_TS_Charging_Threshold_Control          = 0x1B,
        REG0x1C_TS_Charging_Region_Behavior_Control    = 0x1C,
        REG0x1D_TS_Reverse_Mode_Threshold_Control      = 0x1D,
        REG0x1E_Reverse_Undervoltage_Control           = 0x1E,
        REG0x1F_VAC_Max_Power_Point_Detected           = 0x1F,
        REG0x21_Charger_Status_1                       = 0x21,
        REG0x22_Charger_Status_2                       = 0x22,
        REG0x23_Charger_Status_3                       = 0x23,
        REG0x24_Fault_Status                           = 0x24,
        REG0x25_Charger_Flag_1                         = 0x25,
        REG0x26_Charger_Flag_2                         = 0x26,
        REG0x27_Fault_Flag                             = 0x27,
        REG0x28_Charger_Mask_1                         = 0x28,
        REG0x29_Charger_Mask_2                         = 0x29,
        REG0x2A_Fault_Mask                             = 0x2A,
        REG0x2B_ADC_Control                            = 0x2B,
        REG0x2C_ADC_Channel_Control                    = 0x2C,
        REG0x2D_IAC_ADC                                = 0x2D,
        REG0x2F_IBAT_ADC                               = 0x2F,
        REG0x31_VAC_ADC                                = 0x31,
        REG0x33_VBAT_ADC                               = 0x33,
        REG0x35_VSYS_ADC                               = 0x35,
        REG0x37_TS_ADC                                 = 0x37,
        REG0x39_VFB_ADC                                = 0x39,
        REG0x3B_Gate_Driver_Strength_Control           = 0x3B,
        REG0x3C_Gate_Driver_Dead_Time_Control          = 0x3C,
        REG0x3D_Part_Information                       = 0x3D,
        REG0x62_Reverse_Mode_Battery_Discharge_Current = 0x3E
    };
}       // namespace BQ25820
