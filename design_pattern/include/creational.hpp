#pragma once

namespace design_pattern::creational
{
class CreationalPattern
{
public:
    CreationalPattern();
    virtual ~CreationalPattern() = default;

    void abstractFactoryInstance() const;
    void builderInstance() const;
    void factoryMethodInstance() const;
    void prototypeInstance() const;
    void singletonInstance() const;
};
} // namespace design_pattern::creational
