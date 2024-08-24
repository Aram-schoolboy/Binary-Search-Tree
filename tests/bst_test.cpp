#include <lib/bst.cpp>
#include <gtest/gtest.h>
#include <set>
#include <vector>

void FillTree(BinarySearchTree<int>& tree, int i_max = 1000) {
    for (int i = 0; i < i_max; ++i) {
        tree.insert(i);
    }
}

void FillSet(std::set<int>& set, int i_max = 1000) {
    for (int i = 0; i < i_max; ++i) {
        set.insert(i);
    }
}

template<typename T>
void FillSmartly(T& cont, int i_max = 1000, int left = 0, int right = 1000) {
    int mid = (left + right) / 2;
    cont.insert(mid);
    if (left >= right - i_max / 20) {
        return;
    }
    FillSmartly(cont, i_max, mid, right);
    FillSmartly(cont, i_max, left, mid);
}

template<typename Key, typename Comp = std::less<Key>>
bool EqualToSet(const BinarySearchTree<Key, Comp>& tree, const std::set<Key, Comp>& set) {
    if (tree.size() != set.size()) {
        return false;
    }

    auto it_tree = tree.begin();
    auto it_set = set.begin();

    while (it_tree != tree.end()) {
        if (*it_tree != *it_set) {
            return false;
        }
        ++it_tree;
        ++it_set;
    }

    return true;
}


TEST(bstTestSuite, EmptyTest) {
    BinarySearchTree<int> a;
    ASSERT_TRUE(a.empty());
    a.insert(123);
    ASSERT_FALSE(a.empty());
    a.erase(123);
    ASSERT_TRUE(a.empty());
}

TEST(bstTestSuite, EqualTest) {
    BinarySearchTree<int> a;
    BinarySearchTree<int> b;

    FillTree(a);
    FillTree(b);

    ASSERT_TRUE(a == b);
    ASSERT_TRUE(b == b);
    a.insert(-1);
    ASSERT_TRUE(a != b);
    ASSERT_TRUE(b != a);

    a.clear();
    b.clear();
}

TEST(bstTestSuite, ConstructorsTest) {
    BinarySearchTree<int> a;
    FillTree(a);
    BinarySearchTree<int> b(a);
    BinarySearchTree<int> c = a;
    c = b;

    ASSERT_TRUE(a == b);
    ASSERT_TRUE(b == a);
    ASSERT_TRUE(b == c);
    ASSERT_TRUE(c == c);
    ASSERT_TRUE(c == a);
    ASSERT_TRUE(a == c);

    BinarySearchTree d {1, 2, 3, 4, -1, -2, -3};
    BinarySearchTree d_copy = d;

    ASSERT_TRUE(d == d_copy);
}

TEST(bstTestSuite, InsertEraseSizeTest) {
    BinarySearchTree<int> tree;
    std::set<int> set;

    FillTree(tree);
    FillSet(set);

    for (int i = 5; i < 567; i += 2) {
        tree.erase(i);
        set.erase(i);
        tree.erase(i);
        set.erase(i);

        if (i % 74 == 0) {
            tree.insert(i);
            set.insert(i);
            tree.insert(i);
            set.insert(i);
        }
    }

    ASSERT_TRUE(EqualToSet(tree, set));
    ASSERT_TRUE(tree.size() == set.size());

    tree.clear();
    set.clear();
    FillSmartly(set);
    FillSmartly(tree);

    for (int i = 5; i < 567; i += 2) {
        tree.erase(i);
        set.erase(i);
        tree.erase(i);
        set.erase(i);

        if (i % 74 == 0) {
            tree.insert(i);
            set.insert(i);
            tree.insert(i);
            set.insert(i);
        }
    }

    ASSERT_TRUE(EqualToSet(tree, set));
    ASSERT_TRUE(tree.size() == set.size());
}

