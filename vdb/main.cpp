#include "threadpool.h"

#include <iostream>

int main()
{
    vdb::ThreadPool pool(std::thread::hardware_concurrency());

    pool.enqueuWithCallback([](const std::string &text) { return text; },
                            [](const std::string &text) { std::cout << text; },
                            std::string("Hello world"));

    auto future = pool.enqueue([](const std::string &text) { return text; }, std::string{"text"});

    std::cout << future.get() << std::endl;
}