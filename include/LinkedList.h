#ifndef LINKEDLIST_H
#define LINKEDLIST_H

template<typename T>
struct Node {
    T data;
    Node* next;
    
    Node(const T& value) : data(value), next(nullptr) {}
};

template<typename T>
class LinkedList {
private:
    Node<T>* head;
    Node<T>* tail;
    int size;
    
    // Helper function to swap data of two nodes
    void swap(Node<T>* a, Node<T>* b) {
        T temp = a->data;
        a->data = b->data;
        b->data = temp;
    }

    // Helper function to partition the list and return pivot node
    Node<T>* partition(Node<T>* low, Node<T>* high) {
        if (low == high || low == nullptr || high == nullptr)
            return low;

        T pivot = high->data;
        Node<T>* i = low->next;
        Node<T>* j = low;

        while (i != high->next) {
            if (i->data < pivot) {
                j = j->next;
                swap(i, j);
            }
            i = i->next;
        }
        swap(j, high);
        return j;
    }

    // Helper function for quickSort
    void quickSortRecursive(Node<T>* low, Node<T>* high) {
        if (low != nullptr && high != nullptr && low != high && low != high->next) {
            Node<T>* pivot = partition(low, high);
            Node<T>* temp = head;
            
            while (temp != nullptr && temp->next != pivot) {
                temp = temp->next;
            }
            
            if (temp != nullptr) {
                quickSortRecursive(low, temp);
            }
            
            quickSortRecursive(pivot->next, high);
        }
    }

public:
    LinkedList() : head(nullptr), tail(nullptr), size(0) {}
    
    ~LinkedList() {
        Node<T>* current = head;
        while (current != nullptr) {
            Node<T>* next = current->next;
            delete current;
            current = next;
        }
    }
    
    void insertAtEnd(const T& value) {
        Node<T>* newNode = new Node<T>(value);
        if (tail == nullptr) {
            head = tail = newNode;
        } else {
            tail->next = newNode;
            tail = newNode;
        }
        size++;
    }
    
    Node<T>* getHead() const {
        return head;
    }
    
    int getSize() const {
        return size;
    }

    // Public quickSort function
    void quickSort() {
        if (head == nullptr || head->next == nullptr)
            return;

        Node<T>* temp = head;
        while (temp->next != nullptr)
            temp = temp->next;

        quickSortRecursive(head, temp);
    }
};

#endif // LINKEDLIST_H