#pragma once

namespace SigourneyEngine {
namespace FunctionalLayer {
namespace Enumeration {

/// <summary>
/// Enumerate a source enumerable, sending the data into the acceptor in an set order determined by the enumerable.
/// </summary>
/// <typeparam name="TAcceptor">type of acceptor; it's a strategy object and a context depending on client code; it should have method Accept</typeparam>
/// <typeparam name="TEnumerable">type of source enumerable; it should have methods IsCompleted() and Take()</typeparam>
/// <typeparam name="TEnumerator">type of enumerator; it's a context wrapper</typeparam>
/// <typeparam name="TElement">type of elements from source enumerable</typeparam>
/// <param name="acceptor"></param>
/// <param name="enumerable"></param>
/// <param name="enumeratorFactory"></param>
template <typename TAcceptor, typename TEnumerable, typename TEnumerator, typename TElement>
void Enumerate(TAcceptor& acceptor, TEnumerable& enumerable, TEnumerator& enumerator)
{
    TEnumerator enumerator = enumeratorFactory();
    while (!enumerable.IsCompleted(enumerator))
    {
        acceptor.Accept(enumerable.Take(enumerator));
    }
}

template <typename TElement>
struct BaseEnumerable
{
    TElement* Source;
    size_t Bound;

    // lock the state into a sequential enumeration and a condition to avoid using enumerators.
    template <typename TAcceptor>
    void Enumerate(TAcceptor& acceptor)
    {
        for (int i = 0; i < Bound; i++)
        {
            if (!acceptor.Accept(Source[i]))
            {
                return;
            }
        }
    }
};

template <typename TTransform, typename TAcceptor>
struct SelectorAcceptor
{
    const TTransform& Transform;
    TAcceptor& Acceptor;

    template <typename TElement>
    bool Accept(const TElement& element)
    {
        return Acceptor.Accept(Transform.Transform(element));
    }
};

template <typename TSrouceSequence, typename TTransform>
struct Selector
{
    TSrouceSequence Source;
    TTransform Transform;

    template <typename TAcceptor>
    void Enumerate(TAcceptor& acceptor)
    {
        SelectorAcceptor<TTransform, TAcceptor> proxyAcceptor = {Transform, acceptor };
        Source.Enumerate(proxyAcceptor);
    }
};

template <typename TSourceSequence, typename TTransform>
Selector<TSourceSequence, TTransform> Select(TSourceSequence sourceSequence, TTransform transform)
{
    Selector<TSourceSequence, TTransform> newSelector{ sourceSequence, transform };
    return newSelector;
}

template <typename TPredicate, typename TAcceptor>
struct BoolFilterAcceptor
{
    const TPredicate& Predicate;
    TAcceptor& Acceptor;

    template <typename TElement>
    bool Accept(const TElement& element)
    {
        if (Predicate.Allow(element))
            return Acceptor.Accept(element);
        else
            return true; // skip the element by not invoking the owner acceptor
    }
};

template <typename TSourceSequence, typename TPredicate>
struct BooleanFilter
{
    TSourceSequence Source;
    TPredicate Predicate;

    template <typename TAcceptor>
    void Enumerate(TAcceptor& acceptor)
    {
        BoolFilterAcceptor<TPredicate, TAcceptor> proxyAcceptor = { Predicate, acceptor };
        Source.Enumerate(proxyAcceptor);
    }
};

template <typename TSourceSequence, typename TPredicate>
BooleanFilter<TSourceSequence, TPredicate> Where(TSourceSequence source, TPredicate pred)
{
    BooleanFilter<TSourceSequence, TPredicate> newFilter{ source, pred };
    return newFilter;
}

}
}
}