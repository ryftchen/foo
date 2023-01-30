//! @file structural.cpp
//! @author ryftchen
//! @brief The definitions (structural) in the data structure module.
//! @version 0.1
//! @copyright Copyright (c) 2022-2023
#include "structural.hpp"
#include <map>
#include <vector>
#ifndef _NO_PRINT_AT_RUNTIME
#include "utility/include/common.hpp"

//! @brief Display structural result.
#define STRUCTURAL_RESULT "\r\n*%-9s instance:\r\n%s"
//! @brief Print structural result content.
#define STRUCTURAL_PRINT_RESULT_CONTENT(method)                      \
    COMMON_PRINT(STRUCTURAL_RESULT, method, output().str().c_str()); \
    output().str("");                                                \
    output().clear()
#else
#include <memory>
#include <sstream>

//! @brief Print structural result content.
#define STRUCTURAL_PRINT_RESULT_CONTENT(method) \
    output().str("");                           \
    output().clear()
#endif

namespace design_pattern::structural
{
StructuralPattern::StructuralPattern()
{
#ifndef _NO_PRINT_AT_RUNTIME
    std::cout << "\r\nInstances of the structural pattern:" << std::endl;
#endif
}

//! @brief The adapter pattern.
namespace adapter
{
//! @brief Output stream for the adapter pattern. Need to be cleared manually.
//! @return reference of output stream object, which is on string based.
static std::ostringstream& output()
{
    static std::ostringstream stream;
    return stream;
}

//! @brief The specific interface that the client uses.
class Target
{
public:
    //! @brief Destroy the Target object.
    virtual ~Target() = default;

    //! @brief Generate a request.
    virtual void request() = 0;
};

//! @brief Get calls that client makes on the target because of the adapter.
class Adaptee
{
public:
    //! @brief The specific request.
    static void specificRequest() { output() << "specific request" << std::endl; }
};

//! @brief Delegate the call to an adaptee when getting a method call.
class Adapter : public Target
{
public:
    //! @brief Construct a new Adapter object.
    Adapter() : adaptee(std::make_unique<Adaptee>()) {}
    //! @brief Destroy the Adapter object.
    ~Adapter() override { adaptee.reset(); }

    //! @brief Generate a request.
    void request() override { adaptee->specificRequest(); }

private:
    //! @brief
    std::unique_ptr<Adaptee> adaptee;
};

} // namespace adapter

void StructuralPattern::adapterInstance() const // NOLINT(readability-convert-member-functions-to-static)
{
    using adapter::Adapter;
    using adapter::output;
    using adapter::Target;

    std::shared_ptr<Target> t = std::make_shared<Adapter>();
    t->request();

    STRUCTURAL_PRINT_RESULT_CONTENT("Adapter");
}

//! @brief The bridge pattern.
namespace bridge
{
//! @brief Output stream for the bridge pattern. Need to be cleared manually.
//! @return reference of output stream object, which is on string based.
static std::ostringstream& output()
{
    static std::ostringstream stream;
    return stream;
}

//! @brief The interface for implementation.
class Implementor
{
public:
    //! @brief Destroy the Implementor object.
    virtual ~Implementor() = default;

    //! @brief Implemented action.
    virtual void action() = 0;
};

//! @brief The concrete implementor.
class ConcreteImplementorA : public Implementor
{
public:
    //! @brief Destroy the ConcreteImplementorA object.
    ~ConcreteImplementorA() override = default;

    //! @brief Implemented action.
    void action() override { output() << "concrete implementor A" << std::endl; }
};

//! @brief The concrete implementor.
class ConcreteImplementorB : public Implementor
{
public:
    //! @brief Destroy the ConcreteImplementorB object.
    ~ConcreteImplementorB() override = default;

    //! @brief Implemented action.
    void action() override { output() << "concrete implementor B" << std::endl; }
};

//! @brief The interface for abstraction.
class Abstraction
{
public:
    //! @brief Destroy the Abstraction object.
    virtual ~Abstraction() = default;

    //! @brief Abstracted operation.
    virtual void operation() = 0;
};

//! @brief Extend abstraction’s interface.
class RefinedAbstraction : public Abstraction
{
public:
    //! @brief Construct a new RefinedAbstraction object
    //! @param implementor - target implementor
    explicit RefinedAbstraction(std::unique_ptr<Implementor> implementor) : implementor(std::move(implementor)) {}
    //! @brief Destroy the RefinedAbstraction object.
    ~RefinedAbstraction() override = default;

