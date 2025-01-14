# Proposal: Inter-Module Parallelization

As mentioned in the architecture doc, modules are strictly encapsulated from each other.
This gives us a chance to implement thread-pooling during the gameplay loop.
When a script code produces a signal, the receiving end of that signal is dispatched into a thread pool instead of executed on the same thread, and since module code during this phase can only manage internal state, they are perfectly safe as long as there's two callbacks from the same module running at once.
