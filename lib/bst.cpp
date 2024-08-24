#include <numeric>
#include <functional>

#include "bst.h"



template <typename Key, typename Compare = std::less<Key>, typename Allocator = std::allocator<Key>>
class BinarySearchTree {
private:
    struct BaseNode {
        BaseNode* left = nullptr;
        BaseNode* right = nullptr;
        BaseNode* parent = nullptr;
    };

    struct Node: BaseNode {
        Node(const Key key): key(key) {}

        const Key key;
    };

    template<typename traversal_type = InOrder>
    class Iterator {
        friend BinarySearchTree;

    public:
        using pointer_type = const Key*;
        using reference_type = const Key&;
        using difference_type = size_t;
        using value_type = Key;
        using key_type = Key;

    private:
        BaseNode* node_;

        Iterator(BaseNode* node): node_(node) {}

    public:
        Iterator(const Iterator<traversal_type>& other) {
            node_ = other.node_;
        }

        Iterator& operator=(const Iterator<traversal_type>& other) {
            node_ = other.node_;
            return *this;
        }

        reference_type operator*() const {
            return static_cast<Node*>(node_)->key;
        }

        pointer_type operator->() const {
            return &static_cast<Node*>(node_)->key;
        }


        Iterator& operator++() {
            Increment(traversal_type{});
            return *this;
        }


        Iterator operator++(int) {
            iterator<traversal_type> iterator_copy = *this;
            ++(*this);
            return iterator_copy;
        }


        Iterator& operator--() {
            Decrement(traversal_type{});
            return *this;
        }


        Iterator operator--(int)  {
            iterator<traversal_type> iterator_copy = *this;
            --(*this);
            return iterator_copy;
        }

        bool operator==(const Iterator&) const = default;

        bool operator!=(const Iterator&) const = default;

    private:
        void Increment(InOrder) {
            if (node_->right) {
                node_ = node_->right;
                while (node_->left) {
                    node_ = node_->left;
                }
            } else {
                while (node_->parent->left != node_) {
                    node_ = node_->parent;
                }
                node_ = node_->parent;
            }
        }

        void Increment(PreOrder) {
            if (node_->left) {
                node_ = node_->left;
                return;
            }
            if (node_->right) {
                node_ = node_->right;
                return;
            }
            while (!(node_->parent->left == node_ && node_->parent->right)) {
                node_ = node_->parent;
            }
            node_ = node_->parent;

            if (!IsFakeNode(node_)) {
                node_ = node_->right;
            }
        }


        void Increment(PostOrder) {
            if (IsFakeNode(node_->parent) || node_->parent->right == node_) {
                node_ = node_->parent;
                return;
            }

            node_ = node_->parent;
            if (!node_->right) {
                return;
            }
            node_ = node_->right;
            while (node_->left || node_->right) {
                if (node_->left) {
                    node_= node_->left;
                } else {
                    node_ = node_->right;
                }
            }
        }

        void Decrement(InOrder) {
            if (node_->left) {
                node_ = node_->left;
                while (node_->right) {
                    node_ = node_->right;
                }
            } else {
                while (node_->parent->right != node_) {
                    node_ = node_->parent;
                }
                node_ = node_->parent;
            }
        }

        void Decrement(PreOrder) {
            if (node_->parent->left == node_) {
                node_ = node_->parent;
                return;
            }

            if (!IsFakeNode(node_)) {
                node_ = node_->parent;
                if (!node_->left) {
                    return;
                }
            }
            node_ = node_->left;

            while (node_->right || node_->left) {
                if (node_->right) {
                    node_ = node_->right;
                } else {
                    node_ = node_->left;
                }
            }
        }

        void Decrement(PostOrder)  {
            if (IsFakeNode(node_)) {
                node_ = node_->left;
                return;
            }
            if (node_->right) {
                node_ = node_->right;
                return;
            }
            if (node_->left) {
                node_ = node_->left;
                return;
            }

            while (!(node_->parent->right == node_ && node_->parent->left)) {
                node_ = node_->parent;
            }
            node_ = node_->parent->left;
        }

