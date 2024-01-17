#pragma once
#include <memory>

enum AccessorType {
    FPGA,
    AFE7769,
    LMK4828
};

class AbstractAccessor
{
public:
    using Ptr = std::shared_ptr<AbstractAccessor>;

    virtual ~AbstractAccessor() = default;
    virtual AccessorType type() const = 0;
 };