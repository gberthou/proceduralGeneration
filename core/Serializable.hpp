#ifndef SERIALIZABLE_H
#define SERIALIZABLE_H

#include <iostream>

namespace pg
{
    class InputStream;
    class OutputStream;

    class Serializable
    {
        public:
            Serializable() = default;
            virtual ~Serializable() = default;

            virtual InputStream &Deserialize(InputStream &stream) = 0;
            virtual OutputStream &Serialize(OutputStream &stream) const = 0;
    };

    class InputStream
    {
        public:
            InputStream(std::istream &s, bool b = true):
                stream(s),
                binary(b)
            {
            }

            template<class T, class=
            typename std::enable_if<!std::is_base_of<Serializable, T>::value,
                     InputStream &>::type>
            InputStream &operator>>(T &x)
            {
                // TODO: Fix endianness
                stream.read(reinterpret_cast<char*>(&x), sizeof(T));
                return *this;
            }

            InputStream &operator>>(Serializable &x)
            {
                return x.Deserialize(*this);
            }

        protected:
            std::istream &stream;
            bool binary;
    };

    class OutputStream
    {
        public:
            OutputStream(std::ostream &s, bool b = true):
                stream(s),
                binary(b)
            {
            }

            template<class T, class=
            typename std::enable_if<!std::is_base_of<Serializable, T>::value,
                     OutputStream &>::type>
                OutputStream &operator<<(const T &x)
            {
                // TODO: Fix endianness
                stream.write(reinterpret_cast<const char*>(&x), sizeof(T));
                return *this;
            }

            OutputStream &operator<<(const Serializable &x)
            {
                return x.Serialize(*this);
            }

        protected:
            std::ostream &stream;
            bool binary;
    };
}

#endif

