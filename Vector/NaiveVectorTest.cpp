#include <cassert>
#include "NaiveVector.h"

using namespace naive;

bool check_empty()
{
    Vector<int> vector;
    return vector.empty();
}

bool check_push_back_pop_back()
{
    Vector<int> vector;

    vector.push_back(5);
    if (vector.size() != 1 || vector.back() != 5) {
        return false;
    }

    vector.pop_back();
    return vector.empty();
}

bool check_insert_clear()
{
    Vector<int> vector;

    int i = 3;
    for (auto it = vector.insert(vector.begin(), i); i < 39; it = vector.insert(it, i)) {
        i += 2;
    }

    i = 39;
    for (auto it = vector.begin(); it != vector.end(); ++it) {
        if (i != *it) {
            return false;
        }
        i -= 2;;
    }

    vector.clear();
    return vector.empty();
}

bool check_erase_one()
{
    Vector<int> vector;

    for (int i = 1; i < 51; ++i) {
        vector.push_back(i);
    }

    auto it = vector.begin();
    for (int i = 1; i < 51; i += 2) {
        if (it == vector.end() || i != *it) {
            return false;
        }

        it = vector.erase(it);
        ++it;
    }

    return true;
}

bool check_erase_range()
{
    Vector<int> vector;

    static const size_t ELEMENTS_LEFT = 5;

    for (int i = 1; i < 51; ++i) {
        vector.push_back(i);
    }

    auto it = vector.begin();
    for (size_t i = 0; i < ELEMENTS_LEFT; ++i) {
        ++it;
    }
    it = vector.erase(it, vector.end());

    if (vector.size() != ELEMENTS_LEFT || it != vector.end()) {
        return false;
    }

    it = vector.begin();
    for (int i = 1; i < ELEMENTS_LEFT; ++i) {
        if (it == vector.end() || i != *it) {
            return false;
        }

        ++it;
    }


    return true;
}

bool check_clear()
{
    Vector<int> vector(5, 7);

    if (vector.size() != 5) {
        return false;
    }

    vector.clear();
    return vector.empty();
}

int main()
{
    assert(check_empty());
    assert(check_push_back_pop_back());
    assert(check_insert_clear());
    assert(check_erase_one());
    assert(check_erase_range());
    assert(check_clear());

    return 0;
}