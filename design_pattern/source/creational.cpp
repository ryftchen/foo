//! @file creational.cpp
//! @author ryftchen
//! @brief The definitions (creational) in the data structure module.
//! @version 0.1
//! @copyright Copyright (c) 2022-2023
#include "creational.hpp"
#ifndef _NO_PRINT_AT_RUNTIME
#include "utility/include/common.hpp"

//! @brief Display creational result.
#define CREATIONAL_RESULT "\r\n*%-15s instance:\r\n%s"
//! @brief Print creational result content.
#define CREATIONAL_PRINT_RESULT_CONTENT(method)                      \
    COMMON_PRINT(CREATIONAL_RESULT, method, output().str().c_str()); \
    output().str("");                                                \
    output().clear()
#else
#include <memory>
#include <sstream>

//! @brief Print creational result content.
#define CREATIONAL_PRINT_RESULT_CONTENT(method) \
    output().str("");                           \
    output().clear()
#endif

namespace design_pattern::creational
{
CreationalPattern::CreationalPattern()
{
#ifndef _NO_PRINT_AT_RUNTIME
    std::cout << "\r\nInstances of the creational pattern:" << std::endl;
#endif
}

//! @brief The abstract factory pattern.
namespace abstract_factory
{
//! @brief Output stream for the abstract factory pattern. Need to be cleared manually.
//! @return reference of output stream object, which is on string based.
static std::ostringstream& output()
{
    static std::ostringstream stream;
    return stream;
}

//! @brief Implement the same interface. The others can refer to the interface not the concrete product.
class ProductA
{
public:
    //! @brief Destroy the ProductA object.
    virtual ~ProductA() = default;

    //! @brief Get the product name.
    //! @return product name
    virtual std::string getName() = 0;
};

//! @brief The concrete product.
class ConcreteProductAX : public ProductA
{
public:
    //! @brief Destroy the ConcreteProductAX object.
    ~ConcreteProductAX() override = default;

    //! @brief Get the product name.
    //! @return product name
    std::string getName() override { return "A-X"; }
};

//! @brief The concrete product.
class ConcreteProductAY : public ProductA
{
public:
    //! @brief Destroy the ConcreteProductAY object.
    ~ConcreteProductAY() override = default;

    //! @brief Get the product name.
    //! @return product name
    std::string getName() override { return "A-Y"; }
};

//! @brief Implement the same interface. The others can refer to the interface not the concrete product.
class ProductB
{
public:
    //! @brief Destroy the ProductB object.
    virtual ~ProductB() = default;

    //! @brief Get the product name.
    //! @return product name
    virtual std::string getName() = 0;
};

//! @brief The concrete product.
class ConcreteProductBX : public ProductB
{
public:
    //! @brief Destroy the ConcreteProductBX object.
    ~ConcreteProductBX() override = default;

    //! @brief Get the product name.
    //! @return product name
    std::string getName() override { return "B-X"; }
};

//! @brief The concrete product.
class ConcreteProductBY : public ProductB
{
public:
    //! @brief Destroy the ConcreteProductBY object.
    ~ConcreteProductBY() override = default;

    //! @brief Get the product name.
    //! @return product name
    std::string getName() override { return "B-Y"; }
};

//! @brief Provide the abstract interface for creating the family of products.
//!        It never has to instantiate a product object
class AbstractFactory
{
public:
    //! @brief Destroy the AbstractFactory object.
    virtual ~AbstractFactory() = default;

    //! @brief Create product A.
    //! @return product A
    virtual std::unique_ptr<ProductA> createProductA() = 0;
    //! @brief Create product B.
    //! @return product B
    virtual std::unique_ptr<ProductB> createProductB() = 0;
};

//! @brief The concrete factory.
class ConcreteFactoryX : public AbstractFactory
{
public:
    //! @brief Destroy the ConcreteFactoryX object.
    ~ConcreteFactoryX() override = default;

    //! @brief Create product A.
    //! @return product A
    std::unique_ptr<ProductA> createProductA() override { return std::make_unique<ConcreteProductAX>(); }
    //! @brief Create product B.
    //! @return product B
    std::unique_ptr<ProductB> createProductB() override { return std::make_unique<ConcreteProductBX>(); }
};

//! @brief The concrete factory.
class ConcreteFactoryY : public AbstractFactory
{
public:
    //! @brief Destroy the ConcreteFactoryY object.
    ~ConcreteFactoryY() override = default;