TEST(bstTestSuite, InsertResultIteratorTest) {
    BinarySearchTree<int> tree;
    std::set<int> set;

    FillTree(tree);
    FillSet(set);

    auto tree_it = tree.insert(123).first;
    auto set_it = set.insert(123).first;

    ASSERT_TRUE(*tree_it == *set_it);
    ASSERT_TRUE(*++tree_it == *++set_it);
    ASSERT_TRUE(*--tree_it == *--set_it);
    ASSERT_TRUE(*tree_it++ == *set_it++);
    ASSERT_TRUE(*tree_it-- == *set_it--);
}

TEST(bstTestSuite, InsertByIteratorsTest) {
    std::vector<int> a {1, 3, -2, -5, 3, 5, 7, 3, 9, -10, -24, -5};
    BinarySearchTree<int> tree;
    std::set<int> set;

    tree.insert(a.begin(), a.end());
    set.insert(a.begin(), a.end());

    ASSERT_TRUE(EqualToSet(tree, set));
}

TEST(bstTestSuite, InsertByInitializerListTest) {
    std::initializer_list<int> a {1, 3, -2, -5, 3, 5, 7, 3, 9, -10, -24, -5};
    BinarySearchTree<int> tree;
    std::set<int> set;

    tree.insert(a);
    set.insert(a);

    ASSERT_TRUE(EqualToSet(tree, set));
}

template<typename iterator_type>
struct comparator_of_iterators {

    bool operator()(const iterator_type& it1, const iterator_type& it2) const {
        return &it1 < &it2;
    }
};


TEST(bstTestSuite, EraseByIterator) {
    BinarySearchTree<int> tree;
    std::set<int> set;

    FillSmartly(set);
    FillSmartly(tree);

    std::set<BinarySearchTree<int>::iterator<InOrder>, comparator_of_iterators<BinarySearchTree<int>::iterator<InOrder>>> tree_iterators_in_set;
    std::set<std::set<int>::iterator, comparator_of_iterators<std::set<int>::iterator>> set_iterators_in_set;

    BinarySearchTree<BinarySearchTree<int>::iterator<InOrder>, comparator_of_iterators<BinarySearchTree<int>::iterator<InOrder>>> tree_iterators_in_bst;
    BinarySearchTree<std::set<int>::iterator, comparator_of_iterators<std::set<int>::iterator>> set_iterators_in_bst;


    for (int i = 5; i < 1000; i += 2) {
        tree.erase(i);
        set.erase(i);
        tree.erase(i);
        set.erase(i);

        if (i % 74 == 0) {
            tree_iterators_in_set.insert(tree.insert(i).first);
            tree_iterators_in_bst.insert(tree.insert(i).first);

            set_iterators_in_set.insert(set.insert(i).first);
            set_iterators_in_bst.insert(set.insert(i).first);
        }
    }

    ASSERT_TRUE(EqualToSet(tree_iterators_in_bst, tree_iterators_in_set));
    ASSERT_TRUE(EqualToSet(set_iterators_in_bst, set_iterators_in_set));

    for (auto it: tree_iterators_in_set) {
        tree.erase(it);
    }

    for (auto it: set_iterators_in_set) {
        set.erase(it);
    }

    ASSERT_TRUE(EqualToSet(tree, set));
}

