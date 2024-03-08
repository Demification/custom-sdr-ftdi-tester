#pragma once
#include <memory>

enum AccessorType {
    LMK4828,
    AFE7769,
    FPGA
};

class AbstractAccessor
{
public:
    using Ptr = std::shared_ptr<AbstractAccessor>;

    virtual ~AbstractAccessor() = default;
    virtual AccessorType type() const = 0;

    virtual void init() = 0;
 };