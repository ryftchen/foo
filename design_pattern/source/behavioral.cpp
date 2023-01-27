//! @file behavioral.cpp
//! @author ryftchen
//! @brief The definitions (behavioral) in the data structure module.
//! @version 0.1
//! @copyright Copyright (c) 2022-2023
#include "behavioral.hpp"
#include <map>
#include <memory>
#include <vector>
#ifndef _NO_PRINT_AT_RUNTIME
#include "utility/include/common.hpp"

//! @brief Display behavioral result.
#define BEHAVIORAL_RESULT "\r\n*%-21s instance:\r\n%s"
//! @brief Print behavioral result content.
#define BEHAVIORAL_PRINT_RESULT_CONTENT(method)                      \
    COMMON_PRINT(BEHAVIORAL_RESULT, method, output().str().c_str()); \
    output().clear()
#else
#include <sstream>

//! @brief Print behavioral result content.
#define BEHAVIORAL_PRINT_RESULT_CONTENT(method) output().clear()
#endif

namespace design_pattern::behavioral
{
BehavioralPattern::BehavioralPattern()
{
#ifndef _NO_PRINT_AT_RUNTIME
    std::cout << "\r\nBehavioral pattern:" << std::endl;
#endif
}

//! @brief The chain of responsibility pattern.
namespace chain_of_responsibility
{
//! @brief Output stream for the chain of responsibility pattern. Need to be cleared manually.
//! @return reference of output stream object, which is on string based.
static std::ostringstream& output()
{
    static std::ostringstream stream;
    return stream;
}

//! @brief Handle requests that they are responsible for. Optionally implement the successor link.
class Handler
{
public:
    //! @brief Destroy the Handler object.
    virtual ~Handler() = default;

    //! @brief Set the handler.
    //! @param handler - target handler
    virtual void setHandler(std::shared_ptr<Handler> handler) { successor = std::move(handler); }
    //! @brief Handle the request.
    virtual void handleRequest()
    {
        if (successor)
        {
            successor->handleRequest();
        }
    }

private:
    //! @brief The successor.
    std::shared_ptr<Handler> successor;
};

//! @brief The concrete handler.
class ConcreteHandler1 : public Handler
{
public:
    //! @brief Destroy the ConcreteHandler1 object.
    ~ConcreteHandler1() override = default;

    //! @brief Check whether it can handle the request.
    //! @return can handle or not
    static bool canHandle() { return false; }
    //! @brief Handle the request.
    void handleRequest() override
    {
        if (canHandle())
        {
            output() << "handled by concrete handler 1" << std::endl;
        }
        else
        {
            output() << "cannot be handled by handler 1" << std::endl;
            Handler::handleRequest();
        }
    }
};

//! @brief The concrete handler.
class ConcreteHandler2 : public Handler
{
public:
    //! @brief Destroy the ConcreteHandler2 object.
    ~ConcreteHandler2() override = default;

    //! @brief Check whether it can handle the request.
    //! @return can handle or not
    static bool canHandle() { return true; }
    //! @brief Handle the request.
    void handleRequest() override
    {
        if (canHandle())
        {
            output() << "handled by handler 2" << std::endl;
        }
        else
        {
            output() << "cannot be handled by handler 2" << std::endl;
            Handler::handleRequest();
        }
    }
};
} // namespace chain_of_responsibility

void BehavioralPattern::chainOfResponsibilityInstance() const // NOLINT(readability-convert-member-functions-to-static)
{
    using chain_of_responsibility::ConcreteHandler1;
    using chain_of_responsibility::ConcreteHandler2;
    using chain_of_responsibility::output;

    std::shared_ptr<ConcreteHandler1> handler1 = std::make_shared<ConcreteHandler1>();
    std::shared_ptr<ConcreteHandler2> handler2 = std::make_shared<ConcreteHandler2>();

    handler1->setHandler(handler2);
    handler1->handleRequest();

    BEHAVIORAL_PRINT_RESULT_CONTENT("ChainOfResponsibility");
}

//! @brief The command pattern.
namespace command
{
//! @brief Output stream for the command pattern. Need to be cleared manually.
//! @return reference of output stream object, which is on string based.
static std::ostringstream& output()
{
    static std::ostringstream stream;
    return stream;
}

//! @brief Receiver associated with the command.
class Receiver
{
public:
    //! @brief Perform the operations associated with carrying out the request.
    static void action() { output() << "receiver: execute action" << std::endl; }
};

//! @brief Implement the execution in command.
class Command
{
public:
    //! @brief Destroy the Command object.
    virtual ~Command() = default;

