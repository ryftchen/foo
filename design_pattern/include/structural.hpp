#pragma once

namespace design_pattern::structural
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
} // namespace design_pattern::structural
