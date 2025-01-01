//! @file behavioral.hpp
//! @author ryftchen
//! @brief The declarations (behavioral) in the design pattern module.
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
//! @brief Behavioral-related functions in the design pattern module.
namespace behavioral
{
extern const char* version() noexcept;

//! @brief The chain of responsibility pattern.
namespace chain_of_responsibility
{
//! @brief Handle requests that they are responsible for. Optionally implement the successor link.
class Handler
{
public:
    //! @brief Destroy the Handler object.
    virtual ~Handler() = default;

    //! @brief Set the handler.
    //! @param handler - target handler
    virtual void setHandler(std::shared_ptr<Handler> handler);
    //! @brief Handle the request.
    virtual void handleRequest();

private:
    //! @brief The successor.
    std::shared_ptr<Handler> successor{};
};

//! @brief The concrete handler.
class ConcreteHandler1 : public Handler
{
public:
    //! @brief Destroy the ConcreteHandler1 object.
    ~ConcreteHandler1() override = default;

    //! @brief Check whether it can handle the request.
    //! @return can handle or not
    static bool canHandle();
    //! @brief Handle the request.
    void handleRequest() override;
};

//! @brief The concrete handler.
class ConcreteHandler2 : public Handler
{
public:
    //! @brief Destroy the ConcreteHandler2 object.
    ~ConcreteHandler2() override = default;

    //! @brief Check whether it can handle the request.
    //! @return can handle or not
    static bool canHandle();
    //! @brief Handle the request.
    void handleRequest() override;
};

extern std::ostringstream& output();
} // namespace chain_of_responsibility

//! @brief The command pattern.
namespace command
{
//! @brief Receiver associated with the command.
class Receiver
{
public:
    //! @brief Perform the operations associated with carrying out the request.
    static void action();
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
    ~ConcreteCommand() override;

    //! @brief Execute the command.
    void execute() override;

private:
    //! @brief The receiver.
    std::weak_ptr<Receiver> receiver{};
};

//! @brief Invoke the corresponding operation.
class Invoker
{
public:
    //! @brief Set the command.
    //! @param c - command
    void set(const std::shared_ptr<Command>& c);
    //! @brief Ask the command to carry out the request.
    void confirm();

private:
    //! @brief The command.
    std::weak_ptr<Command> command{};
};

extern std::ostringstream& output();
} // namespace command

//! @brief The interpreter pattern.
namespace interpreter
{
//! @brief Global context.
class Context
{
public:
    //! @brief Set the pair of the expression and the value.
    //! @param expr - expression
    //! @param val - value
    void set(const std::string_view expr, const bool val);
    //! @brief Get the value by expression.
    //! @param expr - expression
    //! @return value
    bool get(const std::string_view expr);

private:
    //! @brief The variables.
    std::map<std::string, bool> vars{};
};

//! @brief The abstract interpret operation.
class AbstractExpression
{
public:
    //! @brief Destroy the AbstractExpression object.
    virtual ~AbstractExpression() = default;

    //! @brief The interpret that is common to all nodes in the abstract syntax tree.
    //! @param context - global context
    //! @return value
    virtual bool interpret(const std::shared_ptr<Context> context);
};

//! @brief The terminal interpret operation. An instance is required for every terminal symbol in a sentence.
class TerminalExpression : public AbstractExpression
{
public:
    //! @brief Construct a new TerminalExpression object.
    //! @param value - target value
    explicit TerminalExpression(const std::string_view value) : value(value) {}
    //! @brief Destroy the TerminalExpression object.
    ~TerminalExpression() override = default;

    //! @brief The interpret that associated with terminal symbols in the grammar.
    //! @param context - global context
    //! @return value
    bool interpret(const std::shared_ptr<Context> context) override;

private:
    //! @brief The value of the terminal expression.
    std::string value{};
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
    ~NonTerminalExpression() override;

    //! @brief The interpret that associated with non-terminal symbols in the grammar.
    //! @param context - global context
    //! @return value
    bool interpret(const std::shared_ptr<Context> context) override;

private:
    //! @brief The left operation of the non-terminal expression.
    std::shared_ptr<AbstractExpression> leftOp{};
    //! @brief The right operation of the non-terminal expression.
    std::shared_ptr<AbstractExpression> rightOp{};
};

extern std::ostringstream& output();
} // namespace interpreter

//! @brief The iterator pattern.
namespace iterator
{
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
    explicit ConcreteAggregate(const std::uint32_t size);
    //! @brief Destroy the ConcreteAggregate object.
    ~ConcreteAggregate() override;