        bool IsFakeNode(const BaseNode* node) const {
            return node->parent == node || (node->parent->left != node && node->parent->right != node);
        }

    };

public:
    using key_type = Key;
    using value_type  = Key;
    using reference = Key&;
    using const_reference = const Key&;
    using key_compare = Compare;
    using value_compare = Compare;
    using node_type = Node; // Or Node*

    template<typename traversal_type>
    using iterator = Iterator<traversal_type>;

    template<typename traversal_type>
    using const_iterator = Iterator<traversal_type>;

    template<typename traversal_type>
    using reverse_iterator = std::reverse_iterator<iterator<traversal_type>>;

    template<typename traversal_type>
    using const_reverse_iterator = std::reverse_iterator<const_iterator<traversal_type>>;

    template<typename traversal_type>
    using difference_type = std::iterator_traits<iterator<traversal_type>>::difference_type;

    using size_type = size_t;

    using AllocTraits = std::allocator_traits<typename std::allocator_traits<Allocator>::template rebind_alloc<Node>>;
    std::allocator_traits<Allocator>::template rebind_alloc<Node> alloc_;


    BinarySearchTree(key_compare comparator = Compare()): comparator_(comparator) {
        SetDefaultFakeNodePointers();
    }

    template<typename Iter>
    BinarySearchTree(Iter iterator_start, Iter iterator_finish, key_compare comparator = Compare()): comparator_(comparator) {
        SetDefaultFakeNodePointers();
        while (iterator_start != iterator_finish) {
            insert(*iterator_start);
            ++iterator_start;
        }
    }

    BinarySearchTree(std::initializer_list<value_type> initializer_list, Compare comparator = Compare()): comparator_(comparator) {
        SetDefaultFakeNodePointers();
        for (auto element: initializer_list) {
            insert(element);
        }
    }

    BinarySearchTree(const BinarySearchTree& other): alloc_(other.alloc_), comparator_(other.comparator_) {
        SetDefaultFakeNodePointers();
        for (auto element: other) {
            insert(element);
        }
    }

    ~BinarySearchTree() {
        clear();
    }


    BinarySearchTree& operator=(const BinarySearchTree& other)  {
        if (*this == other) {
            return *this;
        }
        clear();
        SetDefaultFakeNodePointers();
        for (auto element: other) {
            insert(element);
        }
        return *this;
    }

    BinarySearchTree& operator=(const std::initializer_list<value_type>& initializer_list) {
        clear();
        SetDefaultFakeNodePointers();
        for (auto element: initializer_list) {
            insert(element);
        }
        return *this;
    }

    template<typename traversal_type = InOrder>
    iterator<traversal_type> begin() const {
        return cbegin<traversal_type>();
    }

    template<typename traversal_type = InOrder>
    iterator<traversal_type> end() const {
        return cend<traversal_type>();
    }

    template<typename traversal_type = InOrder>
    const_iterator<traversal_type> cbegin() const {
        return cbegin(traversal_type{});
    }


    template<typename traversal_type = InOrder>
    const_iterator<traversal_type> cend() const {
        return iterator<traversal_type>(const_cast<BaseNode*>(&fake_node_));
    }


    template<typename traversal_type = InOrder>
    reverse_iterator<traversal_type> rbegin() const {
        return rcbegin<traversal_type>();
    }


    template<typename traversal_type = InOrder>
    reverse_iterator<traversal_type> rend() const {
        return rcend<traversal_type>();
    }


    template<typename traversal_type = InOrder>
    const_reverse_iterator<traversal_type> rcbegin() const {
        return reverse_iterator<traversal_type>(end<traversal_type>());
    }


    template<typename traversal_type = InOrder>
    const_reverse_iterator<traversal_type> rcend() const {
        return reverse_iterator<traversal_type>(begin<traversal_type>());
    }


