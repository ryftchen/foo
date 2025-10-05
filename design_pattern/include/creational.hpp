//! @file creational.hpp
//! @author ryftchen
//! @brief The declarations (creational) in the design pattern module.
//! @version 0.1.0
//! @copyright Copyright (c) 2022-2025 ryftchen. All rights reserved.

#pragma once

#include <memory>
#include <sstream>

//! @brief The design pattern module.
namespace design_pattern // NOLINT(modernize-concat-nested-namespaces)
{
//! @brief Creational-related functions in the design pattern module.
namespace creational
{
//! @brief Brief function description.
//! @return function description (module_function)
inline static const char* description() noexcept
{
    return "DP_CREATIONAL";
}
extern const char* version() noexcept;

//! @brief The abstract factory pattern.
namespace abstract_factory
{
//! @brief Implement the same interface. The others can refer to the interface not the concrete product.
class ProductA
{
public:
    //! @brief Construct a new ProductA object.
    ProductA() = default;
    //! @brief Destroy the ProductA object.
    virtual ~ProductA() = default;
    //! @brief Construct a new ProductA object.
    ProductA(const ProductA&) = default;
    //! @brief Construct a new ProductA object.
    ProductA(ProductA&&) noexcept = default;
    //! @brief The operator (=) overloading of ProductA class.
    //! @return reference of the ProductA object
    ProductA& operator=(const ProductA&) = default;
    //! @brief The operator (=) overloading of ProductA class.
    //! @return reference of the ProductA object
    ProductA& operator=(ProductA&&) noexcept = default;

    //! @brief Get the product name.
    //! @return product name
    [[nodiscard]] virtual std::string getName() const = 0;
};

//! @brief The concrete product.
class ConcreteProductAX : public ProductA
{
public:
    //! @brief Get the product name.
    //! @return product name
    [[nodiscard]] std::string getName() const override;
};

//! @brief The concrete product.
class ConcreteProductAY : public ProductA
{
public:
    //! @brief Get the product name.
    //! @return product name
    [[nodiscard]] std::string getName() const override;
};

//! @brief Implement the same interface. The others can refer to the interface not the concrete product.
class ProductB
{
public:
    //! @brief Construct a new ProductB object.
    ProductB() = default;
    //! @brief Destroy the ProductB object.
    virtual ~ProductB() = default;
    //! @brief Construct a new ProductB object.
    ProductB(const ProductB&) = default;
    //! @brief Construct a new ProductB object.
    ProductB(ProductB&&) noexcept = default;
    //! @brief The operator (=) overloading of ProductB class.
    //! @return reference of the ProductB object
    ProductB& operator=(const ProductB&) = default;
    //! @brief The operator (=) overloading of ProductB class.
    //! @return reference of the ProductB object
    ProductB& operator=(ProductB&&) noexcept = default;