    //! @brief Create an iterator.
    //! @return iterator
    std::shared_ptr<Iterator> createIterator() override;
    //! @brief Get the size of the concrete aggregate.
    //! @return size of the concrete aggregate
    [[nodiscard]] std::uint32_t size() const;
    //! @brief Get the item by index.
    //! @param index - index of item
    //! @return item
    int at(std::uint32_t index);

private:
    //! @brief Collection of items.
    std::unique_ptr<int[]> list{};
    //! @brief Size of the concrete aggregate.
    std::uint32_t count{0};
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
    void first() override;
    //! @brief Set the current index to the next.
    void next() override;
    //! @brief Check whether the traversal is done.
    //! @return be done or not
    [[nodiscard]] bool isDone() const override;
    //! @brief Get the item by current index.
    //! @return current item
    [[nodiscard]] int currentItem() const override;

private:
    //! @brief Collection of items.
    std::shared_ptr<ConcreteAggregate> list{};
    //! @brief Current Index.
    std::uint32_t index{0};
};

extern std::ostringstream& output();
} // namespace iterator

//! @brief The mediator pattern.
namespace mediator
{
class Colleague;

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
    virtual void distribute(const std::shared_ptr<Colleague>& sender, const std::string_view msg) = 0;

protected:
    //! @brief Construct a new Mediator object.
    Mediator() = default;
};

//! @brief The concrete mediator.
class ConcreteMediator : public Mediator
{
public:
    //! @brief Destroy the ConcreteMediator object.
    ~ConcreteMediator() override;

    //! @brief Add colleague.
    //! @param colleague - target colleague
    void add(const std::shared_ptr<Colleague>& colleague) override;
    //! @brief Distribute message.
    //! @param sender - sender in colleagues
    //! @param msg - message from sender
    void distribute(const std::shared_ptr<Colleague>& sender, const std::string_view msg) override;

private:
    //! @brief Collection of colleagues.
    std::vector<std::weak_ptr<Colleague>> colleagues{};
};

//! @brief The colleague communicates with its mediator.
//!        Whenever it would have otherwise communicated with another colleague.
class Colleague
{
public:
    //! @brief Construct a new Colleague object.
    //! @param mediator - target mediator
    //! @param id - target id
    Colleague(const std::shared_ptr<Mediator> mediator, const std::uint32_t id) : mediator(mediator), id(id) {}
    //! @brief Destroy the Colleague object.
    virtual ~Colleague() = default;

    //! @brief Get the id of the colleague.
    //! @return id of the colleague
    [[nodiscard]] std::uint32_t getId() const;
    //! @brief Send message.
    //! @param msg - sending message
    virtual void send(const std::string_view msg) = 0;
    //! @brief Receive message.
    //! @param msg - receiving message
    virtual void receive(const std::string_view msg) = 0;

protected:
    //! @brief Mediator of the colleague.
    std::weak_ptr<Mediator> mediator{};
    //! @brief Id of the colleague.
    std::uint32_t id{0};
};

//! @brief The concrete colleague.
class ConcreteColleague : public Colleague, public std::enable_shared_from_this<ConcreteColleague>
{
public:
    //! @brief Construct a new ConcreteColleague object.
    //! @param mediator - target mediator
    //! @param id - target id
    ConcreteColleague(const std::shared_ptr<Mediator>& mediator, const std::uint32_t id) : Colleague(mediator, id) {}
    //! @brief Destroy the ConcreteColleague object.
    ~ConcreteColleague() override = default;

    //! @brief Send message.
    //! @param msg - sending message
    void send(const std::string_view msg) override;
    //! @brief Receive message.
    //! @param msg - receiving message
    void receive(const std::string_view msg) override;
};

extern std::ostringstream& output();
} // namespace mediator

//! @brief The memento pattern.
namespace memento
{
//! @brief Store the internal state of the originator. Protect against access by other than the originator.
class Memento
{
private:
    friend class Originator;
    //! @brief Construct a new Memento object.
    //! @param state - target state
    explicit Memento(const int state) : state(state) {}

    //! @brief State of memento.
    int state{0};
    //! @brief Set the state of memento.
    //! @param s - target state
    void setState(const int s);
    //! @brief Get the state of memento.
    //! @return state of memento
    [[nodiscard]] int getState() const;
};

//! @brief Create a memento containing a snapshot of its current internal state.
//!        Use the memento to restore its internal state.
class Originator
{
public:
    //! @brief Set the state of originator.
    //! @param s - target state
    void setState(const int s);
    //! @brief Get the state of originator.
    //! @return state of originator
    [[nodiscard]] int getState() const;
    //! @brief Set the state of originator by memento.
    //! @param memento - target memento
    void setMemento(const std::shared_ptr<Memento> memento);
    //! @brief Create a memento.
    //! @return memento
    [[nodiscard]] std::shared_ptr<Memento> createMemento() const;

private:
    //! @brief State of originator.
    int state{0};
};

//! @brief Safeguard memento.
class CareTaker
{
public:
    //! @brief Construct a new CareTaker object.
    //! @param originator - target originator
    explicit CareTaker(const std::shared_ptr<Originator> originator) : originator(originator) {}
    //! @brief Destroy the CareTaker object.
    ~CareTaker();

    //! @brief Save the current state to history.
    void save();
    //! @brief Undo the last state.
    void undo();

private:
    //! @brief Specific originator.
    std::shared_ptr<Originator> originator{};
    //! @brief State history.
    std::vector<std::shared_ptr<Memento>> history{};
};

extern std::ostringstream& output();
} // namespace memento

