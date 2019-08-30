#include "module.hpp"
#include <graphene/wallet/wallet_utility.hpp>
#include <graphene/wallet/wallet.hpp>
#include <graphene/utilities/dirhelper.hpp>
#include <fc/log/logger_config.hpp>

namespace wa = graphene::wallet;

namespace dcore {

template<typename T>
bp::list to_list(const T &container)
{
    bp::list l;
    for(const auto& v : container)
        l.append(v);
    return l;
}

template<typename T>
bp::dict to_dict(const T &container)
{
    bp::dict d;
    for(const auto& v : container)
        d.setdefault(v.first, v.second);
    return d;
}

template<typename T, boost::filesystem::path (T::* method)() const>
std::string decode_path(const T &obj)
{
    return fc::path_to_utf8((obj.*method)());
}

template<typename T, void (T::* method)(const boost::filesystem::path&)>
void encode_path(T &obj, const std::string &p)
{
    (obj.*method)(fc::path_from_utf8(p));
}

void configure_logging(const std::string &config)
{
    fc::configure_logging(fc::json::from_string(config).as<fc::logging_config>());
}

std::string default_logging()
{
    return fc::json::to_pretty_string(fc::logging_config::default_config());
}

struct Wallet : public wa::WalletAPI
{
    void connect(const std::string &wallet_file, const std::string &server, const std::string &user, const std::string &password)
    {
        Connect(fc::path_from_utf8(wallet_file), { server, user, password });
    }

    // wallet file
    bool is_new() { return exec(&wa::wallet_api::is_new).wait(); }
    bool is_locked() { return exec(&wa::wallet_api::is_locked).wait(); }
    bool lock() { return exec(&wa::wallet_api::lock).wait(); }
    bool unlock(const std::string &password) { return exec(&wa::wallet_api::unlock, password).wait(); }
    void set_password(const std::string &password) { exec(&wa::wallet_api::set_password, password).wait(); }
    void save(const std::string &wallet_file) { exec(&wa::wallet_api::save_wallet_file, fc::path_from_utf8(wallet_file)).wait(); }
    bool load(const std::string &wallet_file) { return exec(&wa::wallet_api::load_wallet_file, fc::path_from_utf8(wallet_file)).wait(); }
    std::string get_filename() { return fc::path_to_utf8(exec(&wa::wallet_api::get_wallet_filename).wait()); }
    bool import_key(const std::string &account, const std::string &key) { return exec(&wa::wallet_api::import_key, account, key).wait(); }
    bool import_single_key(const std::string &account, const std::string &key) { return exec(&wa::wallet_api::import_single_key, account, key).wait(); }
    std::string get_private_key(const graphene::chain::public_key_type &pubkey) { return exec(&wa::wallet_api::get_private_key, pubkey).wait(); }
    std::string dump_private_keys() { return object_repr(exec(&wa::wallet_api::dump_private_keys).wait()); }
    bp::list list_my_accounts() { return to_list(exec(&wa::wallet_api::list_my_accounts).wait()); }

    // general
    wa::wallet_about about() { return exec(&wa::wallet_api::about).wait(); }
    wa::wallet_info info() { return exec(&wa::wallet_api::info).wait(); }
    graphene::chain::global_property_object get_global_properties() { return exec(&wa::wallet_api::get_global_properties).wait(); }
    graphene::chain::dynamic_global_property_object get_dynamic_global_properties() { return exec(&wa::wallet_api::get_dynamic_global_properties).wait(); }
    bp::object get_block(uint32_t num) { return optional_value(exec(&wa::wallet_api::get_block, num).wait()); }
    fc::time_point_sec head_block_time() { return exec(&wa::wallet_api::head_block_time).wait(); }

    // account
    uint64_t get_account_count() { return exec(&wa::wallet_api::get_account_count).wait(); }
    bp::dict list_accounts(const std::string& lowerbound, uint32_t limit) { return to_dict(exec(&wa::wallet_api::list_accounts, lowerbound, limit).wait()); }
    bp::list search_accounts(const std::string& term, const std::string& order, const std::string& id, uint32_t limit) { return to_list(exec(&wa::wallet_api::search_accounts, term, order, id, limit).wait()); }
    bp::list list_account_balances(const std::string& account) { return to_list(exec(&wa::wallet_api::list_account_balances, account).wait()); }
    graphene::chain::account_object get_account(const std::string& account) { return exec(&wa::wallet_api::get_account, account).wait(); }
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
    dcore::register_chain();

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