    //! @brief Get the product name.
    //! @return product name
    [[nodiscard]] virtual std::string getName() const = 0;
};

//! @brief The concrete product.
class ConcreteProductBX : public ProductB
{
public:
    //! @brief Get the product name.
    //! @return product name
    [[nodiscard]] std::string getName() const override;
};

//! @brief The concrete product.
class ConcreteProductBY : public ProductB
{
public:
    //! @brief Get the product name.
    //! @return product name
    [[nodiscard]] std::string getName() const override;
};

//! @brief Provide the abstract interface for creating the family of products.
//!        It never has to instantiate a product object.
class AbstractFactory
{
public:
    //! @brief Construct a new AbstractFactory object.
    AbstractFactory() = default;
    //! @brief Destroy the AbstractFactory object.
    virtual ~AbstractFactory() = default;
    //! @brief Construct a new AbstractFactory object.
    AbstractFactory(const AbstractFactory&) = default;
    //! @brief Construct a new AbstractFactory object.
    AbstractFactory(AbstractFactory&&) noexcept = default;
    //! @brief The operator (=) overloading of AbstractFactory class.
    //! @return reference of the AbstractFactory object
    AbstractFactory& operator=(const AbstractFactory&) = default;
    //! @brief The operator (=) overloading of AbstractFactory class.
    //! @return reference of the AbstractFactory object
    AbstractFactory& operator=(AbstractFactory&&) noexcept = default;

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
    //! @brief Create product A.
    //! @return product A
    std::unique_ptr<ProductA> createProductA() override;
    //! @brief Create product B.
    //! @return product B
    std::unique_ptr<ProductB> createProductB() override;
};

extern std::ostringstream& output() noexcept;
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
    void makeA(const std::string_view part);
    //! @brief Make part B.
    //! @param part - target part
    void makeB(const std::string_view part);
    //! @brief Make part C.
    //! @param part - target part
    void makeC(const std::string_view part);
    //! @brief Get all parts.
    //! @return all parts
    [[nodiscard]] std::string get() const;

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
    //! @brief Construct a new Builder object.
    Builder() = default;
    //! @brief Destroy the Builder object.
    virtual ~Builder() = default;
    //! @brief Construct a new Builder object.
    Builder(const Builder&) = default;
    //! @brief Construct a new Builder object.
    Builder(Builder&&) noexcept = default;
    //! @brief The operator (=) overloading of Builder class.
    //! @return reference of the Builder object
    Builder& operator=(const Builder&) = delete;
    //! @brief The operator (=) overloading of Builder class.
    //! @return reference of the Builder object
    Builder& operator=(Builder&&) noexcept = delete;

    //! @brief Get the product.
    //! @return product
    [[nodiscard]] Product get() const;
    //! @brief Build part A.
    virtual void buildPartA() = 0;
    //! @brief Build part B.
    virtual void buildPartB() = 0;
    //! @brief Build part C.
    virtual void buildPartC() = 0;

protected:
    //! @brief The product.
    Product product{};
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
    //! @brief Set the builder.
    //! @param b - target builder
    void set(std::unique_ptr<Builder> b);
    //! @brief Get the product from the builder.
    //! @return product
    [[nodiscard]] Product get() const;
    //! @brief Construct products by the builder.
    void construct();

private:
    //! @brief The builder.
    std::unique_ptr<Builder> builder;
};

extern std::ostringstream& output() noexcept;
} // namespace builder

//! @brief The factory method pattern.
namespace factory_method
{
//! @brief Implement the same interface. The others can refer to the interface not the concrete product.
class Product
{
public:
    //! @brief Construct a new Product object.
    Product() = default;
    //! @brief Destroy the Product object.
    virtual ~Product() = default;
    //! @brief Construct a new Product object.
    Product(const Product&) = default;
    //! @brief Construct a new Product object.
    Product(Product&&) noexcept = default;
    //! @brief The operator (=) overloading of Product class.
    //! @return reference of the Product object
    Product& operator=(const Product&) = default;
    //! @brief The operator (=) overloading of Product class.
    //! @return reference of the Product object
    Product& operator=(Product&&) noexcept = default;

