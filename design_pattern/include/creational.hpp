#pragma once

namespace dp_creational
{
class Creational
{
public:
    Creational();
    virtual ~Creational() = default;

    static void abstractFactoryInstance();
    static void builderInstance();
    static void factoryMethodInstance();
    static void prototypeInstance();
    static void singletonInstance();
};
} // namespace dp_creational