    //! @brief Abstracted operation.
    void operation() override { return implementor->action(); }

private:
    //! @brief The implementor.
    std::unique_ptr<Implementor> implementor;
};
} // namespace bridge

void StructuralPattern::bridgeInstance() const // NOLINT(readability-convert-member-functions-to-static)
{
    using bridge::Abstraction;
    using bridge::ConcreteImplementorA;
    using bridge::ConcreteImplementorB;
    using bridge::Implementor;
    using bridge::output;
    using bridge::RefinedAbstraction;

    std::unique_ptr<Abstraction> abstract1 =
        std::make_unique<RefinedAbstraction>(std::make_unique<ConcreteImplementorA>());
    abstract1->operation();

    std::unique_ptr<Abstraction> abstract2 =
        std::make_unique<RefinedAbstraction>(std::make_unique<ConcreteImplementorB>());
    abstract2->operation();

    STRUCTURAL_PRINT_RESULT_CONTENT("Bridge");
}

//! @brief The composite pattern.
namespace composite
{
//! @brief Output stream for the composite pattern. Need to be cleared manually.
//! @return reference of output stream object, which is on string based.
static std::ostringstream& output()
{
    static std::ostringstream stream;
    return stream;
}

//! @brief Both the composite and the leaf nodes are in the composition.
class Component
{
public:
    //! @brief Destroy the Component object.
    virtual ~Component() = default;

    //! @brief Get the child component by index.
    virtual std::shared_ptr<Component> getChild(const uint32_t /*unused*/) { return nullptr; }
    //! @brief Add the child component.
    virtual void add(const std::shared_ptr<Component>& /*unused*/) {}
    //! @brief Remove the child component by index.
    virtual void remove(const uint32_t /*unused*/) {}
    //! @brief Execute all child components' operations.
    virtual void operation() = 0;
};

//! @brief The behavior of the components having children.
class Composite : public Component
{
public:
    //! @brief Destroy the Composite object.
    ~Composite() override
    {
        std::for_each(
            children.begin(),
            children.end(),
            [](auto& component)
            {
                component.reset();
            });
    }

    //! @brief Get the child component by index.
    //! @param index - child component index
    //! @return child component
    std::shared_ptr<Component> getChild(const uint32_t index) override { return children[index]; }
    //! @brief Add the child component.
    //! @param component - child component to be added
    void add(const std::shared_ptr<Component>& component) override { children.emplace_back(component); }
    //! @brief Remove the child component by index.
    //! @param index - child component index
    void remove(const uint32_t index) override
    {
        std::shared_ptr<Component> child = children[index];
        children.erase(children.begin() + index);
        child.reset();
    }
    //! @brief Execute operation.
    void operation() override
    {
        std::for_each(
            children.cbegin(),
            children.cend(),
            [](const auto& component)
            {
                component->operation();
            });
    }

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
    explicit Leaf(const int id) : id(id) {}
    //! @brief Destroy the Leaf object.
    ~Leaf() override = default;

    //! @brief Execute operation.
    void operation() override { output() << "leaf " << id << " operation" << std::endl; }

private:
    //! @brief Leaf node id.
    int id;
};
} // namespace composite

void StructuralPattern::compositeInstance() const // NOLINT(readability-convert-member-functions-to-static)
{
    using composite::Composite;
    using composite::Leaf;
    using composite::output;

    constexpr uint32_t count = 5;
    Composite composite;

    for (uint32_t i = 0; i < count; ++i)
    {
        composite.add(std::make_shared<Leaf>(i));
    }
    composite.remove(0);
    composite.operation();

    STRUCTURAL_PRINT_RESULT_CONTENT("Composite");
}

//! @brief The decorator pattern.
namespace decorator
{
//! @brief Output stream for the decorator pattern. Need to be cleared manually.
//! @return reference of output stream object, which is on string based.
static std::ostringstream& output()
{
    static std::ostringstream stream;
    return stream;
}

//! @brief The component to which additional responsibilities can be attached.
class Component
{
public:
    //! @brief Destroy the Component object.
    virtual ~Component() = default;

    //! @brief The related operation.
    virtual void operation() = 0;
};

//! @brief The concrete component.
class ConcreteComponent : public Component
{
public:
    //! @brief Destroy the ConcreteComponent object.
    ~ConcreteComponent() override = default;