    //! @brief Execute the command.
    virtual void execute() = 0;

protected:
    //! @brief Construct a new Command object.
    Command() = default;
};

//! @brief The concrete command.
class ConcreteCommand : public Command
{
public:
    //! @brief Construct a new ConcreteCommand object.
    //! @param receiver - receiver associated with the command
    explicit ConcreteCommand(const std::shared_ptr<Receiver>& receiver) : receiver(receiver) {}

    //! @brief Destroy the ConcreteCommand object.
    ~ConcreteCommand() override
    {
        if (auto r = receiver.lock())
        {
            r.reset();
        }
    }

    //! @brief Execute the command.
    void execute() override { receiver.lock()->action(); }

private:
    //! @brief The receiver.
    std::weak_ptr<Receiver> receiver;
};

//! @brief Invoke the corresponding operation.
class Invoker
{
public:
    //! @brief Set the command.
    //! @param c - command
    void set(const std::shared_ptr<Command>& c) { command = c; }
    //! @brief Ask the command to carry out the request.
    void confirm()
    {
        if (const auto c = command.lock())
        {
            c->execute();
        }
    }

private:
    //! @brief The command.
    std::weak_ptr<Command> command;
};
} // namespace command

void BehavioralPattern::commandInstance() const // NOLINT(readability-convert-member-functions-to-static)
{
    using command::Command;
    using command::ConcreteCommand;
    using command::Invoker;
    using command::output;
    using command::Receiver;

    std::shared_ptr<ConcreteCommand> command = std::make_shared<ConcreteCommand>((std::make_shared<Receiver>()));

    Invoker invoker;
    invoker.set(command);
    invoker.confirm();

    BEHAVIORAL_PRINT_RESULT_CONTENT("Command");
}

//! @brief The interpreter pattern.
namespace interpreter
{
//! @brief Output stream for the interpreter pattern. Need to be cleared manually.
//! @return reference of output stream object, which is on string based.
static std::ostringstream& output()
{
    static std::ostringstream stream;
    return stream;
}

//! @brief Global context.
class Context
{
public:
    //! @brief Set the pair of the expression and the value.
    //! @param expr - expression
    //! @param val - value
    void set(const std::string& expr, const bool val) { vars.insert(std::pair<std::string, bool>(expr, val)); }
    //! @brief Get the value by expression.
    //! @param expr - expression
    //! @return value
    bool get(const std::string& expr) { return vars[expr]; }

private:
    //! @brief The variables.
    std::map<std::string, bool> vars;
};

//! @brief The abstract interpret operation.
class AbstractExpression
{
public:
    //! @brief Destroy the AbstractExpression object.
    virtual ~AbstractExpression() = default;

    //! @brief The interpret that is common to all nodes in the abstract syntax tree.
    //! @return value
    virtual bool interpret(const std::shared_ptr<Context> /*unused*/) { return false; }
};

//! @brief The terminal interpret operation. An instance is required for every terminal symbol in a sentence.
class TerminalExpression : public AbstractExpression
{
public:
    //! @brief Construct a new TerminalExpression object.
    //! @param value - target value
    explicit TerminalExpression(const std::string& value) : value(value) {}
    //! @brief Destroy the TerminalExpression object.
    ~TerminalExpression() override = default;

    //! @brief The interpret that associated with terminal symbols in the grammar.
    //! @param context - global context
    //! @return value
    bool interpret(const std::shared_ptr<Context> context) override { return context->get(value); }

private:
    //! @brief The value of the terminal expression.
    std::string value;
};

//! @brief The non-terminal interpret operation. One such class is required for every rule in grammar.
class NonTerminalExpression : public AbstractExpression
{
public:
    //! @brief Construct a new NonTerminalExpression object.
    //! @param left - target left operation
    //! @param right - target right operation
    NonTerminalExpression(std::shared_ptr<AbstractExpression> left, std::shared_ptr<AbstractExpression> right) :
        leftOp(left), rightOp(right)
    {
    }
    //! @brief Destroy the NonTerminalExpression object.
    ~NonTerminalExpression() override
    {
        leftOp.reset();
        rightOp.reset();
    }

