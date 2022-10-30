#include "creational.hpp"
#include "utility/include/common.hpp"

#define CREATIONAL_RESULT "\r\n*%-15s instance:\r\n%s"

namespace dp_creational
{
Creational::Creational()
{
    std::cout << "\r\nCreational pattern:" << std::endl;
}

// Abstract Factory
namespace abstract_factory
{
static std::ostringstream& output()
{
    static std::ostringstream stream;
    return stream;
}

class ProductA
{
public:
    virtual ~ProductA() = default;

    virtual std::string getName() = 0;
};

class ConcreteProductAX : public ProductA
{
public:
    ~ConcreteProductAX() override = default;

    std::string getName() override { return "A-X"; }
};

class ConcreteProductAY : public ProductA
{
public:
    ~ConcreteProductAY() override = default;

    std::string getName() override { return "A-Y"; }
};

class ProductB
{
public:
    virtual ~ProductB() = default;

    virtual std::string getName() = 0;
};

class ConcreteProductBX : public ProductB
{
public:
    ~ConcreteProductBX() override = default;

    std::string getName() override { return "B-X"; }
};

class ConcreteProductBY : public ProductB
{
public:
    ~ConcreteProductBY() override = default;

    std::string getName() override { return "B-Y"; }
};

class AbstractFactory
{
public:
    virtual ~AbstractFactory() = default;

    virtual std::unique_ptr<ProductA> createProductA() = 0;
    virtual std::unique_ptr<ProductB> createProductB() = 0;
};

class ConcreteFactoryX : public AbstractFactory
{
public:
    ~ConcreteFactoryX() override = default;

    std::unique_ptr<ProductA> createProductA() override { return std::make_unique<ConcreteProductAX>(); }
    std::unique_ptr<ProductB> createProductB() override { return std::make_unique<ConcreteProductBX>(); }
};

class ConcreteFactoryY : public AbstractFactory
{
public:
    ~ConcreteFactoryY() override = default;

    std::unique_ptr<ProductA> createProductA() override { return std::make_unique<ConcreteProductAY>(); }
    std::unique_ptr<ProductB> createProductB() override { return std::make_unique<ConcreteProductBY>(); }
};
} // namespace abstract_factory

void Creational::abstractFactoryInstance()
{
    using abstract_factory::ConcreteFactoryX;
    using abstract_factory::ConcreteFactoryY;
    using abstract_factory::ProductA;
    using abstract_factory::ProductB;

    std::shared_ptr<ConcreteFactoryX> factoryX = std::make_shared<ConcreteFactoryX>();
    std::shared_ptr<ConcreteFactoryY> factoryY = std::make_shared<ConcreteFactoryY>();

    std::unique_ptr<ProductA> p1 = factoryX->createProductA();
    abstract_factory::output() << "product: " << p1->getName() << std::endl;

    std::unique_ptr<ProductA> p2 = factoryY->createProductA();
    abstract_factory::output() << "product: " << p2->getName() << std::endl;

    std::unique_ptr<ProductB> p3 = factoryX->createProductB();
    abstract_factory::output() << "product: " << p3->getName() << std::endl;

    std::unique_ptr<ProductB> p4 = factoryY->createProductB();
    abstract_factory::output() << "product: " << p4->getName() << std::endl;

    COMMON_PRINT(CREATIONAL_RESULT, "AbstractFactory", abstract_factory::output().str().c_str());
}

// Builder
namespace builder
{
static std::ostringstream& output()
{
    static std::ostringstream stream;
    return stream;
}

class Product
{
public:
    void makeA(const std::string& part) { partA = part; }
    void makeB(const std::string& part) { partB = part; }
    void makeC(const std::string& part) { partC = part; }
    std::string get() { return (partA + " " + partB + " " + partC); }

private:
    std::string partA;
    std::string partB;
    std::string partC;
};

class Builder
{
public:
    virtual ~Builder() = default;

    Product get() { return product; }
    virtual void buildPartA() = 0;
    virtual void buildPartB() = 0;
    virtual void buildPartC() = 0;

protected:
    Product product;
};

class ConcreteBuilderX : public Builder
{
public:
    void buildPartA() override { product.makeA("A-X"); }
    void buildPartB() override { product.makeB("B-X"); }
    void buildPartC() override { product.makeC("C-X"); }
};

class ConcreteBuilderY : public Builder
{
public:
    void buildPartA() override { product.makeA("A-Y"); }
    void buildPartB() override { product.makeB("B-Y"); }
    void buildPartC() override { product.makeC("C-Y"); }
};

class Director
{
public:
    Director() : builder(){};
    ~Director()
    {
        if (builder)
        {
            builder.reset();
        }
    }

    void set(std::unique_ptr<Builder> b)
    {
        if (builder)
        {
            builder.reset();
        }
        builder = std::move(b);
    }
    Product get() { return builder->get(); }
    void construct()
    {
        builder->buildPartA();
        builder->buildPartB();
        builder->buildPartC();
    }

private:
    std::unique_ptr<Builder> builder;
};
} // namespace builder

void Creational::builderInstance()
{
    using builder::ConcreteBuilderX;
    using builder::ConcreteBuilderY;
    using builder::Director;
    using builder::Product;

    Director director;

    director.set(std::make_unique<ConcreteBuilderX>());
    director.construct();
    Product product1 = director.get();
    builder::output() << "1st product parts: " << product1.get() << std::endl;

    director.set(std::make_unique<ConcreteBuilderY>());
    director.construct();
    Product product2 = director.get();
    builder::output() << "2nd product parts: " << product2.get() << std::endl;

    COMMON_PRINT(CREATIONAL_RESULT, "Builder", builder::output().str().c_str());
}

// Factory Method
namespace factory_method
{
static std::ostringstream& output()
{
    static std::ostringstream stream;
    return stream;
}

class Product
{
public:
    virtual ~Product() = default;

