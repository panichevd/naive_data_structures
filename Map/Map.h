#pragma once

#include <utility>

namespace naive {

template <typename Key, typename Value>
class Map
{
private:
    // TODO: accessors (for safety)
    class TreeNode
    {
    public:
        TreeNode() = default;
        TreeNode(TreeNode * parent, const Key & key, const Value & value);

    public:
        TreeNode* uncle() const;
        TreeNode* grandparent() const;
        TreeNode* sibling() const;

    private:
        friend class Map;

    private:
        TreeNode * m_parent = nullptr;
        TreeNode * m_left_child = nullptr;
        TreeNode * m_right_child = nullptr;
        bool m_black = false;
        
        Key m_key;
        Value m_value;
    };


public:
    Map() = default;

public:
    // TODO: retval
    void insert(const Key & key, const Value & value);

    void remove(const Key & key);

    TreeNode * find(const Key & key);

private:
    TreeNode * do_insert(TreeNode * node, TreeNode * parent, const Key & key, const Value & value, TreeNode* & inserted_node);
    void do_insert_repair(TreeNode * node);

    void lr_rotate(TreeNode * node);
    void ll_rotate(TreeNode * node);
    void rl_rotate(TreeNode * node);
    void rr_rotate(TreeNode * node);

private:
    void remove_black_black(TreeNode * node);

private:
    TreeNode * find_one_non_leaf_child_node(TreeNode * node);

private:
    TreeNode * do_find(TreeNode * node, const Key & key);

    TreeNode * find_max(TreeNode * node);

private:
    TreeNode * m_root = nullptr;
};

template <typename Key, typename Value>
Map<Key, Value>::TreeNode::TreeNode(TreeNode * parent, const Key & key, const Value & value) :
    m_parent(parent),
    m_key(key),
    m_value(value)
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
    if (m_parent->m_right_child == this) {
        return m_parent->m_left_child;
    } else {
        return m_parent->m_right_child;
    }
}

template <typename Key, typename Value>
void Map<Key, Value>::insert(const Key & key, const Value & value)
{
    TreeNode * inserted_node = nullptr;
    m_root = do_insert(m_root, nullptr, key, value, inserted_node);

    do_insert_repair(inserted_node);
}

template <typename Key, typename Value>
void Map<Key, Value>::remove(const Key & key)
{
    TreeNode * node = find(key);

    node = find_one_non_leaf_child_node(node);

    TreeNode * child_node = (node->m_left_child != nullptr) ? node->m_left_child : node->m_right_child;

    // Case 1: Node is red. Then its child is black.
    if (!node->m_black) {
        if (node->m_parent->m_right_child == node) {
            node->m_parent->m_right_child = child_node;
        } else {
            node->m_parent->m_left_child = child_node;
        }

        if (child_node != nullptr) {
            child_node->m_parent = node->m_parent;
        }
        delete node;
        return;
    }

    // Case 2. Node is black and its child is red
    if (child_node != nullptr && !child_node->m_black) {
        if (node->m_parent != nullptr) {
            if (node->m_parent->m_right_child == node) {
                node->m_parent->m_right_child = child_node;
            } else {
                node->m_parent->m_left_child = child_node;
            }
        } else {
            m_root = child_node;
        }

        child_node->m_parent = node->m_parent;
        child_node->m_black = true;

        delete node;
        return;
    }

    // Case 3. Node is black and its child is black. Then both node's children are leafs due to Red-Black trees properties
    remove_black_black(node);
}

template <typename Key, typename Value>
void Map<Key, Value>::remove_black_black(TreeNode * node)
{
    // Case 3.1. Node is root
    if (node->m_parent == nullptr) {
        m_root = nullptr;
        delete node;
        return;
    }

    TreeNode * sibling = node->sibling();

    // Case 3.2. Sibling is red
    if (!sibling->m_black) {
        node->m_parent->m_black = false;
        sibling->m_black = true;

        // TODO: rotate
    }

    // TODO: case 3.3
}

template <typename Key, typename Value>
typename Map<Key, Value>::TreeNode * Map<Key, Value>::find_one_non_leaf_child_node(TreeNode * node)
{
    if (node->m_left_child == nullptr || node->m_right_child == nullptr) {
        return node;
    }

    TreeNode * max_node_left_subtree = find_max(node->m_left_child);

    node->m_key = max_node_left_subtree->m_key;
    node->m_value = std::move(max_node_left_subtree->m_value);

    return max_node_left_subtree;
}

template <typename Key, typename Value>
typename Map<Key, Value>::TreeNode * Map<Key, Value>::find(const Key & key)
{
    return do_find(m_root, key);
}

template <typename Key, typename Value>
typename Map<Key, Value>::TreeNode * Map<Key, Value>::do_find(TreeNode * node, const Key & key)
{
    if (node == nullptr) {
        return nullptr;
    }

    if (node->m_key == key) {
        return node;
    }

    if (node->m_key < key) {
        return do_find(node->m_right_child, key);
    }

    return do_find(node->m_left_child, key);
}

