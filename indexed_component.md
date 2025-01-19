# Proposal: Indexed Components in Module Definition

The engine isn't limited to store just the components for each component types; to facilitate sorting and fast accesses, components should have a built-in hint system that instructs the code generator to create support data structures at compile time.
One of such data structures is an indexing tree, such as a b+ tree. 
User should be able to insert in hint strings in their component definition file that indicates which fields of a component should be indexed.
The process of pre-built indices should NOT affect the ability of a component being sorted: the wrapper class for component stores should 