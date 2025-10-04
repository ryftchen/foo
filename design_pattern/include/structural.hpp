//! @file structural.hpp
//! @author ryftchen
//! @brief The declarations (structural) in the design pattern module.
//! @version 0.1.0
//! @copyright Copyright (c) 2022-2025 ryftchen. All rights reserved.

#pragma once

#include <map>
#include <memory>
#include <sstream>
#include <vector>

//! @brief The design pattern module.
namespace design_pattern // NOLINT(modernize-concat-nested-namespaces)
{
//! @brief Structural-related functions in the design pattern module.
namespace structural
{
//! @brief Function name string.
//! @return name string (module_function)
inline const char* name() noexcept
{
    return "DP_STRUCTURAL";
}
extern const char* version() noexcept;

//! @brief The adapter pattern.
namespace adapter
{
//! @brief The specific interface that the client uses.
class Target
{
public:
    //! @brief Construct a new Target object.
    Target() = default;
    //! @brief Destroy the Target object.
    virtual ~Target() = default;
    //! @brief Construct a new Target object.
    Target(const Target&) = default;
    //! @brief Construct a new Target object.
    Target(Target&&) noexcept = default;
    //! @brief The operator (=) overloading of Target class.
    //! @return reference of the Target object
    Target& operator=(const Target&) = default;
    //! @brief The operator (=) overloading of Target class.
    //! @return reference of the Target object
    Target& operator=(Target&&) noexcept = default;

    //! @brief Generate a request.
    virtual void request() = 0;
};

//! @brief Get calls that client makes on the target because of the adapter.
class Adaptee
{
public:
    //! @brief The specific request.
    static void specificRequest();
};

//! @brief Delegate the call to an adaptee when getting a method call.
class Adapter : public Target
{
public:
    //! @brief Construct a new Adapter object.
    Adapter() : adaptee{std::make_unique<Adaptee>()} {}

    //! @brief Generate a request.
    void request() override;

private:
    //! @brief The adaptee.
    std::unique_ptr<Adaptee> adaptee;
};

extern std::ostringstream& output() noexcept;
} // namespace adapter

//! @brief The bridge pattern.
namespace bridge
{
//! @brief The interface for implementation.
class Implementor
{
public:
    //! @brief Construct a new Implementor object.
    Implementor() = default;
    //! @brief Destroy the Implementor object.
    virtual ~Implementor() = default;
    //! @brief Construct a new Implementor object.
    Implementor(const Implementor&) = default;
    //! @brief Construct a new Implementor object.
    Implementor(Implementor&&) noexcept = default;
    //! @brief The operator (=) overloading of Implementor class.
    //! @return reference of the Implementor object
    Implementor& operator=(const Implementor&) = default;
    //! @brief The operator (=) overloading of Implementor class.
    //! @return reference of the Implementor object
    Implementor& operator=(Implementor&&) noexcept = default;

    //! @brief Implemented action.
    virtual void action() = 0;
};

//! @brief The concrete implementor.
class ConcreteImplementorA : public Implementor
{
public:
    //! @brief Implemented action.
    void action() override;
};

//! @brief The concrete implementor.
class ConcreteImplementorB : public Implementor
{
public:
    //! @brief Implemented action.
    void action() override;
};

//! @brief The interface for abstraction.
class Abstraction
{
public:
    //! @brief Construct a new Abstraction object.
    Abstraction() = default;
    //! @brief Destroy the Abstraction object.
    virtual ~Abstraction() = default;
    //! @brief Construct a new Abstraction object.
    Abstraction(const Abstraction&) = default;
    //! @brief Construct a new Abstraction object.
    Abstraction(Abstraction&&) noexcept = default;
    //! @brief The operator (=) overloading of Abstraction class.
    //! @return reference of the Abstraction object
    Abstraction& operator=(const Abstraction&) = default;
    //! @brief The operator (=) overloading of Abstraction class.
    //! @return reference of the Abstraction object
    Abstraction& operator=(Abstraction&&) noexcept = default;

    //! @brief Abstracted operation.
    virtual void operation() = 0;
};

//! @brief Extend abstraction’s interface.
class RefinedAbstraction : public Abstraction
{
public:
    //! @brief Construct a new RefinedAbstraction object.
    //! @param implementor - target implementor
    explicit RefinedAbstraction(std::unique_ptr<Implementor> implementor) : implementor{std::move(implementor)} {}

