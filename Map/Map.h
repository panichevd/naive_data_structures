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
        TreeNode* & parent()
        { return m_parent; }

        TreeNode* & left_child()
        { return m_left_child; }

        TreeNode* & right_child()
        { return m_right_child; }

        void set_black_color()
        { m_black = true; }

        void set_red_color()
        { m_black = false; }

        bool is_black() const
        { return m_black; }

        bool is_red() const
        { return !m_black; }

    public:
        TreeNode* uncle() const;
        TreeNode* grandparent() const;
        TreeNode* sibling() const;

    public:
        Key m_key;
        Value m_value;

    private:
        TreeNode * m_parent = nullptr;
        TreeNode * m_left_child = nullptr;
        TreeNode * m_right_child = nullptr;
        bool m_black = false;
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
    
    if (parent->is_black()) {
        sibling->set_black_color();
    } else {
        sibling->set_red_color();
    }
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

    TreeNode * max_node_left_subtree = find_max(node->left_child());

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
        return do_find(node->right_child(), key);
    }

    return do_find(node->left_child(), key);
}

template <typename Key, typename Value>
typename Map<Key, Value>::TreeNode * Map<Key, Value>::find_max(TreeNode * node) const
{
    if (node->right_child() == nullptr) {
        return node;
    }

    return find_max(node->right_child());
}

template <typename Key, typename Value>
typename Map<Key,Value>::TreeNode * Map<Key, Value>::do_insert(TreeNode * node, TreeNode * parent, const Key & key, const Value & value, TreeNode* & inserted_node)
{
    if (node == nullptr) {
        inserted_node = new TreeNode(parent, key, value);
        return inserted_node;
    }

    if (key < node->m_key) {
        node->left_child() = do_insert(node->left_child(), node, key, value, inserted_node);
    } else if (key > node->m_key) {
        node->right_child() = do_insert(node->right_child(), node, key, value, inserted_node);
    } else {
        inserted_node = node;
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
    // TODO: rewrite is with rotate_left/rotate_right
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
    TreeNode * parent = node->parent();
    TreeNode * uncle = node->uncle();
    TreeNode * grandparent = node->grandparent();

    parent->right_child() = node->left_child();
    if (parent->right_child() != nullptr) {
        parent->right_child()->parent() = parent;
    }

    node->left_child() = parent;
    parent->parent() = node;

    grandparent->left_child() = node;
    node->parent() = grandparent;

    ll_rotate(parent);
}

template <typename Key, typename Value>
void Map<Key, Value>::ll_rotate(TreeNode * node)
{
    TreeNode * parent = node->parent();
    TreeNode * uncle = node->uncle();
    TreeNode * grandparent = node->grandparent();
    TreeNode * grandgrandparent = parent->grandparent();

    grandparent->left_child() = parent->parent();
    if (grandparent->left_child() != nullptr) {
        grandparent->left_child()->parent() = grandparent;
    }

    if (grandgrandparent != nullptr) {
        if (grandgrandparent->left_child() == grandparent) {
            grandgrandparent->left_child() = parent;
        } else {
            grandgrandparent->right_child() = parent;
        }

        parent->parent() = grandgrandparent;
    } else {
        parent->parent() = nullptr;
        m_root = parent;
    }

    parent->right_child() = grandparent;
    grandparent->parent() = parent;

    parent->set_black_color();
    grandparent->set_red_color();
}

template <typename Key, typename Value>
void Map<Key, Value>::rl_rotate(TreeNode * node)
{
    TreeNode * parent = node->parent();
    TreeNode * uncle = node->uncle();
    TreeNode * grandparent = node->grandparent();

    parent->left_child() = node->right_child();
    if (parent->left_child() != nullptr) {
        parent->left_child()->parent() = parent;
    }

    node->right_child() = parent;
    parent->parent() = node;

    grandparent->right_child() = node;
    node->parent() = grandparent;

    rr_rotate(parent);
}

template <typename Key, typename Value>
void Map<Key, Value>::rr_rotate(TreeNode * node)
{
    TreeNode * parent = node->parent();
    TreeNode * uncle = node->uncle();
    TreeNode * grandparent = node->grandparent();
    TreeNode * grandgrandparent = parent->grandparent();

    grandparent->right_child() = parent->left_child();
    if (grandparent->right_child() != nullptr) {
        grandparent->right_child()->parent() = grandparent;
    }

    if (grandgrandparent != nullptr) {
        if (grandgrandparent->left_child() == grandparent) {
            grandgrandparent->left_child() = parent;
        } else {
            grandgrandparent->right_child() = parent;
        }

        parent->parent() = grandgrandparent;
    } else {
        parent->parent() = nullptr;
        m_root = parent;
    }

    parent->left_child() = grandparent;
    grandparent->parent() = parent;

    parent->set_black_color();
    grandparent->set_red_color();
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