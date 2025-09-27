#include "BQ25820/BQ25820.hpp"
#include "BQ25820/BQ25820_Registers.hpp"

#include <linux/i2c-dev.h>
#include <linux/i2c.h>
#include <sys/ioctl.h>

#define bit0 (1 << 0)
#define bit1 (1 << 1)
#define bit2 (1 << 2)
#define bit3 (1 << 3)
#define bit4 (1 << 4)
#define bit5 (1 << 5)
#define bit6 (1 << 6)
#define bit7 (1 << 7)

namespace BQ25820
{

    BQ25820::BQ25820(const char *pDeviceName, uint8_t i2c_address) : i2c_address_(i2c_address), device_name_(pDeviceName) {}

    BQ25820::~BQ25820() { shutdown(); }

    bool BQ25820::initialize()
    {
        // Initialization code for BQ25820
        i2c_.set_i2c_address(i2c_address_);
        i2c_.set_i2c_device_name(device_name_);
        bool ok = i2c_.open_device();
        if (ok)
            {
                // ToDo: Initialize the BQ25820 registers
                // Note: watch-dog timer is set to 40 seconds by default
                // Note: whwen enabling Charger, clear VFB_ADC_DIS bit

                // REG0x02_Charge_Current_Limit
                // REG0x19 EN_BATFET_IDEAL_DIODE
                // REG0x08_Input_Voltage_DPM_Limit
                // REG0x10_Precharge_Current_Limit
                // REG0x14_Precharge_and_Termination_Control EN_TERM, EN_PRECHG
                // REG0x17 EN_CHG
                // REG0x18_Pin_Control Register EN_ICHG_PIN disable?

                // The battery regulation voltage is programmed using a resistor divider to the FB pin.
                // The default internal voltage reference is 1.536V, and can be changed via the VFB_REG register bits.
                // fbt=249k fbb=25.2k  ref=1.536V target=16.7,    ref=1.504 target=16.3     ref=1.566 target=17.0
                // VFB = VREF * (R1 + R2) / R2
                // R2 = R1 * VFB / (VREF - VFB)
                // R1 = 249k, R2 = 25.2k gives VFB = 1.536V
                // R1 = 249k, R2 = 25.2k gives VFB = 1.504V
                // R1 = 249k, R2 = 25.2k gives VFB = 1.566V
            }
        return ok;
    }

    //~-+=~-+=~-+=~-+=~-+=~-+=~-+=~-+=~-+=~-+=~-+=~-+=~-+=~-+=~-+=~-+=~-+=~-+=~-+=
    //~-+=~-+=~-+=~-+=~-+=~-+=~-+=~-+=~-+=~-+=~-+=~-+=~-+=~-+=~-+=~-+=~-+=~-+=~-+=
    void BQ25820::spinOnce()
    {
        // Code to run in each iteration

        //~`=-+~`=-+~`=-+~`=-+~`=-+~`=-+~`=-+~`=-+~`=-+~`=-+~`=-+~`=-+~`=-+~`=-+~`=-+~`=-+~`=-+
        //~`=-+
        //~`=-+  ADC handling
        if ((elap_since_ADC_read_ > elap_since_ADC_start_interval_) && !adc_OneShot_in_progress_)
            {
                start_ADC_Conversion();
            }
        if ((elap_since_ADC_read_ > elap_since_ADC_read_interval_) && adc_OneShot_in_progress_)
            {
                adc_OneShot_in_progress_ = false;
                read_all_ADC_values();
                elap_since_ADC_read_ = 0;
            }
    }

    //~-+=~-+=~-+=~-+=~-+=~-+=~-+=~-+=~-+=~-+=~-+=~-+=~-+=~-+=~-+=~-+=~-+=~-+=~-+=
    //~-+=~-+=~-+=~-+=~-+=~-+=~-+=~-+=~-+=~-+=~-+=~-+=~-+=~-+=~-+=~-+=~-+=~-+=~-+=
    void BQ25820::set_BatFET_is_ideal_diode(bool enable)
    {
        uint8_t reg_value = i2c_.read_reg8(BQ25820_Register::REG0x19_Power_Path_and_Reverse_Mode_Control);
        if (enable) { reg_value |= bit2; }
        else { reg_value &= ~bit2; }
        i2c_.write_reg8(BQ25820_Register::REG0x19_Power_Path_and_Reverse_Mode_Control, reg_value);
    }

    void BQ25820::set_ChargeEnable(bool enable)
    {
        uint8_t reg_value = i2c_.read_reg8(BQ25820_Register::REG0x17_Charger_Control);
        if (enable) { reg_value |= bit0; }
        else { reg_value &= ~bit0; }
        i2c_.write_reg8(BQ25820_Register::REG0x17_Charger_Control, reg_value);
    }

