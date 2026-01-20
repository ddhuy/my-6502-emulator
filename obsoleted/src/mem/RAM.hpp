#pragma once
#include <vector>
#include "Memory.hpp"


class RAM : public Memory
{
    public:
        explicit RAM(std::size_t size = 64 * 1024); // Default to 64KB

        virtual uint8_t read(uint16_t address) const override;
        virtual void write(uint16_t address, uint8_t value) override;

    private:
        std::vector<uint8_t> _data;
};