template <typename Key, typename Value>
typename Map<Key, Value>::TreeNode * Map<Key, Value>::find_max(TreeNode * node)
{
    if (node->m_right_child == nullptr) {
        return node;
    }

    return find_max(node->m_right_child);
}

template <typename Key, typename Value>
typename Map<Key,Value>::TreeNode * Map<Key, Value>::do_insert(TreeNode * node, TreeNode * parent, const Key & key, const Value & value, TreeNode* & inserted_node)
{
    if (node == nullptr) {
        inserted_node = new TreeNode(parent, key, value);
        return inserted_node;
    }

    if (key < node->m_key) {
        node->m_left_child  = do_insert(node->m_left_child, node, key, value, inserted_node);
    } else if (key > node->m_key) {
        node->m_right_child = do_insert(node->m_right_child, node, key, value, inserted_node);
    } else {
        inserted_node = node;
    }

    return node;
}

template <typename Key, typename Value>
void Map<Key, Value>::do_insert_repair(TreeNode * node)
{
    TreeNode * parent = node->m_parent;

    // case 1
    if (parent == nullptr) {
        node->m_black = true;
        return;
    }
    
    // case 2
    if (parent->m_black) {
        return;
    }

    TreeNode * uncle = node->uncle();
    TreeNode * grandparent = node->grandparent();

    // case 3
    if (uncle != nullptr && !(uncle->m_black)) {
        parent->m_black = true;
        uncle->m_black = true;
        grandparent->m_black = false;
        do_insert_repair(grandparent);
        return;
    }

    // case 4
    if (parent == grandparent->m_left_child) {
        // Left Rotate
        if (node == parent->m_right_child) {
            lr_rotate(node);
        } else {
            ll_rotate(node);
        }
    } else {
        // Right Rotate
        if (node == parent->m_left_child) {
            rl_rotate(node);
        } else {
            rr_rotate(node);
        }
    }
}

template <typename Key, typename Value>
void Map<Key, Value>::lr_rotate(TreeNode * node)
{
    TreeNode * parent = node->m_parent;
    TreeNode * uncle = node->uncle();
    TreeNode * grandparent = node->grandparent();

    parent->m_right_child = node->m_left_child;
    if (parent->m_right_child != nullptr) {
        parent->m_right_child->m_parent = parent;
    }

    node->m_left_child = parent;
    parent->m_parent = node;

    grandparent->m_left_child = node;
    node->m_parent = grandparent;

    ll_rotate(parent);
}

template <typename Key, typename Value>
void Map<Key, Value>::ll_rotate(TreeNode * node)
{
    TreeNode * parent = node->m_parent;
    TreeNode * uncle = node->uncle();
    TreeNode * grandparent = node->grandparent();
    TreeNode * grandgrandparent = parent->grandparent();

    grandparent->m_left_child = parent->m_right_child;
    if (grandparent->m_left_child != nullptr) {
        grandparent->m_left_child->m_parent = grandparent;
    }

    if (grandgrandparent != nullptr) {
        if (grandgrandparent->m_left_child == grandparent) {
            grandgrandparent->m_left_child = parent;
        } else {
            grandgrandparent->m_right_child = parent;
        }

        parent->m_parent = grandgrandparent;
    } else {
        parent->m_parent = nullptr;
        m_root = parent;
    }

    parent->m_right_child = grandparent;
    grandparent->m_parent = parent;

    parent->m_black = true;
    grandparent->m_black = false;
}

template <typename Key, typename Value>
void Map<Key, Value>::rl_rotate(TreeNode * node)
{
    TreeNode * parent = node->m_parent;
    TreeNode * uncle = node->uncle();
    TreeNode * grandparent = node->grandparent();

    parent->m_left_child = node->m_right_child;
    if (parent->m_left_child != nullptr) {
        parent->m_left_child->m_parent = parent;
    }

    node->m_right_child = parent;
    parent->m_parent = node;

    grandparent->m_right_child = node;
    node->m_parent = grandparent;

    rr_rotate(parent);
}

template <typename Key, typename Value>
void Map<Key, Value>::rr_rotate(TreeNode * node)
{
    TreeNode * parent = node->m_parent;
    TreeNode * uncle = node->uncle();
    TreeNode * grandparent = node->grandparent();
    TreeNode * grandgrandparent = parent->grandparent();

    grandparent->m_right_child = parent->m_left_child;
    if (grandparent->m_right_child != nullptr) {
        grandparent->m_right_child->m_parent = grandparent;
    }

    if (grandgrandparent != nullptr) {
        if (grandgrandparent->m_left_child == grandparent) {
            grandgrandparent->m_left_child = parent;
        } else {
            grandgrandparent->m_right_child = parent;
        }

        parent->m_parent = grandgrandparent;
    } else {
        parent->m_parent = nullptr;
        m_root = parent;
    }

    parent->m_left_child = grandparent;
    grandparent->m_parent = parent;

    parent->m_black = true;
    grandparent->m_black = false;
}

} /*namespace naive*/