    bp::class_<decent::about_info>("About", bp::no_init)
        .def("__repr__", dcore::object_repr<decent::about_info>)
        .def_readonly("version", &decent::about_info::version)
        .def_readonly("graphene_revision", &decent::about_info::graphene_revision)
        .def_readonly("graphene_revision_age", &decent::about_info::graphene_revision_age)
        .def_readonly("fc_revision", &decent::about_info::fc_revision)
        .def_readonly("fc_revision_age", &decent::about_info::fc_revision_age)
        .def_readonly("compile_date", &decent::about_info::compile_date)
        .def_readonly("boost_version", &decent::about_info::boost_version)
        .def_readonly("openssl_version", &decent::about_info::openssl_version)
        .def_readonly("cryptopp_version", &decent::about_info::cryptopp_version)
        .def_readonly("build", &decent::about_info::build)
    ;

    bp::class_<wa::wallet_about>("AboutFull", bp::no_init)
        .def("__repr__", dcore::object_repr<wa::wallet_about>)
        .def_readonly("daemon", &wa::wallet_about::daemon_info)
        .def_readonly("wallet", &wa::wallet_about::wallet_info)
    ;

    bp::class_<wa::wallet_info>("Info", bp::no_init)
        .def("__repr__", dcore::object_repr<wa::wallet_info>)
        .def_readonly("head_block_num", &wa::wallet_info::head_block_num)
        .def_readonly("head_block_id", &wa::wallet_info::head_block_id)
        .def_readonly("head_block_age", &wa::wallet_info::head_block_age)
        .def_readonly("next_maintenance_time", &wa::wallet_info::next_maintenance_time)
        .def_readonly("chain_id", &wa::wallet_info::chain_id)
        .def_readonly("participation", &wa::wallet_info::participation)
        .add_property("active_miners", dcore::encode_list<wa::wallet_info, std::vector<graphene::chain::miner_id_type>, &wa::wallet_info::active_miners>)
    ;

    bp::class_<wa::extended_asset, bp::bases<graphene::chain::asset>>("BalanceEx", bp::no_init)
        .def("__repr__", dcore::object_repr<wa::extended_asset>)
        .def_readonly("pretty_amount", &wa::extended_asset::pretty_amount)
    ;

    bp::class_<wa::signed_transaction_info, bp::bases<graphene::chain::signed_transaction>>("SignedTransactionEx", bp::no_init)
        .def("__repr__", dcore::object_repr<wa::signed_transaction_info>)
        .def_readonly("transaction_id", &wa::signed_transaction_info::transaction_id)
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
        .def("save", &dcore::Wallet::save, (bp::arg("wallet_file") = ""))
        .def("load", &dcore::Wallet::load, (bp::arg("wallet_file") = ""))
        .def("import_key", &dcore::Wallet::import_key, (bp::arg("account"), bp::arg("key")))
        .def("import_single_key", &dcore::Wallet::import_single_key, (bp::arg("account"), bp::arg("key")))
        .def("get_private_key", &dcore::Wallet::get_private_key, (bp::arg("pubkey")))
        .def("dump_private_keys", &dcore::Wallet::dump_private_keys)
        .def("about", &dcore::Wallet::about)
        .def("list_my_accounts", &dcore::Wallet::list_my_accounts)
        .def("info", &dcore::Wallet::info)
        .def("get_global_properties", &dcore::Wallet::get_global_properties)
        .def("get_dynamic_global_properties", &dcore::Wallet::get_dynamic_global_properties)
        .def("get_block", &dcore::Wallet::get_block, (bp::arg("num")))
        .def("head_block_time", &dcore::Wallet::head_block_time)
        .def("get_account_count", &dcore::Wallet::get_account_count)
        .def("list_accounts", &dcore::Wallet::list_accounts, (bp::arg("lowerbound"), bp::arg("limit")))
        .def("search_accounts", &dcore::Wallet::search_accounts, (bp::arg("term"), bp::arg("order"), bp::arg("id"), bp::arg("limit")))
        .def("list_account_balances", &dcore::Wallet::list_account_balances, (bp::arg("account")))
        .def("get_account", &dcore::Wallet::get_account, (bp::arg("account")))
    ;
}
