# Brief Overview of Engine Structure

## Entity Component System/DOP

Starting with a very brief point, Sigourney Engine uses ECS, sometimes called data-oriented programming, as its programming model.
More detail can be found on the internet trivially, so I omit the explanation of what and why ECS is.

### Entity-based Scene Structure

Taking inspiration from Godot engine, Sigourney Engine treats scenes as a tree of entities.
This means each serialized entity, by itself, is a loadable scene.
Linking entities together in the loaded scene is the chain of transforms, which are NOT treated like a regular component; they receive special treatment due to the need to constantly pointer-traverse them.

### Asset Management

Assets are treated as individual units of bytestreams with a beginning and an end.
This abstraction allows the engine to switch between using different forms of data source: file streams, compressed byte streams, or even network streams if the game so needs.
To be a valid asset type, user needs to define such asset with a name and a set of static behaviors, named appropriately, in code.
ECS components can carry value-based asset references, which includes enough information to instruct the asset manager how to deserialize the referenced data blob.

### Scene Loader

Upon loading an Entity as a scene, the engine first inspects the scene to determine the entire set of assets it needs to load, and load them without duplication in memory first; a light weight reflection system will be used to provide type information on each referenced asset, and the engine doesn't allow wildcard asset reference.
Then it constructs the tree of entities by creating the compoennts and supplying them with the loaded references.

## The Update Loop

An update loop is composed of functional script, module code, and sinks.
In the following text module code and sinks are sometimes collectively referred to as engine low-level.

### Functional Layer

The majority of the game's behavior is composed of code written in a non-mutative fashion.
Upon each cycle of update, a user-defined total order of top-level functions will execute: the input are enumerations of events and components, while the output are enumerations of signals, which will then trigger engine behaviors implemented on a lower level.
This roughly corresponds to the gameplay logic retail engines allows to be attached to scene objects.
It's not meant to replace all data-driven logic.

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

 *Edit:* After some painful re-sketching, I've made some changes and further extensions of the existing modularization plan.
 The engine core will be compiled into a static library, which contains *services and shared data structures*.
 Each module is to be compiled into a separated static library, which contains code that *registers the module* and *registers all of its components, events and signals*
 There will be *two* other static library projects: 
 1. a project with the *main routine*, this is what eventually links the game runtime
 2. a project that doesn't have the runtime main routine, but a set of tools that can be used to access the game's reflection information; this library (or executable) is the bridge between the game and any tool that's designed for or bridged with the engine.

### Encapsulation

There's strictly no information sharing between modules; all that a module should share are definitions of events, signals, and components that are specific to this module.
For example, a snake-body management module will know nothing about physics system, and vice versa.
Same rule is applied to module-level scripts.
The only way to communicate across modules is to use top-level scripts, as they have access to all data structures exposed from every module.
The same rule goes for sinks, since these observe-only modules will largely rely on their own module-level data structures, such as rendering system and rendering target components.
Any module, however, has access to data structure and function defined in the engine runtime, which can include events and components.

### Versioning

Todo.
