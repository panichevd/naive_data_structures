#pragma once

#include <cassert>
#include <utility>

namespace naive {

// TODO: Const Iterators

template <typename Key, typename Value>
class Map
{
private:
    class TreeNode
    {
    public:
        TreeNode() = default;

        template <typename K, typename V>
        TreeNode(TreeNode * parent, K && key, V && value);

    public:
        TreeNode* & parent()
        { return m_parent; }

        TreeNode* & left_child()
        { return m_left_child; }

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
        { return m_key; }

        const Value & value() const
        { return m_value; }

        Value & value()
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

        Key m_key;
        Value m_value;
    };

public:
    class Iterator
    {
    public:
        Iterator() = default;
        explicit Iterator(TreeNode * current);
        Iterator(TreeNode * current, TreeNode * previous);
        //Iterator(const Iterator & it);

    public:
        // TODO: better solution
        std::pair<Key, Value> operator*() const
        {
            return std::make_pair(m_current->key(), m_current->value());
        }

        Value & value()
        { return m_current->value(); }

        /*T & operator*()
        {
            return *m_ptr;
        }*/

        Iterator & operator++();
        Iterator operator++(int);

        /*Iterator & operator--();
        Iterator operator--(int);*/

        bool operator==(const Iterator & it);
        //bool operator==(const ConstIterator & it);
        bool operator!=(const Iterator & it);
        //bool operator!=(const ConstIterator & it);*/

    private:
        TreeNode * m_current = nullptr;
        TreeNode * m_previous = nullptr;;
    };

public:
    static TreeNode * find_min(TreeNode * node);
    static TreeNode * find_max(TreeNode * node);

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
    // TODO cbegin, cend
    Iterator begin();
    Iterator end();

public:
    bool empty() const;
    size_t size() const;

public:
    void clear();

    std::pair<Iterator, bool> insert(const std::pair<const Key, Value> & value);
    std::pair<Iterator, bool> insert(std::pair<const Key, Value> && value);

    void remove(const Key & key);

