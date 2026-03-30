# Reading vs owning polymorphic objects in C++

## The core problem

An abstract class cannot be instantiated or copied. This creates a
practical problem when working with inheritance hierarchies: how do
you store and pass objects whose concrete type is only known at runtime?

## The rule

To read or call virtual methods on a polymorphic object, pass through
a base class reference. Virtual dispatch works correctly and calls the
right derived method. No ownership is involved.

To store or own a polymorphic object, use a pointer to the base class.
The concrete object lives on the heap, the pointer preserves the real
type, virtual dispatch works, and nothing is ever copied or sliced.

## Why a vector of base class objects does not work

```cpp
std::vector<Interaction> interactions; // forbidden — Interaction is abstract
```

Even if the base class were concrete, pushing a derived object into
this vector would slice it — only the base class fields would be copied,
the derived fields would be lost. This is object slicing.

## The correct approach

```cpp
std::vector<std::unique_ptr<Interaction>> interactions;
```

The concrete object (`Contract` or `Appointment`) lives on the heap.
The vector stores pointers to those objects. No copying, no slicing,
full polymorphism intact. `unique_ptr` communicates that the vector
owns the objects.

## Concrete example from InsuraPro

```cpp
// reading — fine, virtual dispatch works correctly
void display(const Interaction& i) {
    i.display(); // calls Contract::display() or Appointment::display()
}

// storing — requires pointer, abstract type cannot be copied into vector
class IInteractionRepository {
public:
    virtual void insertInteraction(
        const std::unique_ptr<Interaction>& interaction) = 0;
    virtual std::vector<std::unique_ptr<Interaction>> findAll() = 0;
};

// caller creates the concrete type and transfers ownership
auto c = std::make_unique<Contract>(
    uuid, date, value, product, signed_date, status);
repo.insertInteraction(std::move(c));
```

The heap-allocated `Contract` is never copied — only the pointer moves
via `std::move`. The full derived object with all its fields is
preserved. The repository takes ownership.

## Why object slicing happens

When you copy a derived object into a base class slot (by value), the
compiler copies only the base class fields — it has no room for the
extra derived fields. The derived part is silently discarded. With
abstract classes the compiler refuses the copy entirely (compile error),
which is the safer outcome. With concrete base classes the copy
succeeds silently and data is lost.

## Summary

- Pass by `const Base&` when you only need to read or call virtual methods
- Use `std::unique_ptr<Base>` when you need to store, own, or transfer
  a polymorphic object
- Never use `std::vector<AbstractBase>` — use
  `std::vector<std::unique_ptr<AbstractBase>>` instead
