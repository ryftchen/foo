//! @file creational.cpp
//! @author ryftchen
//! @brief The definitions (creational) in the data structure module.
//! @version 0.1.0
//! @copyright Copyright (c) 2022-2025 ryftchen. All rights reserved.

#include "creational.hpp"

namespace design_pattern::creational
{
//! @brief Function version number.
//! @return version number (major.minor.patch)
const char* version() noexcept
{
    static const char* const ver = "0.1.0";
    return ver;
}

namespace abstract_factory
{
std::string ConcreteProductAX::getName() const
{
    return "A-X";
}

std::string ConcreteProductAY::getName() const
{
    return "A-Y";
}

std::string ConcreteProductBX::getName() const
{
    return "B-X";
}

std::string ConcreteProductBY::getName() const
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
//! @return reference of the output stream object, which is on string based
std::ostringstream& output() noexcept
{
    static std::ostringstream process{};
    return process;
}
} // namespace abstract_factory

namespace builder
{
void Product::makeA(const std::string_view part)
{
    partA = part;
}

void Product::makeB(const std::string_view part)
{
    partB = part;
}

void Product::makeC(const std::string_view part)
{
    partC = part;
}

std::string Product::get() const
{
    return partA + ' ' + partB + ' ' + partC;
}

Product Builder::get() const
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

void Director::set(std::unique_ptr<Builder> b)
{
    if (builder)
    {
        builder.reset();
    }
    builder = std::move(b);
}

Product Director::get() const
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
//! @return reference of the output stream object, which is on string based
std::ostringstream& output() noexcept
{
    static std::ostringstream process{};
    return process;
}
} // namespace builder

namespace factory_method
{
std::string ConcreteProductA::getName() const
{
    return "type A";
}

std::string ConcreteProductB::getName() const
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
//! @return reference of the output stream object, which is on string based
std::ostringstream& output() noexcept
{
    static std::ostringstream process{};
    return process;
}
} // namespace factory_method

namespace prototype
{
std::unique_ptr<Prototype> Client::types[2] = {};
const int Client::nTypes = 2;

std::unique_ptr<Prototype> ConcretePrototypeA::clone()
{
    return std::make_unique<ConcretePrototypeA>();
}

std::string ConcretePrototypeA::type() const
{
    return "type A";
}

std::unique_ptr<Prototype> ConcretePrototypeB::clone()
{
    return std::make_unique<ConcretePrototypeB>();
}

std::string ConcretePrototypeB::type() const
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
    return (index >= nTypes) ? nullptr : types[index]->clone();
}

//! @brief Output stream for the prototype pattern. Need to be cleared manually.
//! @return reference of the output stream object, which is on string based
std::ostringstream& output() noexcept
{
    static std::ostringstream process{};
    return process;
}
} // namespace prototype

namespace singleton
{
std::shared_ptr<Singleton> Singleton::instance = {};

std::shared_ptr<Singleton>& Singleton::get()
{
    if (!instance)
    {
        instance = std::shared_ptr<Singleton>();
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
//! @return reference of the output stream object, which is on string based
std::ostringstream& output() noexcept
{
    static std::ostringstream process{};
    return process;
}
} // namespace singleton
} // namespace design_pattern::creational
