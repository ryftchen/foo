#pragma once

namespace dp_structural
{
class Structural
{
public:
    Structural();
    virtual ~Structural() = default;

    void adapterInstance() const;
    void bridgeInstance() const;
    void compositeInstance() const;
    void decoratorInstance() const;
    void facadeInstance() const;
    void flyweightInstance() const;
    void proxyInstance() const;
};
} // namespace dp_structural
