#include <iostream>
#include <functional>
using namespace std;

#define LIST_SIZE 32

template <typename T>
class List
{
public:
    virtual ~List() {};
    virtual int getSize() = 0;
    virtual void resize(int n) = 0;
    virtual T & at(int idx) = 0;
    virtual T & operator[](int idx) = 0;
    virtual void replace(int idx, const T &val) = 0;
    virtual void swap(int idx1, int idx2) = 0;
    virtual void swap(int idx, T &val) = 0;
    virtual void clear() = 0;
    virtual void insert(int idx, const T &val) = 0;
    virtual void insert(int idx, T &&val) = 0;
    virtual void remove(int idx) = 0;
    virtual T *find(const T &key, std::function<bool(const T &, const T &)> eq = [](const T &a, const T &b) -> bool
                    { return a == b; }) = 0;
    virtual int findIndex(const T &key, std::function<bool(const T &, const T &)> eq = [](const T &a, const T &b) -> bool
                          { return a == b; }) = 0;
    // virtual List<T*> findAll(const T &key, std::function<bool(const T &, const T &)> eq = [](const T &a, const T &b) -> bool
    //                           { return a == b; }) = 0;
    // virtual List<T*> split(int idx) = 0;
    virtual void append(List<T> &bL) = 0;
    virtual void reverse() = 0;
    virtual void traverse(std::function<void(T &)> op) = 0;
};

struct Error
{
    string msg;
    Error(string msg) : msg(msg) {}
};

template <typename T>
class ArrayList : public List<T>
{
protected:
    T *data;
    int capacity;
    int count;

private:
    void checkIndex(int idx)
    {
        if (idx >= count || idx < 0)
            throw Error("Index out of bound!");
    }

public:
    ArrayList() : data(nullptr), capacity(0), count(0) {}
    ArrayList(int cap) : data(new T[cap]), capacity(cap), count(0) {}
    ArrayList(const ArrayList<T> &aL) : data(new T[aL.capacity]), capacity(aL.capacity), count(aL.count)
    {
        T *p = data;
        aL.traverse([&p](T &val)
                    { *p++ = val; });
    }
    ~ArrayList() { clear(); }

    void traverse(std::function<void(T &)> op)
    {
        for (T *p = data; p < data + count; ++p)
        {
            op(*p);
        }
    }

    int getSize() { return count; }

    void resize(int n)
    {
        if (n <= capacity)
            return;
        int newSize = (n + LIST_SIZE - 1) / LIST_SIZE * LIST_SIZE;
        T *newData = new T[newSize];
        if (data)
        {
            for (T *p = data, *pDes = newData, *pE = p + count; p != pE; p++)
            {
                *pDes++ = std::move(*p);
            }
            delete[] data;
            data = newData;
            capacity = newSize;
        }
    }

    T & at(int idx)
    {
        checkIndex(idx);
        return data[idx];
    }

    T & operator[](int idx)
    {
        checkIndex(idx);
        return data[idx];
    }

    void replace(int idx, const T &val)
    {
        checkIndex(idx);
        data[idx] = val;
    }

    void swap(int idx1, int idx2)
    {
        checkIndex(idx1);
        checkIndex(idx2);
        if (idx1 == idx2)
            throw Error("Indices cannot be the same");

        T tmp = std::move(data[idx1]);
        data[idx1] = std::move(data[idx2]);
        data[idx2] = std::move(tmp);
    }

    void swap(int idx, T &val)
    {
        checkIndex(idx);
        T tmp = std::move(data[idx]);
        data[idx] = std::move(val);
        val = std::move(tmp);
    }

    void clear()
    {
        if (data)
        {
            delete[] data;
            data = nullptr;
        }
        capacity = 0;
        count = 0;
    }

    void insert(int idx, const T &val)
    {
        resize(count + 1);
        if (idx >= count)
            idx = count;
        for (T *p = data + count, *pE = data + idx; p != pE; p--)
        {
            *p = std::move(*(p - 1));
        }
        data[idx] = val;
        count++;
    }

    void insert(int idx, T &&val)
    {
        resize(count + 1);
        if (idx >= count)
            idx = count;
        for (T *p = data + count, *pE = data + idx; p != pE; p--)
        {
            *p = std::move(*(p - 1));
        }
        data[idx] = std::move(val);
        count++;
    }

    void remove(int idx)
    {
        for (T *p = data + idx, *pE = data + count - 1; p != pE; p++)
        {
            *p = std::move(*(p + 1));
        }
        count--;
    }

    T * find(const T &key, std::function<bool(const T &, const T &)> eq = [](const T &a, const T &b) -> bool
            { return a == b; })
    {
        for (T *p = data, *pE = data + count; p != pE; p++)
        {
            if (eq(*p, key))
                return p;
        }
        return nullptr;
    }
    int findIndex(const T &key, std::function<bool(const T &, const T &)> eq = [](const T &a, const T &b) -> bool
                  { return a == b; })
    {
        for (T *p = data, *pE = data + count; p != pE; p++)
        {
            if (eq(*p, key))
                return p - data;
        }
        return -1;
    }

    // List<T*> findAll(const T &key, std::function<bool(const T &, const T &)> eq = [](const T &a, const T &b) -> bool { return a == b; }) {
    //     ArrayList<T*> result;
    //     for (T *p = data, *pE = data + count; p != pE; p++)
    //     {
    //         if (eq(*p, key))
    //             result.insert(result.getSize(), p);
    //     }
    //     return result;
    // }

    // List<T*> split(int idx) {
    //     ArrayList<T*> result;
    //     for (T *p = data + idx, *pE = data + count; p != pE; p++) {
    //         result.insert(result.getSize(), p);
    //     }
    //     count = idx;
    //     return result;
    // }

    void append(List<T> &bL)
    {
        resize(count + bL.getSize());
        T *p = data + count;
        bL.traverse([&p](T &val)
                    { *p++ = val; });
        count += bL.getSize();
    }
    void reverse()
    {
        for (T *p = data, *pE = data + count / 2; p != pE; p++)
        {
            swap(p - data, data + count - 1 - p);
        }
    }
};

int main()
{
    ArrayList<int> myList(10);

    myList.insert(0, 10);
    myList.insert(1, 20);
    myList.insert(2, 30);
    myList.insert(3, 40);
    myList.insert(4, 50);

    for (int i = 0; i < myList.getSize(); i++)
    {
        cout << myList[i] << " ";
    }
    cout << endl;

    return 0;
}