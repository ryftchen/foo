#include "structural.hpp"
#include <map>
#include <vector>
#ifndef _NO_PRINT_AT_RUNTIME
#include "utility/include/common.hpp"

#define STRUCTURAL_RESULT "\r\n*%-9s instance:\r\n%s"
#define STRUCTURAL_PRINT_RESULT_CONTENT(method) COMMON_PRINT(STRUCTURAL_RESULT, method, output().str().c_str())
#else
#include <memory>
#include <sstream>

#define STRUCTURAL_PRINT_RESULT_CONTENT(method)
#endif

namespace design_pattern::structural
{
StructuralPattern::StructuralPattern()
{
#ifndef _NO_PRINT_AT_RUNTIME
    std::cout << "\r\nStructural pattern:" << std::endl;
#endif
}

// Adapter
namespace adapter
{
static std::ostringstream& output()
{
    static std::ostringstream stream;
    return stream;
}

class Target
{
public:
    virtual ~Target() = default;

    virtual void request() = 0;
};

class Adaptee
{
public:
    static void specificRequest() { output() << "specific request" << std::endl; }
};

class Adapter : public Target
{
public:
    Adapter() : adaptee(std::make_unique<Adaptee>()) {}
    ~Adapter() override { adaptee.reset(); }

    void request() override { adaptee->specificRequest(); }

private:
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

// Bridge
namespace bridge
{
static std::ostringstream& output()
{
    static std::ostringstream stream;
    return stream;
}

class Implementor
{
public:
    virtual ~Implementor() = default;

    virtual void action() = 0;
};

class ConcreteImplementorA : public Implementor
{
public:
    ~ConcreteImplementorA() override = default;

    void action() override { output() << "concrete implementor A" << std::endl; }
};

class ConcreteImplementorB : public Implementor
{
public:
    ~ConcreteImplementorB() override = default;

    void action() override { output() << "concrete implementor B" << std::endl; }
};

class Abstraction
{
public:
    virtual ~Abstraction() = default;

    virtual void operation() = 0;
};

class RefinedAbstraction : public Abstraction
{
public:
    ~RefinedAbstraction() override = default;
    explicit RefinedAbstraction(std::unique_ptr<Implementor> implementor) : implementor(std::move(implementor)) {}

    void operation() override { return implementor->action(); }

private:
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

// Composite
namespace composite
{
static std::ostringstream& output()
{
    static std::ostringstream stream;
    return stream;
}

class Component
{
public:
    virtual ~Component() = default;

    virtual std::shared_ptr<Component> getChild(const uint32_t /*unused*/) { return nullptr; }
    virtual void add(const std::shared_ptr<Component>& /*unused*/) {}
    virtual void remove(const uint32_t /*unused*/) {}
    virtual void operation() = 0;
};

class Composite : public Component
{
public:
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

    std::shared_ptr<Component> getChild(const uint32_t index) override { return children[index]; }
    void add(const std::shared_ptr<Component>& component) override { children.emplace_back(component); }
    void remove(const uint32_t index) override
    {
        std::shared_ptr<Component> child = children[index];
        children.erase(children.begin() + index);
        child.reset();
    }
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
    std::vector<std::shared_ptr<Component>> children;
};

class Leaf : public Component
{
public:
    explicit Leaf(const int id) : id(id) {}
    ~Leaf() override = default;

    void operation() override { output() << "leaf " << id << " operation" << std::endl; }

private:
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

// Decorator
namespace decorator
{
static std::ostringstream& output()
{
    static std::ostringstream stream;
    return stream;
}

class Component
{
public:
    virtual ~Component() = default;

    virtual void operation() = 0;
};

class ConcreteComponent : public Component
{
public:
    ~ConcreteComponent() override = default;

    void operation() override { output() << "concrete component operation" << std::endl; }
};

class Decorator : public Component
{
public:
    ~Decorator() override = default;
    explicit Decorator(std::shared_ptr<Component> component) : component(std::move(component)) {}

    void operation() override { component->operation(); }

private:
    std::shared_ptr<Component> component;
};

class ConcreteDecoratorA : public Decorator
{
public:
    explicit ConcreteDecoratorA(std::shared_ptr<Component> decorator) : Decorator(std::move(decorator)) {}

    void operation() override
    {
        Decorator::operation();
        output() << "decorator A" << std::endl;
    }
};

class ConcreteDecoratorB : public Decorator
{
public:
    explicit ConcreteDecoratorB(std::shared_ptr<Component> decorator) : Decorator(std::move(decorator)) {}

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

// Facade
namespace facade
{
static std::ostringstream& output()
{
    static std::ostringstream stream;
    return stream;
}

class SubsystemA
{
public:
    static void suboperation() { output() << "subsystem A method" << std::endl; }
};

class SubsystemB
{
public:
    static void suboperation() { output() << "subsystem B method" << std::endl; }
};

class SubsystemC
{
public:
    static void suboperation() { output() << "subsystem C method" << std::endl; }
};

class Facade
{
public:
    Facade() : subsystemA(), subsystemB(), subsystemC() {}

    void operation1()
    {
        subsystemA->suboperation();
        subsystemB->suboperation();
    }
    void operation2() { subsystemC->suboperation(); }

private:
    std::shared_ptr<SubsystemA> subsystemA;
    std::shared_ptr<SubsystemB> subsystemB;
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

// Flyweight
namespace flyweight
{
static std::ostringstream& output()
{
    static std::ostringstream stream;
    return stream;
}

class Flyweight
{
public:
    virtual ~Flyweight() = default;

    virtual void operation() = 0;
};

class UnsharedConcreteFlyweight : public Flyweight
{
public:
    explicit UnsharedConcreteFlyweight(const int intrinsicState) : state(intrinsicState) {}
    ~UnsharedConcreteFlyweight() override = default;

    void operation() override { output() << "unshared flyweight with state " << state << std::endl; }

private:
    int state;
};

class ConcreteFlyweight : public Flyweight
{
public:
    explicit ConcreteFlyweight(const int allState) : state(allState) {}
    ~ConcreteFlyweight() override = default;

    void operation() override { output() << "concrete flyweight with state " << state << std::endl; }

private:
    int state;
};

class FlyweightFactory
{
public:
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

// Proxy
namespace proxy
{
static std::ostringstream& output()
{
    static std::ostringstream stream;
    return stream;
}

class Subject
{
public:
    virtual ~Subject() = default;

    virtual void request() = 0;
};

class RealSubject : public Subject
{
public:
    void request() override { output() << "real subject request" << std::endl; }
};

class Proxy : public Subject
{
public:
    ~Proxy() override
    {
        if (subject)
        {
            subject.reset();
        }
    }

    void request() override { realSubject().request(); }

private:
    std::unique_ptr<RealSubject> subject;

protected:
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