# Sigourney Engine

A collection of programming language and game engine-related projects made for fun.

## Parser Framework and Semantic Machine (Obselete)

A general-purpose parser that can be used to define and parse context-free grammar.
The Semantic Machine is a use case where I defined a simple functional language that uses the C-style invocation (foo.bar()) instead of LiSP style ((bar; foo)).

This section of the engine is no longer used, scripting language will be redesigned to be pure-data.

## Omni Parser (C++ project)

Porting Parser Framework into C++ because I can.

## EngineCore

The engine core runtime support library. 
By itself EngineCore doesn't do anything, it only supplies the necessary data structures and platform protocols to run a game.

## Game

The entry point project that leads to running the game. Depends on EngineCore and every xxModule project.

## "XxxModule" C++ Projects

Modules are high-level engine extensions; each module defines its own asset and component types, as well as providing one or more Module classes to be registered with the engine.