    void BQ25820::set_BatFET_Force_off(bool enable)
    {
        uint8_t reg_value = i2c_.read_reg8(BQ25820_Register::REG0x19_Power_Path_and_Reverse_Mode_Control);
        if (enable) { reg_value |= bit4; }
        else { reg_value &= ~bit4; }
        i2c_.write_reg8(BQ25820_Register::REG0x19_Power_Path_and_Reverse_Mode_Control, reg_value);
    }

    bool BQ25820::input_power_good()
    {
        uint8_t stat2     = i2c_.read_reg8(BQ25820_Register::REG0x22_Charger_Status_2);
        input_power_good_ = (stat2 & bit7) != 0;
        return input_power_good_;
    }

    void BQ25820::set_ADC_Control(bool enable, bool one_shot, bool average)
    {
        uint8_t reg_value = i2c_.read_reg8(BQ25820_Register::REG0x2B_ADC_Control);
        if (enable) { reg_value |= bit7; }
        else { reg_value &= ~bit7; }

        if (one_shot)
            {
                reg_value |= bit6;       // one-shot
            }
        else
            {
                reg_value &= ~bit6;       // continuous
            }

        if (average)
            {
                reg_value |= bit3;       // average
                reg_value |= bit2;       // Start average using new ADC conversion
            }
        else
            {
                reg_value &= ~bit3;       // no average
            }

        i2c_.write_reg8(BQ25820_Register::REG0x2B_ADC_Control, reg_value);
    }

    void BQ25820::start_ADC_Conversion()
    {
        uint8_t reg_value = i2c_.read_reg8(BQ25820_Register::REG0x2B_ADC_Control);
        reg_value |= bit7;       // enable ADC
        reg_value |= bit6;       // enable one-shot

        i2c_.write_reg8(BQ25820_Register::REG0x2B_ADC_Control, reg_value);
        adc_OneShot_in_progress_ = true;
    }

    // // bool BQ25820::check_if_ADC_conversion_complete()
    // // {
    // //     if (adc_mode_is_continuous_) return true;       // always complete in continuous mode

    // //     bool complete = false;
    // //     if (adc_OneShot_in_progress_)
    // //         {
    // //             uint8_t reg_value = i2c_.read_reg8(BQ25820_Register::REG0x21_Charger_Status_1);
    // //             if ((reg_value & bit7) != 0)       // one-shot complete
    // //                 {
    // //                     adc_OneShot_in_progress_ = false;
    // //                     complete                 = true;
    // //                 }
    // //         }
    // //     return complete;
    // // }

    //~-+=~-+=~-+=~-+=~-+=~-+=~-+=~-+=~-+=~-+=~-+=~-+=~-+=~-+=~-+=~-+=~-+=~-+=~-+=
    void BQ25820::read_all_ADC_values()
    {
        // uint8_t reg_value = i2c_.read_reg8(BQ25820_Register::REG0x2B_ADC_Control);

        if (!adc_OneShot_in_progress_)
            {
                //--------------------- read ADC values ---------------------------
                if (input_power_good())
                    {
                        int16_t raw_IAC     = static_cast<int16_t>(i2c_.read_reg16(BQ25820_Register::REG0x2D_IAC_ADC));
                        ADC_IAC_value_amps_ = (raw_IAC * 2.0f) / 1000.0f;       // LSB = 2 mA

                        int16_t raw_VAC      = static_cast<int16_t>(i2c_.read_reg16(BQ25820_Register::REG0x31_VAC_ADC));
                        ADC_VAC_value_volts_ = (raw_VAC * 2.0f) / 1000.0f;       // LSB = 2 mV
                    }
                else
                    {
                        ADC_IAC_value_amps_  = 0.0f;
                        ADC_VAC_value_volts_ = 0.0f;
                    }

                int16_t raw_IBAT     = static_cast<int16_t>(i2c_.read_reg16(BQ25820_Register::REG0x2F_IBAT_ADC));
                ADC_IBAT_value_amps_ = (raw_IBAT * 2.0f) / 1000.0f;       // LSB = 2 mA

                int16_t raw_VBAT      = static_cast<int16_t>(i2c_.read_reg16(BQ25820_Register::REG0x33_VBAT_ADC));
                ADC_VBAT_value_volts_ = (raw_VBAT * 2.0f) / 1000.0f;       // LSB = 2 mV

                int16_t raw_VSYS      = static_cast<int16_t>(i2c_.read_reg16(BQ25820_Register::REG0x35_VSYS_ADC));
                ADC_VSYS_value_volts_ = (raw_VSYS * 2.0f) / 1000.0f;       // LSB = 2 mV

                int16_t raw_TS                = static_cast<int16_t>(i2c_.read_reg16(BQ25820_Register::REG0x37_TS_ADC));
                ADC_TS_value_percent_of_REGN_ = (raw_TS * 0.09765625f);       // LSB = 0.09765625 % of REGN

                // ToDo: convert ADC_TS_value_percent_of_REGN_ to degrees C

                // Note: VAC_ADC is only valid when AC is connected
            }
        // // }
    }