    //! @brief The interpret that associated with non-terminal symbols in the grammar.
    //! @param context - global context
    //! @return value
    bool interpret(const std::shared_ptr<Context> context) override
    {
        return (leftOp->interpret(context) && rightOp->interpret(context));
    }

private:
    //! @brief The left operation of the non-terminal expression.
    std::shared_ptr<AbstractExpression> leftOp;
    //! @brief The right operation of the non-terminal expression.
    std::shared_ptr<AbstractExpression> rightOp;
};
} // namespace interpreter

void BehavioralPattern::interpreterInstance() const // NOLINT(readability-convert-member-functions-to-static)
{
    using interpreter::AbstractExpression;
    using interpreter::Context;
    using interpreter::NonTerminalExpression;
    using interpreter::output;
    using interpreter::TerminalExpression;

    std::shared_ptr<AbstractExpression> a = std::make_shared<TerminalExpression>("A");
    std::shared_ptr<AbstractExpression> b = std::make_shared<TerminalExpression>("B");
    std::shared_ptr<AbstractExpression> exp = std::make_shared<NonTerminalExpression>(a, b);

    std::shared_ptr<Context> context = std::make_shared<Context>();
    context->set("A", true);
    context->set("B", false);

    output() << context->get("A") << " AND " << context->get("B");
    output() << " = " << exp->interpret(context) << std::endl;

    BEHAVIORAL_PRINT_RESULT_CONTENT("Interpreter");
}

//! @brief The iterator pattern.
namespace iterator
{
//! @brief Output stream for the iterator pattern. Need to be cleared manually.
//! @return reference of output stream object, which is on string based.
static std::ostringstream& output()
{
    static std::ostringstream stream;
    return stream;
}

class Iterator;
class ConcreteAggregate;

//! @brief The aggregate decouples the client from the implementation of the collection of items.
class Aggregate
{
public:
    //! @brief Destroy the Aggregate object.
    virtual ~Aggregate() = default;

    //! @brief Create an iterator.
    //! @return iterator
    virtual std::shared_ptr<Iterator> createIterator() = 0;
};

//! @brief The concrete aggregate.
class ConcreteAggregate : public Aggregate, public std::enable_shared_from_this<ConcreteAggregate>
{
public:
    //! @brief Construct a new ConcreteAggregate object.
    //! @param size - size of the concrete aggregate
    explicit ConcreteAggregate(const uint32_t size)
    {
        list = std::make_unique<int[]>(size);
        std::fill(list.get(), list.get() + size, 1);
        count = size;
    }
    //! @brief Destroy the ConcreteAggregate object.
    ~ConcreteAggregate() override { list.reset(); }

    //! @brief Create an iterator.
    //! @return iterator
    std::shared_ptr<Iterator> createIterator() override;
    //! @brief Get the size of the concrete aggregate.
    //! @return size of the concrete aggregate
    [[nodiscard]] uint32_t size() const { return count; }
    //! @brief Get the item by index.
    //! @param index - index of item
    //! @return item
    int at(uint32_t index) { return list[index]; }

private:
    //! @brief Collection of items.
    std::unique_ptr<int[]> list;
    //! @brief Size of the concrete aggregate.
    uint32_t count;
};

//! @brief Manage the current index of the iterator. A set of methods for traversing over items.
class Iterator
{
public:
    //! @brief Destroy the Iterator object.
    virtual ~Iterator() = default;

    //! @brief Set the current index to the first.
    virtual void first() = 0;
    //! @brief Set the current index to the next.
    virtual void next() = 0;
    //! @brief Check whether the traversal is done.
    //! @return be done or not
    [[nodiscard]] virtual bool isDone() const = 0;
    //! @brief Get the item by current index.
    //! @return current item
    [[nodiscard]] virtual int currentItem() const = 0;
};

//! @brief The concrete iterator.
class ConcreteIterator : public Iterator
{
public:
    //! @brief Construct a new ConcreteIterator object.
    //! @param list - target collection of items
    explicit ConcreteIterator(std::shared_ptr<ConcreteAggregate> list) : list(list), index(0) {}
    //! @brief Destroy the ConcreteIterator object.
    ~ConcreteIterator() override = default;

