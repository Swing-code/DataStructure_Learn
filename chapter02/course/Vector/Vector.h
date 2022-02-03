#pragma once
// Vector source code
#define DEFAULT_CAPACITY 3  // 默认初始容量(实际应用中可设置为更大)
using Rank = int;
template <typename T>
class Vector
{
private:
    Rank _size;
    int _capacity;
    T * _elem;
protected:
    void copyFrom(T const * A, Rank lo, Rank hi);
    void expand();
    void shrink();
public:
    Vector(int c = DEFAULT_CAPACITY) {_elem = new T[_capacity = c]; _size = 0;} // 默认
    Vector(T const * A, Rank lo, Rank hi) { copyFrom(A, lo, hi); }
    Vector(Vector<T> const & V, Rank lo, Rank hi) // 向量区间复制
    { copyFrom(V._elem, lo, hi); }
    Vector(Vector<T> const & V) // 向量整体复制
    { copyFrom(V._elem, 0, V._size);}
    ~Vector() { delete []_elem; }// 释放内部空间
/******************************无序向量*******************************/
//基本操作
    T & operator[](Rank r) { return _elem[r]; } // 可作为左值
    const T & operator[](Rank r) const { return _elem[r]; } // 仅限于右值
    Rank insert(Rank r, T const & e);
    int remove(Rank lo, Rank hi);
    T remove(Rank r);
// 顺序查找
    Rank find(T const & e, Rank lo, Rank hi);
// 去重
    int deduplicate();
// 遍历
    void traverse(void (* visit)(T &));   // 函数指针
    template <typename VST>     // 函数对象
    void traverse(VST & visit);
/***************************无序向量*********************************/

/***************************有序向量*********************************/
// 有序性甄别
    int disordered() const;
// 去重
    int uniquify();
};


// 复制算法
template <typename T>
void Vector<T>::copyFrom(T const * A, Rank lo, Rank hi)
{
    _elem = new T[_capacity = DEFAULT_CAPACITY < 2*(hi - lo) ? 2*(hi - lo) : DEFAULT_CAPACITY];  // 分配空间
    for (_size = 0; lo < hi; _size++, lo++) // A[lo,hi)内的元素注意复制到
        _elem[_size] = A[lo];   // _elem[0,hi - lo)中
}   // O(hi - lo) = O(n)

// 扩容算法
template <typename T>
void Vector<T>::expand()    // 向量空间不足时扩容
{
    if(_size < _capacity)   // 尚未满员时，不必扩容
        return;
    _capacity = _capacity < DEFAULT_CAPACITY ? DEFAULT_CAPACITY : _capacity;
    T * oldElem = _elem;    // 将原有的数据记录
    _elem = new T[_capacity <<= 1]; // 容量加倍
    for (int i = 0; i < _size; i++) // 复制原向量内容
        _elem[i] = oldElem[i];      // T为基本类型或已重载运算符"="
    delete []oldElem;   // 释放原空间
}   // 得益于向量的封装，扩容之后数据区的物理地址有所改变，却不致出现野指针

/************************* 无序向量 ******************************/
// 插入算法
template <typename T>
Rank Vector<T>::insert(Rank r, T const & e)
{   // 0 <= r <= size
    expand();   // 若有必要，扩容
    for (int i = _size; i > r; i--) // O(n-r):自后向前
        _elem[i] = _elem[i - 1];    // 后继元素顺次后移一个单元
    _elem[r] = e;   // 置入新元素
    _size++;        // 更新向量尺寸
    return r;       // 返回秩
}

// 删除区间
template <typename T>
int Vector<T>::remove(Rank lo, Rank hi)
{// 0 <=lo<=hi<=n
    if (hi == lo)   // 出于效率，单独处理退化情况
        return 0;
    while (hi < _size)  // O(n - hi):[hi,n)顺次前移
        _elem[lo++] = _elem[hi++];
    _size = lo; // 更新规模
    shrink();
    return hi - lo;
    // for (int i = lo; i < lo + _size - hi; i++)
    //     _elem[i] = _elem[i + hi - lo];
    // _size = _size -hi + lo;
    // shrink();
    // return hi - lo;
}

// 删除单个元素
template <typename T>
T Vector<T>::remove(Rank r)
{
    T e = _elem[r];     // 备份
    remove(r, r + 1);   // "区间删除"，将单元素视作区间特例
    return e;       // 返回被删除元素
}   //O(n-r), 0 <= r < _size
// 顺序查找
template <typename T>
Rank Vector<T>::find(T const & e, Rank lo, Rank hi) // O(hi - lo) = O(n)
{   // 0 <= lo <= hi <_size
    while ((lo < hi--) && (e != _elem[hi])) // 逆向查找
    return hi;  // 返回值小于lo即意味着失败；否则即命中者的秩(有多个是，返回最大者)
    // while (hi > lo)
    // {
    //     if (_elem[--hi] = e)
    //         return hi;
    // }
    // return -1;
}

// 去重
template <typename T>
int Vector<T>::deduplicate()
{
    int oldSize = _size;    // 备份想看iang规模
    Rank i = 1;     // 从1开始
    while (i < _size)
        if (find(_elem[i], 0, i) < 0)   // 没有找到该元素，不重复
            i++;
        else    // 该元素重复
            remove(i);
    return oldSize - _size;
}

// 遍历-函数指针
template <typename T>
void Vector<T>::traverse(void (* visit)(T &))
{
    for (int i = 0; i < _size; i++)
        visit(_elem[i]);
}

// 遍历-函数对象
template <typename T>
template <typename VST>
void Vector<T>::traverse(VST & visit)
{
    for (int i = 0; i < _size; i++)
        visit(_elem[i]);
}
/*********************************无序向量****************************/

/*********************************有序向量****************************/

// 有序性甄别
template <typename T>
int Vector<T>::disordered() const
{
    int n = 0;  // 相邻逆序对的计数器
    for (int i = 1; i < _size; i++) // 逐一检查各相邻元素
        n += (_elem[i - 1] > _elem[i]);
        // if (_elem[i] < _elem[i-1])
        //     n++;
    return n;
}

// 去重
template <typename T>
int Vector<T>::uniquify()
{
    // int oldSize = _size;
    Rank i = 0, j = 0;
    while (++j < _size) // 找到第一个不相等的值，赋给++i
        if (_elem[i] != _elem[j])
            _elem[++i] = _elem[j];
    _size = ++i;    // 更新向量规模
    shrink();       // 如果必要，缩小容量
    return j - i;   // 返回删除元素个数
        // for(j; j <_size; j++)
        //     if (_elem[i] != _elem[j])
        //         _elem[++i] = _elem[j];
}

/*********************************有序向量****************************/