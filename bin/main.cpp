#include <lib/bst.cpp>
#include <iostream>
#include <set>

void FillTree(BinarySearchTree<int>& tree, int i_max = 1000) {
    for (int i = 0; i < i_max; ++i) {
        tree.insert(i);
    }
}

void FillTreeSmartly(BinarySearchTree<int>& tree, int i_max = 1000, int left = 0, int right = 1000) {
    int mid = (left + right) / 2;
    tree.insert(mid);
    if (left >= right - i_max / 20) {
        return;
    }
    FillTreeSmartly(tree, i_max, mid, right);
    FillTreeSmartly(tree, i_max, left, mid);
}

void FillSet(std::set<int>& set, int i_max = 1000) {
    for (int i = 0; i < i_max; ++i) {
        set.insert(i);
    }
}

void FillSetSmartly(std::set<int>& set, int i_max = 1000, int left = 0, int right = 1000) {
    int mid = (left + right) / 2;
    set.insert(mid);
    if (left >= right - i_max / 20) {
        return;
    }
    FillSetSmartly(set, i_max, mid, right);
    FillSetSmartly(set, i_max, left, mid);
}


template<typename Key>
bool EqualToSet(BinarySearchTree<Key>& tree, std::set<Key> set) {
    if (tree.size() != set.size()) {
        return false;
    }

    auto it_tree = tree.begin();
    auto it_set = set.begin();

    while (it_tree != tree.end()) {
        std::cout << *it_tree << ' ' << *it_set << '\n';
        if (*it_tree != *it_set) {
            return false;
        }
        ++it_tree;
        ++it_set;
    }

    return true;
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


int main() {
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

    std::cout << set2.size();

    return 0;
}