    //! @brief The related operation.
    void operation() override { output() << "concrete component operation" << std::endl; }
};

//! @brief Reference to the component. Add responsibilities to the component.
class Decorator : public Component
{
public:
    //! @brief Construct a new Decorator object.
    //! @param component - target component
    explicit Decorator(std::shared_ptr<Component> component) : component(std::move(component)) {}
    //! @brief Destroy the Decorator object.
    ~Decorator() override = default;

    //! @brief The related operation.
    void operation() override { component->operation(); }

private:
    //! @brief The component.
    std::shared_ptr<Component> component;
};

//! @brief The concrete decorator.
class ConcreteDecoratorA : public Decorator
{
public:
    //! @brief Construct a new ConcreteDecoratorA object.
    //! @param decorator - target decorator
    explicit ConcreteDecoratorA(std::shared_ptr<Component> decorator) : Decorator(std::move(decorator)) {}

    //! @brief The related operation.
    void operation() override
    {
        Decorator::operation();
        output() << "decorator A" << std::endl;
    }
};

//! @brief The concrete decorator.
class ConcreteDecoratorB : public Decorator
{
public:
    //! @brief Construct a new ConcreteDecoratorB object.
    //! @param decorator - target decorator
    explicit ConcreteDecoratorB(std::shared_ptr<Component> decorator) : Decorator(std::move(decorator)) {}

    //! @brief The related operation.
    void operation() override
    {
        Decorator::operation();
        output() << "decorator B" << std::endl;
    }
};
} // namespace decorator

void StructuralPattern::decoratorInstance() const // NOLINT(readability-convert-member-functions-to-static)
{
    using decorator::Component;
    using decorator::ConcreteComponent;
    using decorator::ConcreteDecoratorA;
    using decorator::ConcreteDecoratorB;
    using decorator::output;

    std::shared_ptr<ConcreteComponent> cc = std::make_shared<ConcreteComponent>();
    std::shared_ptr<ConcreteDecoratorA> da = std::make_shared<ConcreteDecoratorA>(cc);
    std::shared_ptr<ConcreteDecoratorB> db = std::make_shared<ConcreteDecoratorB>(da);

    std::shared_ptr<Component> component = db;
    component->operation();

    STRUCTURAL_PRINT_RESULT_CONTENT("Decorator");
}

//! @brief The facade pattern.
namespace facade
{
//! @brief Output stream for the facade pattern. Need to be cleared manually.
//! @return reference of output stream object, which is on string based.
static std::ostringstream& output()
{
    static std::ostringstream stream;
    return stream;
}

//! @brief Implement complex subsystem functionality.
class SubsystemA
{
public:
    //! @brief The sub-operation.
    static void suboperation() { output() << "subsystem A method" << std::endl; }
};

//! @brief Implement complex subsystem functionality.
class SubsystemB
{
public:
    //! @brief The sub-operation.
    static void suboperation() { output() << "subsystem B method" << std::endl; }
};

//! @brief Implement complex subsystem functionality.
class SubsystemC
{
public:
    //! @brief The sub-operation.
    static void suboperation() { output() << "subsystem C method" << std::endl; }
};

//! @brief Delegate client requests to appropriate subsystem and unified interface.
class Facade
{
public:
    //! @brief Construct a new Facade object.
    Facade() : subsystemA(), subsystemB(), subsystemC() {}

    //! @brief The operation 1.
    void operation1()
    {
        subsystemA->suboperation();
        subsystemB->suboperation();
    }
    //! @brief The operation 2.
    void operation2() { subsystemC->suboperation(); }

private:
    //! @brief Subsystem A.
    std::shared_ptr<SubsystemA> subsystemA;
    //! @brief Subsystem B.
    std::shared_ptr<SubsystemB> subsystemB;
    //! @brief Subsystem C.
    std::shared_ptr<SubsystemC> subsystemC;
};
} // namespace facade

void StructuralPattern::facadeInstance() const // NOLINT(readability-convert-member-functions-to-static)
{
    using facade::Facade;
    using facade::output;

    std::shared_ptr<Facade> facade = std::make_shared<Facade>();
    facade->operation1();
    facade->operation2();

    STRUCTURAL_PRINT_RESULT_CONTENT("Facade");
}

//! @brief The flyweight pattern.
namespace flyweight
{
//! @brief Output stream for the flyweight pattern. Need to be cleared manually.
//! @return reference of output stream object, which is on string based.
static std::ostringstream& output()
{
    static std::ostringstream stream;
    return stream;
}

//! @brief Receive and act on the extrinsic state through which flyweights.
class Flyweight
{
public:
    //! @brief Destroy the Flyweight object.
    virtual ~Flyweight() = default;

