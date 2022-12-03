#pragma once

namespace design_pattern::behavioral
{
class BehavioralPattern
{
public:
    BehavioralPattern();
    virtual ~BehavioralPattern() = default;

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
} // namespace design_pattern::behavioral
