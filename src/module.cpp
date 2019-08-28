#include "module.hpp"
#include <graphene/wallet/wallet_utility.hpp>
#include <graphene/wallet/wallet.hpp>
#include <graphene/utilities/dirhelper.hpp>
#include <fc/log/logger_config.hpp>

namespace dcore {

template<typename T, boost::filesystem::path (T::* method)() const>
std::string decode_path(const T &obj)
{
    return (obj.*method)().generic_string();
}

template<typename T, void (T::* method)(const boost::filesystem::path&)>
void encode_path(T &obj, const std::string &p)
{
    (obj.*method)(p);
}

void configure_logging(const std::string &config)
{
    fc::configure_logging(fc::json::from_string(config).as<fc::logging_config>());
}

std::string default_logging()
{
    return fc::json::to_pretty_string(fc::logging_config::default_config());
}

struct Wallet : public graphene::wallet::WalletAPI
{
    void connect(const std::string &wallet_file, const std::string &server, const std::string &user, const std::string &password)
    {
        Connect(wallet_file, { server, user, password });
    }

    // wallet file
    bool is_new() { return exec(&graphene::wallet::wallet_api::is_new).wait(); }
    bool is_locked() { return exec(&graphene::wallet::wallet_api::is_locked).wait(); }
    bool lock() { return exec(&graphene::wallet::wallet_api::lock).wait(); }
    bool unlock(const std::string &password) { return exec(&graphene::wallet::wallet_api::unlock, password).wait(); }
    void set_password(const std::string &password) { exec(&graphene::wallet::wallet_api::set_password, password).wait(); }
    void save(const std::string &filepath) { exec(&graphene::wallet::wallet_api::save_wallet_file, filepath).wait(); }
    bool load(const std::string &filepath) { return exec(&graphene::wallet::wallet_api::load_wallet_file, filepath).wait(); }
    std::string get_filename() { return exec(&graphene::wallet::wallet_api::get_wallet_filename).wait().generic_string(); }
    bool import_key(const std::string &account, const std::string &key) { return exec(&graphene::wallet::wallet_api::import_key, account, key).wait(); }
    bool import_single_key(const std::string &account, const std::string &key) { return exec(&graphene::wallet::wallet_api::import_single_key, account, key).wait(); }
    std::string get_private_key(const graphene::chain::public_key_type &pubkey) { return exec(&graphene::wallet::wallet_api::get_private_key, pubkey).wait(); }
    std::string dump_private_keys() { return object_repr(exec(&graphene::wallet::wallet_api::dump_private_keys).wait()); }
    bp::list list_my_accounts() { return encode_container(exec(&graphene::wallet::wallet_api::list_my_accounts).wait()); }

    // general
    graphene::wallet::wallet_about about() { return exec(&graphene::wallet::wallet_api::about).wait(); }
    graphene::wallet::wallet_info info() { return exec(&graphene::wallet::wallet_api::info).wait(); }
    bp::object get_block(uint32_t num) { return optional_value(exec(&graphene::wallet::wallet_api::get_block, num).wait()); }
    fc::time_point_sec head_block_time() { return exec(&graphene::wallet::wallet_api::head_block_time).wait(); }
};

} // dcore

#if defined(__GNUC__) && __GNUC__ <= 7 && __GNUC_MINOR__ <= 4
#pragma GCC visibility push(default)
#endif

