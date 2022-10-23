#include "behavioral.hpp"
#include <map>
#include <vector>
#include "utility/include/common.hpp"

#define BEHAVIORAL_RESULT "\r\n*%-21s instance:\r\n%s"

namespace dp_behavioral
{
Behavioral::Behavioral()
{
    std::cout << "\r\nBehavioral pattern:" << std::endl;
}

// Chain Of Responsibility
namespace chain_of_responsibility
{
static std::stringstream& stringstream()
{
    static std::stringstream stream;
    return stream;
}

class Handler
{
public:
    virtual ~Handler() = default;

    virtual void setHandler(std::shared_ptr<Handler> handler) { successor = handler; }
    virtual void handleRequest()
    {
        if (successor)
        {
            successor->handleRequest();
        }
    }

private:
    std::shared_ptr<Handler> successor;
};

class ConcreteHandler1 : public Handler
{
public:
    ~ConcreteHandler1() override = default;

    static bool canHandle() { return false; }
    void handleRequest() override
    {
        if (canHandle())
        {
            stringstream() << "handled by concrete handler 1" << std::endl;
        }
        else
        {
            stringstream() << "cannot be handled by handler 1" << std::endl;
            Handler::handleRequest();
        }
    }
};

class ConcreteHandler2 : public Handler
{
public:
    ~ConcreteHandler2() override = default;

    static bool canHandle() { return true; }
    void handleRequest() override
    {
        if (canHandle())
        {
            stringstream() << "handled by handler 2" << std::endl;
        }
        else
        {
            stringstream() << "cannot be handled by handler 2" << std::endl;
            Handler::handleRequest();
        }
    }
};
} // namespace chain_of_responsibility

void Behavioral::chainOfResponsibilityInstance()
{
    using chain_of_responsibility::ConcreteHandler1;
    using chain_of_responsibility::ConcreteHandler2;
    using chain_of_responsibility::stringstream;

    std::shared_ptr<ConcreteHandler1> handler1 = std::make_shared<ConcreteHandler1>();
    std::shared_ptr<ConcreteHandler2> handler2 = std::make_shared<ConcreteHandler2>();

    handler1->setHandler(handler2);
    handler1->handleRequest();

    COMMON_PRINT(BEHAVIORAL_RESULT, "ChainOfResponsibility", stringstream().str().c_str());
}

// Command
namespace command
{
static std::stringstream& stringstream()
{
    static std::stringstream stream;
    return stream;
}

class Receiver
{
public:
    static void action() { stringstream() << "receiver: execute action" << std::endl; }
};

class Command
{
public:
    virtual ~Command() = default;

    virtual void execute() = 0;

protected:
    Command() = default;
};

class ConcreteCommand : public Command
{
public:
    explicit ConcreteCommand(std::shared_ptr<Receiver> receiver) : receiver(receiver) {}

    ~ConcreteCommand() override
    {
        if (receiver)
        {
            receiver.reset();
        }
    }

    void execute() override { receiver->action(); }

private:
    std::shared_ptr<Receiver> receiver;
};

class Invoker
{
public:
    void set(std::shared_ptr<Command> c) { command = c; }
    void confirm()
    {
        if (command)
        {
            command->execute();
        }
    }

private:
    std::shared_ptr<Command> command;
};
} // namespace command

void Behavioral::commandInstance()
{
    using command::Command;
    using command::ConcreteCommand;
    using command::Invoker;
    using command::Receiver;
    using command::stringstream;

    std::shared_ptr<ConcreteCommand> command = std::make_shared<ConcreteCommand>((std::make_shared<Receiver>()));

    Invoker invoker;
    invoker.set(command);
    invoker.confirm();

    COMMON_PRINT(BEHAVIORAL_RESULT, "Command", stringstream().str().c_str());
}

// Interpreter
namespace interpreter
{
static std::stringstream& stringstream()
{
    static std::stringstream stream;
    return stream;
}

class Context
{
public:
    void set(const std::string& var, const bool value) { vars.insert(std::pair<std::string, bool>(var, value)); }
    bool get(const std::string& exp) { return vars[exp]; }

private:
    std::map<std::string, bool> vars;
};

class AbstractExpression
{
public:
    virtual ~AbstractExpression() = default;

    virtual bool interpret(const std::shared_ptr<Context> /*unused*/) { return false; }
};

class TerminalExpression : public AbstractExpression
{
public:
    explicit TerminalExpression(const std::string& value) : value(value) {}
    ~TerminalExpression() override = default;

