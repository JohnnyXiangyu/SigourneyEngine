#include "ImmutableVector.h"

#include<iostream>

using namespace std;
using namespace OmniParser;

int main()
{
    int source[10] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };

    ImmutableVector<int> vector;
    ImmutableVector<int> initializedVector = vector.AppendCopy(source, source + 10);

    cout << vector.Length() << endl;
    cout << initializedVector.Length() << endl;

    ImmutableVector<int> otherVector = vector.AppendCopy(source + 3, source + 6);
    for (int i = 0; i < otherVector.Length(); i++)
    {
        cout << otherVector.At(i) << endl;
    }
}