    //! @brief Set the current index to the first.
    void first() override { index = 0; }
    //! @brief Set the current index to the next.
    void next() override { ++index; }
    //! @brief Check whether the traversal is done.
    //! @return be done or not
    [[nodiscard]] bool isDone() const override { return (index >= list->size()); }
    //! @brief Get the item by current index.
    //! @return current item
    [[nodiscard]] int currentItem() const override
    {
        if (isDone())
        {
            return -1;
        }
        return list->at(index);
    }

private:
    //! @brief Collection of items.
    std::shared_ptr<ConcreteAggregate> list;
    //! @brief Current Index.
    uint32_t index;
};

std::shared_ptr<Iterator> ConcreteAggregate::createIterator()
{
    return std::make_shared<ConcreteIterator>(shared_from_this());
}
} // namespace iterator

void BehavioralPattern::iteratorInstance() const // NOLINT(readability-convert-member-functions-to-static)
{
    using iterator::ConcreteAggregate;
    using iterator::Iterator;
    using iterator::output;

    constexpr uint32_t size = 5;
    std::shared_ptr<ConcreteAggregate> list = std::make_shared<ConcreteAggregate>(size);
    std::shared_ptr<Iterator> iter = list->createIterator();

    for (; !iter->isDone(); iter->next())
    {
        output() << "item value: " << iter->currentItem() << std::endl;
    }

    BEHAVIORAL_PRINT_RESULT_CONTENT("Iterator");
}

//! @brief The mediator pattern.
namespace mediator
{
//! @brief Output stream for the mediator pattern. Need to be cleared manually.
//! @return reference of output stream object, which is on string based.
static std::ostringstream& output()
{
    static std::ostringstream stream;
    return stream;
}

class Mediator;

//! @brief The colleague communicates with its mediator.
//!        Whenever it would have otherwise communicated with another colleague.
class Colleague
{
public:
    //! @brief Construct a new Colleague object.
    //! @param mediator - target mediator
    //! @param id - target id
    Colleague(const std::shared_ptr<Mediator> mediator, const uint32_t id) : mediator(mediator), id(id) {}
    //! @brief Destroy the Colleague object.
    virtual ~Colleague() = default;

    //! @brief Get the id of the colleague.
    //! @return id of the colleague
    [[nodiscard]] uint32_t getID() const { return id; }
    //! @brief Send message.
    //! @param msg - sending message
    virtual void send(const std::string& msg) = 0;
    //! @brief Receive message.
    //! @param msg - receiving message
    virtual void receive(const std::string& msg) = 0;

protected:
    //! @brief Mediator of the colleague.
    std::weak_ptr<Mediator> mediator;
    //! @brief ID of the colleague.
    uint32_t id;
};

//! @brief Implement cooperative behavior by coordinating colleagues.
class Mediator
{
public:
    //! @brief Destroy the Mediator object.
    virtual ~Mediator() = default;

    //! @brief Add colleague.
    //! @param colleague - target colleague
    virtual void add(const std::shared_ptr<Colleague>& colleague) = 0;
    //! @brief Distribute message.
    //! @param sender - sender in colleagues
    //! @param msg - message from sender
    virtual void distribute(const std::shared_ptr<Colleague>& sender, const std::string& msg) = 0;

protected:
    //! @brief Construct a new Mediator object.
    Mediator() = default;
};

//! @brief The concrete colleague.
class ConcreteColleague : public Colleague, public std::enable_shared_from_this<ConcreteColleague>
{
public:
    //! @brief Construct a new ConcreteColleague object.
    //! @param mediator - target mediator
    //! @param id - target id
    ConcreteColleague(const std::shared_ptr<Mediator>& mediator, const uint32_t id) : Colleague(mediator, id) {}
    //! @brief Destroy the ConcreteColleague object.
    ~ConcreteColleague() override = default;

    //! @brief Send message.
    //! @param msg - sending message
    void send(const std::string& msg) override
    {
        output() << "message \"" << msg << "\" sent by colleague " << id << std::endl;
        mediator.lock()->distribute(shared_from_this(), msg);
    }
    //! @brief Receive message.
    //! @param msg - receiving message
    void receive(const std::string& msg) override
    {
        output() << "message \"" << msg << "\" received by colleague " << id << std::endl;
    }
};

//! @brief The concrete mediator.
class ConcreteMediator : public Mediator
{
public:
    //! @brief Destroy the ConcreteMediator object.
    ~ConcreteMediator() override
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

