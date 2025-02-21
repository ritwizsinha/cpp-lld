#include <string>
#include <iostream>


// Forward declarations
class A;
class B;

// Abstract Visitor base class
class Visitor {
public:
    virtual void visit(A* a) = 0;
    virtual void visit(B* b) = 0;
    virtual ~Visitor() = default;  // Virtual destructor for proper cleanup
};

// Abstract Element base class
class Element {
public:

    virtual void accept(Visitor* visitor) = 0;
    virtual ~Element() = default;
};

// Concrete Element A
class A : public Element {
public:
    void accept(Visitor* visitor) override {
        visitor->visit(this);
    }
    
    std::string getDataA() const {
        return "Class A Data";
    }
};

// Concrete Element B
class B : public Element {
public:
    void accept(Visitor* visitor) override {
        visitor->visit(this);
    }
    
    std::string getDataB() const {
        return "Class B Data";
    }
};

// Concrete Visitor implementation
class ConcreteVisitor : public Visitor {
public:
    void visit(A* a) override {
        std::cout << "Visiting A: " << a->getDataA() << std::endl;
    }
    
    void visit(B* b) override {
        std::cout << "Visiting B: " << b->getDataB() << std::endl;
    }
};

