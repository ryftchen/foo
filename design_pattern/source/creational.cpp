#include "creational.hpp"
#include "utility/include/common.hpp"

#define CREATIONAL_RESULT "\r\n*%-15s instance:\r\n%s"

namespace dp_creational
{
Creational::Creational()
{
    std::cout << "\r\nCreational pattern:" << std::endl;
}

// abstract factory
namespace abstract_factory
{
static std::stringstream& stringstream()
{
    static std::stringstream stream;
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

    virtual std::shared_ptr<ProductA> createProductA() = 0;
    virtual std::shared_ptr<ProductB> createProductB() = 0;
};

class ConcreteFactoryX : public AbstractFactory
{
public:
    ~ConcreteFactoryX() override = default;

    std::shared_ptr<ProductA> createProductA() override { return std::make_shared<ConcreteProductAX>(); }
    std::shared_ptr<ProductB> createProductB() override { return std::make_shared<ConcreteProductBX>(); }
};

class ConcreteFactoryY : public AbstractFactory
{
public:
    ~ConcreteFactoryY() override = default;

    std::shared_ptr<ProductA> createProductA() override { return std::make_shared<ConcreteProductAY>(); }
    std::shared_ptr<ProductB> createProductB() override { return std::make_shared<ConcreteProductBY>(); }
};
} // namespace abstract_factory

void Creational::abstractFactoryInstance()
{
    using abstract_factory::ConcreteFactoryX;
    using abstract_factory::ConcreteFactoryY;
    using abstract_factory::ProductA;
    using abstract_factory::ProductB;
    using abstract_factory::stringstream;

    std::shared_ptr<ConcreteFactoryX> factoryX = std::make_shared<ConcreteFactoryX>();
    std::shared_ptr<ConcreteFactoryY> factoryY = std::make_shared<ConcreteFactoryY>();

    std::shared_ptr<ProductA> p1 = factoryX->createProductA();
    stringstream() << "product: " << p1->getName() << std::endl;

    std::shared_ptr<ProductA> p2 = factoryY->createProductA();
    stringstream() << "product: " << p2->getName() << std::endl;

    std::shared_ptr<ProductB> p3 = factoryX->createProductB();
    stringstream() << "product: " << p3->getName() << std::endl;

    std::shared_ptr<ProductB> p4 = factoryY->createProductB();
    stringstream() << "product: " << p4->getName() << std::endl;

    COMMON_PRINT(CREATIONAL_RESULT, "AbstractFactory", stringstream().str().c_str());
}

// builder
namespace builder
{
static std::stringstream& stringstream()
{
    static std::stringstream stream;
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

    void set(std::shared_ptr<Builder> b)
    {
        if (builder)
        {
            builder.reset();
        }
        builder = b;
    }
    Product get() { return builder->get(); }
    void construct()
    {
        builder->buildPartA();
        builder->buildPartB();
        builder->buildPartC();
    }

private:
    std::shared_ptr<Builder> builder;
};
} // namespace builder

void Creational::builderInstance()
{
    using builder::ConcreteBuilderX;
    using builder::ConcreteBuilderY;
    using builder::Director;
    using builder::Product;
    using builder::stringstream;

    Director director;

    director.set(std::make_shared<ConcreteBuilderX>());
    director.construct();
    Product product1 = director.get();
    stringstream() << "1st product parts: " << product1.get() << std::endl;

    director.set(std::make_shared<ConcreteBuilderY>());
    director.construct();
    Product product2 = director.get();
    stringstream() << "2nd product parts: " << product2.get() << std::endl;

    COMMON_PRINT(CREATIONAL_RESULT, "Builder", stringstream().str().c_str());
}

// factory method
namespace factory_method
{
static std::stringstream& stringstream()
{
    static std::stringstream stream;
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

    virtual std::shared_ptr<Product> createProductA() = 0;
    virtual std::shared_ptr<Product> createProductB() = 0;
    virtual void removeProduct(std::shared_ptr<Product>& product) = 0;
};

class ConcreteCreator : public Creator
{
public:
    ~ConcreteCreator() override = default;

