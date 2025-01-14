# Brief Overview of Engine Structure

## Entity Component System/DOP

Starting with a very brief point, Sigourney Engine uses ECS, sometimes called data-oriented programming, as its programming model.
More detail can be found on the internet trivially, so I omit the explanation of what and why ECS is.

## The Update Loop

An update loop is composed of functional script, module code, and sinks.
In the following text module code and sinks are sometimes collectively referred to as engine low-level.

### Functional Scripting Language

The majority of the game's behavior is composed of scripts using linq-over-cpp (definitely need to change the name later). 
While they are written and presented as a functional language, it's translated into C++ code and compiled with the runtime; in a way, it's more like a way to safeguard the use of a limited set of native C++ features provided by the engine's runtime.
Upon each cycle of update, a user-defined total order of top-level functions will execute: the input are enumerations of events and components, while the output are enumerations of signals, which will then trigger engine behaviors implemented on a lower level.

### Event vs. Signal

While the naming is arbitrary, here "events" refer to data structures sent by engine low-level to scripting, while signals are the output from scripting for low-level logic.
For example, a frame update, or a physics collision, is an event because a script is needed to resolve that (more on modular encapsulation later), whereas a command to move a rigid body or creating an entity is a signal for low level code.

### Mutation in Update

Scripts are non-mutative while module code is mutative.
During an update, scripts are invoked first, triggering paralliizable behavior in module code which are only allowed to update internal state within each module; then after all events have been processed by the scritps, a synchronous pass of modules allows modules to update their external states and generate follow up events.
More on automatic thread-pooling in another doc maybe.
Now there's one big edge case in all of these: archetype instantiation.
Since archetypes are typically used to define a prefab entity with a set of necessary compoenents from various modules, they can only be invoked in top-level scripts.
Hence we designate archetype instantiation, static or dynamic, a signal that's handled by the engine instead of module code.

### Module and Sink

Module is the middle layer, immediately after scripts; as mentioned above, scripts output signals that are handled by module code, but in implementaton these signals are translated to function calls directly. 
In a way, a module is but a big collection of callback functions scripts can invoke.
Sinks on the other hand are logic that execute after all updates are done; they only observe the final state after an update loop.
Rendering, for example, can be a sink since the easiest way to implement it doesn't require the rendering code to interact with much of the rest of the engine.

## Code Organization

### Modularization

The engine would eventually be made of core runtime, scripts, and modules; I'm reusing the name "module" here because I'm very creative.
Core runtime is comprised of all supporting libraries, such as math and the backend for the functional scripting langauge, and the hard-coded behavior that actually runs the game.
Modules are code that define dynamic behavior that implements anything the game-specific logic uses; for example, a snake game can have a dedicated module that manages the state of all snake bodies.
Modules can be either built-in and user-defined. 
A built-in module would be something like physics system, or the rendering system itself since "sinks" are just a special kind of modules that registeres themselves with flags.
Modules can and often need to contain scripts, since this allows a module to hook into system events without having the user manually setting everything up, especially necessary for code distribution.
Lastly, a script is a plain text file that contains code written under the functional language, the compiler will collect all existing scripts and translate them into c++ code to compile with the rest of the engine.

### Encapsulation

There's strictly no information sharing between modules; all that a module should share are definitions of events, signals, and components that are specific to this module.
For example, a snake-body management module will know nothing about physics system, and vice versa.
Same rule is applied to module-level scripts.
The only way to communicate across modules is to use top-level scripts, as they have access to all data structures exposed from every module.
The same rule goes for sinks, since these observe-only modules will largely rely on their own module-level data structures, such as rendering system and rendering target components.
Any module, however, has access to data structure and function defined in the engine runtime, which can include events and components.

### Versioning

Todo.