    //TreeNode * find(const Key & key) const;

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

template <typename Key, typename Value>
template <typename K, typename V>
Map<Key, Value>::TreeNode::TreeNode(TreeNode * parent, K && key, V && value) :
    m_parent(parent),
    m_key(std::forward<K>(key)),
    m_value(std::forward<V>(value))
{
}

template <typename Key, typename Value>
typename Map<Key, Value>::TreeNode * Map<Key, Value>::TreeNode::uncle() const
{
    TreeNode * grand_parent = m_parent->m_parent;

    if (grand_parent->m_left_child == m_parent) {
        return grand_parent->m_right_child;
    } else {
        return grand_parent->m_left_child;
    }
}

template <typename Key, typename Value>
typename Map<Key, Value>::TreeNode * Map<Key, Value>::TreeNode::grandparent() const
{
    return m_parent->m_parent;
}

template <typename Key, typename Value>
typename Map<Key, Value>::TreeNode * Map<Key, Value>::TreeNode::sibling() const
{
    if (m_parent == nullptr) {
        return nullptr;
    }

    if (m_parent->m_right_child == this) {
        return m_parent->m_left_child;
    } else {
        return m_parent->m_right_child;
    }
}

template <typename Key, typename Value>
Map<Key, Value>::Iterator::Iterator(TreeNode * current) :
    m_current(current)
{
    if (m_current->left_child() != nullptr) {
        m_previous = m_current->left_child();
    } else {
        m_previous = m_current->parent();
    }
}

template <typename Key, typename Value>
Map<Key, Value>::Iterator::Iterator(TreeNode * current, TreeNode * previous) :
    m_current(current),
    m_previous(previous)
{
}

template <typename Key, typename Value>
typename Map<Key, Value>::Iterator & Map<Key, Value>::Iterator::operator++()
{
    while (true) {
        // End when we're at the root and there is either no right subtree or we have already traversed it
        if (m_current->parent() == nullptr &&
                (m_current->right_child() == nullptr || m_previous == m_current->right_child())) {
            m_previous = m_current = nullptr;
            return *this;
        }

        if (m_previous == m_current->parent() || m_previous == m_current->left_child()) {
            // If we came from above or left - means we have already been at the left subtree

            // First try to go to the min node of the right subtree
            if (m_current->right_child() != nullptr) {
                m_current = Map::find_min(m_current->right_child());
                m_previous = m_current->parent();
                return *this;
            }

            // As there is no right element, go up
            m_previous = m_current;
            m_current = m_current->parent();
            if (m_previous->parent()->left_child() == m_previous) {
                // If we were at the left subtree - it is the next element
                return *this;
            }
        } else {
            // If we came from right - we can only go up
            m_previous = m_current;
            m_current = m_current->parent();
            if (m_previous->parent()->left_child() == m_previous) {
                // If we were at the left subtree - it is the next element
                return *this;
            }
        }
    }

    assert(false);
    return *this;
}

template <typename Key, typename Value>
typename Map<Key, Value>::Iterator Map<Key, Value>::Iterator::operator++(int)
{
    Iterator it = *this;
    operator++();
    return it;
}

template <typename Key, typename Value>
bool Map<Key, Value>::Iterator::operator==(const Iterator & it)
{
    return m_current == it.m_current;
}

template <typename Key, typename Value>
bool Map<Key, Value>::Iterator::operator!=(const Iterator & it)
{
    return m_current != it.m_current;
}

template <typename Key, typename Value>
Map<Key, Value>::Map(const Map & map) :
    m_size(map.m_size)
{
    m_root = do_copy(nullptr, map.m_root);
}

template <typename Key, typename Value>
Map<Key, Value> & Map<Key, Value>::operator=(const Map & map)
{
    clear();
    m_size = map.m_size;
    m_root = do_copy(nullptr, map.m_root);
    return *this;
}

template <typename Key, typename Value>
Map<Key, Value> & Map<Key, Value>::operator=(Map && map)
{
    std::swap(m_size, map.m_size);
    std::swap(m_root, map.m_root);
    return *this;
}

template <typename Key, typename Value>
Map<Key, Value>::Map(Map && map) :
    m_root(map.m_root),
    m_size(map.m_size)
{
    map.m_root = nullptr;
    map.m_size = 0;
}

template <typename Key, typename Value>
Map<Key, Value>::~Map()
{
    if (m_root != nullptr) {
        do_clear(m_root);
    }
}

template <typename Key, typename Value>
Value & Map<Key, Value>::at(const Key & key)
{
    TreeNode * node = do_find(key);

    if (node == nullptr) {
        throw std::out_of_range("");
    }
    return node->value();
}

template <typename Key, typename Value>
const Value & Map<Key, Value>::at(const Key & key) const
{
    TreeNode * node = do_find(key);

    if (node == nullptr) {
        throw std::out_of_range("");
    }
    return node->value();
}

template <typename Key, typename Value>
Value & Map<Key, Value>::operator[](const Key & key)
{
    TreeNode * inserted_node = nullptr;
    bool inserted = false;
    m_root = do_insert(m_root, nullptr, key, Value(), inserted_node, inserted);

    return inserted_node->value();
}

template <typename Key, typename Value>
Value & Map<Key, Value>::operator[](Key && key)
{
    TreeNode * inserted_node = nullptr;
    bool inserted = false;
    m_root = do_insert(m_root, nullptr, std::move(key), Value(), inserted_node, inserted);

    return inserted_node->value();
}

template <typename Key, typename Value>
typename Map<Key, Value>::Iterator Map<Key, Value>::begin()
{
    if (m_root == nullptr) {
        return end();
    }

    TreeNode * min = find_min(m_root);
    return Iterator(min, min->parent());
}

template <typename Key, typename Value>
typename Map<Key, Value>::Iterator Map<Key, Value>::end()
{
    return Iterator(nullptr, nullptr);
}

template <typename Key, typename Value>
bool Map<Key, Value>::empty() const
{
    return m_size == 0;
}

template <typename Key, typename Value>
size_t Map<Key, Value>::size() const
{
    return m_size;
}

template <typename Key, typename Value>
void Map<Key, Value>::clear()
{
    if (m_root != nullptr) {
        do_clear(m_root);
        m_root = nullptr;
        m_size = 0;
    }
}

template<typename Key, typename Value>
std::pair<typename Map<Key, Value>::Iterator, bool> Map<Key, Value>::insert(const std::pair<const Key, Value> & value)
{
    TreeNode * inserted_node = nullptr;
    bool inserted = true;
    m_root = do_insert(m_root, nullptr, value.first, value.second, inserted_node, inserted);

    if (inserted) {
        do_insert_repair(inserted_node);
        ++m_size;
    }

    return std::make_pair(Iterator(inserted_node), inserted);
}

template<typename Key, typename Value>
std::pair<typename Map<Key, Value>::Iterator, bool> Map<Key, Value>::insert(std::pair<const Key, Value> && value)
{
    TreeNode * inserted_node = nullptr;
    bool inserted = true;
    m_root = do_insert(m_root, nullptr, std::move(value.first), std::move(value.second), inserted_node, inserted);

    if (inserted) {
        do_insert_repair(inserted_node);
        ++m_size;
    }

    return std::make_pair(Iterator(inserted_node), inserted);
}

template <typename Key, typename Value>
void Map<Key, Value>::remove(const Key & key)
{
    TreeNode * node = find(key);

    node = find_one_non_leaf_child_node(node);

    TreeNode * child_node = (node->left_child() != nullptr) ? node->left_child() : node->right_child();

    // Case 1: Node is red. Then both its children are leafs
    if (node->is_red()) {
        if (node->parent()->right_child() == node) {
            node->parent()->right_child() = nullptr;
        } else {
            node->parent()->left_child() = nullptr;
        }

        delete node;
        return;
    }

    // Case 2. Node is black and its child is red
    if (child_node != nullptr && child_node->is_red()) {
        if (node->parent() != nullptr) {
            if (node->parent()->right_child() == node) {
                node->parent()->right_child() = child_node;
            } else {
                node->parent()->left_child() = child_node;
            }
        } else {
            m_root = child_node;
        }

        child_node->parent() = node->parent();
        child_node->set_black_color();

        delete node;
        return;
    }

    // Case 3. Node is black and its both children are black. Because of RB trees properties they are leafs.

    // First delete the node
    TreeNode * parent = node->parent();
    if (parent != nullptr) {
        if (parent->right_child() == node) {
            parent->right_child() = child_node;
        } else {
            parent->left_child() = child_node;
        }
    } else {
        m_root = nullptr;
    }

    TreeNode * sibling = node->sibling();
    delete node;

    // Now fix the tree
    do_remove_double_black_repair(child_node, parent, sibling);

    --m_size;
}

template <typename Key, typename Value>
typename Map<Key, Value>::TreeNode * Map<Key, Value>::do_copy(TreeNode * parent, TreeNode * source_node)
{
    TreeNode * node = nullptr;

    if (source_node != nullptr) {
        node = new TreeNode(parent, source_node->key(), source_node->value());
        node->set_color(source_node->is_black());
        node->left_child() = do_copy(node, source_node->left_child());
        node->right_child() = do_copy(node, source_node->right_child());
    }

    return node;
}

template <typename Key, typename Value>
void Map<Key, Value>::do_clear(TreeNode * node)
{
    if (node->left_child() != nullptr) {
        do_clear(node->left_child());
    }

    if (node->right_child() != nullptr) {
        do_clear(node->right_child());
    }

    delete node;
}

template <typename Key, typename Value>
void Map<Key, Value>::do_remove_double_black_repair(TreeNode * node, TreeNode * parent, TreeNode * sibling)
{
    // Case 3.1 Node is root, we are done.
    if (parent == nullptr) {
        return;
    }

    // Case 3.2. Sibling is red
    if (sibling->is_red()) {
        parent->set_red_color();
        sibling->set_black_color();
        if (node == parent->left_child()) {
            rotate_left(parent);
        } else {
            rotate_right(parent);
        }

        // After rotation node has a new black sibling and a red father
        sibling = node->sibling();
    }

    // Case 3.3. Sibling is black and both sibling children are black
    if ((sibling->left_child()  == nullptr || sibling->left_child()->is_black()) &&
        (sibling->right_child() == nullptr || sibling->right_child()->is_black())) {

        if (parent->is_black()) {
            // Case 3.3.1. Parent is black

            // Recolor sibling to red. Now the subtree starting at parent is a valid RB tree, but its black height of every path in it is smaller by one than
            // black height of any other path in the whole tree. So we have to go upper in the tree and fix it again the same way.
            sibling->set_red_color();
            do_remove_double_black_repair(parent, parent->parent(), parent->sibling());
        } else {
            // Case 3.3.2. Parent is red
            parent->set_black_color();
            sibling->set_red_color();
        }

        // In both cases we are done here.
        //
        // In case 3.3.1 the algorithm will go recursively upward the tree, on each step getting a valid RB subtree of the whole tree.
        // It will stop when it either reaches the root or reach a case when imbalance will be fixed by rotation/recoloring
        //
        // In case 3.3.2 recoloring is enough to have the same RB height in the whole tree.
        return;
    }

    // Case 3.4. Node is a left child, sibling is black, its right child is black and left child is red (or symmetrically when node is a right child)
    if (node == parent->left_child() && (sibling->right_child() == nullptr || sibling->right_child()->is_black()) && (sibling->left_child() != nullptr && sibling->left_child()->is_red())) {
        // Recolor and rotate around sibling. Now node has a new black sibling and a configuration which will be handled by case 3.5
        sibling->set_red_color();
        sibling->left_child()->set_black_color();
        rotate_right(sibling);
        sibling = node->sibling();
    } else if (node == parent->right_child() && (sibling->left_child() == nullptr || sibling->left_child()->is_black()) && (sibling->right_child() != nullptr && sibling->right_child()->is_red())){
        sibling->set_red_color();
        sibling->right_child()->set_black_color();
        rotate_left(sibling);
        sibling = node->sibling();
    }

    // Case 3.5. Here sibling is black and its right child is red (when node is a left  child)
    //                                  or its left  child is red (when node is a right child)
    
    sibling->set_color(parent->is_black());
    parent->set_black_color();

    if (node == parent->left_child()) {
        sibling->right_child()->set_black_color();
        rotate_left(parent);
    } else {
        sibling->left_child()->set_black_color();
        rotate_right(parent);
    }
    // Now black-height is the same in the whole tree, so we are done
}

template <typename Key, typename Value>
typename Map<Key, Value>::TreeNode * Map<Key, Value>::find_one_non_leaf_child_node(TreeNode * node)
{
    if (node->left_child() == nullptr || node->right_child() == nullptr) {
        return node;
    }

    TreeNode * child = find_max(node->left_child());

    // Swap the nodes
    child->right_child() = node->right_child();
    child->right_child()->parent() = child; // right child of node is not null as per condition in the method start
    node->right_child() = nullptr; // right child of max_node_left_subtree is nullptr as it is max element in the subtree

    TreeNode * node_left_child = node->left_child();
    node->left_child() = child->left_child();
    if (node->left_child() != nullptr) {
        node->left_child()->parent() = node;
    }

    TreeNode * child_parent = child->parent();
    if (node->parent() != nullptr) {
        if (node->parent()->left_child() == node) {
            node->parent()->left_child() = child;
        } else {
            node->parent()->right_child() = child;
        }
    } else {
        m_root = child;
    }
    child->parent() = node->parent();

    if (node_left_child == child) {
        child->left_child() = node;
        node->parent() = child;
    } else {
        child->left_child() = node_left_child;
        child->left_child()->parent() = child; // child->left_child() is not nullptr as we went left from node while searching for the child

        node->parent() = child_parent;
        node->parent()->right_child() = node; // We always went right in the left subtree, and it is not the first node
    }

    bool tmp_color = child->is_black();
    child->set_color(node->is_black());
    node->set_color(tmp_color);

    return node;
}

template <typename Key, typename Value>
typename Map<Key, Value>::TreeNode * Map<Key, Value>::do_find(const Key & key) const
{
    return do_find(m_root, key);
}

template <typename Key, typename Value>
typename Map<Key, Value>::TreeNode * Map<Key, Value>::do_find(TreeNode * node, const Key & key) const
{
    if (node == nullptr) {
        return nullptr;
    }

    if (key == node->key()) {
        return node;
    }

    if (key < node->key()) {
        return do_find(node->left_child(), key);
    }

    return do_find(node->right_child(), key);
}

template <typename Key, typename Value>
typename Map<Key, Value>::TreeNode * Map<Key, Value>::find_min(TreeNode * node)
{
    if (node->left_child() == nullptr) {
        return node;
    }

    return find_min(node->left_child());
}

template <typename Key, typename Value>
typename Map<Key, Value>::TreeNode * Map<Key, Value>::find_max(TreeNode * node)
{
    if (node->right_child() == nullptr) {
        return node;
    }

    return find_max(node->right_child());
}

template <typename Key, typename Value>
template <typename K, typename V>
typename Map<Key,Value>::TreeNode * Map<Key, Value>::do_insert(TreeNode * node, TreeNode * parent, K && key, V && value, TreeNode* & inserted_node, bool & inserted)
{
    if (node == nullptr) {
        inserted_node = new TreeNode(parent, std::forward<K>(key), std::forward<V>(value));
        return inserted_node;
    }

    if (key < node->key()) {
        node->left_child() = do_insert(node->left_child(), node, std::forward<K>(key), std::forward<V>(value), inserted_node, inserted);
    } else if (key > node->key()) {
        node->right_child() = do_insert(node->right_child(), node, std::forward<K>(key), std::forward<V>(value), inserted_node, inserted);
    } else {
        inserted_node = node;
        inserted = false;
    }

    return node;
}

template <typename Key, typename Value>
void Map<Key, Value>::do_insert_repair(TreeNode * node)
{
    TreeNode * parent = node->parent();

    // Case 1. Node is root
    if (parent == nullptr) {
        node->set_black_color();
        return;
    }
    
    // Case 2. Parent is black
    if (parent->is_black()) {
        return;
    }

    TreeNode * uncle = node->uncle();
    TreeNode * grandparent = node->grandparent();

    // Case 3. Parent is red. Uncle is red
    if (uncle != nullptr && uncle->is_red()) {
        parent->set_black_color();
        uncle->set_black_color();
        grandparent->set_red_color();
        do_insert_repair(grandparent);
        return;
    }

    // Case 4. Parent is red. Uncle is black.
    if (parent == grandparent->left_child()) {
        // Left Rotate
        if (node == parent->right_child()) {
            lr_rotate(node);
        } else {
            ll_rotate(node);
        }
    } else {
        // Right Rotate
        if (node == parent->left_child()) {
            rl_rotate(node);
        } else {
            rr_rotate(node);
        }
    }
}

template <typename Key, typename Value>
void Map<Key, Value>::lr_rotate(TreeNode * node)
{
    rotate_left(node->parent());
    ll_rotate(node->left_child());
}

template <typename Key, typename Value>
void Map<Key, Value>::ll_rotate(TreeNode * node)
{
    node->parent()->set_black_color();

    TreeNode * grandparent = node->grandparent();
    grandparent->set_red_color();
    rotate_right(grandparent);
}

template <typename Key, typename Value>
void Map<Key, Value>::rl_rotate(TreeNode * node)
{
    rotate_right(node->parent());
    rr_rotate(node->right_child());
}

template <typename Key, typename Value>
void Map<Key, Value>::rr_rotate(TreeNode * node)
{
    node->parent()->set_black_color();

    TreeNode * grandparent = node->grandparent();
    grandparent->set_red_color();
    rotate_left(grandparent);
}

template <typename Key, typename Value>
void Map<Key, Value>::rotate_left(TreeNode * node)
{
    TreeNode * child = node->right_child();

    child->parent() = node->parent();
    if (node->parent() != nullptr) {
        if (node == node->parent()->left_child()) {
            node->parent()->left_child() = child;
        } else {
            node->parent()->right_child() = child;
        }
    } else {
        m_root = child;
    }

    node->right_child() = child->left_child();
    if (node->right_child() != nullptr) {
        node->right_child()->parent() = node;
    }

    child->left_child() = node;
    node->parent() = child;
}

template <typename Key, typename Value>
void Map<Key, Value>::rotate_right(TreeNode * node)
{
    TreeNode * child = node->left_child();

    child->parent() = node->parent();
    if (node->parent() != nullptr) {
        if (node == node->parent()->left_child()) {
            node->parent()->left_child() = child;
        } else {
            node->parent()->right_child() = child;
        }
    } else {
        m_root = child;
    }

    node->left_child() = child->right_child();
    if (node->left_child() != nullptr) {
        node->left_child()->parent() = node;
    }

    child->right_child() = node;
    node->parent() = child;
}

} /*namespace naive*/