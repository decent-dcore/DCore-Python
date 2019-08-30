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
    while(len--) {
        bp::extract<typename Container::value_type> v(l[len]);
        (obj.*container)[len] = v();
    }
}

template<typename T, typename Container, const Container T::* container>
bp::dict encode_dict(const T &obj)
{
    bp::dict d;
    for(const auto& v : (obj.*container))
        d.setdefault(v.first, v.second);
    return d;
}

template<typename T, typename Container, Container T::* container>
void decode_dict(T& obj, const bp::dict &d)
{
    bp::list l = d.keys();
    auto len = bp::len(l);
    (obj.*container).reserve(len);
    while(len--) {
        bp::object k = l[len];
        bp::extract<typename Container::mapped_type> v(d.get(k));
        (obj.*container)[bp::extract<typename Container::key_type>(k)] = v();
    }
}

template<typename T, typename Container, const Container T::* container>
bp::list encode_set(const T &obj)
{
    bp::list l;
    for(const auto& v : (obj.*container))
        l.append(v);
    return l;
}

template<typename T, typename Container, Container T::* container>
void decode_set(T& obj, const bp::list &l)
{
    auto len = bp::len(l);
    while(len--) {
        bp::extract<typename Container::value_type> v(l[len]);
        (obj.*container).insert(v());
    }
}

template<typename T, typename V, const fc::safe<V> T::* instance>
V decode_safe_type(const T& obj)
{
    return (obj.*instance).value;
}

template<typename T, typename V, fc::safe<V> T::* instance>
void encode_safe_type(T& obj, const V &v)
{
    (obj.*instance) = v;
}

template<typename T>
bp::object optional_value(const T& obj)
{
    return obj.valid() ? bp::object(*obj) : bp::object();
}

template<typename T, typename V, const fc::optional<V> T::* instance>
bp::object decode_optional_type(const T& obj)
{
    return optional_value(obj.*instance);
}

template<typename T, typename V, fc::optional<V> T::* instance>
void encode_optional_type(T& obj, const V &v)
{
    (obj.*instance) = v;
}

template<typename T>
class object_wrapper : public T, public bp::wrapper<T>
{
public:
    template<typename C>
    static C& wrap(C &instance)
    {
        instance.add_property("object_id", &T::id);
        instance.def("get_id", &T::get_id);
        instance.def("__repr__", object_repr<T>);
        return instance;
    }
};

void register_common_types();
void register_account();
void register_chain();

} // dcore
