# Functional Scripting Language Rough Plans

## Targt C++

Generating byte code can be a next step, right now focus on generate C++ code that conceptually produce consistent logic as described by the functional scripts

## Managed Heap and Inheritance Polymorphism

While it's tempting to use stack for fast allocation, the need to have good polymorphic support for the behavior-composing data structures such as enumeration primitives gravitates my design towards using C++ class inheritance, and thus the need to store object on a managed heap (essentially to batch new() commands with placement new).

### High Integrity Allocator

An allocator optimized towards fast allocation and low fragmentation, but overall less efficient storage use.
Leveraging templates in C++, the allocator builds a table of equivalent classes of all possible New() invocations in the source code at start up.
It then uses this information to build a table of managed buffers, which themselves are single-linked lists of buffer nodes, whose key is the size of an individual allocations.
Doing so reduces the problem of heterogeneous buffer seeking to homogeneous buffer seeking, in which case the time complexity is O(1) when there's no need to expand the pre-allocated heap region.
Because this heap allocator is only used for the scripting layer, its content is completely reset after each frame update, allowing it to be reasonably memory efficient if the client code is written properly.
