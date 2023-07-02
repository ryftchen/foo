//! @file creational.hpp
//! @author ryftchen
//! @brief The declarations (creational) in the design pattern module.
//! @version 1.0
//! @copyright Copyright (c) 2022-2023 ryftchen.

#pragma once

#include <memory>
#include <sstream>

//! @brief Creational-related functions in the design pattern module.
namespace design_pattern::creational
{
//! @brief The abstract factory pattern.
namespace abstract_factory
{
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
    std::string getName() override;
};

//! @brief The concrete product.
class ConcreteProductAY : public ProductA
{
public:
    //! @brief Destroy the ConcreteProductAY object.
    ~ConcreteProductAY() override = default;

    //! @brief Get the product name.
    //! @return product name
    std::string getName() override;
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
    std::string getName() override;
};

//! @brief The concrete product.
class ConcreteProductBY : public ProductB
{
public:
    //! @brief Destroy the ConcreteProductBY object.
    ~ConcreteProductBY() override = default;

    //! @brief Get the product name.
    //! @return product name
    std::string getName() override;
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
    std::unique_ptr<ProductA> createProductA() override;
    //! @brief Create product B.
    //! @return product B
    std::unique_ptr<ProductB> createProductB() override;
};

//! @brief The concrete factory.
class ConcreteFactoryY : public AbstractFactory
{
public:
    //! @brief Destroy the ConcreteFactoryY object.
    ~ConcreteFactoryY() override = default;

    //! @brief Create product A.
    //! @return product A
    std::unique_ptr<ProductA> createProductA() override;
    //! @brief Create product B.
    //! @return product B
    std::unique_ptr<ProductB> createProductB() override;
};

extern std::ostringstream& output();
} // namespace abstract_factory

//! @brief The builder pattern.
namespace builder
{
//! @brief Product properties.
class Product
{
public:
    //! @brief Make part A.
    //! @param part - target part
    void makeA(const std::string& part);
    //! @brief Make part B.
    //! @param part - target part
    void makeB(const std::string& part);
    //! @brief Make part C.
    //! @param part - target part
    void makeC(const std::string& part);
    //! @brief Get all parts.
    //! @return all parts
    std::string get();

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
    Product get();
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
    void buildPartA() override;
    //! @brief Build part B.
    void buildPartB() override;
    //! @brief Build part C.
    void buildPartC() override;
};

//! @brief The concrete builder.
class ConcreteBuilderY : public Builder
{
public:
    //! @brief Build part A.
    void buildPartA() override;
    //! @brief Build part B.
    void buildPartB() override;
    //! @brief Build part C.
    void buildPartC() override;
};

//! @brief Manage the correct sequence of creation.
class Director
{
public:
    //! @brief Construct a new Director object.
    Director() : builder(){};
    //! @brief Destroy the Director object.
    ~Director();

    //! @brief Set the builder.
    //! @param b - target builder.
    void set(std::unique_ptr<Builder> b);
    //! @brief Get the product from the builder.
    //! @return product
    Product get();
    //! @brief Construct products by the builder.
    void construct();

private:
    //! @brief The builder.
    std::unique_ptr<Builder> builder;
};

extern std::ostringstream& output();
} // namespace builder

//! @brief The factory method pattern.
namespace factory_method
{
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
    std::string getName() override;
};

//! @brief The concrete product.
class ConcreteProductB : public Product
{
public:
    //! @brief Destroy the ConcreteProductB object.
    ~ConcreteProductB() override = default;

    //! @brief Get the product name.
    //! @return product name
    std::string getName() override;
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
    std::unique_ptr<Product> createProductA() override;
    //! @brief Create product B.
    //! @return product B
    std::unique_ptr<Product> createProductB() override;
    //! @brief Remove product.
    void removeProduct(std::unique_ptr<Product>& product) override;
};

extern std::ostringstream& output();
} // namespace factory_method

//! @brief The prototype pattern.
namespace prototype
{
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
    std::unique_ptr<Prototype> clone() override;
    //! @brief Get the type.
    //! @return type
    std::string type() override;
};

//! @brief The concrete prototype.
class ConcretePrototypeB : public Prototype
{
public:
    //! @brief Destroy the ConcretePrototypeB object.
    ~ConcretePrototypeB() override = default;

    //! @brief Clone self.
    //! @return cloning of self
    std::unique_ptr<Prototype> clone() override;
    //! @brief Get the type.
    //! @return type
    std::string type() override;
};

//! @brief Create by asking the prototype to clone itself.
class Client
{
public:
    //! @brief Initialize all prototypes.
    static void init();
    //! @brief Remove all prototypes.
    static void remove();
    //! @brief Make by prototype index
    //! @param index - prototype index
    //! @return cloning result
    static std::unique_ptr<Prototype> make(const int index);

private:
    //! @brief Collection of prototypes.
    static std::unique_ptr<Prototype> types[2];
    //! @brief The number of types.
    static int nTypes;
};

extern std::ostringstream& output();
} // namespace prototype

//! @brief The singleton factory pattern.
namespace singleton
{
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
    static std::shared_ptr<Singleton> get();
    //! @brief Restart the instance of the singleton.
    static void restart();
    //! @brief Tell external.
    static void tell();

private:
    //! @brief Construct a new Singleton object.
    Singleton() = default;

    //! @brief The instance of the singleton.
    static std::shared_ptr<Singleton> instance;
};

extern std::ostringstream& output();
} // namespace singleton
} // namespace design_pattern::creational