    //! @brief The related operation.
    virtual void operation() = 0;
};

//! @brief The unshared concrete flyweight.
class UnsharedConcreteFlyweight : public Flyweight
{
public:
    //! @brief Construct a new UnsharedConcreteFlyweight object.
    //! @param intrinsicState - target intrinsic state
    explicit UnsharedConcreteFlyweight(const int intrinsicState) : state(intrinsicState) {}
    //! @brief Destroy the UnsharedConcreteFlyweight object.
    ~UnsharedConcreteFlyweight() override = default;

    //! @brief The related operation.
    void operation() override { output() << "unshared flyweight with state " << state << std::endl; }

private:
    //! @brief Intrinsic state.
    int state;
};

//! @brief The concrete flyweight.
class ConcreteFlyweight : public Flyweight
{
public:
    //! @brief Construct a new ConcreteFlyweight object.
    //! @param intrinsicState - target intrinsic state
    explicit ConcreteFlyweight(const int intrinsicState) : state(intrinsicState) {}
    //! @brief Destroy the ConcreteFlyweight object.
    ~ConcreteFlyweight() override = default;

    //! @brief The related operation.
    void operation() override { output() << "concrete flyweight with state " << state << std::endl; }

private:
    //! @brief Intrinsic state.
    int state;
};

//! @brief Manage flyweights and ensure that flyweights are appropriately shared.
class FlyweightFactory
{
public:
    //! @brief Destroy the FlyweightFactory object.
    ~FlyweightFactory()
    {
        std::for_each(
            flies.begin(),
            flies.end(),
            [](auto& file)
            {
                file.second.reset();
            });
        flies.clear();
    }

    //! @brief Get the flyweight by key value.
    //! @param key - key value
    //! @return flyweight
    std::unique_ptr<Flyweight>& getFlyweight(const int key)
    {
        if (flies.find(key) != flies.cend())
        {
            return flies[key];
        }
        std::unique_ptr<Flyweight> fly = std::make_unique<ConcreteFlyweight>(key);
        flies.insert(std::pair<int, std::unique_ptr<Flyweight>>(key, std::move(fly)));
        return flies[key];
    }

private:
    //! @brief The flies.
    std::map<int, std::unique_ptr<Flyweight>> flies;
};
} // namespace flyweight

void StructuralPattern::flyweightInstance() const // NOLINT(readability-convert-member-functions-to-static)
{
    using flyweight::FlyweightFactory;
    using flyweight::output;

    std::shared_ptr<FlyweightFactory> factory = std::make_shared<FlyweightFactory>();
    factory->getFlyweight(1)->operation();
    factory->getFlyweight(2)->operation();

    STRUCTURAL_PRINT_RESULT_CONTENT("Flyweight");
}

//! @brief The proxy pattern.
namespace proxy
{
//! @brief Output stream for the proxy pattern. Need to be cleared manually.
//! @return reference of output stream object, which is on string based.
static std::ostringstream& output()
{
    static std::ostringstream stream;
    return stream;
}

//! @brief Make the proxy can be used anywhere a real subject is expected.
class Subject
{
public:
    //! @brief Destroy the Subject object.
    virtual ~Subject() = default;

    //! @brief The specific request.
    virtual void request() = 0;
};

//! @brief The real object that the proxy represents.
class RealSubject : public Subject
{
public:
    //! @brief The specific request.
    void request() override { output() << "real subject request" << std::endl; }
};

//! @brief Let the proxy access the real subject.
class Proxy : public Subject
{
public:
    //! @brief Destroy the Proxy object.
    ~Proxy() override
    {
        if (subject)
        {
            subject.reset();
        }
    }

    //! @brief The specific request.
    void request() override { realSubject().request(); }

private:
    //! @brief Real subject.
    std::unique_ptr<RealSubject> subject;

protected:
    //! @brief Get the real subject.
    //! @return real subject
    RealSubject& realSubject()
    {
        if (!subject)
        {
            subject = std::make_unique<RealSubject>();
        }
        return *subject;
    }
};
} // namespace proxy

void StructuralPattern::proxyInstance() const // NOLINT(readability-convert-member-functions-to-static)
{
    using proxy::output;
    using proxy::Proxy;

    std::shared_ptr<Proxy> proxy = std::make_shared<Proxy>();
    proxy->request();

    STRUCTURAL_PRINT_RESULT_CONTENT("Proxy");
}
} // namespace design_pattern::structural