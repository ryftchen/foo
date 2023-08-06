//! @file creational.cpp
//! @author ryftchen
//! @brief The definitions (creational) in the data structure module.
//! @version 1.0
//! @copyright Copyright (c) 2022-2023 ryftchen. All rights reserved.

#include "creational.hpp"

namespace design_pattern::creational
{
//! @brief The abstract factory pattern.
namespace abstract_factory
{
std::string ConcreteProductAX::getName()
{
    return "A-X";
}

std::string ConcreteProductAY::getName()
{
    return "A-Y";
}

std::string ConcreteProductBX::getName()
{
    return "B-X";
}

std::string ConcreteProductBY::getName()
{
    return "B-Y";
}

std::unique_ptr<ProductA> ConcreteFactoryX::createProductA()
{
    return std::make_unique<ConcreteProductAX>();
}

std::unique_ptr<ProductB> ConcreteFactoryX::createProductB()
{
    return std::make_unique<ConcreteProductBX>();
}

std::unique_ptr<ProductA> ConcreteFactoryY::createProductA()
{
    return std::make_unique<ConcreteProductAY>();
}

std::unique_ptr<ProductB> ConcreteFactoryY::createProductB()
{
    return std::make_unique<ConcreteProductBY>();
}

//! @brief Output stream for the abstract factory pattern. Need to be cleared manually.
//! @return reference of output stream object, which is on string based
std::ostringstream& output()
{
    static std::ostringstream stream;
    return stream;
}
} // namespace abstract_factory

//! @brief The builder pattern.
namespace builder
{
void Product::makeA(const std::string& part)
{
    partA = part;
}

void Product::makeB(const std::string& part)
{
    partB = part;
}

void Product::makeC(const std::string& part)
{
    partC = part;
}

std::string Product::get()
{
    return (partA + ' ' + partB + ' ' + partC);
}

Product Builder::get()
{
    return product;
}

void ConcreteBuilderX::buildPartA()
{
    product.makeA("A-X");
}

void ConcreteBuilderX::buildPartB()
{
    product.makeB("B-X");
}

void ConcreteBuilderX::buildPartC()
{
    product.makeC("C-X");
}

void ConcreteBuilderY::buildPartA()
{
    product.makeA("A-Y");
}

void ConcreteBuilderY::buildPartB()
{
    product.makeB("B-Y");
}

void ConcreteBuilderY::buildPartC()
{
    product.makeC("C-Y");
}

Director::~Director()
{
    if (builder)
    {
        builder.reset();
    }
}

void Director::set(std::unique_ptr<Builder> b)
{
    if (builder)
    {
        builder.reset();
    }
    builder = std::move(b);
}

Product Director::get()
{
    return builder->get();
}

void Director::construct()
{
    builder->buildPartA();
    builder->buildPartB();
    builder->buildPartC();
}

//! @brief Output stream for the builder pattern. Need to be cleared manually.
//! @return reference of output stream object, which is on string based
std::ostringstream& output()
{
    static std::ostringstream stream;
    return stream;
}
} // namespace builder

//! @brief The factory method pattern.
namespace factory_method
{
std::string ConcreteProductA::getName()
{
    return "type A";
}

std::string ConcreteProductB::getName()
{
    return "type B";
}

std::unique_ptr<Product> ConcreteCreator::createProductA()
{
    return std::make_unique<ConcreteProductA>();
}

std::unique_ptr<Product> ConcreteCreator::createProductB()
{
    return std::make_unique<ConcreteProductB>();
}

void ConcreteCreator::removeProduct(std::unique_ptr<Product>& product)
{
    product.reset();
}

//! @brief Output stream for the factory method pattern. Need to be cleared manually.
//! @return reference of output stream object, which is on string based
std::ostringstream& output()
{
    static std::ostringstream stream;
    return stream;
}
} // namespace factory_method

//! @brief The prototype pattern.
namespace prototype
{
std::unique_ptr<Prototype> Client::types[2];
int Client::nTypes = 2;

std::unique_ptr<Prototype> ConcretePrototypeA::clone()
{
    return std::make_unique<ConcretePrototypeA>();
}

std::string ConcretePrototypeA::type()
{
    return "type A";
}

std::unique_ptr<Prototype> ConcretePrototypeB::clone()
{
    return std::make_unique<ConcretePrototypeB>();
}

std::string ConcretePrototypeB::type()
{
    return "type B";
}

void Client::init()
{
    types[0] = std::make_unique<ConcretePrototypeA>();
    types[1] = std::make_unique<ConcretePrototypeB>();
}

void Client::remove()
{
    types[0].reset();
    types[1].reset();
}

std::unique_ptr<Prototype> Client::make(const int index)
{
    if (index >= nTypes)
    {
        return nullptr;
    }
    return types[index]->clone();
}

//! @brief Output stream for the prototype pattern. Need to be cleared manually.
//! @return reference of output stream object, which is on string based
std::ostringstream& output()
{
    static std::ostringstream stream;
    return stream;
}
} // namespace prototype

//! @brief The singleton factory pattern.
namespace singleton
{
std::shared_ptr<Singleton> Singleton::instance = nullptr;

std::shared_ptr<Singleton> Singleton::get()
{
    if (!instance)
    {
        instance = std::shared_ptr<Singleton>(new Singleton);
    }
    return instance;
}

void Singleton::restart()
{
    if (instance)
    {
        instance.reset();
    }
}

void Singleton::tell()
{
    output() << "this is singleton\n";
}

//! @brief Output stream for the singleton pattern. Need to be cleared manually.
//! @return reference of output stream object, which is on string based
std::ostringstream& output()
{
    static std::ostringstream stream;
    return stream;
}
} // namespace singleton
} // namespace design_pattern::creational
