#ifndef BALANCED_TREE_H
#define BALANCED_TREE_H
#include <bits/stdc++.h>
class BalancedTree {
        class BalancedTreeNode {
            public:
                int id;
                BalancedTreeNode* left;
                BalancedTreeNode* right;
                int height;
                bool available;
                BalancedTreeNode (int id) {
                    this->id = id;
                    available = true;
                    left = NULL;
                    right = NULL;
                }
                void CheckAvailability (int id) {
                    if (this->id == id){
                        available = false;
                    }
                    else {
                        if (left != NULL) {
                            left->CheckAvailability(id);
                        }
                        if (right != NULL) {
                            right->CheckAvailability(id);
                        }
                    }
                }
                void Remove (int id, std::set<int> &ids) {
                    if (left != NULL && left->id == id) {
                        left->RecursionRemove(ids);
                        ids.erase(left->id);
                        delete left;
                        left = NULL;
                    }
                    else if (right != NULL && right->id == id) {
                        right->RecursionRemove(ids);
                        ids.erase(right->id);
                        delete right;
                        right = NULL;
                    }
                    else {
                        if (left != NULL) {
                            left->Remove(id, ids);
                        }
                        if (right != NULL) {
                            right->Remove(id, ids);
                        }
                    }
                }
                void RecursionRemove (std::set<int> &ids) {
                    if (left != NULL) {
                        left->RecursionRemove(ids);
                        ids.erase(left->id);
                        delete left;
                        left = NULL;
                    }
                    if (right != NULL) {
                        right->RecursionRemove(ids);
                        ids.erase(right->id);
                        delete right;
                        right = NULL;
                    }
                }
                void AddInNode (int id, int parent_id, std::set<int> &ids) {
                    if (this->id == parent_id) {
                        if (left == NULL){
                            left = new BalancedTreeNode(id);
                        }
                        else {
                            right = new BalancedTreeNode(id);
                        }
                        ids.insert(id);
                    }
                    else {
                        if (left != NULL) {
                            left->AddInNode(id, parent_id, ids);
                        }
                        if (right != nullptr) {
                            right->AddInNode(id, parent_id, ids);
                        }
                    }
                }
                int MinimalHeight() {
                    if (left == NULL || right == NULL) {
                        return 0;
                    }
                    int left_height = -1;
                    int right_height = -1;
                    if (left != NULL && left->available == true) {
                        left_height = left->MinimalHeight();
                    }
                    if (right != NULL && right->available == true) {
                        right_height = right->MinimalHeight();
                    }
                    if (right_height == -1 && left_height == -1) {
                        available = false;
                        return -1;
                    }
                    else if (right_height  == -1) {
                        return left_height + 1;
                    }
                    else if (left_height == -1) {
                        return right_height + 1;
                    }
                    else {
                        return std::min(left_height, right_height) + 1;
                    }
                }
                int IDMinimalHeight(int height, int current_height) {
                    if (height < current_height) {
                        return -2;
                    }
                    else if (height > current_height) {
                        int current_id = -2;
                        if (left != NULL && left->available == true) {
                            current_id = left->IDMinimalHeight(height, (current_height + 1));
                        }
                        if (right != NULL && right->available == true && current_id == -2){
                            current_id = right->IDMinimalHeight(height, (current_height + 1));
                        }
                        return current_id;
                    }
                    else {
                        if (left == NULL || right == NULL){
                            return id;
                        }
                        return -2;
                    }
                }
                ~BalancedTreeNode() {}
        };
    private:
            BalancedTreeNode* root;
    public:
        std::set<int> ids;
        BalancedTree() {
            root = new BalancedTreeNode(-1);
        } 
        bool Exist(int id) {
            if (ids.find(id) != ids.end()) {
                return true;
            }
            return false;
        }
        void AvailabilityCheck(int id) {
            root->CheckAvailability(id);
        }
        int FindID() {
            int h = root->MinimalHeight();
            return root->IDMinimalHeight(h, 0);
        }
        void AddInTree(int id, int parent) {
            root->AddInNode(id, parent, ids);
        }
        void RemoveFromRoot(int idElem) {
            root->Remove(idElem, ids);
        }
        ~BalancedTree() {
            root->RecursionRemove(ids);
            delete root;
        }
};
#endif