    //! @brief Add colleague.
    //! @param colleague - target colleague
    void add(const std::shared_ptr<Colleague>& colleague) override { colleagues.emplace_back(colleague); }
    //! @brief Distribute message.
    //! @param sender - sender in colleagues
    //! @param msg - message from sender
    void distribute(const std::shared_ptr<Colleague>& sender, const std::string& msg) override
    {
        std::for_each(
            colleagues.cbegin(),
            colleagues.cend(),
            [&sender, &msg](const auto& colleague)
            {
                if (const auto c = colleague.lock())
                {
                    if (c->getID() != sender->getID())
                    {
                        c->receive(msg);
                    }
                }
            });
    }

private:
    //! @brief Collection of colleagues.
    std::vector<std::weak_ptr<Colleague>> colleagues;
};
} // namespace mediator

void BehavioralPattern::mediatorInstance() const // NOLINT(readability-convert-member-functions-to-static)
{
    using mediator::Colleague;
    using mediator::ConcreteColleague;
    using mediator::ConcreteMediator;
    using mediator::Mediator;
    using mediator::output;

    constexpr uint32_t id1 = 1, id2 = 2, id3 = 3;
    std::shared_ptr<Mediator> mediator = std::make_shared<ConcreteMediator>();
    std::shared_ptr<Colleague> c1 = std::make_shared<ConcreteColleague>(mediator, id1);
    std::shared_ptr<Colleague> c2 = std::make_shared<ConcreteColleague>(mediator, id2);
    std::shared_ptr<Colleague> c3 = std::make_shared<ConcreteColleague>(mediator, id3);

    mediator->add(c1);
    mediator->add(c2);
    mediator->add(c3);
    c1->send("Hi!");
    c3->send("Hello!");

    BEHAVIORAL_PRINT_RESULT_CONTENT("Mediator");
}

//! @brief The memento pattern.
namespace memento
{
//! @brief Output stream for the memento pattern. Need to be cleared manually.
//! @return reference of output stream object, which is on string based.
static std::ostringstream& output()
{
    static std::ostringstream stream;
    return stream;
}

//! @brief Store the internal state of the originator. Protect against access by other than the originator.
class Memento
{
private:
    friend class Originator;
    //! @brief Construct a new Memento object.
    //! @param state - target state
    explicit Memento(const int state) : state(state) {}

    //! @brief State of memento.
    int state;
    //! @brief Set the state of memento.
    //! @param s - target state
    void setState(const int s) { state = s; }
    //! @brief Get the state of memento.
    //! @return state of memento
    [[nodiscard]] int getState() const { return state; }
};

//! @brief Create a memento containing a snapshot of its current internal state.
//!        Use the memento to restore its internal state.
class Originator
{
public:
    //! @brief Set the state of originator.
    //! @param s - target state
    void setState(const int s)
    {
        output() << "set state to " << s << std::endl;
        state = s;
    }
    //! @brief Get the state of originator.
    //! @return state of originator
    [[nodiscard]] int getState() const { return state; }
    //! @brief Set the state of originator by memento.
    //! @param memento - target memento
    void setMemento(const std::shared_ptr<Memento> memento) { state = memento->getState(); }
    //! @brief Create a memento.
    //! @return memento
    [[nodiscard]] std::shared_ptr<Memento> createMemento() const
    {
        return std::shared_ptr<Memento>(new Memento(state));
    }

private:
    //! @brief State of originator.
    int state;
};

//! @brief Safeguard memento.
class CareTaker
{
public:
    //! @brief Construct a new CareTaker object.
    //! @param originator - target originator
    explicit CareTaker(const std::shared_ptr<Originator> originator) : originator(originator) {}
    //! @brief Destroy the CareTaker object.
    ~CareTaker()
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

    //! @brief Save the current state to history.
    void save()
    {
        output() << "save state" << std::endl;
        history.emplace_back(originator->createMemento());
    }
    //! @brief Undo the last state.
    void undo()
    {
        if (history.empty())
        {
            output() << "unable to undo state" << std::endl;
            return;
        }

        std::shared_ptr<Memento> memento = history.back();
        originator->setMemento(memento);
        output() << "undo state" << std::endl;

        history.pop_back();
        memento.reset();
    }

private:
    //! @brief Specific originator.
    std::shared_ptr<Originator> originator;
    //! @brief State history.
    std::vector<std::shared_ptr<Memento>> history;
};
} // namespace memento

void BehavioralPattern::mementoInstance() const // NOLINT(readability-convert-member-functions-to-static)
{
    using memento::CareTaker;
    using memento::Originator;
    using memento::output;

    constexpr int state1 = 1, state2 = 2, state3 = 3;
    std::shared_ptr<Originator> originator = std::make_shared<Originator>();
    std::shared_ptr<CareTaker> caretaker = std::make_shared<CareTaker>(originator);

    originator->setState(state1);
    caretaker->save();
    originator->setState(state2);
    caretaker->save();
    originator->setState(state3);
    caretaker->undo();

    output() << "actual state is " << originator->getState() << std::endl;

    BEHAVIORAL_PRINT_RESULT_CONTENT("Memento");
}

//! @brief The observer pattern.
namespace observer
{
//! @brief Output stream for the observer pattern. Need to be cleared manually.
//! @return reference of output stream object, which is on string based.
static std::ostringstream& output()
{
    static std::ostringstream stream;
    return stream;
}

class Subject;

//! @brief Store state of interest to the observer. Send a notification to its observers when its state changes.
class Observer
{
public:
    //! @brief Destroy the Observer object.
    virtual ~Observer() = default;