    //~-+=~-+=~-+=~-+=~-+=~-+=~-+=~-+=~-+=~-+=~-+=~-+=~-+=~-+=~-+=~-+=~-+=~-+=~-+=
    //~-+=~-+=~-+=~-+=~-+=~-+=~-+=~-+=~-+=~-+=~-+=~-+=~-+=~-+=~-+=~-+=~-+=~-+=~-+=
    void BQ25820::shutdown()
    {
        // Shutdown code for BQ25820
        // ToDo: turn off charger
        i2c_.close_device();
    }

    //~-+=~-+=~-+=~-+=~-+=~-+=~-+=~-+=~-+=~-+=~-+=~-+=~-+=~-+=~-+=~-+=~-+=~-+=~-+=
    //~-+=~-+=~-+=~-+=~-+=~-+=~-+=~-+=~-+=~-+=~-+=~-+=~-+=~-+=~-+=~-+=~-+=~-+=~-+=
    //------------- ADC ------------------
    // Note: ADC: use one-shot if not AC  (ADC_RATE, ADC_EN)
    // Note: ADC: use continious if AC is connected

    // REG0x2B_ADC_Control
    // REG0x2C_ADC_Channel_Control
    // then read data from:
    //       REG0x2D_IAC_ADC
    //       REG0x2F_IBAT_ADC
    //       REG0x31_VAC_ADC
    //       REG0x33_VBAT_ADC
    //       REG0x35_VSYS_ADC
    //       REG0x37_TS_ADC

    //~-+=~-+=~-+=~-+=~-+=~-+=~-+=~-+=~-+=~-+=~-+=~-+=~-+=~-+=~-+=~-+=~-+=~-+=~-+=
    //~-+=~-+=~-+=~-+=~-+=~-+=~-+=~-+=~-+=~-+=~-+=~-+=~-+=~-+=~-+=~-+=~-+=~-+=~-+=

    //~-+=~-+=~-+=~-+=~-+=~-+=~-+=~-+=~-+=~-+=~-+=~-+=~-+=~-+=~-+=~-+=~-+=~-+=~-+=

    bool BQ25820::beginCharging()
    {
        bool ok = false;
        // ToDo: Implement charging start sequence
        return ok;
    }

    bool BQ25820::endCharging()
    {
        bool ok = false;
        // ToDo: Implement charging stop sequence
        return ok;
    }

    void BQ25820::set_ChargeCurrentLimit(float limit_in_amps)
    {
        if (limit_in_amps < 1) limit_in_amps = 1;
        if (limit_in_amps > 20) limit_in_amps = 20;
        float    val_f     = (limit_in_amps * 20);
        uint16_t reg_value = static_cast<uint16_t>(val_f);
        i2c_.write_reg16(static_cast<uint8_t>(BQ25820_Register::REG0x02_Charge_Current_Limit), reg_value);
    }

    //~-+=~-+=~-+=~-+=~-+=~-+=~-+=~-+=~-+=~-+=~-+=~-+=~-+=~-+=~-+=~-+=~-+=~-+=~-+=
    //~-+=~-+=~-+=~-+=~-+=~-+=~-+=~-+=~-+=~-+=~-+=~-+=~-+=~-+=~-+=~-+=~-+=~-+=~-+=

    std::string BQ25820::get_Charger_Status1_str(uint8_t stat1) const
    {
        std::string status;
        if (stat1 & bit6) status += " InputCurrentReg";
        if (stat1 & bit5) status += " InputVoltageReg";
        if (stat1 & bit3) status += " WatchDogExp";
        uint8_t charge_cycle = stat1 & (bit0 | bit1 | bit2);
        switch (charge_cycle)
            {
                case 0: status += " Not Charging"; break;
                case 1: status += " Trickle Charging"; break;
                case 2: status += " Pre-Charge"; break;
                case 3: status += " Fast Charge"; break;
                case 4: status += " Taper Charge"; break;
                case 5: status += " Reserved"; break;
                case 6: status += " Top-off Charge"; break;
                case 7: status += " Charge Termination"; break;
            }

        return status;
    }

    std::string BQ25820::get_Charger_Status2_str(uint8_t stat2) const
    {
        std::string status;
        if (stat2 & bit7) status += " Input Power GOOD";
        uint8_t ntc = stat2 & (bit6 | bit5 | bit4);
        ntc         = ntc >> 4;
        switch (ntc)
            {
                case 0: status += " NTC Normal"; break;
                case 1: status += " NTC Warm"; break;
                case 2: status += " NTC Cool"; break;
                case 3: status += " NTC Cold"; break;
                case 4: status += " NTC Hot"; break;
                case 5: status += " NTC Not Connected"; break;
                case 6: status += " NTC Reserved"; break;
                case 7: status += " NTC Reserved"; break;
            }
        return status;
    }