    size_type size() const {
        return size_;
    }

    size_type max_size() const{
        return std::numeric_limits<size_type>::max();
    }

    bool empty() const {
        return size_ == 0;
    }


    Compare key_comp() const {
        return comparator_;
    }


    Compare value_comp() const {
        return comparator_;
    }

    template<typename traversal_type = InOrder>
    std::pair<iterator<traversal_type>, bool> insert(const Key& key) {
        Node* new_node = alloc_.allocate(1);
        AllocTraits::construct(alloc_, new_node, key);

        if (fake_node_.left == &fake_node_) {
            ++size_;
            fake_node_.left = new_node;
            fake_node_.right = new_node;
            fake_node_.parent = new_node;
            fake_node_.left->parent = &fake_node_;
            return std::make_pair(iterator<traversal_type>(new_node), true);
        }

        if (comparator_(key, *begin())) {
            ++size_;
            Node* smallest_node = static_cast<Node*>(begin().node_);
            smallest_node->left = new_node;
            new_node->parent = smallest_node;
            fake_node_.right = new_node;
            fake_node_.parent = new_node;
            return std::make_pair(iterator<traversal_type>(new_node), true);
        }

        Node* current = static_cast<Node*>(fake_node_.left);
        while (true) {
            if (comparator_(key, current->key)) {
                if (current->left) {
                    current = static_cast<Node*>(current->left);
                } else {
                    current->left = new_node;
                    break;
                }
            } else if (comparator_(current->key, key)) {
                if (current->right) {
                    current = static_cast<Node*>(current->right);
                } else {
                    current->right = new_node;
                    break;
                }
            } else {
                return std::make_pair(iterator<traversal_type>(current), false);
            }
        }

        new_node->parent = current;
        if (fake_node_.right == fake_node_.left || comparator_(new_node->key, static_cast<Node*>(fake_node_.right->parent)->key)) {
            // новая нода лежит в правом поддереве бегина и может стать новой X
            // новая нода ребенок постбегина либо она меньше чем постбегин
            if (comparator_(new_node->key, static_cast<Node*>(fake_node_.parent)->key) || new_node->parent == fake_node_.parent) {
                fake_node_.parent = new_node;
            }
        }

        ++size_;
        return std::make_pair( iterator<traversal_type>(new_node),true);
    }

    template<typename Iter>
    void insert(Iter start_iterator, Iter finish_iterator) {
        while (start_iterator != finish_iterator) {
            insert(*start_iterator);
            ++start_iterator;
        }
    }


    void insert(std::initializer_list<value_type> initializer_list) {
        insert(initializer_list.begin(), initializer_list.end());
    }

    size_type erase(const Key& key) {
        Iterator target = find(key);
        if (target == end()) {
            return 0;
        }

        Delete(static_cast<Node*>(target.node_));
        if (target.node_ == fake_node_.parent) {
            SetPostOrderBegin();
        }

        return 1;
    }

    template<typename traversal_type = InOrder>
    iterator<traversal_type> erase(const_iterator<traversal_type> iterator) {
        auto next_iterator = upper_bound<traversal_type>(static_cast<Node*>(iterator.node_)->key);
        Delete(static_cast<Node*>(iterator.node_));
        if (iterator.node_ == fake_node_.parent) {
            SetPostOrderBegin();
        }

        return next_iterator;
    }

    template<typename traversal_type = InOrder>
    iterator<traversal_type> erase(iterator<traversal_type> iterator_start, iterator<traversal_type> iterator_finish)  {
        while (iterator_start != iterator_finish) {
            iterator_start = erase(iterator_start);
        }
        return iterator_finish;
    }


    void clear() {
        erase(begin(), end());
    }

