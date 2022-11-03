#pragma once

namespace dp_creational
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
} // namespace dp_creational