BOOST_PYTHON_MODULE(dcore)
{
    fc::configure_logging(fc::logging_config());
    bp::def("configure_logging", dcore::configure_logging);
    bp::def("default_logging", dcore::default_logging);

    dcore::register_common_types();
    dcore::register_account();

    bp::class_<graphene::utilities::decent_path_finder, boost::noncopyable>("Path", bp::no_init)
        .def("instance", graphene::utilities::decent_path_finder::instance, bp::return_value_policy<bp::reference_existing_object>())
        .staticmethod("instance")
        .add_property("home", dcore::decode_path<graphene::utilities::decent_path_finder, &graphene::utilities::decent_path_finder::get_decent_home>)
        .add_property("logs", dcore::decode_path<graphene::utilities::decent_path_finder, &graphene::utilities::decent_path_finder::get_decent_logs>,
                              dcore::encode_path<graphene::utilities::decent_path_finder, &graphene::utilities::decent_path_finder::set_decent_logs_path>)
        .add_property("temp", dcore::decode_path<graphene::utilities::decent_path_finder, &graphene::utilities::decent_path_finder::get_decent_temp>,
                              dcore::encode_path<graphene::utilities::decent_path_finder, &graphene::utilities::decent_path_finder::set_decent_temp_path>)
        .add_property("packages", dcore::decode_path<graphene::utilities::decent_path_finder, &graphene::utilities::decent_path_finder::get_decent_packages>,
                                  dcore::encode_path<graphene::utilities::decent_path_finder, &graphene::utilities::decent_path_finder::set_packages_path>)
    ;

    bp::class_<decent::about_info>("About", bp::init<>())
        .def("__repr__", dcore::object_repr<decent::about_info>)
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
        .def("__repr__", dcore::object_repr<graphene::wallet::wallet_about>)
        .def_readwrite("daemon", &graphene::wallet::wallet_about::daemon_info)
        .def_readwrite("wallet", &graphene::wallet::wallet_about::wallet_info)
    ;

    bp::class_<graphene::wallet::wallet_info>("Info", bp::init<>())
        .def("__repr__", dcore::object_repr<graphene::wallet::wallet_info>)
        .def_readwrite("head_block_num", &graphene::wallet::wallet_info::head_block_num)
        .def_readwrite("head_block_id", &graphene::wallet::wallet_info::head_block_id)
        .def_readwrite("head_block_age", &graphene::wallet::wallet_info::head_block_age)
        .def_readwrite("next_maintenance_time", &graphene::wallet::wallet_info::next_maintenance_time)
        .def_readwrite("chain_id", &graphene::wallet::wallet_info::chain_id)
        .def_readwrite("participation", &graphene::wallet::wallet_info::participation)
        .add_property("active_miners",
            dcore::encode_list<graphene::wallet::wallet_info, std::vector<graphene::chain::miner_id_type>, &graphene::wallet::wallet_info::active_miners>,
            dcore::decode_list<graphene::wallet::wallet_info, std::vector<graphene::chain::miner_id_type>, &graphene::wallet::wallet_info::active_miners>)
    ;

    bp::class_<dcore::Wallet, boost::noncopyable>("Wallet", bp::init<>())
        .def("__bool__", &dcore::Wallet::is_new)
        .add_property("locked", &dcore::Wallet::is_locked)
        .add_property("connected", &dcore::Wallet::is_connected)
        .add_property("filename", &dcore::Wallet::get_filename)
        .def("connect", &dcore::Wallet::connect, (bp::arg("wallet_file"), bp::arg("server") = "ws://localhost:8090", bp::arg("user") = "", bp::arg("password") = ""))
        .def("lock", &dcore::Wallet::lock)
        .def("unlock", &dcore::Wallet::unlock, (bp::arg("password")))
        .def("set_password", &dcore::Wallet::set_password, (bp::arg("password")))
        .def("save", &dcore::Wallet::save, (bp::arg("filepath")))
        .def("load", &dcore::Wallet::load, (bp::arg("filepath")))
        .def("import_key", &dcore::Wallet::import_key, (bp::arg("account"), bp::arg("key")))
        .def("import_single_key", &dcore::Wallet::import_single_key, (bp::arg("account"), bp::arg("key")))
        .def("get_private_key", &dcore::Wallet::get_private_key, (bp::arg("pubkey")))
        .def("dump_private_keys", &dcore::Wallet::dump_private_keys)
        .def("about", &dcore::Wallet::about)
        .def("list_my_accounts", &dcore::Wallet::list_my_accounts)
        .def("info", &dcore::Wallet::info)
        .def("get_block", &dcore::Wallet::get_block, (bp::arg("num")))
        .def("head_block_time", &dcore::Wallet::head_block_time)
    ;
}
