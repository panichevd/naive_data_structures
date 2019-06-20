#pragma once

#include <utility>

namespace naive {

// TODO: Iterators

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
        bool black() const
        { return m_black; }

        bool red() const
        { return !m_black; }

    public:
        TreeNode* uncle() const;
        TreeNode* grandparent() const;
        TreeNode* sibling() const;

    public:
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

    TreeNode * find(const Key & key) const;

private:
    TreeNode * do_insert(TreeNode * node, TreeNode * parent, const Key & key, const Value & value, TreeNode* & inserted_node);
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
    TreeNode * do_find(TreeNode * node, const Key & key) const;

    TreeNode * find_max(TreeNode * node) const;

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

    // Case 1: Node is red. Then both its children are leafs
    if (node->red()) {
        if (node->m_parent->m_right_child == node) {
            node->m_parent->m_right_child = nullptr;
        } else {
            node->m_parent->m_left_child = nullptr;
        }

        delete node;
        return;
    }

    // Case 2. Node is black and its child is red
    if (child_node != nullptr && child_node->red()) {
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

    // Case 3. Node is black and its both children are black. Because of RB trees properties they are leafs.

    // First delete the node
    TreeNode * parent = node->m_parent;
    if (parent != nullptr) {
        if (parent->m_right_child == node) {
            parent->m_right_child = child_node;
        } else {
            parent->m_left_child = child_node;
        }
    } else {
        m_root = nullptr;
    }

    TreeNode * sibling = node->sibling();
    delete node;

    // Now fix the tree
    do_remove_double_black_repair(child_node, parent, sibling);
}

template <typename Key, typename Value>
void Map<Key, Value>::do_remove_double_black_repair(TreeNode * node, TreeNode * parent, TreeNode * sibling)
{
    // Case 3.1 Node is root, we are done.
    if (parent == nullptr) {
        return;
    }

    // Case 3.2. Sibling is red
    if (sibling->red()) {
        parent->m_black = false;
        sibling->m_black = true;
        if (node == parent->m_left_child) {
            rotate_left(parent);
        } else {
            rotate_right(parent);
        }

        // After rotation node has a new black sibling and a red father
        sibling = node->sibling();
    }

    // Case 3.3. Sibling is black and both sibling children are black
    if ((sibling->m_left_child == nullptr || sibling->m_left_child->black()) &&
        (sibling->m_right_child == nullptr || sibling->m_right_child->black())) {

        if (parent->black()) {
            // Case 3.3.1. Parent is black

            // Recolor sibling to red. Now the subtree starting at parent is a valid RB tree, but its black height of every path in it is smaller by one than
            // black height of any other path in the whole tree. So we have to go upper in the tree and fix it again the same way.
            sibling->m_black = false;
            do_remove_double_black_repair(parent, parent->m_parent, parent->sibling());
        } else {
            // Case 3.3.2. Parent is red
            parent->m_black = true;
            sibling->m_black = false;
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
    if (node == parent->m_left_child && (sibling->m_right_child == nullptr || sibling->m_right_child->black()) && (sibling->m_left_child != nullptr && sibling->m_left_child->red())) {
        // Recolor and rotate around sibling. Now node has a new black sibling and a configuration which will be handled by case 3.5
        sibling->m_black = false;
        sibling->m_left_child->m_black = true;
        rotate_right(sibling);
        sibling = node->sibling();
    } else if (node == parent->m_right_child && (sibling->m_left_child == nullptr || sibling->m_left_child->black()) && (sibling->m_right_child != nullptr && sibling->m_right_child->red())){
        sibling->m_black = false;
        sibling->m_right_child->m_black = true;
        rotate_left(sibling);
        sibling = node->sibling();
    }

    // Case 3.5. Here sibling is black and its right child is red (when node is a left  child)
    //                                  or its left  child is red (when node is a right child)
    sibling->m_black = parent->m_black;
    parent->m_black = true;

    if (node == parent->m_left_child) {
        sibling->m_right_child->m_black = true;
        rotate_left(parent);
    } else {
        sibling->m_left_child->m_black = true;
        rotate_right(parent);
    }
    // Now black-height is the same in the whole tree, so we are done
}

template <typename Key, typename Value>
typename Map<Key, Value>::TreeNode * Map<Key, Value>::find_one_non_leaf_child_node(TreeNode * node)
{
    if (node->m_left_child == nullptr || node->m_right_child == nullptr) {
        return node;
    }

    TreeNode * max_node_left_subtree = find_max(node->m_left_child);

    // TODO: this is ugly... work on the nodes instead of keys/ values
    node->m_key = max_node_left_subtree->m_key;
    node->m_value = std::move(max_node_left_subtree->m_value);

    return max_node_left_subtree;
}

template <typename Key, typename Value>
typename Map<Key, Value>::TreeNode * Map<Key, Value>::find(const Key & key) const
{
    return do_find(m_root, key);
}

template <typename Key, typename Value>
typename Map<Key, Value>::TreeNode * Map<Key, Value>::do_find(TreeNode * node, const Key & key) const
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
typename Map<Key, Value>::TreeNode * Map<Key, Value>::find_max(TreeNode * node) const
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

    // Case 1. Node is root
    if (parent == nullptr) {
        node->m_black = true;
        return;
    }
    
    // Case 2. Parent is black
    if (parent->black()) {
        return;
    }

    TreeNode * uncle = node->uncle();
    TreeNode * grandparent = node->grandparent();

    // Case 3. Parent is red. Uncle is red
    if (uncle != nullptr && uncle->red()) {
        parent->m_black = true;
        uncle->m_black = true;
        grandparent->m_black = false;
        do_insert_repair(grandparent);
        return;
    }

    // Case 4. Parent is red. Uncle is black.
    // TODO: rewrite is with rotate_left/rotate_right
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

template <typename Key, typename Value>
void Map<Key, Value>::rotate_left(TreeNode * node)
{
    TreeNode * child = node->m_right_child;

    child->m_parent = node->m_parent;
    if (node->m_parent != nullptr) {
        if (node == node->m_parent->m_left_child) {
            node->m_parent->m_left_child = child;
        } else {
            node->m_parent->m_right_child = child;
        }
    } else {
        m_root = child;
    }

    node->m_right_child = child->m_left_child;
    if (node->m_right_child != nullptr) {
        node->m_right_child->m_parent = node;
    }

    child->m_left_child = node;
    node->m_parent = child;
}

template <typename Key, typename Value>
void Map<Key, Value>::rotate_right(TreeNode * node)
{
    TreeNode * child = node->m_left_child;

    child->m_parent = node->m_parent;
    if (node->m_parent != nullptr) {
        if (node == node->m_parent->m_left_child) {
            node->m_parent->m_left_child = child;
        } else {
            node->m_parent->m_right_child = child;
        }
    } else {
        m_root = child;
    }

    node->m_left_child = child->m_right_child;
    if (node->m_left_child != nullptr) {
        node->m_left_child->m_parent = node;
    }

    child->m_right_child = node;
    node->m_parent = child;
}

} /*namespace naive*/