    //! @brief Get the state of observer.
    //! @return state of observer
    virtual int getState() = 0;
    //! @brief Update the state of observer by subject.
    //! @param subject - target subject
    virtual void update(const std::shared_ptr<Subject>& subject) = 0;
};

//! @brief Attach, detach and notify observers.
class Subject : public std::enable_shared_from_this<Subject>
{
public:
    //! @brief Destroy the Subject object.
    virtual ~Subject() = default;

    //! @brief Attach observer.
    //! @param observer - observer to be attached
    void attach(const std::shared_ptr<Observer>& observer) { observers.emplace_back(observer); }
    //! @brief Detach observer by index.
    //! @param index - observer index
    void detach(const int index) { observers.erase(observers.begin() + index); }
    //! @brief Notify all observers.
    void notify()
    {
        std::for_each(
            observers.cbegin(),
            observers.cend(),
            [this](const auto& observer)
            {
                observer->update(shared_from_this());
            });
    }
    //! @brief Get the state of subject.
    //! @return state of subject
    virtual int getState() = 0;
    //! @brief Set the state of subject.
    //! @param s - target state of subject
    virtual void setState(const int s) = 0;

private:
    //! @brief Collection of observers.
    std::vector<std::shared_ptr<Observer>> observers;
};

//! @brief The concrete observer.
class ConcreteObserver : public Observer
{
public:
    //! @brief Construct a new ConcreteObserver object.
    //! @param state - target state of observer
    explicit ConcreteObserver(const int state) : observerState(state) {}
    //! @brief Destroy the ConcreteObserver object.
    ~ConcreteObserver() override = default;

    //! @brief Get the state of observer.
    //! @return state of observer
    int getState() override { return observerState; }
    //! @brief Update the state of observer by subject.
    //! @param subject - target subject
    void update(const std::shared_ptr<Subject>& subject) override
    {
        observerState = subject->getState();
        output() << "observer state updated" << std::endl;
    }

private:
    //! @brief State of observer.
    int observerState;
};

//! @brief The concrete subject.
class ConcreteSubject : public Subject
{
public:
    //! @brief Construct a new ConcreteSubject object.
    ConcreteSubject() : subjectState() {}
    //! @brief Destroy the ConcreteSubject object.
    ~ConcreteSubject() override = default;

    //! @brief Get the state of subject.
    //! @return state of subject
    int getState() override { return subjectState; }
    //! @brief Set the state of subject.
    //! @param s - target state of subject
    void setState(const int s) override { subjectState = s; }

private:
    //! @brief State of subject.
    int subjectState;
};
} // namespace observer

void BehavioralPattern::observerInstance() const // NOLINT(readability-convert-member-functions-to-static)
{
    using observer::ConcreteObserver;
    using observer::ConcreteSubject;
    using observer::output;
    using observer::Subject;

    constexpr int state1 = 1, state2 = 2, state3 = 3;
    std::shared_ptr<ConcreteObserver> observer1 = std::make_shared<ConcreteObserver>(state1);
    std::shared_ptr<ConcreteObserver> observer2 = std::make_shared<ConcreteObserver>(state2);

    output() << "observer1 state: " << observer1->getState() << std::endl;
    output() << "observer2 state: " << observer2->getState() << std::endl;

    std::shared_ptr<Subject> subject = std::make_shared<ConcreteSubject>();
    subject->attach(observer1);
    subject->attach(observer2);
    subject->setState(state3);
    subject->notify();

    output() << "observer1 state: " << observer1->getState() << std::endl;
    output() << "observer2 state: " << observer2->getState() << std::endl;

    BEHAVIORAL_PRINT_RESULT_CONTENT("Observer");
}

//! @brief The state pattern.
namespace state
{
//! @brief Output stream for the state pattern. Need to be cleared manually.
//! @return reference of output stream object, which is on string based.
static std::ostringstream& output()
{
    static std::ostringstream stream;
    return stream;
}

//! @brief Behaviors associated with a particular state of the context.
class State
{
public:
    //! @brief Destroy the State object.
    virtual ~State() = default;

    //! @brief Handle in the state.
    virtual void handle() = 0;
};

//! @brief The concrete state.
class ConcreteStateA : public State
{
public:
    //! @brief Destroy the ConcreteStateA object.
    ~ConcreteStateA() override = default;

    //! @brief Handle in the state.
    void handle() override { output() << "state A handled" << std::endl; }
};

//! @brief The concrete state.
class ConcreteStateB : public State
{
public:
    //! @brief Destroy the ConcreteStateB object.
    ~ConcreteStateB() override = default;

