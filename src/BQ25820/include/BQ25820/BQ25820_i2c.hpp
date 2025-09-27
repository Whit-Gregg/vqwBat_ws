#pragma once

#include <cstdint>
#include <fcntl.h>
#include <i2c/smbus.h>
#include <linux/i2c-dev.h>
#include <linux/i2c.h>
#include <stdexcept>
#include <stdio.h>
#include <string>
#include <sys/ioctl.h>
#include <unistd.h>
#include "BQ25820_Registers.hpp"

namespace BQ25820
{

    class BQ25820_I2C
    {
      private:
        std::string i2c_device_name_;
        uint8_t     i2c_address_;
        int         fd_;

        bool read_regs(uint8_t reg, uint8_t *buf, size_t len) const
        {
            if (fd_ < 0) throw std::runtime_error("I2C device not opened");
            if (len == 0) return false;
            unsigned char sbuf[2] = {reg, 0};

            i2c_msg msgs[2];
            msgs[0].addr  = static_cast<uint16_t>(i2c_address_);
            msgs[0].flags = {};
            msgs[0].len   = static_cast<uint16_t>(1);
            msgs[0].buf   = sbuf;

            msgs[1].addr  = static_cast<uint16_t>(i2c_address_);
            msgs[1].flags = I2C_M_RD;
            msgs[1].len   = static_cast<uint16_t>(len);
            msgs[1].buf   = buf;

            i2c_rdwr_ioctl_data wrapper; // = {.msgs = msgs, .nmsgs = 2};
            wrapper.msgs = msgs;
            wrapper.nmsgs = 2;

            if (ioctl(fd_, I2C_RDWR, &wrapper) < 0) return false;
            return (msgs[0].len == 1) && (msgs[1].len == len);
        }

        bool write_regs(uint8_t reg, uint8_t *buf, size_t len) const
        {
            if (fd_ < 0) throw std::runtime_error("I2C device not opened");
            if (len == 0) return false;
            if (len > 32) return false;

            uint8_t data[36];
            data[0] = reg;
            for (size_t i = 0; i < len; i++) data[i + 1] = buf[i];

            i2c_msg msg;
            msg.addr  = static_cast<uint16_t>(i2c_address_);
            msg.flags = {};
            msg.len   = static_cast<uint16_t>(len + 1);
            msg.buf   = data;

            i2c_rdwr_ioctl_data wrapper;
            wrapper.msgs = &msg;
            wrapper.nmsgs = 1;

            if (ioctl(fd_, I2C_RDWR, &wrapper) < 0) return false;
            return (msg.len == len + 1);
        }

      public:
        BQ25820_I2C() : fd_(-1) {}
        BQ25820_I2C(const std::string &i2c_device_name, uint8_t address) : i2c_device_name_(i2c_device_name), i2c_address_(address), fd_(-1) {}

        ~BQ25820_I2C() { close_device(); }

        void set_i2c_address(uint8_t address) { i2c_address_ = address; }
        void set_i2c_device_name(const std::string &device_name) { close_device(); i2c_device_name_ = device_name; }

        bool open_device()
        {
            if (fd_ >= 0) close_device();
            fd_ = open(i2c_device_name_.c_str(), O_RDWR);
            return (fd_ >= 0);
            // if (fd_ < 0) throw std::runtime_error("Failed to open I2C device");
            //  if (ioctl(fd_, I2C_SLAVE, i2c_address_) < 0) throw std::runtime_error("Failed to set I2C address");
        }

        void close_device()
        {
            if (fd_ >= 0) close(fd_);
            fd_ = -1;
        }

        uint8_t read_reg8(uint8_t reg) const
        {
            uint8_t val = 0;
            //bool    ok  = read_regs(reg, &val, 1);
            read_regs(reg, &val, 1);
            return val;
        }

        uint8_t read_reg8(BQ25820_Register reg) const
        {
            return read_reg8(static_cast<uint8_t>(reg));
        }

        uint16_t read_reg16(uint8_t reg) const
        {
            union       // BQ25820 16-bit register is little-endian
            {
                uint8_t  bytes[2];
                uint16_t word;
            } U;
            U.word  = 0;
            //bool ok = read_regs(reg, U.bytes, 2);
            read_regs(reg, U.bytes, 2);
            return U.word;
        }
        uint16_t read_reg16(BQ25820_Register reg) const
        {
            return read_reg16(static_cast<uint8_t>(reg));
        }

        bool write_reg8(uint8_t reg, uint8_t val) const { return write_regs(reg, &val, 1); }
        bool write_reg8(BQ25820_Register reg, uint8_t val) const { return write_reg8(static_cast<uint8_t>(reg), val); }

        bool write_reg16(uint8_t reg, uint16_t val) const
        {
            union       // BQ25820 16-bit register is little-endian
            {
                uint8_t  bytes[2];
                uint16_t word;
            } U;
            U.word  = val;
            bool ok = write_regs(reg, U.bytes, 2);
            return ok;
        }
        bool write_reg16(BQ25820_Register reg, uint16_t val) const { return write_reg16(static_cast<uint8_t>(reg), val); }

    };       // class BQ25820_I2C

}       // namespace BQ25820