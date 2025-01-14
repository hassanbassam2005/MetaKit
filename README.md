# MetaKit

## Project Overview
MetaKit is a cutting-edge C++ metaprogramming toolkit built from scratch. It provides developers with powerful tools to manipulate types and compile-time constructs efficiently.

## Features
- **Type Manipulation:** Easily create, transform, and combine types.
- **Compile-Time Computation:** Perform operations at compile time for optimized runtime performance.
- **Flexible Constructs:** Use customizable templates and utilities.

## Installation
Clone the repository:
```bash
git clone https://github.com/hassanbassam2005/MetaKit.git
```

## Usage Examples
Here’s a basic example to demonstrate MetaKit:
```cpp
#include <metakit/typelist.hpp>

int main()
{
  //make 2 tuples and concatenate them
  tuple tot{ 3,true};
  tuple tot2{ 2.4,"saas"};
  auto t_t = tuple_cat(tot, tot2);
}
```
More examples are available in the documentation and source files.

## Contributing
Contributions are welcome! Please follow these steps:
1. Fork the repository.
2. Create a new branch for your feature or bug fix.
3. Submit a pull request with a detailed description.

## License
This project is licensed under the MIT License. See the `LICENSE` file for details.

## Contact
For questions or support, open an issue on the [GitHub repository](https://github.com/hassanbassam2005/MetaKit/issues).

---
Your contributions and feedback will help make MetaKit even better. Thank you for your support!