TEST(bstTestSuite, EraseByIteratorsRange) {
    BinarySearchTree<int> tree;
    std::set<int> set;

    FillSmartly(set);
    FillSmartly(tree);

    std::set<BinarySearchTree<int>::iterator<InOrder>, comparator_of_iterators<BinarySearchTree<int>::iterator<InOrder>>> tree_iterators_in_set;
    std::set<std::set<int>::iterator, comparator_of_iterators<std::set<int>::iterator>> set_iterators_in_set;

    BinarySearchTree<BinarySearchTree<int>::iterator<InOrder>, comparator_of_iterators<BinarySearchTree<int>::iterator<InOrder>>> tree_iterators_in_bst;
    BinarySearchTree<std::set<int>::iterator, comparator_of_iterators<std::set<int>::iterator>> set_iterators_in_bst;


    for (int i = 5; i < 2000; i += 2) {
        tree.erase(i);
        set.erase(i);
        tree.erase(i);
        set.erase(i);

        if (i % 74 == 0) {
            tree_iterators_in_set.insert(tree.insert(i).first);
            tree_iterators_in_bst.insert(tree.insert(i).first);

            set_iterators_in_set.insert(set.insert(i).first);
            set_iterators_in_bst.insert(set.insert(i).first);
        }
    }

    tree.erase(*tree_iterators_in_bst.begin(), *tree_iterators_in_bst.end());
    set.erase(*set_iterators_in_set.begin(), *set_iterators_in_set.end());

    ASSERT_TRUE(EqualToSet(tree, set));
}

TEST(bstTestSuite, FindTest) {
    BinarySearchTree<int> tree;
    std::set<int> set;

    FillSmartly(set);
    FillSmartly(tree);

    for (int i = 5; i < 567; i += 2) {
        tree.erase(i);
        set.erase(i);
        tree.erase(i);
        set.erase(i);

        if (i % 74 == 0) {
            tree.insert(i);
            set.insert(i);
            tree.insert(i);
            set.insert(i);
        }
    }
    bool flag = true;
    for (int i = 0; i < 1000; ++i) {
        if (tree.find(i) == tree.end()) {
            if (set.find(i) != set.end()) {
                flag = false;
                break;
            }
        } else {
            if (*tree.find(i) != *set.find(i)) {
                flag = false;
            }
        }
    }

    ASSERT_TRUE(flag);
}

TEST(bstTestSuite, LowerBoundTest) {
    BinarySearchTree<int> tree;
    std::set<int> set;

    FillSmartly(set);
    FillSmartly(tree);

    for (int i = 5; i < 567; i += 2) {
        tree.erase(i);
        set.erase(i);
        tree.erase(i);
        set.erase(i);

        if (i % 74 == 0) {
            tree.insert(i);
            set.insert(i);
            tree.insert(i);
            set.insert(i);
        }
    }
    bool flag = true;
    for (int i = 0; i < 1000; ++i) {
        if (tree.lower_bound(i) == tree.end()) {
            if (set.lower_bound(i) != set.end()) {
                flag = false;
                break;
            }
        } else {
            if (*tree.lower_bound(i) != *set.lower_bound(i)) {
                flag = false;
            }
        }
    }

    ASSERT_TRUE(flag);
}

TEST(bstTestSuite, UpperBoundTest) {
    BinarySearchTree<int> tree;
    std::set<int> set;

    FillSmartly(set);
    FillSmartly(tree);

    for (int i = 5; i < 567; i += 2) {
        tree.erase(i);
        set.erase(i);
        tree.erase(i);
        set.erase(i);

        if (i % 74 == 0) {
            tree.insert(i);
            set.insert(i);
            tree.insert(i);
            set.insert(i);
        }
    }
    bool flag = true;
    for (int i = 0; i < 1000; ++i) {
        if (tree.upper_bound(i) == tree.end()) {
            if (set.upper_bound(i) != set.end()) {
                flag = false;
                break;
            }
        } else {
            if (*tree.upper_bound(i) != *set.upper_bound(i)) {
                flag = false;
            }
        }
    }

    ASSERT_TRUE(flag);
}

TEST(bstTestSuite, SwapTest) {
    BinarySearchTree<int> tree;
    BinarySearchTree<int> tree2;

    std::set<int> set;
    std::set<int> set2;

    FillSmartly(set);
    FillSmartly(set2);
    FillSmartly(tree);
    FillSmartly(tree2);

    for (int i = 5; i < 567; i += 2) {
        tree.erase(i);
        set.erase(i);
        tree.erase(i);
        set.erase(i);

        if (i % 74 == 0) {
            tree.insert(i);
            set.insert(i);
            tree.insert(i);
            set.insert(i);
            tree2.erase(i);
            set2.erase(i);
        }
    }

    set.swap(set2);
    tree.swap(tree2);


    ASSERT_TRUE(EqualToSet(tree, set));
    ASSERT_TRUE(EqualToSet(tree2, set2));
}

