#pragma once

#include <cassert>
#include <utility>
#include <tuple>

namespace naive {

template <typename Key, typename Value>
class Map
{
public:
    typedef std::pair<const Key, Value> ValueType;

private:
    class TreeNode
    {
    public:
        TreeNode() = default;

        template <typename K, typename V>
        TreeNode(TreeNode * parent, K && key, V && value);

        template <typename ... Args>
        TreeNode(TreeNode * parent, Args && ... args);

    public:
        TreeNode * parent() const
        { return m_parent; }

        TreeNode* & parent()
        { return m_parent; }

        TreeNode * left_child() const
        { return m_left_child; }

        TreeNode* & left_child()
        { return m_left_child; }

        TreeNode * right_child() const
        { return m_right_child; }

        TreeNode* & right_child()
        { return m_right_child; }

        void set_color(bool black)
        { m_black = black; }

        void set_black_color()
        { m_black = true; }

        void set_red_color()
        { m_black = false; }

        bool is_black() const
        { return m_black; }

        bool is_red() const
        { return !m_black; }

        const Key & key() const
        { return m_value.first; }

        const ValueType & value() const
        { return m_value; }

        ValueType & value()
        { return m_value; }

    public:
        TreeNode* uncle() const;
        TreeNode* grandparent() const;
        TreeNode* sibling() const;

    private:
        TreeNode * m_parent = nullptr;
        TreeNode * m_left_child = nullptr;
        TreeNode * m_right_child = nullptr;
        bool m_black = false;

        ValueType m_value;
    };

public:
    class ConstIterator;

    class Iterator
    {
    public:
        Iterator() = default;
        explicit Iterator(TreeNode * current);
        Iterator(TreeNode * current, TreeNode * previous);
        //Iterator(const Iterator & it);

    public:
        const ValueType & operator*() const
        { return m_current->value(); }

        ValueType & operator*()
        { return m_current->value(); }

        Iterator & operator++();
        Iterator operator++(int);

        /*Iterator & operator--();
        Iterator operator--(int);*/

        bool operator==(const Iterator & it);
        //bool operator==(const ConstIterator & it);
        bool operator!=(const Iterator & it);
        //bool operator!=(const ConstIterator & it);*/

    private:
        friend class ConstIterator;
        friend class Map;

    private:
        TreeNode * m_current = nullptr;
        TreeNode * m_previous = nullptr;;
    };

    class ConstIterator
    {
    public:
        ConstIterator() = default;
        explicit ConstIterator(const TreeNode * current);
        ConstIterator(const TreeNode * current, const TreeNode * previous);
        ConstIterator(const Iterator & it);

    public:
        const ValueType & operator*() const
        { return m_current->value(); }

        ConstIterator & operator++();
        ConstIterator operator++(int);

        /*Iterator & operator--();
        Iterator operator--(int);*/

        bool operator==(const ConstIterator & it);
        //bool operator==(const ConstIterator & it);
        bool operator!=(const ConstIterator & it);
        //bool operator!=(const ConstIterator & it);*/

    private:
        const TreeNode * m_current = nullptr;
        const TreeNode * m_previous = nullptr;;
    };

public:
    static TreeNode * find_min(const TreeNode * node);
    static TreeNode * find_max(const TreeNode * node);

public:
    Map() = default;
    Map(const Map & map);
    Map(Map && map);
    ~Map();

    Map & operator=(const Map & map);
    Map & operator=(Map && map);

public:
    Value & at(const Key & key);
    const Value & at(const Key & key) const;

    Value & operator[](const Key & key);
    Value & operator[](Key && key);

public:
    Iterator begin();
    Iterator end();

    ConstIterator cbegin();
    ConstIterator cend();

public:
    bool empty() const;
    size_t size() const;

public:
    void clear();

    std::pair<Iterator, bool> insert(const ValueType & value);
    std::pair<Iterator, bool> insert(ValueType && value);
    std::pair<Iterator, bool> insert(Iterator hint, const ValueType & value);
    std::pair<Iterator, bool> insert(Iterator hint, ValueType && value);

    template <typename ... Args>
    std::pair<Iterator, bool> emplace(Args && ... args);

    Iterator erase(Iterator pos);
    Iterator erase(Iterator first, Iterator last);
    size_t erase(const Key & key);

public:
    Iterator find(const Key & key) const;

private:
    TreeNode * do_copy(TreeNode * parent, TreeNode * source_node);
    void do_clear(TreeNode * node);

private:
    template <typename K, typename V>
    TreeNode * do_insert(TreeNode * node, TreeNode * parent, K && key, V && value, TreeNode* & inserted_node, bool & inserted);

    void do_insert_repair(TreeNode * node);

    void lr_rotate(TreeNode * node);
    void ll_rotate(TreeNode * node);
    void rl_rotate(TreeNode * node);
    void rr_rotate(TreeNode * node);

private:
    void rotate_left(TreeNode * node);
    void rotate_right(TreeNode * node);

private:
    void do_remove_double_black_repair(TreeNode * node, TreeNode * parent, TreeNode * sibling);

private:
    TreeNode * find_one_non_leaf_child_node(TreeNode * node);

private:
    TreeNode * do_find(const Key & key) const;
    TreeNode * do_find(TreeNode * node, const Key & key) const;

private:
    TreeNode * m_root = nullptr;
    size_t m_size = 0;
};

} /*namespace naive*/

#include "Map.inc""