    std::string BQ25820::get_Charger_Status3_str(uint8_t stat3) const
    {
        std::string status;
        if (stat3 & bit2) status += " Reverse Mode";
        if (stat3 & bit1) status += " ACFET On";
        if (stat3 & bit0) status += " BATFET On";
        return status;
    }

    std::string BQ25820::get_Fault_Status_str(uint8_t fault) const
    {
        std::string status;
        if (fault & bit7) status += " Input Under Voltage";
        if (fault & bit6) status += " Input Over Voltage";
        if (fault & bit5) status += " Bat Over Current";
        if (fault & bit4) status += " BAT Over Voltage";
        if (fault & bit3) status += " Thermal Shutdown";
        if (fault & bit2) status += " Safty Timer Exp";
        if (fault & bit1) status += " DRV_SUP Voltage Bad";
        if (fault & bit0) status += " Reserved";
        return status;
    }
    std::string BQ25820::get_Charger_Flag1_str(uint8_t flag1) const
    {
        std::string status;
        if (flag1 & bit7) status += " ADC Done INT";
        if (flag1 & bit6) status += " Input Current INT";
        if (flag1 & bit5) status += " Input Voltage INT";
        if (flag1 & bit4) status += " Reserved";
        if (flag1 & bit3) status += " WD_STAT rising INT";
        if (flag1 & bit2) status += " Reserved";
        if (flag1 & bit1) status += " Reserved";
        if (flag1 & bit0) status += " Charge Cycle Changed INT";
        return status;
    }
    std::string BQ25820::get_Charger_Flag2_str(uint8_t flag2) const
    {
        std::string status;
        if (flag2 & bit7) status += " PG changed INT";
        if (flag2 & bit6) status += " ACFET changed INT";
        if (flag2 & bit5) status += " BATFET changed INT";
        if (flag2 & bit4) status += " BAT NTC changed INT";
        if (flag2 & bit3) status += " Reverse Mode INT";
        if (flag2 & bit2) status += " Reserved";
        if (flag2 & bit1) status += " Reserved";
        if (flag2 & bit0) status += " MPPT changed INT";
        return status;
    }
    std::string BQ25820::get_Fault_Flag_str(uint8_t fault_flag) const
    {
        std::string status;
        if (fault_flag & bit7) status += " Input Under Voltage INT";
        if (fault_flag & bit6) status += " Input Over Voltage INT";
        if (fault_flag & bit5) status += " Bat Over Current INT";
        if (fault_flag & bit4) status += " BAT Over Voltage INT";
        if (fault_flag & bit3) status += " Thermal Shutdown INT";
        if (fault_flag & bit2) status += " Safty Timer Exp INT";
        if (fault_flag & bit1) status += " DRV_SUP Voltage Bad INT";
        if (fault_flag & bit0) status += " Reserved";
        return status;
    }

    std::string BQ25820::get_Part_Information_str(uint8_t part_info) const
    {
        std::string status;
        uint8_t     part_num = (part_info & (bit5 | bit4 | bit3)) >> 3;
        uint8_t     dev_rev  = part_info & (bit2 | bit1 | bit0);
        if (part_num == 0x03)
            status += " Part Number: BQ25820\n";
        else
            status += " Part Number: Unknown\n";
        status += "Device Revision: " + std::to_string(dev_rev);
        return status;
    }

    std::string BQ25820::get_Status_str() const
    {
        std::string status     = "BQ25820 Status:\n";
        uint8_t     stat1      = get_Charger_Status1();
        uint8_t     stat2      = get_Charger_Status2();
        uint8_t     stat3      = get_Charger_Status3();
        uint8_t     fault      = get_Fault_Status();
        uint8_t     flag1      = get_Charger_Flag1();
        uint8_t     flag2      = get_Charger_Flag2();
        uint8_t     fault_flag = get_Fault_Flag();
        uint8_t     part_info  = get_Part_Information();

        status += "BQ25820 Status:\n";
        status += "Stat1: " + get_Charger_Status1_str(stat1) + "\n";
        status += "Stat2: " + get_Charger_Status2_str(stat2) + "\n";
        status += "Stat3: " + get_Charger_Status3_str(stat3) + "\n";
        status += "Fault: " + get_Fault_Status_str(fault) + "\n";
        status += "Flag1: " + get_Charger_Flag1_str(flag1) + "\n";
        status += "Flag2: " + get_Charger_Flag2_str(flag2) + "\n";
        status += "Fault Flag: " + get_Fault_Flag_str(fault_flag) + "\n";
        status += "Part Information: " + get_Part_Information_str(part_info) + "\n";

        return status;
    }

}       // namespace BQ25820