TEST(bstTestSuite, EmptySwapTest) {
    BinarySearchTree<int> tree;
    BinarySearchTree<int> tree2;

    std::set<int> set;
    std::set<int> set2;

    tree.swap(tree2);
    set.swap(set2);

    FillSmartly(set);
    FillSet(set2);

    FillSmartly(tree);
    FillTree(tree2);

    ASSERT_TRUE(EqualToSet(tree, set));
    ASSERT_TRUE(EqualToSet(tree2, set2));
}

TEST(bstTestSuite, PostOrderTest) {
    BinarySearchTree<float> a {1, 0, -1, -2, 0.5, 0.7, 0.6, 5, 3, 4, 6, 10, 8, 7, 9, 15};
    float right_order[] {-2, -1, 0.6, 0.7, 0.5, 0, 4, 3, 7, 9, 8, 15, 10, 6, 5, 1};

    auto it = a.begin<PostOrder>();
    int index = 0;
    bool flag = true;
    while (it != a.end<PostOrder>()) {
        if (*it != right_order[index]) {
            flag = false;
            break;
        }
        ++it;
        --it;
        it++;
        it--;
        ++it;
        ++index;
    }

    ASSERT_TRUE(flag);
}

TEST(bstTestSuite, PostOrderReverseTest) {
    BinarySearchTree<float> a {1, 0, -1, -2, 0.5, 0.7, 0.6, 5, 3, 4, 6, 10, 8, 7, 9, 15};
    float right_order[] {-2, -1, 0.6, 0.7, 0.5, 0, 4, 3, 7, 9, 8, 15, 10, 6, 5, 1};

    auto it = a.rbegin<PostOrder>();
    int index = 0;
    bool flag = true;
    while (it != a.rend<PostOrder>()) {
        if (*it != right_order[a.size() - 1 - index]) {
            flag = false;
            break;
        }
        ++it;
        --it;
        it++;
        it--;
        ++it;
        ++index;
    }

    ASSERT_TRUE(flag);
}

TEST(bstTestSuite, PreOrderTest) {
    BinarySearchTree<float> a {1, 0, -1, -2, 0.5, 0.7, 0.6, 5, 3, 4, 6, 10, 8, 7, 9, 15};
    float right_order[] {1, 0, -1, -2, 0.5, 0.7, 0.6, 5, 3, 4, 6, 10, 8, 7, 9, 15};

    auto it = a.begin<PreOrder>();
    int index = 0;
    bool flag = true;
    while (it != a.end<PreOrder>()) {
        if (*it != right_order[index]) {
            flag = false;
            break;
        }
        ++it;
        --it;
        it++;
        it--;
        ++it;
        ++index;
    }

    ASSERT_TRUE(flag);
}

TEST(bstTestSuite, PreOrderReverseTest) {
    BinarySearchTree<float> a {1, 0, -1, -2, 0.5, 0.7, 0.6, 5, 3, 4, 6, 10, 8, 7, 9, 15};
    float right_order[] {1, 0, -1, -2, 0.5, 0.7, 0.6, 5, 3, 4, 6, 10, 8, 7, 9, 15};

    auto it = a.rbegin<PreOrder>();
    int index = 0;
    bool flag = true;
    while (it != a.rend<PreOrder>()) {
        if (*it != right_order[a.size() - 1 - index]) {
            flag = false;
            break;
        }
        ++it;
        --it;
        it++;
        it--;
        ++it;
        ++index;
    }

    ASSERT_TRUE(flag);
}

