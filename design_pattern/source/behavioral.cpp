//! @file behavioral.cpp
//! @author ryftchen
//! @brief The definitions (behavioral) in the data structure module.
//! @version 1.0
//! @copyright Copyright (c) 2022-2023 ryftchen. All rights reserved.

#include "behavioral.hpp"

#include <algorithm>

namespace design_pattern::behavioral
{
//! @brief The chain of responsibility pattern.
namespace chain_of_responsibility
{
void Handler::setHandler(std::shared_ptr<Handler> handler)
{
    successor = std::move(handler);
}

void Handler::handleRequest()
{
    if (successor)
    {
        successor->handleRequest();
    }
}

bool ConcreteHandler1::canHandle()
{
    return false;
}

void ConcreteHandler1::handleRequest()
{
    if (canHandle())
    {
        output() << "handled by concrete handler 1\n";
    }
    else
    {
        output() << "cannot be handled by handler 1\n";
        Handler::handleRequest();
    }
}

bool ConcreteHandler2::canHandle()
{
    return true;
}

void ConcreteHandler2::handleRequest()
{
    if (canHandle())
    {
        output() << "handled by handler 2\n";
    }
    else
    {
        output() << "cannot be handled by handler 2\n";
        Handler::handleRequest();
    }
}

//! @brief Output stream for the chain of responsibility pattern. Need to be cleared manually.
//! @return reference of the output stream object, which is on string based
std::ostringstream& output()
{
    static std::ostringstream stream{};
    return stream;
}
} // namespace chain_of_responsibility

//! @brief The command pattern.
namespace command
{
void Receiver::action()
{
    output() << "receiver: execute action\n";
}

ConcreteCommand::~ConcreteCommand()
{
    if (auto r = receiver.lock())
    {
        r.reset();
    }
}

void ConcreteCommand::execute()
{
    receiver.lock()->action();
}

void Invoker::set(const std::shared_ptr<Command>& c)
{
    command = c;
}

void Invoker::confirm()
{
    if (const auto c = command.lock())
    {
        c->execute();
    }
}

//! @brief Output stream for the command pattern. Need to be cleared manually.
//! @return reference of the output stream object, which is on string based
std::ostringstream& output()
{
    static std::ostringstream stream{};
    return stream;
}
} // namespace command

//! @brief The interpreter pattern.
namespace interpreter
{
void Context::set(const std::string& expr, const bool val)
{
    vars.insert(std::pair<std::string, bool>(expr, val));
}

bool Context::get(const std::string& expr)
{
    return vars[expr];
}

bool AbstractExpression::interpret(const std::shared_ptr<Context> /*context*/)
{
    return false;
}

bool TerminalExpression::interpret(const std::shared_ptr<Context> context)
{
    return context->get(value);
}

NonTerminalExpression::~NonTerminalExpression()
{
    leftOp.reset();
    rightOp.reset();
}

bool NonTerminalExpression::interpret(const std::shared_ptr<Context> context)
{
    return (leftOp->interpret(context) && rightOp->interpret(context));
}

//! @brief Output stream for the interpreter pattern. Need to be cleared manually.
//! @return reference of the output stream object, which is on string based
std::ostringstream& output()
{
    static std::ostringstream stream{};
    return stream;
}
} // namespace interpreter

//! @brief The iterator pattern.
namespace iterator
{
ConcreteAggregate::ConcreteAggregate(const std::uint32_t size)
{
    list = std::make_unique<int[]>(size);
    std::fill(list.get(), list.get() + size, 1);
    count = size;
}

ConcreteAggregate::~ConcreteAggregate()
{
    list.reset();
}

std::shared_ptr<Iterator> ConcreteAggregate::createIterator()
{
    return std::make_shared<ConcreteIterator>(shared_from_this());
}

std::uint32_t ConcreteAggregate::size() const
{
    return count;
}

int ConcreteAggregate::at(std::uint32_t index)
{
    return list[index];
}

void ConcreteIterator::first()
{
    index = 0;
}

void ConcreteIterator::next()
{
    ++index;
}

bool ConcreteIterator::isDone() const
{
    return (index >= list->size());
}

int ConcreteIterator::currentItem() const
{
    if (isDone())
    {
        return -1;
    }
    return list->at(index);
}

//! @brief Output stream for the iterator pattern. Need to be cleared manually.
//! @return reference of the output stream object, which is on string based
std::ostringstream& output()
{
    static std::ostringstream stream{};
    return stream;
}
} // namespace iterator

//! @brief The mediator pattern.
namespace mediator
{
ConcreteMediator::~ConcreteMediator()
{
    std::for_each(
        colleagues.begin(),
        colleagues.end(),
        [](auto& colleague)
        {
            colleague.reset();
        });
    colleagues.clear();
}

void ConcreteMediator::add(const std::shared_ptr<Colleague>& colleague)
{
    colleagues.emplace_back(colleague);
}

void ConcreteMediator::distribute(const std::shared_ptr<Colleague>& sender, const std::string& msg)
{
    std::for_each(
        colleagues.cbegin(),
        colleagues.cend(),
        [&sender, &msg](const auto& colleague)
        {
            if (const auto c = colleague.lock())
            {
                if (c->getId() != sender->getId())
                {
                    c->receive(msg);
                }
            }
        });
}

std::uint32_t Colleague::getId() const
{
    return id;
}

void ConcreteColleague::send(const std::string& msg)
{
    output() << "message \"" << msg << "\" sent by colleague " << id << '\n';
    mediator.lock()->distribute(shared_from_this(), msg);
}

void ConcreteColleague::receive(const std::string& msg)
{
    output() << "message \"" << msg << "\" received by colleague " << id << '\n';
}

//! @brief Output stream for the mediator pattern. Need to be cleared manually.
//! @return reference of the output stream object, which is on string based
std::ostringstream& output()
{
    static std::ostringstream stream{};
    return stream;
}
} // namespace mediator

//! @brief The memento pattern.
namespace memento
{
void Memento::setState(const int s)
{
    state = s;
}

int Memento::getState() const
{
    return state;
}

void Originator::setState(const int s)
{
    output() << "set state to " << s << '\n';
    state = s;
}

int Originator::getState() const
{
    return state;
}

void Originator::setMemento(const std::shared_ptr<Memento> memento)
{
    state = memento->getState();
}

std::shared_ptr<Memento> Originator::createMemento() const
{
    return std::shared_ptr<Memento>(new Memento(state));
}

CareTaker::~CareTaker()
{
    std::for_each(
        history.begin(),
        history.end(),
        [](auto& memento)
        {
            memento.reset();
        });
    history.clear();
}

void CareTaker::save()
{
    output() << "save state\n";
    history.emplace_back(originator->createMemento());
}

void CareTaker::undo()
{
    if (history.empty())
    {
        output() << "unable to undo state\n";
        return;
    }

    std::shared_ptr<Memento> memento = history.back();
    originator->setMemento(memento);
    output() << "undo state\n";

    history.pop_back();
    memento.reset();
}

//! @brief Output stream for the memento pattern. Need to be cleared manually.
//! @return reference of the output stream object, which is on string based
std::ostringstream& output()
{
    static std::ostringstream stream{};
    return stream;
}
} // namespace memento

//! @brief The observer pattern.
namespace observer
{
void Subject::attach(const std::shared_ptr<Observer>& observer)
{
    observers.emplace_back(observer);
}

void Subject::detach(const int index)
{
    observers.erase(observers.begin() + index);
}

void Subject::notify()
{
    std::for_each(
        observers.cbegin(),
        observers.cend(),
        [this](const auto& observer)
        {
            observer->update(shared_from_this());
        });
}

int ConcreteObserver::getState()
{
    return observerState;
}

void ConcreteObserver::update(const std::shared_ptr<Subject>& subject)
{
    observerState = subject->getState();
    output() << "observer state updated\n";
}

int ConcreteSubject::getState()
{
    return subjectState;
}

void ConcreteSubject::setState(const int s)
{
    subjectState = s;
}

//! @brief Output stream for the observer pattern. Need to be cleared manually.
//! @return reference of the output stream object, which is on string based
std::ostringstream& output()
{
    static std::ostringstream stream{};
    return stream;
}
} // namespace observer

//! @brief The state pattern.
namespace state
{
void ConcreteStateA::handle()
{
    output() << "state A handled\n";
}

void ConcreteStateB::handle()
{
    output() << "state B handled\n";
}

Context::~Context()
{
    state.reset();
}

void Context::setState(std::unique_ptr<State> s)
{
    if (state)
    {
        state.reset();
    }
    state = std::move(s);
}

void Context::request()
{
    state->handle();
}

//! @brief Output stream for the state pattern. Need to be cleared manually.
//! @return reference of the output stream object, which is on string based
std::ostringstream& output()
{
    static std::ostringstream stream{};
    return stream;
}
} // namespace state

//! @brief The strategy pattern.
namespace strategy
{
void ConcreteStrategyA::algorithmInterface()
{
    output() << "concrete strategy A\n";
}

void ConcreteStrategyB::algorithmInterface()
{
    output() << "concrete strategy B\n";
}

Context::~Context()
{
    strategy.reset();
}

void Context::contextInterface()
{
    strategy->algorithmInterface();
}

//! @brief Output stream for the strategy pattern. Need to be cleared manually.
//! @return reference of the output stream object, which is on string based
std::ostringstream& output()
{
    static std::ostringstream stream{};
    return stream;
}
} // namespace strategy

//! @brief The template method pattern.
namespace template_method
{
void AbstractClass::templateMethod()
{
    primitiveOperation1();
    primitiveOperation2();
}

void ConcreteClass::primitiveOperation1()
{
    output() << "primitive operation 1\n";
}

void ConcreteClass::primitiveOperation2()
{
    output() << "primitive operation 2\n";
}

//! @brief Output stream for the template method pattern. Need to be cleared manually.
//! @return reference of the output stream object, which is on string based
std::ostringstream& output()
{
    static std::ostringstream stream{};
    return stream;
}
} // namespace template_method

//! @brief The visitor pattern.
namespace visitor
{
void ConcreteVisitor1::visitElementA(const std::shared_ptr<ConcreteElementA>& /*element*/)
{
    output() << "concrete visitor 1: element A visited\n";
}

void ConcreteVisitor1::visitElementB(const std::shared_ptr<ConcreteElementB>& /*element*/)
{
    output() << "concrete visitor 1: element B visited\n";
}

void ConcreteVisitor2::visitElementA(const std::shared_ptr<ConcreteElementA>& /*element*/)
{
    output() << "concrete visitor 2: element A visited\n";
}

void ConcreteVisitor2::visitElementB(const std::shared_ptr<ConcreteElementB>& /*element*/)
{
    output() << "concrete visitor 2: element B visited\n";
}

void ConcreteElementA::accept(Visitor& visitor)
{
    visitor.visitElementA(shared_from_this());
}

void ConcreteElementB::accept(Visitor& visitor)
{
    visitor.visitElementB(shared_from_this());
}

//! @brief Output stream for the visitor pattern. Need to be cleared manually.
//! @return reference of the output stream object, which is on string based
std::ostringstream& output()
{
    static std::ostringstream stream{};
    return stream;
}
} // namespace visitor
} // namespace design_pattern::behavioral