    bool interpret(const std::shared_ptr<Context> context) override { return context->get(value); }

private:
    std::string value;
};

class NonterminalExpression : public AbstractExpression
{
public:
    NonterminalExpression(std::shared_ptr<AbstractExpression> left, std::shared_ptr<AbstractExpression> right) :
        leftOp(left), rightOp(right)
    {
    }
    ~NonterminalExpression() override
    {
        leftOp.reset();
        rightOp.reset();
    }

    bool interpret(const std::shared_ptr<Context> context) override
    {
        return (leftOp->interpret(context) && rightOp->interpret(context));
    }

private:
    std::shared_ptr<AbstractExpression> leftOp;
    std::shared_ptr<AbstractExpression> rightOp;
};
} // namespace interpreter

void Behavioral::interpreterInstance()
{
    using interpreter::AbstractExpression;
    using interpreter::Context;
    using interpreter::NonterminalExpression;
    using interpreter::stringstream;
    using interpreter::TerminalExpression;

    std::shared_ptr<AbstractExpression> a = std::make_shared<TerminalExpression>("A");
    std::shared_ptr<AbstractExpression> b = std::make_shared<TerminalExpression>("B");
    std::shared_ptr<AbstractExpression> exp = std::make_shared<NonterminalExpression>(a, b);

    std::shared_ptr<Context> context = std::make_shared<Context>();
    context->set("A", true);
    context->set("B", false);

    stringstream() << context->get("A") << " AND " << context->get("B");
    stringstream() << " = " << exp->interpret(context) << std::endl;

    COMMON_PRINT(BEHAVIORAL_RESULT, "Interpreter", stringstream().str().c_str());
}

// Iterator
namespace iterator
{
static std::stringstream& stringstream()
{
    static std::stringstream stream;
    return stream;
}

class Iterator;
class ConcreteAggregate;

class Aggregate
{
public:
    virtual ~Aggregate() = default;

    virtual std::shared_ptr<Iterator> createIterator() = 0;
};

class ConcreteAggregate : public Aggregate, public std::enable_shared_from_this<ConcreteAggregate>
{
public:
    explicit ConcreteAggregate(const uint32_t size)
    {
        list = std::make_unique<int[]>(size);
        std::fill(list.get(), list.get() + size, 1);
        count = size;
    }
    ~ConcreteAggregate() override { list.reset(); }

    std::shared_ptr<Iterator> createIterator() override;
    [[nodiscard]] uint32_t size() const { return count; }
    int at(uint32_t index) { return list[index]; }

private:
    std::unique_ptr<int[]> list;
    uint32_t count;
};

class Iterator
{
public:
    virtual ~Iterator() = default;

    virtual void first() = 0;
    virtual void next() = 0;
    [[nodiscard]] virtual bool isDone() const = 0;
    [[nodiscard]] virtual int currentItem() const = 0;
};

class ConcreteIterator : public Iterator
{
public:
    explicit ConcreteIterator(std::shared_ptr<ConcreteAggregate> list) : list(list), index(0) {}
    ~ConcreteIterator() override = default;

    void first() override { index = 0; }
    void next() override { ++index; }
    [[nodiscard]] bool isDone() const override { return (index >= list->size()); }
    [[nodiscard]] int currentItem() const override
    {
        if (isDone())
        {
            return -1;
        }
        return list->at(index);
    }

private:
    std::shared_ptr<ConcreteAggregate> list;
    uint32_t index;
};

std::shared_ptr<Iterator> ConcreteAggregate::createIterator()
{
    return std::make_shared<ConcreteIterator>(shared_from_this());
}
} // namespace iterator

void Behavioral::iteratorInstance()
{
    using iterator::ConcreteAggregate;
    using iterator::Iterator;
    using iterator::stringstream;

    constexpr uint32_t size = 5;
    std::shared_ptr<ConcreteAggregate> list = std::make_shared<ConcreteAggregate>(size);
    std::shared_ptr<Iterator> iter = list->createIterator();

    for (; !iter->isDone(); iter->next())
    {
        stringstream() << "item value: " << iter->currentItem() << std::endl;
    }

    COMMON_PRINT(BEHAVIORAL_RESULT, "Iterator", stringstream().str().c_str());
}

// Mediator
namespace mediator
{
static std::stringstream& stringstream()
{
    static std::stringstream stream;
    return stream;
}

class Mediator;

class Colleague
{
public:
    Colleague(const std::shared_ptr<Mediator> mediator, const uint32_t id) : mediator(mediator), id(id) {}
    virtual ~Colleague() = default;

