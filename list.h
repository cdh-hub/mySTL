#ifndef __MY_LIST__
#define __MY_LIST__

#include<iostream>

//链表节点
template<class T>
struct list_node {
    list_node<T>* prev;
    list_node<T>* next;
    T data;
};


//迭代器
template<class T>
class list_iterator {
public:
    typedef list_iterator<T> iterator;
    typedef list_node<T>* link_type;
    typedef T& reference;
    typedef T* pointer;
    
    link_type node;

    list_iterator() {}
    list_iterator(link_type p) : node(p) {}
    list_iterator(const iterator& it) : node(it.node) {}
    bool operator==(const iterator& it) const {
        return node == it.node;
    }
    bool operator!=(const iterator& it) const {
        return node != it.node;
    }
    reference operator*() const {
        return node->data;
    }
    pointer operator->() const {
        return &node->data;
    }
    iterator& operator++() {    //前++
        node = node->next;
        return *this;
    }
    iterator operator++(int) { //后++
        iterator tmp = *this;
        ++*this;
        return tmp;
    }
    iterator& operator--() {
        node = node->prev;
        return *this;
    }
    iterator operator--(int) {
        iterator tmp = *this;
        --*this;
        return tmp;
    }
};

#include<memory>
#include<cstddef>

template<class T, class Alloc = std::allocator<list_node<T>>>
class list {
public:
    typedef list_node<T>* link_type;
    typedef list_iterator<T> iterator;
    typedef Alloc list_node_allocator;
    // typedef size_t size_type;
    typedef T& reference;
    typedef T* pointer;
protected:
    link_type sentinel_node;    //哨兵节点
    // size_type node_num;
    list_node_allocator a;
public:
    list() {
        empty_initialize();
    }
protected:
    void empty_initialize() {
        // node_num = 0;
        sentinel_node = get_node();
        sentinel_node->next = sentinel_node;
        sentinel_node->prev = sentinel_node;
    }
    link_type get_node() {
        return a.allocate(1);
    }
    void put_node(link_type p) {
        a.deallocate(p, 1);
    }
public:
    iterator begin() const {
        return sentinel_node->next;
    }
    iterator end() const {
        return sentinel_node;
    }
    bool empty() const{
        return sentinel_node->next == sentinel_node;
    }
    // size_type size() const {
    //     return node_num;
    // }
    reference front() const {
        return *begin();
    }
    reference back() const {
        return *(--end());
    }
    iterator insert(iterator pos, const T& x) {
        link_type tmp = create_node(x);
        tmp->next = pos.node;
        tmp->prev = pos.node->prev;
        tmp->prev->next = tmp;
        tmp->next->prev = tmp;
        // ++node_num;
        return tmp;
    }
    iterator erase(iterator pos) {
        link_type next_node = pos.node->next;
        pos.node->prev->next = pos.node->next;
        pos.node->next->prev = pos.node->prev;
        destroy_node(pos.node);
        // --node_num;
        return next_node;
    }
protected:
    link_type create_node(const T& x) {
        link_type node = get_node();
        a.construct(&node->data, x);
        return node;
    }
    void destroy_node(link_type p) {
        a.destroy(&p->data);
        put_node(p);
    }
public:
    void push_front(const T& x) {
        insert(begin(), x);
    }
    void push_back(const T& x) {
        insert(end(), x);
    }
    void pop_front() {
        erase(begin());
    }
    void pop_back() {
        erase(--end());
    }
    reference front() {
        return *begin();
    }
    reference back() {
        return *--end();
    }
    void clear() {
        link_type cur = sentinel_node->next;
        while (cur != sentinel_node) {
            link_type tmp = cur->next;
            destroy_node(cur);
            cur = tmp;
        }
        sentinel_node->next = sentinel_node;
        sentinel_node->prev = sentinel_node;
    }
    //移除所有值为value的元素
    void remove(const T& value) {
        for (iterator it = begin(); it != end(); ++it) {
            while (it != end() && *it == value) it = erase(it);
        }
    }
    //移除数值相同的连续元素
    void unique() {
        T prev_value = *begin();
        for (iterator it = ++begin(); it != end(); ++it) {
            while (it != end() && *it == prev_value) it = erase(it);
            prev_value = *it;
        }
    }

// protected:
public:
    //将[first, last)中的元素移动到pos之前
    void transfer(iterator pos, iterator first, iterator last) {
        if (pos != last) {
            link_type tmp = last.node->prev;
            first.node->prev->next = last.node;
            last.node->prev = first.node->prev;
            first.node->prev = pos.node->prev;
            tmp->next = pos.node;
            pos.node->prev->next = first.node;
            pos.node->prev = tmp;
        }
    }
public:
    //将lst的所有元素移动到pos之前
    void splice(iterator pos, list& lst) {
        if (!lst.empty()) {
            transfer(pos, lst.begin(), lst.end());
        }
    }
    //将del的元素移动到pos之前
    void splice(iterator pos, list&, iterator del) {
        iterator del_next = del;
        ++del_next;
        if (pos != del) {
            transfer(pos, del, del_next);
        }
    }
    //将[first, last)的元素移动到pos之前
    void splice(iterator pos, list&, iterator first, iterator last) {
        if (first != last) {
            transfer(pos, first, last);
        }
    }
    //将x合并到*this上，前提是二者都经过递增排序
    void merge(list& x) {
        iterator it2 = x.begin();
        for (iterator it = begin(); it != end(); ++it) {
            iterator tmp = it2;
            while (it2 != x.end() && *it2 < *it) ++it2;
            if (tmp != it2) transfer(it, tmp, it2);
            if (it2 == x.end()) break;
        }
        if (it2 != x.end()) transfer(end(), it2, x.end());
    }
    void reverse() {
        iterator it = begin();
        ++it;
        while (it != end()) {
            iterator tmp = it;
            transfer(begin(), tmp, ++it);
        }
    }
    void swap(list& x) {
        // iterator tmp = begin();
        // transfer(begin(), x.begin(), x.end());
        // transfer(x.end(), tmp, end());
        if (empty() && x.empty()) return;
        else if (empty()) {
            sentinel_node->next = x.sentinel_node->next;
            sentinel_node->prev = x.sentinel_node->prev;
            sentinel_node->prev->next = sentinel_node;
            sentinel_node->next->prev = sentinel_node;
            x.sentinel_node->next = x.sentinel_node;
            x.sentinel_node->prev = x.sentinel_node;
        }
        else if (x.empty()) {
            x.sentinel_node->next = sentinel_node->next;
            x.sentinel_node->prev = sentinel_node->prev;
            x.sentinel_node->prev->next = x.sentinel_node;
            x.sentinel_node->next->prev = x.sentinel_node;
            sentinel_node->next = sentinel_node;
            sentinel_node->prev = sentinel_node;
        }
        else {
            link_type tmp1 = sentinel_node->next, tmp2 = sentinel_node->prev;
            sentinel_node->next = x.sentinel_node->next;
            sentinel_node->prev = x.sentinel_node->prev;
            x.sentinel_node->next = tmp1;
            x.sentinel_node->prev = tmp2;
            sentinel_node->prev->next = sentinel_node->prev;
            sentinel_node->next->prev = sentinel_node->next;
            x.sentinel_node->prev->next = x.sentinel_node->prev;
            x.sentinel_node->next->prev = x.sentinel_node->next;
        }
    }
    
    void sort() {
        list<T, Alloc> carry;
        list<T, Alloc> counter[64];
        int fill = 0;
        // int cnt = 0;
        while (!empty()) {
            // cnt++;
            carry.splice(carry.begin(), *this, begin());
            // std::cout << "carry"<< cnt<<": " ;
            // for (list<int>::iterator it = carry.begin(); it != carry.end(); ++it) {
            //     std::cout << *it << ", ";
            // }
            // std::cout << std::endl;
            int i = 0;
            while (i < fill && !counter[i].empty()) {
                // std::cout << "counter["<<i<<"]: " ;
                // for (list<int>::iterator it = counter[i].begin(); it != counter[i].end(); ++it) {
                //     std::cout << *it << ", ";
                // }
                // std::cout << std::endl;
                counter[i].merge(carry);
                carry.swap(counter[i++]);
            }
            carry.swap(counter[i]);
            if (i == fill) ++fill;
        }
        for (int i = 1; i < fill; ++i) {
            counter[i].merge(counter[i-1]);
        }
        swap(counter[fill-1]);
    }
};

#endif      //end of __MY_LIST__