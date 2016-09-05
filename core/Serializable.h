#ifndef SERIALIZABLE_H
#define SERIALIZABLE_H

#include <iostream>

class Serializable
{
    public:
        Serializable() = default;
        virtual ~Serializable() = default;

        virtual std::istream &Deserialize(std::istream &stream) = 0;
        virtual std::ostream &Serialize(std::ostream &stream) const = 0;
};

std::istream &operator>>(std::istream &stream, Serializable &object);
std::ostream &operator<<(std::ostream &stream, const Serializable &object);

#endif

