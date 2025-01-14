# Proposal: Interface Versioning

Here "interface" refer to the definition (format tbd) of a module's exposed content, such as component types and event types.
Since much of the hooking code needs to be generated from a non-cpp language that defines these, it's possible that we version interfaces implicitly using a serialization of the intermediate data structure parsed from the definition language.
