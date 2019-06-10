#pragma once

namespace naive {

template <typename Key, typename Value>
class Map
{
private:
    // TODO: accessors (for safety)
    class TreeItem
    {
    public:
        TreeItem() = default;
        TreeItem(TreeItem * parent, const Key & key, const Value & value);

    public:
        TreeItem* uncle() const;
        TreeItem* grandparent() const;

    private:
        friend class Map;

    private:
        TreeItem * m_parent = nullptr;
        TreeItem * m_left_child = nullptr;
        TreeItem * m_right_child = nullptr;
        bool m_black = false;
        
        Key m_key;
        Value m_value;
    };


public:
    Map() = default;

public:
    // TODO: retval
    void insert(const Key & key, const Value & value);

private:
    TreeItem * do_insert(TreeItem * node, TreeItem * parent, const Key & key, const Value & value, TreeItem* & inserted_node);
    void do_insert_repair(TreeItem * node);

    void lr_rotate(TreeItem * node);
    void ll_rotate(TreeItem * node);

private:
    TreeItem * m_root = nullptr;
};

template <typename Key, typename Value>
Map<Key, Value>::TreeItem::TreeItem(TreeItem * parent, const Key & key, const Value & value) :
    m_parent(parent),
    m_key(key),
    m_value(value)
{
}

template <typename Key, typename Value>
typename Map<Key, Value>::TreeItem * Map<Key, Value>::TreeItem::uncle() const
{
    TreeItem * grand_parent = m_parent->m_parent;

    if (grand_parent->m_left_child == m_parent) {
        return grand_parent->m_right_child;
    } else {
        return grand_parent->m_left_child;
    }
}

template <typename Key, typename Value>
typename Map<Key, Value>::TreeItem * Map<Key, Value>::TreeItem::grandparent() const
{
    return m_parent->m_parent;
}

template <typename Key, typename Value>
void Map<Key, Value>::insert(const Key & key, const Value & value)
{
    TreeItem * inserted_node = nullptr;
    m_root = do_insert(m_root, nullptr, key, value, inserted_node);

    do_insert_repair(inserted_node);
}

template <typename Key, typename Value>
typename Map<Key,Value>::TreeItem * Map<Key, Value>::do_insert(TreeItem * node, TreeItem * parent, const Key & key, const Value & value, TreeItem* & inserted_node)
{
    if (node == nullptr) {
        inserted_node = new TreeItem(parent, key, value);
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
void Map<Key, Value>::do_insert_repair(TreeItem * node)
{
    TreeItem * parent = node->m_parent;

    // case 1
    if (parent == nullptr) {
        node->m_black = true;
        return;
    }
    
    // case 2
    if (parent->m_black) {
        return;
    }

    TreeItem * uncle = node->uncle();
    TreeItem * grandparent = node->grandparent();

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
    }
}

template <typename Key, typename Value>
void Map<Key, Value>::lr_rotate(TreeItem * node)
{
    TreeItem * parent = node->m_parent;
    TreeItem * uncle = node->uncle();
    TreeItem * grandparent = node->grandparent();

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
void Map<Key, Value>::ll_rotate(TreeItem * node)
{
    TreeItem * parent = node->m_parent;
    TreeItem * uncle = node->uncle();
    TreeItem * grandparent = node->grandparent();
    TreeItem * grandgrandparent = parent->grandparent();

    grandparent->m_left_child = parent->m_right_child;
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

    parent->m_right_child = grandparent;
    grandparent->m_parent = parent;

    parent->m_black = true;
    grandparent->m_black = false;
}

} /*namespace naive*/