    //! @brief Abstracted operation.
    void operation() override;

private:
    //! @brief The implementor.
    const std::unique_ptr<Implementor> implementor;
};

extern std::ostringstream& output() noexcept;
} // namespace bridge

//! @brief The composite pattern.
namespace composite
{
//! @brief Both the composite and the leaf nodes are in the composition.
class Component
{
public:
    //! @brief Construct a new Component object.
    Component() = default;
    //! @brief Destroy the Component object.
    virtual ~Component() = default;
    //! @brief Construct a new Component object.
    Component(const Component&) = default;
    //! @brief Construct a new Component object.
    Component(Component&&) noexcept = default;
    //! @brief The operator (=) overloading of Component class.
    //! @return reference of the Component object
    Component& operator=(const Component&) = default;
    //! @brief The operator (=) overloading of Component class.
    //! @return reference of the Component object
    Component& operator=(Component&&) noexcept = default;

    //! @brief Get the child component by index.
    //! @param index - child component index
    virtual std::shared_ptr<Component> getChild(const std::uint32_t index);
    //! @brief Add the child component.
    //! @param component - child component to be added
    virtual void add(const std::shared_ptr<Component>& component);
    //! @brief Remove the child component by index.
    //! @param index - child component index
    virtual void remove(const std::uint32_t index);
    //! @brief Execute all child components' operations.
    virtual void operation() = 0;
};

//! @brief The behavior of the components having children.
class Composite : public Component
{
public:
    //! @brief Get the child component by index.
    //! @param index - child component index
    //! @return child component
    std::shared_ptr<Component> getChild(const std::uint32_t index) override;
    //! @brief Add the child component.
    //! @param component - child component to be added
    void add(const std::shared_ptr<Component>& component) override;
    //! @brief Remove the child component by index.
    //! @param index - child component index
    void remove(const std::uint32_t index) override;
    //! @brief Execute operation.
    void operation() override;

private:
    //! @brief Child components.
    std::vector<std::shared_ptr<Component>> children;
};

//! @brief The behavior of the element that has no child in the composition.
class Leaf : public Component
{
public:
    //! @brief Construct a new Leaf object.
    //! @param id - target leaf node id
    explicit Leaf(const int id) : id{id} {}

    //! @brief Execute operation.
    void operation() override;

private:
    //! @brief Leaf node id.
    const int id{0};
};

extern std::ostringstream& output() noexcept;
} // namespace composite

//! @brief The decorator pattern.
namespace decorator
{
//! @brief The component to which additional responsibilities can be attached.
class Component
{
public:
    //! @brief Construct a new Component object.
    Component() = default;
    //! @brief Destroy the Component object.
    virtual ~Component() = default;
    //! @brief Construct a new Component object.
    Component(const Component&) = default;
    //! @brief Construct a new Component object.
    Component(Component&&) noexcept = default;
    //! @brief The operator (=) overloading of Component class.
    //! @return reference of the Component object
    Component& operator=(const Component&) = default;
    //! @brief The operator (=) overloading of Component class.
    //! @return reference of the Component object
    Component& operator=(Component&&) noexcept = default;

    //! @brief The related operation.
    virtual void operation() = 0;
};

//! @brief The concrete component.
class ConcreteComponent : public Component
{
public:
    //! @brief The related operation.
    void operation() override;
};

//! @brief Reference to the component. Add responsibilities to the component.
class Decorator : public Component
{
public:
    //! @brief Construct a new Decorator object.
    //! @param component - target component
    explicit Decorator(std::shared_ptr<Component> component) : component{std::move(component)} {}

    //! @brief The related operation.
    void operation() override;

private:
    //! @brief The component.
    const std::shared_ptr<Component> component;
};

//! @brief The concrete decorator.
class ConcreteDecoratorA : public Decorator
{
public:
    //! @brief Construct a new ConcreteDecoratorA object.
    //! @param decorator - target decorator
    explicit ConcreteDecoratorA(std::shared_ptr<Component> decorator) : Decorator(std::move(decorator)) {}

    //! @brief The related operation.
    void operation() override;
};

//! @brief The concrete decorator.
class ConcreteDecoratorB : public Decorator
{
public:
    //! @brief Construct a new ConcreteDecoratorB object.
    //! @param decorator - target decorator
    explicit ConcreteDecoratorB(std::shared_ptr<Component> decorator) : Decorator(std::move(decorator)) {}

