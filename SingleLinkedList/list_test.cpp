#include <cassert>
#include <string>

#include "SingleLinkedList.h"

#include <forward_list>

using namespace naive;

bool check_empty()
{
    List<int> list;
    return list.empty();
}

bool check_push_front()
{
    List<int> list;
    list.push_front(5);

    return list.front() == 5;
}

bool check_push_pop()
{
    List<int> list;

    list.push_front(3);
    if (list.size() != 1) {
        return false;
    }

    list.push_front(2);
    if (list.size() != 2) {
        return false;
    }

    list.pop_front();
    list.pop_front();
    return list.empty();
}

bool check_insert_clear()
{
    List<int> list;

    auto it = list.insert_after(list.before_begin(), 3);
    for (int i = 5; i < 39; i += 2) {
        it = list.insert_after(it, i);
    }

    int i = 3;
    for (auto it = list.begin(); it != list.end(); ++it) {
	    if (i != *it) {
            return false;
        }
        i += 2;
    }

    list.clear();
    return list.empty();
}

bool check_remove()
{
    List<int> list;

    for (int i = 1; i < 51; ++i) {
        list.push_front(i);
    }

    auto it = list.begin();
    for (int i = 50; i > 0; i -= 2) {
        if (it == list.end() || i != *it) {
            return false;
        }

        it = list.erase_after(it);
    }

    return true;
}

class Test
{
public:
    Test() = delete;
    Test(const Test & test) = delete;

    Test(int int_val, std::string && string_val) :
        m_int(int_val),
        m_string(std::move(string_val))
    {
    }

public:
    int m_int;
    std::string m_string;
};

bool check_emplace_clear()
{
    List<Test> list;

    auto it = list.emplace_after(list.before_begin(), 3, "B");
    for (int i = 5; i < 39; i += 2) {
        it = list.emplace_after(it, i, "B");
    }

    int i = 3;
    for (auto it = list.begin(); i < 39; ++it) {
        if (it == list.end() || i != (*it).m_int) {
            return false;
        }
        i += 2;
    }

    list.clear();
    return list.empty();
}

int main()
{
    assert(check_empty());
    assert(check_push_front());
    assert(check_push_pop());
    assert(check_insert_clear());
    assert(check_remove());
    assert(check_emplace_clear());

    return 0;
}