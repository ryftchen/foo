//! @file structural.cpp
//! @author ryftchen
//! @brief The definitions (structural) in the data structure module.
//! @version 0.1.0
//! @copyright Copyright (c) 2022-2025 ryftchen. All rights reserved.

#include "structural.hpp"

#include <algorithm>

namespace design_pattern::structural
{
//! @brief Function version number.
//! @return version number (major.minor.patch)
const char* version() noexcept
{
    static const char* const ver = "0.1.0";
    return ver;
}

namespace adapter
{
void Adaptee::specificRequest()
{
    output() << "specific request\n";
}

Adapter::~Adapter()
{
    adaptee.reset();
}

void Adapter::request()
{
    adaptee->specificRequest();
}

//! @brief Output stream for the adapter pattern. Need to be cleared manually.
//! @return reference of the output stream object, which is on string based
std::ostringstream& output() noexcept
{
    static std::ostringstream process{};
    return process;
}
} // namespace adapter

namespace bridge
{
void ConcreteImplementorA::action()
{
    output() << "concrete implementor A\n";
}

void ConcreteImplementorB::action()
{
    output() << "concrete implementor B\n";
}

void RefinedAbstraction::operation()
{
    implementor->action();
}

//! @brief Output stream for the bridge pattern. Need to be cleared manually.
//! @return reference of the output stream object, which is on string based
std::ostringstream& output() noexcept
{
    static std::ostringstream process{};
    return process;
}
} // namespace bridge

namespace composite
{
std::shared_ptr<Component> Component::getChild(const std::uint32_t /*index*/)
{
    return nullptr;
}

void Component::add(const std::shared_ptr<Component>& /*component*/)
{
}

void Component::remove(const std::uint32_t /*index*/)
{
}

Composite::~Composite()
{
    std::for_each(children.begin(), children.end(), [](auto& component) { component.reset(); });
}

std::shared_ptr<Component> Composite::getChild(const std::uint32_t index)
{
    return children.at(index);
}

void Composite::add(const std::shared_ptr<Component>& component)
{
    children.emplace_back(component);
}

void Composite::remove(const std::uint32_t index)
{
    std::shared_ptr<Component> child = children.at(index);
    children.erase(children.cbegin() + index);
    child.reset();
}

void Composite::operation()
{
    std::for_each(children.cbegin(), children.cend(), [](const auto& component) { component->operation(); });
}

void Leaf::operation()
{
    output() << "leaf " << id << " operation\n";
}

//! @brief Output stream for the composite pattern. Need to be cleared manually.
//! @return reference of the output stream object, which is on string based
std::ostringstream& output() noexcept
{
    static std::ostringstream process{};
    return process;
}
} // namespace composite

namespace decorator
{
void ConcreteComponent::operation()
{
    output() << "concrete component operation\n";
}

void Decorator::operation()
{
    component->operation();
}

void ConcreteDecoratorA::operation()
{
    Decorator::operation();
    output() << "decorator A\n";
}

//! @brief The related operation.
void ConcreteDecoratorB::operation()
{
    Decorator::operation();
    output() << "decorator B\n";
}

//! @brief Output stream for the decorator pattern. Need to be cleared manually.
//! @return reference of the output stream object, which is on string based
std::ostringstream& output() noexcept
{
    static std::ostringstream process{};
    return process;
}
} // namespace decorator

namespace facade
{
void SubsystemA::suboperation()
{
    output() << "subsystem A method\n";
}

void SubsystemB::suboperation()
{
    output() << "subsystem B method\n";
}

void SubsystemC::suboperation()
{
    output() << "subsystem C method\n";
}

void Facade::operation1()
{
    subsystemA->suboperation();
    subsystemB->suboperation();
}

void Facade::operation2()
{
    subsystemC->suboperation();
}

//! @brief Output stream for the facade pattern. Need to be cleared manually.
//! @return reference of the output stream object, which is on string based
std::ostringstream& output() noexcept
{
    static std::ostringstream process{};
    return process;
}
} // namespace facade

namespace flyweight
{
void UnsharedConcreteFlyweight::operation()
{
    output() << "unshared flyweight with state " << state << '\n';
}

void ConcreteFlyweight::operation()
{
    output() << "concrete flyweight with state " << state << '\n';
}

FlyweightFactory::~FlyweightFactory()
{
    std::for_each(flies.begin(), flies.end(), [](auto& file) { file.second.reset(); });
    flies.clear();
}

std::unique_ptr<Flyweight>& FlyweightFactory::getFlyweight(const int key)
{
    if (flies.contains(key))
    {
        return flies[key];
    }

    std::unique_ptr<Flyweight> fly = std::make_unique<ConcreteFlyweight>(key);
    flies.emplace(key, std::move(fly));

    return flies[key];
}

//! @brief Output stream for the flyweight pattern. Need to be cleared manually.
//! @return reference of the output stream object, which is on string based
std::ostringstream& output() noexcept
{
    static std::ostringstream process{};
    return process;
}
} // namespace flyweight

namespace proxy
{
void RealSubject::request()
{
    output() << "real subject request\n";
}

Proxy::~Proxy()
{
    if (subject)
    {
        subject.reset();
    }
}

void Proxy::request()
{
    realSubject().request();
}

RealSubject& Proxy::realSubject()
{
    if (!subject)
    {
        subject = std::make_unique<RealSubject>();
    }

    return *subject;
}

//! @brief Output stream for the proxy pattern. Need to be cleared manually.
//! @return reference of the output stream object, which is on string based
std::ostringstream& output() noexcept
{
    static std::ostringstream process{};
    return process;
}
} // namespace proxy
} // namespace design_pattern::structural
