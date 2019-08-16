#include <boost/python.hpp>
#include <graphene/wallet/wallet_utility.hpp>
#include <graphene/wallet/wallet.hpp>

namespace bp = boost::python;

namespace {

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

template<typename T>
void register_object_id(const char* name)
{
    bp::class_<T>(name, bp::init<uint64_t>())
        .def(bp::init<graphene::db::object_id_type>())
        .def("__repr__", object_repr<T>)
        .def("__str__", object_id_str<T>)
        .def("__hash__", &T::operator uint64_t)
        .def_readonly("object_id", &T::operator graphene::db::object_id_type)
    ;
};

template<typename T>
void register_hash(const char* name)
{
    bp::class_<T>(name, bp::init<>())
        .def(bp::init<std::string>())
        .def("__repr__", object_repr<T>)
        .def("__str__", &T::operator std::string)
        .def("__hash__", object_hash<T>)
    ;
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

struct Wallet : public graphene::wallet::WalletAPI
{
    void connect(const char *wallet_file, const char *server, const char *user = nullptr, const char *password = nullptr)
    {
        std::atomic_bool cancellation_token(false);
        Connect(cancellation_token, wallet_file, { server, user, password });
    }

    // wallet file
    bool is_new() { return exec(&graphene::wallet::wallet_api::is_new).wait(); }
    bool is_locked() { return exec(&graphene::wallet::wallet_api::is_locked).wait(); }
    bool lock() { return exec(&graphene::wallet::wallet_api::lock).wait(); }
    bool unlock(const char *password) { return exec(&graphene::wallet::wallet_api::unlock, password).wait(); }
    void set_password(const char *password) { exec(&graphene::wallet::wallet_api::set_password, password).wait(); }
    void save(const char *filepath) { exec(&graphene::wallet::wallet_api::save_wallet_file, filepath).wait(); }

    // general
    graphene::wallet::wallet_about about() { return exec(&graphene::wallet::wallet_api::about).wait(); }
    graphene::wallet::wallet_info info() { return exec(&graphene::wallet::wallet_api::info).wait(); }
};

}

BOOST_PYTHON_MODULE(dcore)
{
    register_hash<fc::ripemd160>("RIPEMD160");
    register_hash<fc::sha256>("SHA256");

    bp::class_<graphene::db::object_id_type>("ObjectId", bp::init<uint8_t, uint8_t, uint8_t>())
        .def(bp::init<std::string>())
        .def("__repr__", object_repr<graphene::db::object_id_type>)
        .def("__str__", &graphene::db::object_id_type::operator std::string)
        .def("__hash__", &graphene::db::object_id_type::operator uint64_t)
        .def_readonly("space", &graphene::db::object_id_type::space)
        .def_readonly("type", &graphene::db::object_id_type::type)
        .def_readonly("instance", &graphene::db::object_id_type::instance)
        .def("is_null", &graphene::db::object_id_type::is_null)
    ;

    register_object_id<graphene::chain::miner_id_type>("MinerId");

    bp::class_<decent::about_info>("About", bp::init<>())
        .def("__repr__", object_repr<decent::about_info>)
        .def_readwrite("version", &decent::about_info::version)
        .def_readwrite("graphene_revision", &decent::about_info::graphene_revision)
        .def_readwrite("graphene_revision_age", &decent::about_info::graphene_revision_age)
        .def_readwrite("fc_revision", &decent::about_info::fc_revision)
        .def_readwrite("fc_revision_age", &decent::about_info::fc_revision_age)
        .def_readwrite("compile_date", &decent::about_info::compile_date)
        .def_readwrite("boost_version", &decent::about_info::boost_version)
        .def_readwrite("openssl_version", &decent::about_info::openssl_version)
        .def_readwrite("cryptopp_version", &decent::about_info::cryptopp_version)
        .def_readwrite("build", &decent::about_info::build)
    ;

    bp::class_<graphene::wallet::wallet_about>("AboutFull", bp::init<>())
        .def("__repr__", object_repr<graphene::wallet::wallet_about>)
        .def_readwrite("daemon", &graphene::wallet::wallet_about::daemon_info)
        .def_readwrite("wallet", &graphene::wallet::wallet_about::wallet_info)
    ;

    bp::class_<graphene::wallet::wallet_info>("Info", bp::init<>())
        .def("__repr__", object_repr<graphene::wallet::wallet_info>)
        .def_readwrite("head_block_num", &graphene::wallet::wallet_info::head_block_num)
        .def_readwrite("head_block_id", &graphene::wallet::wallet_info::head_block_id)
        .def_readwrite("head_block_age", &graphene::wallet::wallet_info::head_block_age)
        .def_readwrite("next_maintenance_time", &graphene::wallet::wallet_info::next_maintenance_time)
        .def_readwrite("chain_id", &graphene::wallet::wallet_info::chain_id)
        .def_readwrite("participation", &graphene::wallet::wallet_info::participation)
        .add_property("active_miners",
            encode_list<graphene::wallet::wallet_info, std::vector<graphene::chain::miner_id_type>, &graphene::wallet::wallet_info::active_miners>,
            decode_list<graphene::wallet::wallet_info, std::vector<graphene::chain::miner_id_type>, &graphene::wallet::wallet_info::active_miners>)
    ;

    bp::class_<Wallet, boost::noncopyable>("Wallet", bp::init<>())
        .def("__bool__", &Wallet::is_new)
        .def_readonly("locked", &Wallet::is_locked)
        .def_readonly("connected", &Wallet::is_connected)
        .def("connect", &Wallet::connect, (bp::arg("wallet_file"), bp::arg("server"), bp::arg("user") = "", bp::arg("password") = ""))
        .def("lock", &Wallet::lock)
        .def("unlock", &Wallet::unlock, (bp::arg("password")))
        .def("set_password", &Wallet::set_password, (bp::arg("password")))
        .def("save", &Wallet::save, (bp::arg("filepath")))
        .def("about", &Wallet::about)
        .def("info", &Wallet::info)
    ;
}