    std::shared_ptr<Product> createProductA() override { return std::make_shared<ConcreteProductA>(); }
    std::shared_ptr<Product> createProductB() override { return std::make_shared<ConcreteProductB>(); }
    void removeProduct(std::shared_ptr<Product>& product) override { product.reset(); }
};
} // namespace factory_method

void Creational::factoryMethodInstance()
{
    using factory_method::ConcreteCreator;
    using factory_method::Creator;
    using factory_method::Product;
    using factory_method::stringstream;

    std::shared_ptr<Creator> creator = std::make_shared<ConcreteCreator>();

    std::shared_ptr<Product> p1 = creator->createProductA();
    stringstream() << "product: " << p1->getName() << std::endl;
    creator->removeProduct(p1);

    std::shared_ptr<Product> p2 = creator->createProductB();
    stringstream() << "product: " << p2->getName() << std::endl;
    creator->removeProduct(p2);

    COMMON_PRINT(CREATIONAL_RESULT, "FactoryMethod", stringstream().str().c_str());
}

// prototype
namespace prototype
{
static std::stringstream& stringstream()
{
    static std::stringstream stream;
    return stream;
}

class Prototype
{
public:
    virtual ~Prototype() = default;

    virtual std::shared_ptr<Prototype> clone() = 0;
    virtual std::string type() = 0;
};

class ConcretePrototypeA : public Prototype
{
public:
    ~ConcretePrototypeA() override = default;

    std::shared_ptr<Prototype> clone() override { return std::make_shared<ConcretePrototypeA>(); }
    std::string type() override { return "type A"; }
};

class ConcretePrototypeB : public Prototype
{
public:
    ~ConcretePrototypeB() override = default;

    std::shared_ptr<Prototype> clone() override { return std::make_shared<ConcretePrototypeB>(); }
    std::string type() override { return "type B"; }
};

class Client
{
public:
    static void init()
    {
        types[0] = std::make_shared<ConcretePrototypeA>();
        types[1] = std::make_shared<ConcretePrototypeB>();
    }
    static void remove()
    {
        types[0].reset();
        types[1].reset();
    }
    static std::shared_ptr<Prototype> make(const int index)
    {
        if (index >= nTypes)
        {
            return nullptr;
        }
        return types[index]->clone();
    }

private:
    static std::shared_ptr<Prototype> types[2];
    static int nTypes;
};

std::shared_ptr<Prototype> Client::types[2];
int Client::nTypes = 2;
} // namespace prototype

void Creational::prototypeInstance()
{
    using prototype::Client;
    using prototype::Prototype;
    using prototype::stringstream;

    Client::init();

    std::shared_ptr<Prototype> prototype1 = Client::make(0);
    stringstream() << "prototype: " << prototype1->type() << std::endl;

    std::shared_ptr<Prototype> prototype2 = Client::make(1);
    stringstream() << "prototype: " << prototype2->type() << std::endl;

    Client::remove();

    COMMON_PRINT(CREATIONAL_RESULT, "Prototype", stringstream().str().c_str());
}

// singleton
namespace singleton
{
static std::stringstream& stringstream()
{
    static std::stringstream stream;
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
    static void tell() { stringstream() << "this is singleton" << std::endl; }

private:
    Singleton() = default;

    static std::shared_ptr<Singleton> instance;
};

std::shared_ptr<Singleton> Singleton::instance = nullptr;
} // namespace singleton

void Creational::singletonInstance()
{
    using singleton::Singleton;
    using singleton::stringstream;

    Singleton::get()->tell();
    Singleton::restart();

    COMMON_PRINT(CREATIONAL_RESULT, "Singleton", stringstream().str().c_str());
}
} // namespace dp_creational