    [[nodiscard]] uint32_t getID() const { return id; }
    virtual void send(const std::string& msg) = 0;
    virtual void receive(const std::string& msg) = 0;

protected:
    std::weak_ptr<Mediator> mediator;
    uint32_t id;
};

class Mediator
{
public:
    virtual ~Mediator() = default;

    virtual void add(const std::shared_ptr<Colleague> colleague) = 0;
    virtual void distribute(const std::shared_ptr<Colleague> sender, const std::string& msg) = 0;

protected:
    Mediator() = default;
};

class ConcreteColleague : public Colleague, public std::enable_shared_from_this<ConcreteColleague>
{
public:
    ConcreteColleague(const std::shared_ptr<Mediator> mediator, const uint32_t id) : Colleague(mediator, id) {}
    ~ConcreteColleague() override = default;

    void send(const std::string& msg) override
    {
        stringstream() << "message \"" << msg << "\" sent by colleague " << id << std::endl;
        mediator.lock()->distribute(shared_from_this(), msg);
    }
    void receive(const std::string& msg) override
    {
        stringstream() << "message \"" << msg << "\" received by colleague " << id << std::endl;
    }
};

class ConcreteMediator : public Mediator
{
public:
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

    void add(const std::shared_ptr<Colleague> colleague) override { colleagues.emplace_back(colleague); }
    void distribute(const std::shared_ptr<Colleague> sender, const std::string& msg) override
    {
        std::for_each(
            colleagues.cbegin(),
            colleagues.cend(),
            [&sender, &msg](const auto& colleague)
            {
                if (colleague->getID() != sender->getID())
                {
                    colleague->receive(msg);
                }
            });
    }

private:
    std::vector<std::shared_ptr<Colleague>> colleagues;
};
} // namespace mediator

void Behavioral::mediatorInstance()
{
    using mediator::Colleague;
    using mediator::ConcreteColleague;
    using mediator::ConcreteMediator;
    using mediator::Mediator;
    using mediator::stringstream;

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

    COMMON_PRINT(BEHAVIORAL_RESULT, "Mediator", stringstream().str().c_str());
}

// Memento
namespace memento
{
static std::stringstream& stringstream()
{
    static std::stringstream stream;
    return stream;
}

class Memento
{
private:
    friend class Originator;
    explicit Memento(const int state) : state(state) {}

    int state;
    void setState(const int s) { state = s; }
    [[nodiscard]] int getState() const { return state; }
};

class Originator
{
public:
    void setState(const int s)
    {
        stringstream() << "set state to " << s << std::endl;
        state = s;
    }
    [[nodiscard]] int getState() const { return state; }
    void setMemento(const std::shared_ptr<Memento> memento) { state = memento->getState(); }
    [[nodiscard]] std::shared_ptr<Memento> createMemento() const
    {
        return std::shared_ptr<Memento>(new Memento(state));
    }

private:
    int state;
};

class CareTaker
{
public:
    explicit CareTaker(const std::shared_ptr<Originator> originator) : originator(originator) {}
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

    void save()
    {
        stringstream() << "save state" << std::endl;
        history.emplace_back(originator->createMemento());
    }
    void undo()
    {
        if (history.empty())
        {
            stringstream() << "unable to undo state" << std::endl;
            return;
        }

        std::shared_ptr<Memento> memento = history.back();
        originator->setMemento(memento);
        stringstream() << "undo state" << std::endl;

        history.pop_back();
        memento.reset();
    }

private:
    std::shared_ptr<Originator> originator;
    std::vector<std::shared_ptr<Memento>> history;
};
} // namespace memento

void Behavioral::mementoInstance()
{
    using memento::CareTaker;
    using memento::Originator;
    using memento::stringstream;

    constexpr int state1 = 1, state2 = 2, state3 = 3;
    std::shared_ptr<Originator> originator = std::make_shared<Originator>();
    std::shared_ptr<CareTaker> caretaker = std::make_shared<CareTaker>(originator);

    originator->setState(state1);
    caretaker->save();
    originator->setState(state2);
    caretaker->save();
    originator->setState(state3);
    caretaker->undo();

    stringstream() << "actual state is " << originator->getState() << std::endl;

    COMMON_PRINT(BEHAVIORAL_RESULT, "Memento", stringstream().str().c_str());
}

// Observer
namespace observer
{
static std::stringstream& stringstream()
{
    static std::stringstream stream;
    return stream;
}

class Subject;

class Observer
{
public:
    virtual ~Observer() = default;