//! @brief The observer pattern.
namespace observer
{
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
    virtual void update(const std::weak_ptr<Subject>& subject) = 0;
};

//! @brief Attach, detach and notify observers.
class Subject : public std::enable_shared_from_this<Subject>
{
public:
    //! @brief Destroy the Subject object.
    virtual ~Subject() = default;

    //! @brief Attach observer.
    //! @param observer - observer to be attached
    void attach(const std::shared_ptr<Observer>& observer);
    //! @brief Detach observer by index.
    //! @param index - observer index
    void detach(const int index);
    //! @brief Notify all observers.
    void notify();
    //! @brief Get the state of subject.
    //! @return state of subject
    virtual int getState() = 0;
    //! @brief Set the state of subject.
    //! @param s - target state of subject
    virtual void setState(const int s) = 0;

private:
    //! @brief Collection of observers.
    std::vector<std::weak_ptr<Observer>> observers{};
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
    int getState() override;
    //! @brief Update the state of observer by subject.
    //! @param subject - target subject
    void update(const std::weak_ptr<Subject>& subject) override;

private:
    //! @brief State of observer.
    int observerState{0};
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
    int getState() override;
    //! @brief Set the state of subject.
    //! @param s - target state of subject
    void setState(const int s) override;

private:
    //! @brief State of subject.
    int subjectState{0};
};

extern std::ostringstream& output();
} // namespace observer

//! @brief The state pattern.
namespace state
{
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
    void handle() override;
};

//! @brief The concrete state.
class ConcreteStateB : public State
{
public:
    //! @brief Destroy the ConcreteStateB object.
    ~ConcreteStateB() override = default;

    //! @brief Handle in the state.
    void handle() override;
};

//! @brief Interest in clients.
class Context
{
public:
    //! @brief Construct a new Context object.
    Context() : state() {}
    //! @brief Destroy the Context object.
    ~Context();

    //! @brief Set the state of context.
    //! @param s - target state
    void setState(std::unique_ptr<State> s);
    //! @brief Request handling.
    void request();

private:
    //! @brief State of context.
    std::unique_ptr<State> state{};
};

extern std::ostringstream& output();
} // namespace state

//! @brief The strategy pattern.
namespace strategy
{
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
    void algorithmInterface() override;
};

//! @brief The concrete strategy.
class ConcreteStrategyB : public Strategy
{
public:
    //! @brief Destroy the ConcreteStrategyB object.
    ~ConcreteStrategyB() override = default;

    //! @brief The interface of the algorithm.
    void algorithmInterface() override;
};

//! @brief Maintain reference to the strategy object.
class Context
{
public:
    //! @brief Construct a new Context object.
    //! @param strategy - target strategy
    explicit Context(std::unique_ptr<Strategy> strategy) : strategy(std::move(strategy)) {}
    //! @brief Destroy the Context object.
    ~Context();

    //! @brief The interface of the context.
    void contextInterface();

private:
    //! @brief Strategy of context.
    std::unique_ptr<Strategy> strategy{};
};

extern std::ostringstream& output();
} // namespace strategy

//! @brief The template method pattern.
namespace template_method
{
//! @brief Implement the template method defining the skeleton of the algorithm.
class AbstractClass
{
public:
    //! @brief Destroy the AbstractClass object.
    virtual ~AbstractClass() = default;

    //! @brief The template method.
    void templateMethod();
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
    void primitiveOperation1() override;
    //! @brief The primitive operation 2.
    void primitiveOperation2() override;
};

extern std::ostringstream& output();
} // namespace template_method

//! @brief The visitor pattern.
namespace visitor
{
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
    //! @param element - element to be visited
    void visitElementA(const std::shared_ptr<ConcreteElementA>& element) override;
    //! @brief Visit element B.
    //! @param element - element to be visited
    void visitElementB(const std::shared_ptr<ConcreteElementB>& element) override;
};

//! @brief The concrete visitor.
class ConcreteVisitor2 : public Visitor
{
public:
    //! @brief Destroy the ConcreteVisitor2 object.
    ~ConcreteVisitor2() override = default;

    //! @brief Visit element A.
    //! @param element - element to be visited
    void visitElementA(const std::shared_ptr<ConcreteElementA>& element) override;
    //! @brief Visit element B.
    //! @param element - element to be visited
    void visitElementB(const std::shared_ptr<ConcreteElementB>& element) override;
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
    void accept(Visitor& visitor) override;
};

//! @brief The concrete element.
class ConcreteElementB : public Element, public std::enable_shared_from_this<ConcreteElementB>
{
public:
    //! @brief Destroy the ConcreteElementB object.
    ~ConcreteElementB() override = default;

    //! @brief Accept visitor.
    //! @param visitor - visitor to be accepted
    void accept(Visitor& visitor) override;
};

extern std::ostringstream& output();
} // namespace visitor
} // namespace behavioral
} // namespace design_pattern