    //! @brief Create product A.
    //! @return product A
    std::unique_ptr<ProductA> createProductA() override { return std::make_unique<ConcreteProductAY>(); }
    //! @brief Create product B.
    //! @return product B
    std::unique_ptr<ProductB> createProductB() override { return std::make_unique<ConcreteProductBY>(); }
};
} // namespace abstract_factory

void CreationalPattern::abstractFactoryInstance() const // NOLINT(readability-convert-member-functions-to-static)
{
    using abstract_factory::ConcreteFactoryX;
    using abstract_factory::ConcreteFactoryY;
    using abstract_factory::output;
    using abstract_factory::ProductA;
    using abstract_factory::ProductB;

    std::shared_ptr<ConcreteFactoryX> factoryX = std::make_shared<ConcreteFactoryX>();
    std::shared_ptr<ConcreteFactoryY> factoryY = std::make_shared<ConcreteFactoryY>();

    std::unique_ptr<ProductA> p1 = factoryX->createProductA();
    output() << "product: " << p1->getName() << std::endl;

    std::unique_ptr<ProductA> p2 = factoryY->createProductA();
    output() << "product: " << p2->getName() << std::endl;

    std::unique_ptr<ProductB> p3 = factoryX->createProductB();
    output() << "product: " << p3->getName() << std::endl;

    std::unique_ptr<ProductB> p4 = factoryY->createProductB();
    output() << "product: " << p4->getName() << std::endl;

    CREATIONAL_PRINT_RESULT_CONTENT("AbstractFactory");
}

//! @brief The builder pattern.
namespace builder
{
//! @brief Output stream for the builder pattern. Need to be cleared manually.
//! @return reference of output stream object, which is on string based.
static std::ostringstream& output()
{
    static std::ostringstream stream;
    return stream;
}

//! @brief Product properties.
class Product
{
public:
    //! @brief Make part A.
    //! @param part - target part
    void makeA(const std::string& part) { partA = part; }
    //! @brief Make part B.
    //! @param part - target part
    void makeB(const std::string& part) { partB = part; }
    //! @brief Make part C.
    //! @param part - target part
    void makeC(const std::string& part) { partC = part; }
    //! @brief Get all parts.
    //! @return all parts
    std::string get() { return (partA + " " + partB + " " + partC); }

private:
    //! @brief Part A.
    std::string partA;
    //! @brief Part B.
    std::string partB;
    //! @brief Part C.
    std::string partC;
};

//! @brief The builder for creating products.
class Builder
{
public:
    //! @brief Destroy the Builder object.
    virtual ~Builder() = default;

    //! @brief Get the product.
    //! @return product
    Product get() { return product; }
    //! @brief Build part A.
    virtual void buildPartA() = 0;
    //! @brief Build part B.
    virtual void buildPartB() = 0;
    //! @brief Build part C.
    virtual void buildPartC() = 0;

protected:
    //! @brief The product.
    Product product;
};

//! @brief The concrete builder.
class ConcreteBuilderX : public Builder
{
public:
    //! @brief Build part A.
    void buildPartA() override { product.makeA("A-X"); }
    //! @brief Build part B.
    void buildPartB() override { product.makeB("B-X"); }
    //! @brief Build part C.
    void buildPartC() override { product.makeC("C-X"); }
};

//! @brief The concrete builder.
class ConcreteBuilderY : public Builder
{
public:
    //! @brief Build part A.
    void buildPartA() override { product.makeA("A-Y"); }
    //! @brief Build part B.
    void buildPartB() override { product.makeB("B-Y"); }
    //! @brief Build part C.
    void buildPartC() override { product.makeC("C-Y"); }
};

//! @brief Manage the correct sequence of creation.
class Director
{
public:
    //! @brief Construct a new Director object.
    Director() : builder(){};
    //! @brief Destroy the Director object.
    ~Director()
    {
        if (builder)
        {
            builder.reset();
        }
    }

