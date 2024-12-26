# Sigourney Engine Architecture Overview

My study notes for dynamic task dependency generation 

## Goal

Before employing any graphics pipeline that makes this engine a real game engine, 
the purpose of this project is to explore the possiblility of using a dynamically generated dependency data structure to construct the gameplay loop.

The idea is to strictly control all input/output data each gameplay system uses, 
and have a coordinator dynamically schedule everything to run on threadpools.

While it's heavily influenced by a entity-component architecture, 
my vision is to break out of the rigidity of the concept of "systems" and instead let programmers write smaller pieces of logic and assemble them into the equivalent of systems at run time by capturing the dependency of each piece.

Since using unmanaged language like C/C++ results in lack of supporting features like reflection, a custom build system is needed too. 
Therefore, I also wish to make a less convoluted build system (for the code side at least), where each action can be individually configured and invoked in a more straightforward manner compared to popular commercial solutions like scons.

Lastly, while I still can, I wish to keep the gameplay framework in POP land: no object-oriented programming unless absolute necessary.
Out of everything, this means mostly I'm using C-style programming for most of the program instead of C++.
If it ever gets out of prototyping phase, I'll move the development environment into a more proper one for pure C.

## Feature: Scheduler (Code Gen)

Using a basic and not 100% correct algrithm, we have an O(N\^2\*log(N)) algorithm that turns a moral graph of systems into a "shallow" DAG. 
Here "shallow" means the length of the longest path inside a DAG is relatively short, if not the shortest among its peers.
The exact implementation can be found in SystemGraph in CodeGenerator project, and its result is baked into a more compact data structure, which supports a runtime task availability calculation and nothing else.

## Feature: ThreadPool (Runtime)

This feature is currently only available on Windows.
A thread pool that is hard-coded to support only the scheduler using generated constants.
Thread communication is implemented through IOCP, hence it's only available on Windows; in the future a linux implementation can also be implemented through the Posix async interface.

## Feature: Dependency Injection (Code Gen)

TODO: need to figure out how to implement the resource provider, which is generated for each system so each system has a somewhat consistent entry point signature, thus can be stored in a function pointer.

## Feature: Functional Scripting Language (Code Gen + Runtime)

TODO: figure out what kind of language is needed before moving on to design a language translation system, that translates a non-imperative language to imperative language to achieve both optimization and intuitiveness.

## Other Notes

Original system isn't designed for dynamic dependency resolution. This makes the dynamic dependency capture isn't going to differentiate.
How does a task-based system compare to a system and iteration based system?
Iteration based system has less granularity: system A creates an event for system B, and only when all of system A is done can system B start processing these.
Then the gameplay loop controller needs to route all the way back to process new events for system A.
In a task based system, task A1 of type A creates a new task B1 of type B, then task B1 creates new task A1 ...
Neither method avoids running into dead loops so termination isn't a dealbreaker.

New direction: dependency-injection style dependency resolving. When constructing a new DI setup/container, we deterministically know their dependencies.
This works for traditional ECS. If we want to break the systems into smaller tasks the semantics are not as clear.
This approach has been adopted by unity's dots design before it was taken out of release; unlike the file system's access, access to gameplay data can be completely guarded against.
The benefit of this approach lies in the pipeline efficiency: if someone makes a change in a system they make, the game will automatically pick up a correct way to run the systems.

Bonus point: make the heuristic for dependency resolving dynamic and configurable, maybe we can have different parts of the game use different resolving heuristics.