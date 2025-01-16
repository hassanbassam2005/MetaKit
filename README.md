# MetaKit

🚀 **A lightweight C++ metaprogramming toolkit for type manipulation and compile-time computations.**

## 📌 Overview
MetaKit is a **header-only C++ library** designed to simplify **metaprogramming** by providing utilities for **type lists**, **compile-time transformations**, and **static type computations**. It serves as a **lightweight alternative** to Boost.MP11 and standard C++ type traits, focusing on **minimalism and high performance**.

## ✨ Features
- **Type List Manipulation**: Create and transform compile-time type lists.
- **Type Traits Enhancements**: Additional utilities beyond `<type_traits>`.
- **Metaprogramming Algorithms**: Compile-time `map`, `filter`, `reduce` operations.
- **Static Reflection Utilities**: Simplify type introspection without runtime overhead.
- **Zero Runtime Overhead**: Fully optimized for compile-time execution.

## 🚀 Getting Started
### Prerequisites
- C++17 or later (Recommended: C++20 for best support)
- A modern C++ compiler (GCC, Clang, MSVC)

### Installation
MetaKit is header-only. Simply clone the repository and include the headers:

```sh
git clone https://github.com/hassanbassam2005/MetaKit.git
```

Then include it in your project:

```cpp
#include "metakit/type_list.hpp"
```

## 📌 Usage Example
```cpp
#include "metakit/type_list.hpp"
#include <iostream>

using namespace metakit;

// Define a type list
using MyTypes = type_list<int, double, char>;

// Get the first type
using FirstType = front<MyTypes>;

int main() {
    std::cout << "First type is: " << typeid(FirstType).name() << '\n';
    return 0;
}
```

## 🛠️ Future Enhancements
- Additional compile-time algorithms.
- Better integration with standard type traits.
- Benchmark comparisons with Boost.MP11.

## 🤝 Contributing
Contributions are welcome! Please open an issue or submit a pull request if you have improvements or feature requests.

## 📜 License
This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## 🌟 Acknowledgments
Inspired by [EASTL](https://github.com/electronicarts/EASTL) and the C++ standard library's metaprogramming utilities.