    //! @brief Set the builder.
    //! @param b - target builder.
    void set(std::unique_ptr<Builder> b)
    {
        if (builder)
        {
            builder.reset();
        }
        builder = std::move(b);
    }
    //! @brief Get the product from the builder.
    //! @return product
    Product get() { return builder->get(); }
    //! @brief Construct products by the builder.
    void construct()
    {
        builder->buildPartA();
        builder->buildPartB();
        builder->buildPartC();
    }

private:
    //! @brief The builder.
    std::unique_ptr<Builder> builder;
};
} // namespace builder

void CreationalPattern::builderInstance() const // NOLINT(readability-convert-member-functions-to-static)
{
    using builder::ConcreteBuilderX;
    using builder::ConcreteBuilderY;
    using builder::Director;
    using builder::output;
    using builder::Product;

    Director director;

    director.set(std::make_unique<ConcreteBuilderX>());
    director.construct();
    Product product1 = director.get();
    output() << "1st product parts: " << product1.get() << std::endl;

    director.set(std::make_unique<ConcreteBuilderY>());
    director.construct();
    Product product2 = director.get();
    output() << "2nd product parts: " << product2.get() << std::endl;

    CREATIONAL_PRINT_RESULT_CONTENT("Builder");
}

//! @brief The factory method pattern.
namespace factory_method
{
//! @brief Output stream for the factory method pattern. Need to be cleared manually.
//! @return reference of output stream object, which is on string based.
static std::ostringstream& output()
{
    static std::ostringstream stream;
    return stream;
}

//! @brief Implement the same interface. The others can refer to the interface not the concrete product.
class Product
{
public:
    //! @brief Destroy the Product object.
    virtual ~Product() = default;

    //! @brief Get the product name.
    //! @return product name
    virtual std::string getName() = 0;
};

//! @brief The concrete product.
class ConcreteProductA : public Product
{
public:
    //! @brief Destroy the ConcreteProductA object.
    ~ConcreteProductA() override = default;

    //! @brief Get the product name.
    //! @return product name
    std::string getName() override { return "type A"; }
};

//! @brief The concrete product.
class ConcreteProductB : public Product
{
public:
    //! @brief Destroy the ConcreteProductB object.
    ~ConcreteProductB() override = default;

    //! @brief Get the product name.
    //! @return product name
    std::string getName() override { return "type B"; }
};

//! @brief Contain the implementation for all methods to manipulate products except for the factory method.
//!        Know how to create the products.
class Creator
{
public:
    //! @brief Destroy the Creator object.
    virtual ~Creator() = default;

    //! @brief Create product A.
    //! @return product A
    virtual std::unique_ptr<Product> createProductA() = 0;
    //! @brief Create product B.
    //! @return product B
    virtual std::unique_ptr<Product> createProductB() = 0;
    //! @brief Remove product.
    virtual void removeProduct(std::unique_ptr<Product>& product) = 0;
};

//! @brief The concrete creator.
class ConcreteCreator : public Creator
{
public:
    //! @brief Destroy the ConcreteCreator object.
    ~ConcreteCreator() override = default;

    //! @brief Create product A.
    //! @return product A
    std::unique_ptr<Product> createProductA() override { return std::make_unique<ConcreteProductA>(); }
    //! @brief Create product B.
    //! @return product B
    std::unique_ptr<Product> createProductB() override { return std::make_unique<ConcreteProductB>(); }
    //! @brief Remove product.
    void removeProduct(std::unique_ptr<Product>& product) override { product.reset(); }
};
} // namespace factory_method

void CreationalPattern::factoryMethodInstance() const // NOLINT(readability-convert-member-functions-to-static)
{
    using factory_method::ConcreteCreator;
    using factory_method::Creator;
    using factory_method::output;
    using factory_method::Product;

    std::shared_ptr<Creator> creator = std::make_shared<ConcreteCreator>();

    std::unique_ptr<Product> p1 = creator->createProductA();
    output() << "product: " << p1->getName() << std::endl;
    creator->removeProduct(p1);

    std::unique_ptr<Product> p2 = creator->createProductB();
    output() << "product: " << p2->getName() << std::endl;
    creator->removeProduct(p2);

    CREATIONAL_PRINT_RESULT_CONTENT("FactoryMethod");
}

//! @brief The prototype pattern.
namespace prototype
{
//! @brief Output stream for the prototype pattern. Need to be cleared manually.
//! @return reference of output stream object, which is on string based.
static std::ostringstream& output()
{
    static std::ostringstream stream;
    return stream;
}

//! @brief The interface for cloning itself.
class Prototype
{
public:
    //! @brief Destroy the Prototype object.
    virtual ~Prototype() = default;