    //! @brief Handle in the state.
    void handle() override { output() << "state B handled" << std::endl; }
};

//! @brief Interest in clients.
class Context
{
public:
    //! @brief Construct a new Context object.
    Context() : state() {}
    //! @brief Destroy the Context object.
    ~Context() { state.reset(); }

    //! @brief Set the state of context.
    //! @param s - target state
    void setState(std::unique_ptr<State> s)
    {
        if (state)
        {
            state.reset();
        }
        state = std::move(s);
    }

    //! @brief Request handling.
    void request() { state->handle(); }

private:
    //! @brief State of context.
    std::unique_ptr<State> state;
};
} // namespace state

void BehavioralPattern::stateInstance() const // NOLINT(readability-convert-member-functions-to-static)
{
    using state::ConcreteStateA;
    using state::ConcreteStateB;
    using state::Context;
    using state::output;

    std::shared_ptr<Context> context = std::make_shared<Context>();

    context->setState(std::make_unique<ConcreteStateA>());
    context->request();

    context->setState(std::make_unique<ConcreteStateB>());
    context->request();

    BEHAVIORAL_PRINT_RESULT_CONTENT("State");
}

//! @brief The strategy pattern.
namespace strategy
{
//! @brief Output stream for the strategy pattern. Need to be cleared manually.
//! @return reference of output stream object, which is on string based.
static std::ostringstream& output()
{
    static std::ostringstream stream;
    return stream;
}

//! @brief Implement the algorithm using the strategy interface. Common to all supported algorithms.
class Strategy
{
public:
    //! @brief Destroy the Strategy object.
    virtual ~Strategy() = default;

    //! @brief The interface of the algorithm.
    virtual void algorithmInterface() = 0;
};

//! @brief The concrete strategy.
class ConcreteStrategyA : public Strategy
{
public:
    //! @brief Destroy the ConcreteStrategyA object.
    ~ConcreteStrategyA() override = default;

    //! @brief The interface of the algorithm.
    void algorithmInterface() override { output() << "concrete strategy A" << std::endl; }
};

//! @brief The concrete strategy.
class ConcreteStrategyB : public Strategy
{
public:
    //! @brief Destroy the ConcreteStrategyB object.
    ~ConcreteStrategyB() override = default;

    //! @brief The interface of the algorithm.
    void algorithmInterface() override { output() << "concrete strategy B" << std::endl; }
};

//! @brief Maintain reference to the strategy object.
class Context
{
public:
    //! @brief Construct a new Context object.
    //! @param strategy - target strategy
    explicit Context(std::unique_ptr<Strategy> strategy) : strategy(std::move(strategy)) {}
    //! @brief Destroy the Context object.
    ~Context() { strategy.reset(); }

    //! @brief The interface of the context.
    void contextInterface() { strategy->algorithmInterface(); }

private:
    //! @brief Strategy of context.
    std::unique_ptr<Strategy> strategy;
};
} // namespace strategy

void BehavioralPattern::strategyInstance() const // NOLINT(readability-convert-member-functions-to-static)
{
    using strategy::ConcreteStrategyA;
    using strategy::ConcreteStrategyB;
    using strategy::Context;
    using strategy::output;

    Context contextA(std::make_unique<ConcreteStrategyA>());
    contextA.contextInterface();

    Context contextB(std::make_unique<ConcreteStrategyB>());
    contextB.contextInterface();

    BEHAVIORAL_PRINT_RESULT_CONTENT("Strategy");
}

//! @brief The template method pattern.
namespace template_method
{
//! @brief Output stream for the template method pattern. Need to be cleared manually.
//! @return reference of output stream object, which is on string based.
static std::ostringstream& output()
{
    static std::ostringstream stream;
    return stream;
}

//! @brief Implement the template method defining the skeleton of the algorithm.
class AbstractClass
{
public:
    //! @brief Destroy the AbstractClass object.
    virtual ~AbstractClass() = default;

    //! @brief The template method.
    void templateMethod()
    {
        primitiveOperation1();
        primitiveOperation2();
    }
    //! @brief The primitive operation 1.
    virtual void primitiveOperation1() = 0;
    //! @brief The primitive operation 2.
    virtual void primitiveOperation2() = 0;
};

//! @brief Implement the primitive operations to carry out specific steps of the algorithm.
class ConcreteClass : public AbstractClass
{
public:
    //! @brief Destroy the ConcreteClass object.
    ~ConcreteClass() override = default;

