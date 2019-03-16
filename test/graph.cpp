#include "util/graph.hpp"

#include "util/common.h"

#include <string>
#include <ostream>
#include <iostream>

// Not reflective for example
class SimpleTypeExtractor
{
public:
    typedef size_t Id;

    template <typename T>
    inline static Id extract_typeid()
    {
        return typeid(T).hash_code();
    }

    template <typename T>
    inline static void* import_value(T const& v)
    {
        return (void*)v;
    }

    template <typename T>
    inline static T* export_value(void* v)
    {
        return (T*)v;
    }
};

int main(int argc, char const *argv[])
{
    craft::Graph<
        craft::GraphTyped<
            craft::GraphCore<void*>,
            SimpleTypeExtractor
        >
    > graph;

    auto nhello = graph.addNode(new std::string("hello"));
    auto nworld = graph.addNode("world");
    auto nnext = graph.addEdge(new std::string("next"), { nhello, nworld });

    graph.forAllNodes([&](auto n)
    {
        std::cerr << n->data << n->type << std::endl;

        graph.forAllEdgesOnNode(n, [&](auto e)
        {
            std::cerr << "  " << e->data << e->type << std::endl;
        });
    });

    return 0;
}
