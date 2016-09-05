#include "Serializable.h"

/*
std::istream &Serializable::Deserialize(std::istream &stream)
{
    return stream;
}

std::ostream &Serializable::Serialize(std::ostream &stream) const
{
    return stream;
}
*/

std::istream &operator>>(std::istream &stream, Serializable &object)
{
    return object.Deserialize(stream);
}

std::ostream &operator<<(std::ostream &stream, const Serializable &object)
{
    return object.Serialize(stream);
}

