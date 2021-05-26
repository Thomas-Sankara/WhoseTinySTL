// TypeTraits.h
// 负责萃取型别(type)特性，这是根据iterator traits的想法衍生出来的更泛化和实用的东西
#ifndef _TYPE_TRAITS_H
#define _TYPE_TRAITS_H

namespace WhoseTinySTL
{
    namespace{ // 未命名的命名空间，是c++用来代替文件的static变量的
        template<bool, class Ta, class Tb>
        struct IfThenElse;

        template<class Ta, class Tb>
        struct IfThenElse < true, Ta, Tb > {
            using result = Ta;
        };

        template<class Ta, class Tb>
        struct IfThenElse < false, Ta, Tb > {
            using result = Tb;
        };
    } // 但我没看出来这玩意是干啥的，待我日后再研究**********************************

    // p104下，需要对象来进行模板推导，空的类又不会造成额外开销
    struct _true_type {};
    struct _false_type {};

    /*
	** 萃取传入的T类型的类型特性
	*/
    template<class T> // p105，认为非内置类型全都有用户自定义的constructor和destructor
    struct _type_traits
    {
        typedef _true_type this_dummy_member_must_be_first; // 提高对某些编译器的健壮性

        typedef _false_type has_trivial_default_constructor;
        typedef _false_type has_trivial_copy_constructor;
        typedef _false_type has_trivial_assignment_operator;
        typedef _false_type has_trivial_destructor;
        typedef _false_type is_POD_type;
    };

    template<> // stl在<stl_sonfig.h>中将__STL_TEMPLATE_NULL定义为template<>，我还是算了吧
    struct _type_traits<bool> // 特例化的类模板后面还可以加这样的模板参数列表
    {
        typedef _true_type has_trivial_default_constuctor;
        typedef _true_type has_trivial_copy_constructor;
        typedef _true_type has_trivial_assignment_operator;
        typedef _true_type has_trivial_destructor;
        typedef _true_type is_POD_type;
    };

    template<> // 模板参数列表为空表示全特例化
    struct _type_traits<char>
    {
        typedef _true_type has_trivial_default_constuctor;
        typedef _true_type has_trivial_copy_constructor;
        typedef _true_type has_trivial_assignment_operator;
        typedef _true_type has_trivial_destructor;
        typedef _true_type is_POD_type;
    };

    template<>
    struct _type_traits<unsigned char>
    {
        typedef _true_type has_trivial_default_constuctor;
        typedef _true_type has_trivial_copy_constructor;
        typedef _true_type has_trivial_assignment_operator;
        typedef _true_type has_trivial_destructor;
        typedef _true_type is_POD_type;
    };

    template<>
    struct _type_traits<signed char>
    {
        typedef _true_type has_trivial_default_constuctor;
        typedef _true_type has_trivial_copy_constructor;
        typedef _true_type has_trivial_assignment_operator;
        typedef _true_type has_trivial_destructor;
        typedef _true_type is_POD_type;
    };

    template<>
    struct _type_traits<wchar_t>
    {
        typedef _true_type has_trivial_default_constuctor;
        typedef _true_type has_trivial_copy_constructor;
        typedef _true_type has_trivial_assignment_operator;
        typedef _true_type has_trivial_destructor;
        typedef _true_type is_POD_type;
    };

    template<>
    struct _type_traits<short>
    {
        typedef _true_type has_trivial_default_constuctor;
        typedef _true_type has_trivial_copy_constructor;
        typedef _true_type has_trivial_assignment_operator;
        typedef _true_type has_trivial_destructor;
        typedef _true_type is_POD_type;
    };

    template<>
    struct _type_traits<unsigned short>
    {
        typedef _true_type has_trivial_default_constuctor;
        typedef _true_type has_trivial_copy_constructor;
        typedef _true_type has_trivial_assignment_operator;
        typedef _true_type has_trivial_destructor;
        typedef _true_type is_POD_type;
    };

    template<>
    struct _type_traits<int>
    {
        typedef _true_type has_trivial_default_constuctor;
        typedef _true_type has_trivial_copy_constructor;
        typedef _true_type has_trivial_assignment_operator;
        typedef _true_type has_trivial_destructor;
        typedef _true_type is_POD_type;
    };

    template<>
    struct _type_traits<unsigned int>
    {
        typedef _true_type has_trivial_default_constuctor;
        typedef _true_type has_trivial_copy_constructor;
        typedef _true_type has_trivial_assignment_operator;
        typedef _true_type has_trivial_destructor;
        typedef _true_type is_POD_type;
    };

    template<>
    struct _type_traits<long>
    {
        typedef _true_type has_trivial_default_constuctor;
        typedef _true_type has_trivial_copy_constructor;
        typedef _true_type has_trivial_assignment_operator;
        typedef _true_type has_trivial_destructor;
        typedef _true_type is_POD_type;
    };

    template<>
    struct _type_traits<unsigned long>
    {
        typedef _true_type has_trivial_default_constuctor;
        typedef _true_type has_trivial_copy_constructor;
        typedef _true_type has_trivial_assignment_operator;
        typedef _true_type has_trivial_destructor;
        typedef _true_type is_POD_type;
    };

    template<>
    struct _type_traits<long long>
    {
        typedef _true_type has_trivial_default_constuctor;
        typedef _true_type has_trivial_copy_constructor;
        typedef _true_type has_trivial_assignment_operator;
        typedef _true_type has_trivial_destructor;
        typedef _true_type is_POD_type;
    };

    template<>
    struct _type_traits<unsigned long long>
    {
        typedef _true_type has_trivial_default_constuctor;
        typedef _true_type has_trivial_copy_constructor;
        typedef _true_type has_trivial_assignment_operator;
        typedef _true_type has_trivial_destructor;
        typedef _true_type is_POD_type;
    };

    template<>
    struct _type_traits<float>
    {
        typedef _true_type has_trivial_default_constuctor;
        typedef _true_type has_trivial_copy_constructor;
        typedef _true_type has_trivial_assignment_operator;
        typedef _true_type has_trivial_destructor;
        typedef _true_type is_POD_type;
    };

    template<>
    struct _type_traits<double>
    {
        typedef _true_type has_trivial_default_constuctor;
        typedef _true_type has_trivial_copy_constructor;
        typedef _true_type has_trivial_assignment_operator;
        typedef _true_type has_trivial_destructor;
        typedef _true_type is_POD_type;
    };

    template<>
    struct _type_traits<long double>
    {
        typedef _true_type has_trivial_default_constuctor;
        typedef _true_type has_trivial_copy_constructor;
        typedef _true_type has_trivial_assignment_operator;
        typedef _true_type has_trivial_destructor;
        typedef _true_type is_POD_type;
    };

    template<class T>
    struct _type_traits<T*>
    {
        typedef _true_type has_trivial_default_constuctor;
        typedef _true_type has_trivial_copy_constructor;
        typedef _true_type has_trivial_assignment_operator;
        typedef _true_type has_trivial_destructor;
        typedef _true_type is_POD_type;
    };

    template<class T>
    struct _type_traits<const T*>
    {
        typedef _true_type has_trivial_default_constuctor;
        typedef _true_type has_trivial_copy_constructor;
        typedef _true_type has_trivial_assignment_operator;
        typedef _true_type has_trivial_destructor;
        typedef _true_type is_POD_type;
    };
    // https://github.com/zouxiaohang/TinySTL/blob/master/TinySTL/TypeTraits.h
    // 该作者在后面还有char*,unsigned char*,signed char*,const char*,
    // const unsigned char*和const signed char*的偏特化，我觉得没必要
} // namespace WhoseTniySTL

#endif