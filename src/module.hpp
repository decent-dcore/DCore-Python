#include <boost/python.hpp>
#include <fc/io/json.hpp>
#include <graphene/db/object_id.hpp>

namespace bp = boost::python;

namespace dcore {

template<typename T>
std::size_t object_hash(const T& obj)
{
    return std::hash<T>{}(obj);
}

template<typename T>
std::string object_repr(const T& obj)
{
    return fc::json::to_string(obj);
}

template<typename T>
std::string object_id_str(const T& obj)
{
    return std::string(static_cast<graphene::db::object_id_type>(obj));
}

template<typename T, typename Container, const Container T::* container>
bp::list encode_list(const T &obj)
{
    bp::list l;
    for(const auto& v : (obj.*container))
        l.append(v);
    return l;
}

template<typename T, typename Container, Container T::* container>
void decode_list(T& obj, const bp::list &l)
{
    auto len = bp::len(l);
    (obj.*container).resize(len);
    while(len--)
        (obj.*container)[len] = bp::extract<typename Container::value_type>(l[len]);
}

template<typename T>
T decode_safe_type(const fc::safe<T>& v)
{
    return v.value;
}

template<typename T>
void encode_safe_type(fc::safe<T>& obj, const T& v)
{
    obj.value = v;
}

template<typename T>
bp::object encode_optional(const T& obj)
{
    if(obj.valid())
       return bp::object(*obj);

    return bp::object();
}

void register_common_types();

} // dcore
