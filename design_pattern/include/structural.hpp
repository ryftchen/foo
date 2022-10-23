#pragma once

namespace dp_structural
{
class Structural
{
public:
    Structural();
    virtual ~Structural() = default;

    static void adapterInstance();
    static void bridgeInstance();
    static void compositeInstance();
    static void decoratorInstance();
    static void facadeInstance();
    static void flyweightInstance();
    static void proxyInstance();
};
} // namespace dp_structural