    //! @brief Get the product name.
    //! @return product name
    [[nodiscard]] virtual std::string getName() const = 0;
};

//! @brief The concrete product.
class ConcreteProductA : public Product
{
public:
    //! @brief Get the product name.
    //! @return product name
    [[nodiscard]] std::string getName() const override;
};

//! @brief The concrete product.
class ConcreteProductB : public Product
{
public:
    //! @brief Get the product name.
    //! @return product name
    [[nodiscard]] std::string getName() const override;
};

//! @brief Contain the implementation for all methods to manipulate products except for the factory method.
//!        Know how to create the products.
class Creator
{
public:
    //! @brief Construct a new Creator object.
    Creator() = default;
    //! @brief Destroy the Creator object.
    virtual ~Creator() = default;
    //! @brief Construct a new Creator object.
    Creator(const Creator&) = default;
    //! @brief Construct a new Creator object.
    Creator(Creator&&) noexcept = default;
    //! @brief The operator (=) overloading of Creator class.
    //! @return reference of the Creator object
    Creator& operator=(const Creator&) = default;
    //! @brief The operator (=) overloading of Creator class.
    //! @return reference of the Creator object
    Creator& operator=(Creator&&) noexcept = default;

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
    //! @brief Create product A.
    //! @return product A
    std::unique_ptr<Product> createProductA() override;
    //! @brief Create product B.
    //! @return product B
    std::unique_ptr<Product> createProductB() override;
    //! @brief Remove product.
    void removeProduct(std::unique_ptr<Product>& product) override;
};

extern std::ostringstream& output() noexcept;
} // namespace factory_method

//! @brief The prototype pattern.
namespace prototype
{
//! @brief The interface for cloning itself.
class Prototype
{
public:
    //! @brief Construct a new Prototype object.
    Prototype() = default;
    //! @brief Destroy the Prototype object.
    virtual ~Prototype() = default;
    //! @brief Construct a new Prototype object.
    Prototype(const Prototype&) = default;
    //! @brief Construct a new Prototype object.
    Prototype(Prototype&&) noexcept = default;
    //! @brief The operator (=) overloading of Prototype class.
    //! @return reference of the Prototype object
    Prototype& operator=(const Prototype&) = default;
    //! @brief The operator (=) overloading of Prototype class.
    //! @return reference of the Prototype object
    Prototype& operator=(Prototype&&) noexcept = default;

    //! @brief Clone self.
    //! @return cloning of self
    virtual std::unique_ptr<Prototype> clone() = 0;
    //! @brief Get the type.
    //! @return type
    [[nodiscard]] virtual std::string type() const = 0;
};

//! @brief The concrete prototype.
class ConcretePrototypeA : public Prototype
{
public:
    //! @brief Clone self.
    //! @return cloning of self
    std::unique_ptr<Prototype> clone() override;
    //! @brief Get the type.
    //! @return type
    [[nodiscard]] std::string type() const override;
};

//! @brief The concrete prototype.
class ConcretePrototypeB : public Prototype
{
public:
    //! @brief Clone self.
    //! @return cloning of self
    std::unique_ptr<Prototype> clone() override;
    //! @brief Get the type.
    //! @return type
    [[nodiscard]] std::string type() const override;
};

//! @brief Create by asking the prototype to clone itself.
class Client
{
public:
    //! @brief Initialize all prototypes.
    static void init();
    //! @brief Remove all prototypes.
    static void remove();
    //! @brief Make by prototype index.
    //! @param index - prototype index
    //! @return cloning result
    static std::unique_ptr<Prototype> make(const int index);

private:
    //! @brief Collection of prototypes.
    static std::unique_ptr<Prototype> types[2];
    //! @brief Number of types.
    static const int nTypes;
};

extern std::ostringstream& output() noexcept;
} // namespace prototype

//! @brief The singleton factory pattern.
namespace singleton
{
//! @brief Have a private static variable to hold one instance of the singleton.
//!        It gives a way to instantiate the singleton.
class Singleton
{
public:
    //! @brief Destroy the Memento object.
    virtual ~Singleton() = default;
    //! @brief Construct a new Singleton object.
    Singleton(const Singleton&) = delete;
    //! @brief Construct a new Singleton object.
    Singleton(Singleton&&) = delete;
    //! @brief The operator (=) overloading of Singleton class.
    //! @return reference of the Singleton object
    Singleton& operator=(const Singleton&) = delete;
    //! @brief The operator (=) overloading of Singleton class.
    //! @return reference of the Singleton object
    Singleton& operator=(Singleton&&) = delete;

    //! @brief Get the instance of the singleton.
    //! @return the instance of the singleton
    static std::shared_ptr<Singleton>& get();
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

extern std::ostringstream& output() noexcept;
} // namespace singleton
} // namespace creational
} // namespace design_pattern