    //! @brief The primitive operation 1.
    void primitiveOperation1() override { output() << "primitive operation 1" << std::endl; }
    //! @brief The primitive operation 2.
    void primitiveOperation2() override { output() << "primitive operation 2" << std::endl; }
};
} // namespace template_method

void BehavioralPattern::templateMethodInstance() const // NOLINT(readability-convert-member-functions-to-static)
{
    using template_method::AbstractClass;
    using template_method::ConcreteClass;
    using template_method::output;

    std::shared_ptr<AbstractClass> tm = std::make_shared<ConcreteClass>();
    tm->templateMethod();

    BEHAVIORAL_PRINT_RESULT_CONTENT("TemplateMethod");
}

//! @brief The visitor pattern.
namespace visitor
{
//! @brief Output stream for the visitor pattern. Need to be cleared manually.
//! @return reference of output stream object, which is on string based.
static std::ostringstream& output()
{
    static std::ostringstream stream;
    return stream;
}

class Element;
class ConcreteElementA;
class ConcreteElementB;

//! @brief Implement the algorithm's fragment defined for the structure's corresponding element.
class Visitor
{
public:
    //! @brief Destroy the Visitor object.
    virtual ~Visitor() = default;

    //! @brief Visit element A.
    //! @param element - element to be visited
    virtual void visitElementA(const std::shared_ptr<ConcreteElementA>& element) = 0;
    //! @brief Visit element B.
    //! @param element - element to be visited
    virtual void visitElementB(const std::shared_ptr<ConcreteElementB>& element) = 0;
};

//! @brief The concrete visitor.
class ConcreteVisitor1 : public Visitor
{
public:
    //! @brief Destroy the ConcreteVisitor1 object.
    ~ConcreteVisitor1() override = default;

    //! @brief Visit element A.
    void visitElementA(const std::shared_ptr<ConcreteElementA>& /*element*/) override
    {
        output() << "concrete visitor 1: element A visited" << std::endl;
    }
    //! @brief Visit element B.
    void visitElementB(const std::shared_ptr<ConcreteElementB>& /*element*/) override
    {
        output() << "concrete visitor 1: element B visited" << std::endl;
    }
};

//! @brief The concrete visitor.
class ConcreteVisitor2 : public Visitor
{
public:
    //! @brief Destroy the ConcreteVisitor2 object.
    ~ConcreteVisitor2() override = default;

    //! @brief Visit element A.
    void visitElementA(const std::shared_ptr<ConcreteElementA>& /*element*/) override
    {
        output() << "concrete visitor 2: element A visited" << std::endl;
    }
    //! @brief Visit element B.
    void visitElementB(const std::shared_ptr<ConcreteElementB>& /*element*/) override
    {
        output() << "concrete visitor 2: element B visited" << std::endl;
    }
};

//! @brief Implement the accept operation that takes a visitor as an argument.
class Element
{
public:
    //! @brief Destroy the Element object.
    virtual ~Element() = default;

    //! @brief Accept visitor.
    //! @param visitor - visitor to be accepted
    virtual void accept(Visitor& visitor) = 0;
};

//! @brief The concrete element.
class ConcreteElementA : public Element, public std::enable_shared_from_this<ConcreteElementA>
{
public:
    //! @brief Destroy the ConcreteElementA object.
    ~ConcreteElementA() override = default;

    //! @brief Accept visitor.
    //! @param visitor - visitor to be accepted
    void accept(Visitor& visitor) override { visitor.visitElementA(shared_from_this()); }
};

//! @brief The concrete element.
class ConcreteElementB : public Element, public std::enable_shared_from_this<ConcreteElementB>
{
public:
    //! @brief Destroy the ConcreteElementB object.
    ~ConcreteElementB() override = default;

    //! @brief Accept visitor.
    //! @param visitor - visitor to be accepted
    void accept(Visitor& visitor) override { visitor.visitElementB(shared_from_this()); }
};
} // namespace visitor

void BehavioralPattern::visitorInstance() const // NOLINT(readability-convert-member-functions-to-static)
{
    using visitor::ConcreteElementA;
    using visitor::ConcreteElementB;
    using visitor::ConcreteVisitor1;
    using visitor::ConcreteVisitor2;
    using visitor::output;

    std::shared_ptr<ConcreteElementA> elementA = std::make_shared<ConcreteElementA>();
    std::shared_ptr<ConcreteElementB> elementB = std::make_shared<ConcreteElementB>();
    ConcreteVisitor1 visitor1;
    ConcreteVisitor2 visitor2;

    elementA->accept(visitor1);
    elementA->accept(visitor2);
    elementB->accept(visitor1);
    elementB->accept(visitor2);

    BEHAVIORAL_PRINT_RESULT_CONTENT("Visitor");
}
} // namespace design_pattern::behavioral
