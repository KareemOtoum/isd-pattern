
#include "isd.h"
#include <print>
 
struct IAnimal {  
    virtual void Eat() = 0;
    virtual void Sleep(double duration) = 0;
};

struct Dog { // implements all functions with correct naming and function types
  void Eat() {
    std::println("Dog is eating");
  }

  void Sleep(double duration) {
    std::println("dog is sleeping for {}s", duration);
  }
};

struct Cat {
  void CatEat() { // non matching name
    std::println("Cat is eating");
  }

  void Sleep(int duration) const { // non matching function type
    std::println("dog is sleeping for {}s", duration);
  }
};

struct LostGuy {
  void Eat() {
    std::println("LostGuy is eating");
  }

  void WhereAmI() { } // does not implement all functions
};

struct RuntimeDog : public IAnimal {
  void Eat() override { 
    std::println("Runtime dog is eating"); 
  }

  void Sleep(double duration) override {
    std::println("Runtime dog is sleeping");
  }
};

template<typename T>
concept AnimalLike = isd::implements(^^T, ^^IAnimal);

template<AnimalLike T>
void Feed(T x) {
  x.Eat();
}

int main() {
  static_assert(isd::implements(^^Dog, ^^IAnimal));
  static_assert(isd::implements(^^RuntimeDog, ^^IAnimal));
  static_assert(!isd::implements(^^Cat, ^^IAnimal));
  static_assert(!isd::implements(^^LostGuy, ^^IAnimal));

  Dog dog{};
  Feed(dog);

  RuntimeDog runtimeDog{};
  Feed(runtimeDog);
}