

#include "ObjectStack.hpp"

#include <iostream>

void main_ObjectStack_1()
{
    ObjectStack<int, 4> stc;
    stc.Push(-1);

    int val;
    stc.Pop(val);
    std::cout << "val:" << val << std::endl;

    stc.Push(-1);
    stc.Push(-2);
    stc.Push(-3);

    stc.Pop(val);
    std::cout << "val:" << val << std::endl;

    stc.Push(-4);

    stc.Pop(val);
    std::cout << "val:" << val << std::endl;

    stc.Pop(val);
    std::cout << "val:" << val << std::endl;

    stc.Pop(val);
    std::cout << "val:" << val << std::endl;
}


void main_ObjectStack_2()
{
    ObjectStack<int, 4> stc;
    stc.Push(-1);

    int val;
    stc.Pop(val);
    std::cout << "val:" << val << std::endl;

    stc.Push(-1);
    stc.Push(-2);
    stc.Push(-3);

    stc.Pop(val);
    std::cout << "val:" << val << std::endl;

    stc.Push(-4);

    stc.Pop(val);
    std::cout << "val:" << val << std::endl;

    stc.Pop(val);
    std::cout << "val:" << val << std::endl;

    stc.Pop(val);
    std::cout << "val:" << val << std::endl;
}


