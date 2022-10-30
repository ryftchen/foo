#pragma once

namespace dp_creational
{
class Creational
{
public:
    Creational();
    virtual ~Creational() = default;

    void abstractFactoryInstance() const;
    void builderInstance() const;
    void factoryMethodInstance() const;
    void prototypeInstance() const;
    void singletonInstance() const;
};
} // namespace dp_creational
