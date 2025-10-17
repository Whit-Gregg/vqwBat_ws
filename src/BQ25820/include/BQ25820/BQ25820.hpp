#ifndef BQ25820__BQ25820_HPP_
#define BQ25820__BQ25820_HPP_

#include "BQ25820/BQ25820_Registers.hpp"
#include "BQ25820/BQ25820_i2c.hpp"
#include "BQ25820/elapsedMillis.h"
#include "BQ25820/visibility_control.h"
#include <cstdint>
#include <string>

namespace BQ25820
{

    class BQ25820
    {
      public:
        BQ25820(const char *pDeviceName = "/dev/i2c-0", uint8_t i2c_address = 0x6B);       // Default I2C address for BQ25820 is 0x6B

        virtual ~BQ25820();

        bool initialize();
        void spinOnce();       // call this periodically to handle background tasks, 2 or 3 Hz
        void shutdown();

        bool beginCharging();
        bool endCharging();
        void set_ChargeCurrentLimit(float limit_in_amps);
        void set_BatFET_is_ideal_diode(bool enable);
        void set_ChargeEnable(bool enable);
        void set_BatFET_Force_off(bool enable);
        void set_ADC_Control(bool enable, bool one_shot, bool average);

        std::string get_Status_str() const;
        std::string get_Status_changes_str() const;
        void display_status_changes();

        bool update_statuses();   // true if changes in statuses

        uint8_t get_Charger_Status1() const { return i2c_.read_reg8(BQ25820_Register::REG0x21_Charger_Status_1); }
        uint8_t get_Charger_Status2() const { return i2c_.read_reg8(BQ25820_Register::REG0x22_Charger_Status_2); }
        uint8_t get_Charger_Status3() const { return i2c_.read_reg8(BQ25820_Register::REG0x23_Charger_Status_3); }
        uint8_t get_Fault_Status() const { return i2c_.read_reg8(BQ25820_Register::REG0x24_Fault_Status); }
        uint8_t get_Charger_Flag1() const { return i2c_.read_reg8(BQ25820_Register::REG0x25_Charger_Flag_1); }
        uint8_t get_Charger_Flag2() const { return i2c_.read_reg8(BQ25820_Register::REG0x26_Charger_Flag_2); }
        uint8_t get_Fault_Flag() const { return i2c_.read_reg8(BQ25820_Register::REG0x27_Fault_Flag); }
        uint8_t get_Part_Information() const { return i2c_.read_reg8(BQ25820_Register::REG0x3D_Part_Information); }

        std::string get_Charger_Status1_str(uint8_t stat1) const;
        std::string get_Charger_Status2_str(uint8_t stat2) const;
        std::string get_Charger_Status3_str(uint8_t stat3) const;
        std::string get_Fault_Status_str(uint8_t fault) const;
        std::string get_Charger_Flag1_str(uint8_t flag1) const;
        std::string get_Charger_Flag2_str(uint8_t flag2) const;
        std::string get_Fault_Flag_str(uint8_t fault_flag) const;
        std::string get_Part_Information_str(uint8_t part_info) const;

        std::string get_Charger_Status1_changes_str() const;
        std::string get_Charger_Status2_changes_str() const;
        std::string get_Charger_Status3_changes_str() const;
        std::string get_Fault_Status_changes_str() const;
        std::string get_Charger_Flag1_changes_str() const;
        std::string get_Charger_Flag2_changes_str() const;
        std::string get_Fault_Flag_changes_str() const;

        float get_ADC_IAC_value_amps() const { return ADC_IAC_value_amps_; }
        float get_ADC_IBAT_value_amps() const { return ADC_IBAT_value_amps_; }
        float get_ADC_VAC_value_volts() const { return ADC_VAC_value_volts_; }
        float get_ADC_VBAT_value_volts() const { return ADC_VBAT_value_volts_; }
        float get_ADC_VSYS_value_volts() const { return ADC_VSYS_value_volts_; }
        float get_ADC_TS_value_celsius() const { return ADC_TS_value_celsius_; }

        BQ25820_Charge_Status get_charging_status() const;

        std::string ADC_values_str();
        display_ADC_values();

      private:
        // Private member variables and methods
        uint8_t     i2c_address_;
        std::string device_name_;
        BQ25820_I2C i2c_;
        //-------------------------------------------------------------------------------------
        //---- Status Data --------------------------------------------------------------
        uint8_t status1_current_ {0};
        uint8_t status2_current_ {0};
        uint8_t status3_current_ {0};
        uint8_t fault_status_current_ {0};
        uint8_t flag1_current_ {0};
        uint8_t flag2_current_ {0};
        uint8_t fault_flag_current_ {0};

        uint8_t status1_previous_ {0};
        uint8_t status2_previous_ {0};
        uint8_t status3_previous_ {0};
        uint8_t fault_status_previous_ {0};
        uint8_t flag1_previous_ {0};
        uint8_t flag2_previous_ {0};
        uint8_t fault_flag_previous_ {0};
        //-------------------------------------------------------------------------------------
        uint8_t get_status_bits_changes(uint8_t old_stat1, uint8_t new_stat1) { return old_stat1 ^ new_stat1; }
        elapsedMillis elap_since_status_check;
        const uint32_t elap_since_status_check_interval = 5000; // 5 seconds

        //-------------------------------------------------------------------------------------
        elapsedMillis  elap_since_ADC_read_ {0};
        const uint32_t elap_since_ADC_read_interval_ {5000};                                        // milliseconds between ADC reads
        const uint32_t elap_since_ADC_start_interval_ {elap_since_ADC_read_interval_ - 1500};       // 1.5 seconds before ADC read to start one-shot
        // ADC control
        bool adc_OneShot_in_progress_ {false};
        // // // // // bool adc_mode_is_continuous_ {false};
        bool input_power_good_ {false};
        // ADC values
        float ADC_IAC_value_amps_ {0.0f};
        float ADC_IBAT_value_amps_ {0.0f};
        float ADC_VAC_value_volts_ {0.0f};
        float ADC_VBAT_value_volts_ {0.0f};
        float ADC_VSYS_value_volts_ {0.0f};
        float ADC_TS_value_celsius_ {0.0f};
        float ADC_TS_value_percent_of_REGN_ {0.0f};
        // Private methods
        void start_ADC_Conversion();
        // // bool check_if_ADC_conversion_complete();
        void read_all_ADC_values();
        bool input_power_good();
        // --------------------------------------------------------
        spin_ADC();
        spin_status_change();
    };

}       // namespace BQ25820

#endif       // BQ25820__BQ25820_HPP_