    virtual int getState() = 0;
    virtual void update(std::shared_ptr<Subject> subject) = 0;
};

class Subject : public std::enable_shared_from_this<Subject>
{
public:
    virtual ~Subject() = default;

    void attach(std::shared_ptr<Observer> observer) { observers.emplace_back(observer); }
    void detach(const int index) { observers.erase(observers.begin() + index); }
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
    virtual int getState() = 0;
    virtual void setState(const int s) = 0;

private:
    std::vector<std::shared_ptr<Observer>> observers;
};

class ConcreteObserver : public Observer
{
public:
    explicit ConcreteObserver(const int state) : observerState(state) {}
    ~ConcreteObserver() override = default;

    int getState() override { return observerState; }
    void update(std::shared_ptr<Subject> subject) override
    {
        observerState = subject->getState();
        stringstream() << "observer state updated" << std::endl;
    }

private:
    int observerState;
};

class ConcreteSubject : public Subject
{
public:
    ConcreteSubject() : subjectState() {}
    ~ConcreteSubject() override = default;

    int getState() override { return subjectState; }
    void setState(const int s) override { subjectState = s; }

private:
    int subjectState;
};
} // namespace observer

void Behavioral::observerInstance()
{
    using observer::ConcreteObserver;
    using observer::ConcreteSubject;
    using observer::stringstream;
    using observer::Subject;

    constexpr int state1 = 1, state2 = 2, state3 = 3;
    std::shared_ptr<ConcreteObserver> observer1 = std::make_shared<ConcreteObserver>(state1);
    std::shared_ptr<ConcreteObserver> observer2 = std::make_shared<ConcreteObserver>(state2);

    stringstream() << "observer1 state: " << observer1->getState() << std::endl;
    stringstream() << "observer2 state: " << observer2->getState() << std::endl;

    std::shared_ptr<Subject> subject = std::make_shared<ConcreteSubject>();
    subject->attach(observer1);
    subject->attach(observer2);
    subject->setState(state3);
    subject->notify();

    stringstream() << "observer1 state: " << observer1->getState() << std::endl;
    stringstream() << "observer2 state: " << observer2->getState() << std::endl;

    COMMON_PRINT(BEHAVIORAL_RESULT, "Observer", stringstream().str().c_str());
}

// State
namespace state
{
static std::stringstream& stringstream()
{
    static std::stringstream stream;
    return stream;
}

class State
{
public:
    virtual ~State() = default;

    virtual void handle() = 0;
};

class ConcreteStateA : public State
{
public:
    ~ConcreteStateA() override = default;

    void handle() override { stringstream() << "state A handled" << std::endl; }
};

class ConcreteStateB : public State
{
public:
    ~ConcreteStateB() override = default;

    void handle() override { stringstream() << "state B handled" << std::endl; }
};

class Context
{
public:
    Context() : state() {}
    ~Context() { state.reset(); }

    void setState(const std::shared_ptr<State> s)
    {
        if (state)
        {
            state.reset();
        }
        state = s;
    }

    void request() { state->handle(); }

private:
    std::shared_ptr<State> state;
};
} // namespace state

void Behavioral::stateInstance()
{
    using state::ConcreteStateA;
    using state::ConcreteStateB;
    using state::Context;
    using state::stringstream;

    std::shared_ptr<Context> context = std::make_shared<Context>();

    context->setState(std::make_shared<ConcreteStateA>());
    context->request();

    context->setState(std::make_shared<ConcreteStateB>());
    context->request();

    COMMON_PRINT(BEHAVIORAL_RESULT, "State", stringstream().str().c_str());
}

// Strategy
namespace strategy
{
static std::stringstream& stringstream()
{
    static std::stringstream stream;
    return stream;
}

class Strategy
{
public:
    virtual ~Strategy() = default;

    virtual void algorithmInterface() = 0;
};

class ConcreteStrategyA : public Strategy
{
public:
    ~ConcreteStrategyA() override = default;

    void algorithmInterface() override { stringstream() << "concrete strategy A" << std::endl; }
};

class ConcreteStrategyB : public Strategy
{
public:
    ~ConcreteStrategyB() override = default;

    void algorithmInterface() override { stringstream() << "concrete strategy B" << std::endl; }
};

class Context
{
public:
    explicit Context(const std::shared_ptr<Strategy> strategy) : strategy(strategy) {}
    ~Context() { strategy.reset(); }

