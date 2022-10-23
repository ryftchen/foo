#include "structural.hpp"
#include <map>
#include <vector>
#include "utility/include/common.hpp"

#define STRUCTURAL_RESULT "\r\n*%-9s instance:\r\n%s"

namespace dp_structural
{
Structural::Structural()
{
    std::cout << "\r\nStructural pattern:" << std::endl;
}

// adapter
namespace adapter
{
static std::stringstream& stringstream()
{
    static std::stringstream stream;
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
    static void specificRequest() { stringstream() << "specific request" << std::endl; }
};

class Adapter : public Target
{
public:
    Adapter() : adaptee() {}
    ~Adapter() override { adaptee.reset(); }

    void request() override { adaptee->specificRequest(); }

private:
    std::shared_ptr<Adaptee> adaptee;
};

} // namespace adapter

void Structural::adapterInstance()
{
    using adapter::Adapter;
    using adapter::stringstream;
    using adapter::Target;

    std::shared_ptr<Target> t = std::make_shared<Adapter>();
    t->request();

    COMMON_PRINT(STRUCTURAL_RESULT, "Adapter", stringstream().str().c_str());
}

// bridge
namespace bridge
{
static std::stringstream& stringstream()
{
    static std::stringstream stream;
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

    void action() override { stringstream() << "concrete implementor A" << std::endl; }
};

class ConcreteImplementorB : public Implementor
{
public:
    ~ConcreteImplementorB() override = default;

    void action() override { stringstream() << "concrete implementor B" << std::endl; }
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
    explicit RefinedAbstraction(std::shared_ptr<Implementor> implementor) : implementor(implementor) {}

    void operation() override { return implementor->action(); }

private:
    std::shared_ptr<Implementor> implementor;
};
} // namespace bridge

void Structural::bridgeInstance()
{
    using bridge::Abstraction;
    using bridge::ConcreteImplementorA;
    using bridge::ConcreteImplementorB;
    using bridge::Implementor;
    using bridge::RefinedAbstraction;
    using bridge::stringstream;

    std::shared_ptr<Implementor> implA = std::make_shared<ConcreteImplementorA>();
    std::shared_ptr<Implementor> implB = std::make_shared<ConcreteImplementorB>();

    std::shared_ptr<Abstraction> abstract1 = std::make_shared<RefinedAbstraction>(implA);
    abstract1->operation();

    std::shared_ptr<Abstraction> abstract2 = std::make_shared<RefinedAbstraction>(implB);
    abstract2->operation();

    COMMON_PRINT(STRUCTURAL_RESULT, "Bridge", stringstream().str().c_str());
}

// composite
namespace composite
{
static std::stringstream& stringstream()
{
    static std::stringstream stream;
    return stream;
}

class Component
{
public:
    virtual ~Component() = default;

    virtual std::shared_ptr<Component> getChild(const uint32_t /*unused*/) { return nullptr; }
    virtual void add(std::shared_ptr<Component> /*unused*/) {}
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
    void add(std::shared_ptr<Component> component) override { children.emplace_back(component); }
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

    void operation() override { stringstream() << "leaf " << id << " operation" << std::endl; }

private:
    int id;
};
} // namespace composite

void Structural::compositeInstance()
{
    using composite::Composite;
    using composite::Leaf;
    using composite::stringstream;

    constexpr uint32_t count = 5;
    Composite composite;

    for (uint32_t i = 0; i < count; ++i)
    {
        composite.add(std::make_shared<Leaf>(i));
    }
    composite.remove(0);
    composite.operation();

    COMMON_PRINT(STRUCTURAL_RESULT, "Composite", stringstream().str().c_str());
}

// decorator
namespace decorator
{
static std::stringstream& stringstream()
{
    static std::stringstream stream;
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

    void operation() override { stringstream() << "concrete component operation" << std::endl; }
};

class Decorator : public Component
{
public:
    ~Decorator() override = default;
    explicit Decorator(std::shared_ptr<Component> component) : component(component) {}

    void operation() override { component->operation(); }

private:
    std::shared_ptr<Component> component;
};

class ConcreteDecoratorA : public Decorator
{
public:
    explicit ConcreteDecoratorA(std::shared_ptr<Component> decorator) : Decorator(decorator) {}

