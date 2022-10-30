#pragma once

namespace dp_behavioral
{
class Behavioral
{
public:
    Behavioral();
    virtual ~Behavioral() = default;

    void chainOfResponsibilityInstance() const;
    void commandInstance() const;
    void interpreterInstance() const;
    void iteratorInstance() const;
    void mediatorInstance() const;
    void mementoInstance() const;
    void observerInstance() const;
    void stateInstance() const;
    void strategyInstance() const;
    void templateMethodInstance() const;
    void visitorInstance() const;
};
} // namespace dp_behavioral