    virtual std::string getName() = 0;
};

class ConcreteProductA : public Product
{
public:
    ~ConcreteProductA() override = default;

    std::string getName() override { return "type A"; }
};

class ConcreteProductB : public Product
{
public:
    ~ConcreteProductB() override = default;

    std::string getName() override { return "type B"; }
};

class Creator
{
public:
    virtual ~Creator() = default;

    virtual std::unique_ptr<Product> createProductA() = 0;
    virtual std::unique_ptr<Product> createProductB() = 0;
    virtual void removeProduct(std::unique_ptr<Product>& product) = 0;
};

class ConcreteCreator : public Creator
{
public:
    ~ConcreteCreator() override = default;

    std::unique_ptr<Product> createProductA() override { return std::make_unique<ConcreteProductA>(); }
    std::unique_ptr<Product> createProductB() override { return std::make_unique<ConcreteProductB>(); }
    void removeProduct(std::unique_ptr<Product>& product) override { product.reset(); }
};
} // namespace factory_method

void Creational::factoryMethodInstance()
{
    using factory_method::ConcreteCreator;
    using factory_method::Creator;
    using factory_method::Product;

    std::shared_ptr<Creator> creator = std::make_shared<ConcreteCreator>();

    std::unique_ptr<Product> p1 = creator->createProductA();
    factory_method::output() << "product: " << p1->getName() << std::endl;
    creator->removeProduct(p1);

    std::unique_ptr<Product> p2 = creator->createProductB();
    factory_method::output() << "product: " << p2->getName() << std::endl;
    creator->removeProduct(p2);

    COMMON_PRINT(CREATIONAL_RESULT, "FactoryMethod", factory_method::output().str().c_str());
}

// Prototype
namespace prototype
{
static std::ostringstream& output()
{
    static std::ostringstream stream;
    return stream;
}

class Prototype
{
public:
    virtual ~Prototype() = default;

    virtual std::unique_ptr<Prototype> clone() = 0;
    virtual std::string type() = 0;
};

class ConcretePrototypeA : public Prototype
{
public:
    ~ConcretePrototypeA() override = default;

    std::unique_ptr<Prototype> clone() override { return std::make_unique<ConcretePrototypeA>(); }
    std::string type() override { return "type A"; }
};

class ConcretePrototypeB : public Prototype
{
public:
    ~ConcretePrototypeB() override = default;

    std::unique_ptr<Prototype> clone() override { return std::make_unique<ConcretePrototypeB>(); }
    std::string type() override { return "type B"; }
};

class Client
{
public:
    static void init()
    {
        types[0] = std::make_unique<ConcretePrototypeA>();
        types[1] = std::make_unique<ConcretePrototypeB>();
    }
    static void remove()
    {
        types[0].reset();
        types[1].reset();
    }
    static std::unique_ptr<Prototype> make(const int index)
    {
        if (index >= nTypes)
        {
            return nullptr;
        }
        return types[index]->clone();
    }

private:
    static std::unique_ptr<Prototype> types[2];
    static int nTypes;
};

std::unique_ptr<Prototype> Client::types[2];
int Client::nTypes = 2;
} // namespace prototype

void Creational::prototypeInstance()
{
    using prototype::Client;
    using prototype::Prototype;

    Client::init();

    std::unique_ptr<Prototype> prototype1 = Client::make(0);
    prototype::output() << "prototype: " << prototype1->type() << std::endl;

    std::unique_ptr<Prototype> prototype2 = Client::make(1);
    prototype::output() << "prototype: " << prototype2->type() << std::endl;

    Client::remove();

    COMMON_PRINT(CREATIONAL_RESULT, "Prototype", prototype::output().str().c_str());
}

// Singleton
namespace singleton
{
static std::ostringstream& output()
{
    static std::ostringstream stream;
    return stream;
}

class Singleton
{
public:
    Singleton(Singleton const&) = delete;
    Singleton& operator=(Singleton const&) = delete;

    static std::shared_ptr<Singleton> get()
    {
        if (!instance)
        {
            instance = std::shared_ptr<Singleton>(new Singleton);
        }
        return instance;
    }
    static void restart()
    {
        if (instance)
        {
            instance.reset();
        }
    }
    static void tell() { output() << "this is singleton" << std::endl; }

private:
    Singleton() = default;

    static std::shared_ptr<Singleton> instance;
};

std::shared_ptr<Singleton> Singleton::instance = nullptr;
} // namespace singleton

void Creational::singletonInstance()
{
    using singleton::Singleton;

    Singleton::get()->tell();
    Singleton::restart();

    COMMON_PRINT(CREATIONAL_RESULT, "Singleton", singleton::output().str().c_str());
}
} // namespace dp_creational