    void contextInterface() { strategy->algorithmInterface(); }

private:
    std::shared_ptr<Strategy> strategy;
};
} // namespace strategy

void Behavioral::strategyInstance()
{
    using strategy::ConcreteStrategyA;
    using strategy::ConcreteStrategyB;
    using strategy::Context;
    using strategy::stringstream;

    Context contextA(std::make_shared<ConcreteStrategyA>());
    contextA.contextInterface();

    Context contextB(std::make_shared<ConcreteStrategyB>());
    contextB.contextInterface();

    COMMON_PRINT(BEHAVIORAL_RESULT, "Strategy", stringstream().str().c_str());
}

// Template Method
namespace template_method
{
static std::stringstream& stringstream()
{
    static std::stringstream stream;
    return stream;
}

class AbstractClass
{
public:
    virtual ~AbstractClass() = default;

    void templateMethod()
    {
        primitiveOperation1();
        primitiveOperation2();
    }
    virtual void primitiveOperation1() = 0;
    virtual void primitiveOperation2() = 0;
};

class ConcreteClass : public AbstractClass
{
public:
    ~ConcreteClass() override = default;

    void primitiveOperation1() override { stringstream() << "primitive operation 1" << std::endl; }
    void primitiveOperation2() override { stringstream() << "primitive operation 2" << std::endl; }
};
} // namespace template_method

void Behavioral::templateMethodInstance()
{
    using template_method::AbstractClass;
    using template_method::ConcreteClass;
    using template_method::stringstream;

    std::shared_ptr<AbstractClass> tm = std::make_shared<ConcreteClass>();
    tm->templateMethod();

    COMMON_PRINT(BEHAVIORAL_RESULT, "TemplateMethod", stringstream().str().c_str());
}

// Visitor
namespace visitor
{
static std::stringstream& stringstream()
{
    static std::stringstream stream;
    return stream;
}

class Element;
class ConcreteElementA;
class ConcreteElementB;

class Visitor
{
public:
    virtual ~Visitor() = default;

    virtual void visitElementA(const std::shared_ptr<ConcreteElementA> element) = 0;
    virtual void visitElementB(const std::shared_ptr<ConcreteElementB> element) = 0;
};

class ConcreteVisitor1 : public Visitor
{
public:
    ~ConcreteVisitor1() override = default;

    void visitElementA(const std::shared_ptr<ConcreteElementA> /*element*/) override
    {
        stringstream() << "concrete visitor 1: element A visited" << std::endl;
    }

    void visitElementB(const std::shared_ptr<ConcreteElementB> /*element*/) override
    {
        stringstream() << "concrete visitor 1: element B visited" << std::endl;
    }
};

class ConcreteVisitor2 : public Visitor
{
public:
    ~ConcreteVisitor2() override = default;

    void visitElementA(const std::shared_ptr<ConcreteElementA> /*element*/) override
    {
        stringstream() << "concrete visitor 2: element A visited" << std::endl;
    }
    void visitElementB(const std::shared_ptr<ConcreteElementB> /*element*/) override
    {
        stringstream() << "concrete visitor 2: element B visited" << std::endl;
    }
};

class Element
{
public:
    virtual ~Element() = default;

    virtual void accept(Visitor& visitor) = 0;
};

class ConcreteElementA : public Element, public std::enable_shared_from_this<ConcreteElementA>
{
public:
    ~ConcreteElementA() override = default;

    void accept(Visitor& visitor) override { visitor.visitElementA(shared_from_this()); }
};

class ConcreteElementB : public Element, public std::enable_shared_from_this<ConcreteElementB>
{
public:
    ~ConcreteElementB() override = default;

    void accept(Visitor& visitor) override { visitor.visitElementB(shared_from_this()); }
};
} // namespace visitor

void Behavioral::visitorInstance()
{
    using visitor::ConcreteElementA;
    using visitor::ConcreteElementB;
    using visitor::ConcreteVisitor1;
    using visitor::ConcreteVisitor2;
    using visitor::stringstream;

    std::shared_ptr<ConcreteElementA> elementA = std::make_shared<ConcreteElementA>();
    std::shared_ptr<ConcreteElementB> elementB = std::make_shared<ConcreteElementB>();
    ConcreteVisitor1 visitor1;
    ConcreteVisitor2 visitor2;

    elementA->accept(visitor1);
    elementA->accept(visitor2);
    elementB->accept(visitor1);
    elementB->accept(visitor2);

    COMMON_PRINT(BEHAVIORAL_RESULT, "Visitor", stringstream().str().c_str());
}
} // namespace dp_behavioral