    template<typename traversal_type = InOrder>
    iterator<traversal_type> find(const Key& key) const {
        if (!size_) {
            return end<traversal_type>();
        }

        Node* current = static_cast<Node*>(fake_node_.left);
        while (true) {
            if (comparator_(key, current->key)) {
                if (current->left) {
                    current = static_cast<Node*>(current->left);
                } else {
                    return end<traversal_type>();
                }
            } else if (comparator_(current->key, key)) {
                if (current->right) {
                    current = static_cast<Node*>(current->right);
                } else {
                    return end<traversal_type>();
                }
            } else {
                return iterator<traversal_type>(current);
            }
        }

    }

    size_type count(const Key& key) const {
        return find(key) == end() ? 0: 1;
    }


    bool contains(const Key& key) const {
        return find(key) != end();
    }


    template<typename traversal_type = InOrder>
    iterator<traversal_type> lower_bound(const Key& key) const {
        if (!size_) {
            return end<traversal_type>();
        }

        Node* current = static_cast<Node*>(fake_node_.left);
        Node* best = nullptr;

        while (current != nullptr) {
            if (comparator_(current->key, key)) {
                current = static_cast<Node*>(current->right);
            } else {
                if (best == nullptr || comparator_(current->key, best->key)) {
                    best = current;
                }
                current = static_cast<Node*>(current->left);
            }
        }

        return best ? iterator<traversal_type>(best): end<traversal_type>();
    }

    template<typename traversal_type = InOrder>
    iterator<traversal_type> upper_bound(const Key& key) const {
        if (!size_) {
            return end<traversal_type>();
        }

        Node* current = static_cast<Node*>(fake_node_.left);
        Node* best = nullptr;

        while (current != nullptr) {
            if (comparator_(current->key, key) || !comparator_(key, current->key)) {
                current = static_cast<Node*>(current->right);
            } else {
                if (best == nullptr || comparator_(current->key, best->key)) {
                    best = current;
                }
                current = static_cast<Node*>(current->left);
            }
        }

        return best ? iterator<traversal_type>(best): end<traversal_type>();
    }

    template<typename traversal_type = InOrder>
    std::pair<iterator<traversal_type>, iterator<traversal_type>> equal_range(const Key& key) const {
        return std::make_pair(lower_bound<traversal_type>(key), upper_bound<traversal_type>(key));
    }

