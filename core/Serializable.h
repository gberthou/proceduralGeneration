#ifndef SERIALIZABLE_H
#define SERIALIZABLE_H

#include <iostream>

class Serializable
{
    public:
        Serializable() = default;
        virtual ~Serializable() = default;

        virtual std::istream &Deserialize(std::istream &stream);
        virtual std::ostream &Serialize(std::ostream &stream) const;
};

std::istream &operator>>(std::istream &stream, Serializable &object);
std::ostream &operator<<(std::ostream &stream, const Serializable &object);

#endif

