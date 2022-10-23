#pragma once

namespace dp_behavioral
{
class Behavioral
{
public:
    Behavioral();
    virtual ~Behavioral() = default;

    static void chainOfResponsibilityInstance();
    static void commandInstance();
    static void interpreterInstance();
    static void iteratorInstance();
    static void mediatorInstance();
    static void mementoInstance();
    static void observerInstance();
    static void stateInstance();
    static void strategyInstance();
    static void templateMethodInstance();
    static void visitorInstance();
};
} // namespace dp_behavioral