    void swap(BinarySearchTree& other) {
        if (other.size_) {
            other.fake_node_.left->parent = &fake_node_;
        }
        if (size_) {
            fake_node_.left->parent = &(other.fake_node_);
        }

        std::swap(fake_node_, other.fake_node_);
        std::swap(size_, other.size_);
        std::swap(comparator_, other.comparator_);
        std::swap(alloc_, other.alloc_);

        if (!size_) {
            fake_node_.left = &fake_node_;
            fake_node_.right = fake_node_.left;
            fake_node_.parent = fake_node_.left;
        }
        if (!other.size_) {
            other.fake_node_.left = &other.fake_node_;
            other.fake_node_.right = other.fake_node_.left;
            other.fake_node_.parent = other.fake_node_.left;
        }
    }

private:
    void Delete(Node* node) {
        --size_;

        if (node == fake_node_.right) {
            fake_node_.right = upper_bound(node->key).node_;
        }

        if (!node->left && !node->right) {
            if ((!node->parent) || (node->parent == &fake_node_)) {
                AllocTraits::destroy(alloc_, fake_node_.left);
                alloc_.deallocate(static_cast<Node*>(fake_node_.left), 1);

                fake_node_.left = &fake_node_;
                fake_node_.right = &fake_node_;
                return;
            }
            if (node->parent->left == node) {
                node->parent->left = nullptr;
            } else {
                node->parent->right = nullptr;
            }

            AllocTraits::destroy(alloc_, node);
            alloc_.deallocate(node, 1);

            return;
        }

        if (!node->left) {
            if (node == fake_node_.left) {
                fake_node_.left = node->right;
                node->right->parent = &fake_node_;
                AllocTraits::destroy(alloc_, node);
                alloc_.deallocate(node, 1);
                return;
            }

            node->right->parent = node->parent;
            if (node->parent->left == node) {
                node->parent->left = node->right;
            } else {
                node->parent->right = node->right;
            }
            AllocTraits::destroy(alloc_, node);
            alloc_.deallocate(node, 1);
            return;
        }

        if (!node->right) {
            if (node == fake_node_.parent) {
                fake_node_.left = node->left;
                node->left->parent = &fake_node_;
                AllocTraits::destroy(alloc_, node);
                alloc_.deallocate(node, 1);
                return;
            }

            node->left->parent = node->parent;
            if (node->parent->left == node) {
                node->parent->left = node->left;
            } else {
                node->parent->right = node->left;
            }
            AllocTraits::destroy(alloc_, node);
            alloc_.deallocate(node, 1);
            return;
        }

        Node* node_to_swap_with = static_cast<Node*>(upper_bound(node->key).node_);

        if (node_to_swap_with->parent->left == node_to_swap_with) {
            node_to_swap_with->parent->left = node_to_swap_with->right;
        } else {
            node_to_swap_with->parent->right = node_to_swap_with->right;
        }
        if (node_to_swap_with->right) {
            node_to_swap_with->right->parent = node_to_swap_with->parent;
        }

        node_to_swap_with->parent = node->parent;
        if (node->right != node_to_swap_with) {
            node_to_swap_with->right = node->right;
        } else {
            node_to_swap_with->right = nullptr;
        }
        node_to_swap_with->left = node->left;

        if (node_to_swap_with->right) {
            node_to_swap_with->right->parent = node_to_swap_with;
        }
        node_to_swap_with->left->parent = node_to_swap_with;
        if (node->parent->left == node) {
            node->parent->left = node_to_swap_with;
        } else {
            node->parent->right = node_to_swap_with;
        }

        AllocTraits::destroy(alloc_, node);
        alloc_.deallocate(node, 1);
    }

    void SetPostOrderBegin() {
        if (empty()) {
            SetDefaultFakeNodePointers();
            return;
        }
        BaseNode* current = fake_node_.right;
        while (current->left || current->right) {
            if (current->left) {
                current = current->left;
            } else {
                current = current->right;
            }
        }
        fake_node_.parent = current;
    }

    void SetDefaultFakeNodePointers() {
        fake_node_.left = &fake_node_;
        fake_node_.right = fake_node_.left;
        fake_node_.parent = fake_node_.left;
    }

    const_iterator<InOrder> cbegin(InOrder) const {
        return iterator<InOrder>(fake_node_.right);
    }


    const_iterator<PreOrder> cbegin(PreOrder) const {
        return iterator<PreOrder>(fake_node_.left);
    }

    const_iterator<PostOrder> cbegin(PostOrder) const {
        return iterator<PostOrder>(fake_node_.parent);
    }


    BaseNode fake_node_;
    size_type size_ = 0;

    Compare comparator_;
};



template<typename Key, typename Compare, typename Allocator>
void swap(BinarySearchTree<Key, Compare, Allocator>& first, BinarySearchTree<Key, Compare, Allocator>& second) {
    first.swap(second);
}

template<typename  Key, typename Compare, typename Allocator>
bool operator==(const BinarySearchTree<Key, Compare, Allocator>& first, const BinarySearchTree<Key, Compare, Allocator>& second) {
    if (first.size() != second.size()) {
        return false;
    }
    auto iterator_first = first.begin();
    auto iterator_second = second.begin();

    while (iterator_first != first.end()) {
        if (*iterator_first != *iterator_second) {
            std::cout << *iterator_first << ' ' << *iterator_second << '\n';
            return false;
        }
        ++iterator_first;
        ++iterator_second;
    }

    return true;
}

template<typename  Key, typename Compare, typename Allocator>
bool operator!=(const BinarySearchTree<Key, Compare, Allocator>& first, const BinarySearchTree<Key, Compare, Allocator>& second) {
    return !(first == second);
}






