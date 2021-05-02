// http://www.cppblog.com/abware/archive/2007/04/16/22043.html
// file: 3mylist.h
#include <iostream>

using namespace std;

template <typename T>
class ListItem
{
public:
    ListItem(T value, ListItem<T>* next)
    {
        _value = value;
        _next = next;
    }
    T value() const { return _value; }
    void value(T value) { _value = value; }
    ListItem* next() const { return _next; }
    void next(ListItem* next) { _next = next; }
    //
private:
    T _value;
    ListItem* _next;  // †ÎÏò´®ÁÐ£¨single linked list£©
};

template <typename T>
class List
{
public:
    ~List()
    {
        if(_front == _end) return;
        ListItem<T>* item = _front;
        while(item != _end)
        {
            ListItem<T>* iter = item;
            item = item->next();
            delete iter;
        }
    
    }
    void insert_front(T value)
    {
        _front = new ListItem<T>(value, _front);
    }
    void insert_end(T value)
    {
        if(_front == _end)
        {
            _front = new ListItem<T>(value, _front);
        }
        ListItem<T>* item = _front;
        while(item->next() != _end)
        {
            item = item->next();
        }
        item->next(new ListItem<T>(value, _end));
    }
    void display(std::ostream &os = std::cout) const
    {
        ListItem<T>* item = _front;
        while(item != _end)
        {
            os<<item->value()<<" ";
            item = item->next();
        }
        os<<endl;
    }
    ListItem<T>* front(){ return _front;}
    ListItem<T>* end(){ return _end;}
    // 
private:
    ListItem<T>* _end;
    ListItem<T>* _front;
    long _size;
};