    //! @brief The related operation.
    void operation() override;
};

extern std::ostringstream& output() noexcept;
} // namespace decorator

//! @brief The facade pattern.
namespace facade
{
//! @brief Implement complex subsystem functionality.
class SubsystemA
{
public:
    //! @brief The sub-operation.
    static void suboperation();
};

//! @brief Implement complex subsystem functionality.
class SubsystemB
{
public:
    //! @brief The sub-operation.
    static void suboperation();
};

//! @brief Implement complex subsystem functionality.
class SubsystemC
{
public:
    //! @brief The sub-operation.
    static void suboperation();
};

//! @brief Delegate client requests to appropriate subsystem and unified interface.
class Facade
{
public:
    //! @brief The operation 1.
    void operation1();
    //! @brief The operation 2.
    void operation2();

private:
    //! @brief Subsystem A.
    const std::shared_ptr<SubsystemA> subsystemA;
    //! @brief Subsystem B.
    const std::shared_ptr<SubsystemB> subsystemB;
    //! @brief Subsystem C.
    const std::shared_ptr<SubsystemC> subsystemC;
};

extern std::ostringstream& output() noexcept;
} // namespace facade

//! @brief The flyweight pattern.
namespace flyweight
{
//! @brief Receive and act on the extrinsic state through which flyweights.
class Flyweight
{
public:
    //! @brief Construct a new Flyweight object.
    Flyweight() = default;
    //! @brief Destroy the Flyweight object.
    virtual ~Flyweight() = default;
    //! @brief Construct a new Flyweight object.
    Flyweight(const Flyweight&) = default;
    //! @brief Construct a new Flyweight object.
    Flyweight(Flyweight&&) noexcept = default;
    //! @brief The operator (=) overloading of Flyweight class.
    //! @return reference of the Flyweight object
    Flyweight& operator=(const Flyweight&) = default;
    //! @brief The operator (=) overloading of Flyweight class.
    //! @return reference of the Flyweight object
    Flyweight& operator=(Flyweight&&) noexcept = default;

    //! @brief The related operation.
    virtual void operation() = 0;
};

//! @brief The unshared concrete flyweight.
class UnsharedConcreteFlyweight : public Flyweight
{
public:
    //! @brief Construct a new UnsharedConcreteFlyweight object.
    //! @param intrinsicState - target intrinsic state
    explicit UnsharedConcreteFlyweight(const int intrinsicState) : state{intrinsicState} {}

    //! @brief The related operation.
    void operation() override;

private:
    //! @brief Intrinsic state.
    const int state{0};
};

//! @brief The concrete flyweight.
class ConcreteFlyweight : public Flyweight
{
public:
    //! @brief Construct a new ConcreteFlyweight object.
    //! @param intrinsicState - target intrinsic state
    explicit ConcreteFlyweight(const int intrinsicState) : state{intrinsicState} {}

    //! @brief The related operation.
    void operation() override;

private:
    //! @brief Intrinsic state.
    const int state{0};
};

//! @brief Manage flyweights and ensure that flyweights are appropriately shared.
class FlyweightFactory
{
public:
    //! @brief Get the flyweight by key.
    //! @param key - key of the flyweight
    //! @return flyweight
    std::unique_ptr<Flyweight>& getFlyweight(const int key);

private:
    //! @brief The flies.
    std::map<int, std::unique_ptr<Flyweight>> flies;
};

extern std::ostringstream& output() noexcept;
} // namespace flyweight

//! @brief The proxy pattern.
namespace proxy
{
//! @brief Make the proxy can be used anywhere a real subject is expected.
class Subject
{
public:
    //! @brief Construct a new Subject object.
    Subject() = default;
    //! @brief Destroy the Subject object.
    virtual ~Subject() = default;
    //! @brief Construct a new Subject object.
    Subject(const Subject&) = default;
    //! @brief Construct a new Subject object.
    Subject(Subject&&) noexcept = default;
    //! @brief The operator (=) overloading of Subject class.
    //! @return reference of the Subject object
    Subject& operator=(const Subject&) = default;
    //! @brief The operator (=) overloading of Subject class.
    //! @return reference of the Subject object
    Subject& operator=(Subject&&) noexcept = default;

    //! @brief The specific request.
    virtual void request() = 0;
};

//! @brief The real object that the proxy represents.
class RealSubject : public Subject
{
public:
    //! @brief The specific request.
    void request() override;
};

//! @brief Let the proxy access the real subject.
class Proxy : public Subject
{
public:
    //! @brief The specific request.
    void request() override;

private:
    //! @brief Real subject.
    std::unique_ptr<RealSubject> subject;

protected:
    //! @brief Get the real subject.
    //! @return real subject
    RealSubject& realSubject();
};

extern std::ostringstream& output() noexcept;
} // namespace proxy
} // namespace structural
} // namespace design_pattern
