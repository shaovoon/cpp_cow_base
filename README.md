# C++: Copy-On-Write Base Class Template

This tip presents a really simple Copy-On-Write(COW) class template (to be used as a base class) to ease developer effort to implement COW class. Copy-On-Write as its name implies, is a technique to duplicate(copy) shared resource upon write: the only exception to this write operation is when there is only one existing copy and not shared among multiple instances, duplication is not required. [Qt](https://www.qt.io/) 5 and earlier versions makes use of COW extensively in its framework because it predates C++11. COW can boost performance when used judiciously. Our helper class, `COWBase` has 4 methods: namely, `construct()` to be called in the derived class constructor, `clone_if_needed()` to be called in derived class setter and modifying functions before other code, `ptr()` to return raw internal pointer to `T` and `use_count()` to return number of instances.

```Cpp
template<typename T>
class COWBase
{
protected:
    // call this function in derived constructor
    void construct();

    // call this function in derived&#39;s setter before other code
    void clone_if_needed();
    
    // function to get the internal raw ptr
    const T* ptr() const;
    T* ptr();
    
    // returns count of the shared_ptr instance
    long use_count() const;
private:
    std::shared_ptr<T> m_ptr;
};
```

`construct()` make call to `make_shared()` to instantiate the `shared_ptr`.

```Cpp
// call this function in derived constructor
void construct() {
    m_ptr = std::make_shared<T>();
}
```

`clone_if_needed()` checks whether there is more than 1 instance of `shared_ptr` before making a new copy.

```Cpp
// call this function in derived&#39;s setter before other code
void clone_if_needed() {
    if (m_ptr.use_count() > 1) {
        std::shared_ptr<T> old = m_ptr;
        construct();
        *m_ptr = *old; // copy the old contents to new ptr.
    }
}
```

`ptr()` to get raw pointer to `T` as mentioned.

```Cpp
// function to get the internal raw ptr
const T* ptr() const {
    return m_ptr.get();
}
T* ptr() {
    return m_ptr.get();
}
```

`use_count()` returns number of `shared_ptr` instances.

```Cpp
// returns count of the shared_ptr instance
long use_count() const {
    return m_ptr.use_count();
}
```

## Example of the Inherited Class

We&#39;ll implement a mock `TextBox` for our example. The `TextBoxImpl` holds the data for `TextBox`, has 2 members, a title and color.

```Cpp
struct TextBoxImpl
{
    std::string m_Title;
    int m_Color;
};
```

`TextBox` inherits privately from `COWBase`. For those who are not acquainted with private inheritance. The public inheritance that most developers are accustomed to seeing, is a is-a relationship while private inheritance is implemented-in-terms-of relationship: privately inherited class still can access the public and protected of the base class as per usual, the only difference is user of privately inherited class can neither call the base class methods nor access its data members, hence the meaning private. If reader is keen to read more about private inheritance, go to the excellent resource on this topic: [C++ Tutorial Private Inheritance](https://www.bogotobogo.com/cplusplus/private_inheritance.php). To get back on `TextBox` discussion, its functions include 2 constructors, 2 getters with 2 corresponding setters and a `Display()` method to send the contents to console.

```Cpp
#include "COWBase.h"

class TextBox : private COWBase<TextBoxImpl>
{
public:
    TextBox();
    TextBox(const std::string& title, int color);
	
    int GetColor() const;
    const std::string& GetTitle() const;

    void SetColor(int color);
    void SetTitle(const std::string& title);
	
    void Display();
};
```

The default constructor just call `COWBase`&#39;s `construct()` while the non-default constructor calls `construct()` and then initialize `TextBoxImpl`&#39;s members.

```Cpp
TextBox() {
    construct();
}
TextBox(const std::string& title, int color) {
    construct();
    ptr()->m_Title = title;
    ptr()->m_Color = color;
}
```

For getters, it is business as usual. The members are accessed through the `ptr()`.

```Cpp
int GetColor() const 
{ return ptr()->m_Color; }

const std::string& GetTitle() const 
{ return ptr()->m_Title; }
```

Every setter or modifying function must call `clone_if_needed()` before anything else.

```Cpp
void SetColor(int color) {
    clone_if_needed();
    ptr()->m_Color = color;
}
void SetTitle(const std::string& title) {
    clone_if_needed();
    ptr()->m_Title = title;
}
```

`Display()` shows the contents of `use_count()`, title and color.

```Cpp
void Display() {
    std::cout << "use_count:" << use_count() 
              << ", Title: " << ptr()->m_Title 
              << ", Color: " << ptr()->m_Color 
              << "\n";
}
```

## Usage of the Inherited Class

We create a `TextBox` object named `a` and assigned it to `b`.

```Cpp
TextBox a("Hello", 1);
a.Display();

TextBox b = a;
b.Display();
```

After `a` is assigned to `b`, `use_count` increased to `2`.

```Cpp
use_count:1, Title: Hello, Color: 1
use_count:2, Title: Hello, Color: 1
```

After `b` changes its color to `2`, its `use_count()` is dropped to 1. After `b` changes its title to `"world"`, `use_count()` is still 1 because it is the same instance.

```Cpp
b.SetColor(2);
std::cout << "\nAfter setting color on b\n";
b.Display();

b.SetTitle("world");
std::cout << "\nAfter setting title on b\n";
b.Display();
```

The output is expected as follows:

```
After setting color:2 on b
use_count:1, Title: Hello, Color: 2

After setting title:world on b
use_count:1, Title: world, Color: 2
```

After this tutorial, I hope I have done a good job of convincing you, the reader to see how this `COWBase` class could be tremendous help to writing COW class with C++. The code in this post is hosted at [Github](https://github.com/shaovoon/cpp_cow_base). Happy writing COW class!

## History

* 7<sup>th</sup> March, 2020: Initial version
