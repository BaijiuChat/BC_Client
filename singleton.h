#ifndef SINGLETON_H
#define SINGLETON_H
#include "global.h"

// 模板类的声明和实现要写在一起
// 为了让子类能够构造单例，所以使用protected
template <typename T>
class Singleton{
protected:
    Singleton() = default; //要求编译器生成默认的无参构造
    Singleton(const Singleton<T>&) = delete; // 禁止显式拷贝构造
    Singleton& operator = (const Singleton<T>& st) = delete; // 删除赋值运算符，防止让obj1=obj2来复制单例

    /*
     *  static是为了让所有类的实例共享同一个_instance
     *  shared_ptr是一个智能指针,会自动管理<T>类型的生命周期
     */
    static std::shared_ptr<T> _instance;
public:
    static std::shared_ptr<T> GetInstance(){
        static std::once_flag s_flag;
        std::call_once(s_flag,[&](){
            _instance = std::shared_ptr<T>(new T);
        });

        return _instance;
    }

    void PrintAddress(){
        std::cout << _instance.get() << std::endl;
    }

    ~Singleton(){
        std::cout << "单例已被析构。" << std::endl;
    }
};

template <typename T>
std::shared_ptr<T> Singleton<T>::_instance = nullptr; //类外初始化智能指针为野指针

#endif // SINGLETON_H