    //! @brief Clone self.
    //! @return cloning of self
    virtual std::unique_ptr<Prototype> clone() = 0;
    //! @brief Get the type.
    //! @return type
    virtual std::string type() = 0;
};

//! @brief The concrete prototype.
class ConcretePrototypeA : public Prototype
{
public:
    //! @brief Destroy the ConcretePrototypeA object.
    ~ConcretePrototypeA() override = default;

    //! @brief Clone self.
    //! @return cloning of self
    std::unique_ptr<Prototype> clone() override { return std::make_unique<ConcretePrototypeA>(); }
    //! @brief Get the type.
    //! @return type
    std::string type() override { return "type A"; }
};

//! @brief The concrete prototype.
class ConcretePrototypeB : public Prototype
{
public:
    //! @brief Destroy the ConcretePrototypeB object.
    ~ConcretePrototypeB() override = default;

    //! @brief Clone self.
    //! @return cloning of self
    std::unique_ptr<Prototype> clone() override { return std::make_unique<ConcretePrototypeB>(); }
    //! @brief Get the type.
    //! @return type
    std::string type() override { return "type B"; }
};

//! @brief Create by asking the prototype to clone itself.
class Client
{
public:
    //! @brief Initialize all prototypes.
    static void init()
    {
        types[0] = std::make_unique<ConcretePrototypeA>();
        types[1] = std::make_unique<ConcretePrototypeB>();
    }
    //! @brief Remove all prototypes.
    static void remove()
    {
        types[0].reset();
        types[1].reset();
    }
    //! @brief Make by prototype index
    //! @param index - prototype index
    //! @return cloning result
    static std::unique_ptr<Prototype> make(const int index)
    {
        if (index >= nTypes)
        {
            return nullptr;
        }
        return types[index]->clone();
    }

private:
    //! @brief Collection of prototypes.
    static std::unique_ptr<Prototype> types[2];
    //! @brief The number of types.
    static int nTypes;
};

std::unique_ptr<Prototype> Client::types[2];
int Client::nTypes = 2;
} // namespace prototype

void CreationalPattern::prototypeInstance() const // NOLINT(readability-convert-member-functions-to-static)
{
    using prototype::Client;
    using prototype::output;
    using prototype::Prototype;

    Client::init();

    std::unique_ptr<Prototype> prototype1 = Client::make(0);
    output() << "prototype: " << prototype1->type() << std::endl;

    std::unique_ptr<Prototype> prototype2 = Client::make(1);
    output() << "prototype: " << prototype2->type() << std::endl;

    Client::remove();

    CREATIONAL_PRINT_RESULT_CONTENT("Prototype");
}

//! @brief The singleton factory pattern.
namespace singleton
{
//! @brief Output stream for the singleton pattern. Need to be cleared manually.
//! @return reference of output stream object, which is on string based.
static std::ostringstream& output()
{
    static std::ostringstream stream;
    return stream;
}

//! @brief Have a private static variable to hold one instance of the singleton.
//!        It gives a way to instantiate the singleton.
class Singleton
{
public:
    //! @brief Construct a new Singleton object.
    Singleton(Singleton const&) = delete;
    //! @brief The operator (=) overloading of Singleton class.
    //! @return reference of Singleton object
    Singleton& operator=(Singleton const&) = delete;

    //! @brief Get the instance of the singleton.
    //! @return the instance of the singleton
    static std::shared_ptr<Singleton> get()
    {
        if (!instance)
        {
            instance = std::shared_ptr<Singleton>(new Singleton);
        }
        return instance;
    }
    //! @brief Restart the instance of the singleton.
    static void restart()
    {
        if (instance)
        {
            instance.reset();
        }
    }
    //! @brief Tell external.
    static void tell() { output() << "this is singleton" << std::endl; }

private:
    //! @brief Construct a new Singleton object.
    Singleton() = default;

    //! @brief The instance of the singleton.
    static std::shared_ptr<Singleton> instance;
};

std::shared_ptr<Singleton> Singleton::instance = nullptr;
} // namespace singleton

void CreationalPattern::singletonInstance() const // NOLINT(readability-convert-member-functions-to-static)
{
    using singleton::output;
    using singleton::Singleton;

    Singleton::get()->tell();
    Singleton::restart();

    CREATIONAL_PRINT_RESULT_CONTENT("Singleton");
}
} // namespace design_pattern::creational
