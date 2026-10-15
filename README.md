# Interface-based Static Dispatch Pattern

Using C++26 Reflection and C++20 Concepts we are able to achieve a more idiomatic and modern  
pattern for compile time polymorphism by specifying a concept that accepts a type that structurally satisfies a regular C++ interface. 

This could be a better option than CRTP and handwritten concepts due to its conciseness and stricter function matching

## The improvements of this pattern over its predecessors:
- Requires no inheritance or naming conventions
- Defined only once using a regualar C++ interface 
- Great diagnostics potential due to use of reflection
- Usable with runtime dispatch cases through one interface
- Stricter interface check than handwritten concepts (good or bad depending on use case)

Simply put this pattern is defined by reflecting the type of an interface, extracting its pure virtual functions,
and ensuring the type in question implements these functions, having the same exact name as it is in the code, and 
with the same function type.

```c++
struct IAnimal {  
    virtual void Eat() = 0;
    virtual void Sleep(double duration) = 0;
};

template<typename T>
concept AnimalLike = isd::implements(^^T, ^^IAnimal);

template <AnimalLike T>
void feed(T& a) { a.Eat(); }   // direct call, no vtable
```

`implements(std::meta::info objInfo, std::meta::info interfaceInfo)` here is a predicate that returns true if the first operand defines all the interface's pure virtual functions with the same function name and type. I have included a header with the implementation `include/isd.h`

I believe this pattern makes compile time polymorphism in C++ a whole lot easier to use in certain use cases.

It is important to note that "satisfying" an interface in this context isn't the same as how the C++ standard defines overriding a function. In this pattern a function of an interface `A` is considered satisfied by an implementing function `B` if   `is_same_type(type_of(^^A), type_of(^^B)) && identifier_of(^^A) == identifier_of(^^B)` under the `std::meta` namespace

(note that only accessible pure virtual functions are considered for matching)

| Property    | Must Match? |
| -------- | ------- |
| Name  | Yes    |
| Return Type | Yes     |
| `const`    | Yes    |
| `volatile`    | Yes    |
| Parameters (Type and Order)    | Yes    |
| Ref-qualifier (& / &&)   |  Yes  |
| `noexcept`   |  Yes  |

*Anything that is part of the function type must match*

# CRTP vs ISD vs Concepts

## CRTP based static dispatch (C++23)

```c++
struct Animal {
    void Eat(this auto& self)            { self.EatImpl(); }
    void Sleep(this auto& self, double d) { self.SleepImpl(d); }
};

struct Dog : Animal {
    void EatImpl()          {}
    void SleepImpl(double)  {}
};

Dog d;
d.Eat();   // self deduced as Dog&, calls Dog::EatImpl directly
```
### Notice:  
- Errors only arise at the time of calling 
- Inheritance is needed
- Impl or other naming convention needed in implementing functions (otherwise there is a risk of infinite recursion for a call)
- Is not defined by an interface that classes can use for runtime polymorphism

## Concept based static dispatch (C++20)

```c++
template <class T>
concept AnimalLike = requires(T& t, double d) {
    { t.Eat() }    -> std::same_as<void>;
    { t.Sleep(d) } -> std::same_as<void>;
};
```

### Notice:
- This is a weak interface check, for each function check it is only checking whether the function call is a valid expression and what the return type is. so things like implicit conversions and default values in parameters pass this test. 
- Is not defined by an interface that classes can use for runtime polymorphism
- Arguably not expressive 

However a similair signature based check can be performed with concepts aswell

```c++
template <class T>
concept AnimalLike = requires {
    static_cast<void (T::*)()>(&T::Eat);
    static_cast<void (T::*)(double)>(&T::Sleep);
};
```  

`static_cast` here succeeds only when `T` has a function with *almost* the same exact signature (`noexcept` nuance explained in the following section so it does not exactly match function types)
So this is the best contender so far however:
- It is not expressive
- Is not defined by an interface that classes can use for runtime polymorphism  

Important remark: Does actually benefit from functions marked `noexcept` being able to satisfy a required function that is not marked `noexcept` which is beneficial **(ISD cannot do this as it is a strict type equality check and noexcept is part of the function type)**

All of these patterns however benefit from inherited functions being able to satisfy the interface **(ISD currently does not but this can be implemented atleast for some circumstances)**

## Where this pattern can be improved (and weaknesses)

- Diagnostic messages detailing missing functions
- No support for subsuming (for e.g. Interfaces implementing and extending `IAnimal` will not be prefered over the base interface in a context expecting `IAnimal`) there are some things that can be implemented here like allowing inherited pure virtual functions from other interfaces to be required by the interface 
- This pattern isnt necessarily better than handwritten concepts or CRTP, they are less strict and that can be useful depending on the use case
- Default implementations of functions in the interface cannot be provided

I have included some usage examples in this project

# Build Process (Unix)  

run this command in the root directory to pull the gcc:16 docker image and build the project inside a docker container
(compiles with gcc 16 --std=c++26 -freflection)

```bash
./run.sh
```