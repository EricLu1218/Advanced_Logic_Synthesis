#pragma once
#include <memory>
#include <string>
#include <vector>

namespace raw
{
    struct Gate
    {
        using ptr = std::unique_ptr<Gate>;

        enum struct Type : int
        {
            AND = 0,
            OR,
            NOT,
            BUFFER,
            CONST_0,
            CONST_1,
            NONE
        };

        std::string name;
        Type type;
        std::vector<std::string> fanInNames;

        Gate() : type(Type::NONE) {}
        Gate(const std::string &name, const Type &type, std::vector<std::string> &&fanInNames)
            : name(name), type(type), fanInNames(std::move(fanInNames)) {}
    };
}
