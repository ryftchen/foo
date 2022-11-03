#pragma once

namespace dp_structural
{
class StructuralPattern
{
public:
    StructuralPattern();
    virtual ~StructuralPattern() = default;

    void adapterInstance() const;
    void bridgeInstance() const;
    void compositeInstance() const;
    void decoratorInstance() const;
    void facadeInstance() const;
    void flyweightInstance() const;
    void proxyInstance() const;
};
} // namespace dp_structural