    void operation() override
    {
        Decorator::operation();
        stringstream() << "decorator A" << std::endl;
    }
};

class ConcreteDecoratorB : public Decorator
{
public:
    explicit ConcreteDecoratorB(std::shared_ptr<Component> decorator) : Decorator(decorator) {}

    void operation() override
    {
        Decorator::operation();
        stringstream() << "decorator B" << std::endl;
    }
};
} // namespace decorator

void Structural::decoratorInstance()
{
    using decorator::Component;
    using decorator::ConcreteComponent;
    using decorator::ConcreteDecoratorA;
    using decorator::ConcreteDecoratorB;
    using decorator::stringstream;

    std::shared_ptr<ConcreteComponent> cc = std::make_shared<ConcreteComponent>();
    std::shared_ptr<ConcreteDecoratorA> da = std::make_shared<ConcreteDecoratorA>(cc);
    std::shared_ptr<ConcreteDecoratorB> db = std::make_shared<ConcreteDecoratorB>(da);

    std::shared_ptr<Component> component = db;
    component->operation();

    COMMON_PRINT(STRUCTURAL_RESULT, "Decorator", stringstream().str().c_str());
}

// facade
namespace facade
{
static std::stringstream& stringstream()
{
    static std::stringstream stream;
    return stream;
}

class SubsystemA
{
public:
    static void suboperation() { stringstream() << "subsystem A method" << std::endl; }
};

class SubsystemB
{
public:
    static void suboperation() { stringstream() << "subsystem B method" << std::endl; }
};

class SubsystemC
{
public:
    static void suboperation() { stringstream() << "subsystem C method" << std::endl; }
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

void Structural::facadeInstance()
{
    using facade::Facade;
    using facade::stringstream;

    std::shared_ptr<Facade> facade = std::make_shared<Facade>();

    facade->operation1();
    facade->operation2();

    COMMON_PRINT(STRUCTURAL_RESULT, "Facade", stringstream().str().c_str());
}

// flyweight
namespace flyweight
{
static std::stringstream& stringstream()
{
    static std::stringstream stream;
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

    void operation() override { stringstream() << "unshared flyweight with state " << state << std::endl; }

private:
    int state;
};

class ConcreteFlyweight : public Flyweight
{
public:
    explicit ConcreteFlyweight(const int allState) : state(allState) {}
    ~ConcreteFlyweight() override = default;

    void operation() override { stringstream() << "concrete flyweight with state " << state << std::endl; }

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

    std::shared_ptr<Flyweight> getFlyweight(const int key)
    {
        if (flies.find(key) != flies.cend())
        {
            return flies[key];
        }
        std::shared_ptr<Flyweight> fly = std::make_shared<ConcreteFlyweight>(key);
        flies.insert(std::pair<int, std::shared_ptr<Flyweight>>(key, fly));
        return fly;
    }

private:
    std::map<int, std::shared_ptr<Flyweight>> flies;
};
} // namespace flyweight

void Structural::flyweightInstance()
{
    using flyweight::FlyweightFactory;
    using flyweight::stringstream;

    std::shared_ptr<FlyweightFactory> factory = std::make_shared<FlyweightFactory>();
    factory->getFlyweight(1)->operation();
    factory->getFlyweight(2)->operation();

    COMMON_PRINT(STRUCTURAL_RESULT, "Flyweight", stringstream().str().c_str());
}

// proxy
namespace proxy
{
static std::stringstream& stringstream()
{
    static std::stringstream stream;
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
    void request() override { stringstream() << "real subject request" << std::endl; }
};

class Proxy : public Subject
{
public:
    Proxy() { subject = std::make_shared<RealSubject>(); }
    ~Proxy() override { subject.reset(); }

    void request() override { subject->request(); }

private:
    std::shared_ptr<RealSubject> subject;
};
} // namespace proxy

void Structural::proxyInstance()
{
    using proxy::Proxy;
    using proxy::stringstream;

    std::shared_ptr<Proxy> proxy = std::make_shared<Proxy>();
    proxy->request();

    COMMON_PRINT(STRUCTURAL_RESULT, "Proxy", stringstream().str().c_str());
}
} // namespace dp_structural