#include "module.hpp"
#include <graphene/wallet/wallet_utility.hpp>
#include <graphene/wallet/wallet.hpp>
#include <graphene/utilities/dirhelper.hpp>
#include <fc/log/logger_config.hpp>

namespace wa = graphene::wallet;
namespace ch = graphene::chain;

namespace dcore {

template<typename T>
std::vector<T> vector_from_list(const bp::list &l)
{
    std::vector<T> obj;
    auto len = bp::len(l);
    obj.resize(len);
    while(len--) {
        bp::extract<T> v(l[len]);
        obj[len] = v();
    }
    return obj;
}

template<typename T>
std::set<T> set_from_list(const bp::list &l)
{
    std::set<T> obj;
    auto len = bp::len(l);
    while(len--) {
        bp::extract<T> v(l[len]);
        obj.insert(v());
    }
    return obj;
}

template<typename T>
bp::list to_list(const T &container)
{
    bp::list l;
    for(const auto& v : container)
        l.append(v);
    return l;
}

template<typename T>
bp::list to_optional_list(const std::vector<fc::optional<T>> &container)
{
    bp::list l;
    for(const auto& v : container)
        v.valid() ? l.append(*v) : l.append(bp::object());
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

template<typename V>
V to_safe_value(const fc::safe<V>& v)
{
   return v.value;
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
    std::string get_private_key(const ch::public_key_type &pubkey) { return exec(&wa::wallet_api::get_private_key, pubkey).wait(); }
    std::string dump_private_keys() { return object_repr(exec(&wa::wallet_api::dump_private_keys).wait()); }
    bp::list list_my_accounts() { return to_list(exec(&wa::wallet_api::list_my_accounts).wait()); }

    // general
    decent::about_info about() { return query(&wa::db_api::about).wait(); }
    wa::wallet_info info() { return exec(&wa::wallet_api::info).wait(); }
    ch::configuration get_configuration() { return query(&wa::db_api::get_configuration).wait(); }
    ch::chain_property_object get_chain_properties() { return query(&wa::db_api::get_chain_properties).wait(); }
    ch::global_property_object get_global_properties() { return query(&wa::db_api::get_global_properties).wait(); }
    ch::dynamic_global_property_object get_dynamic_global_properties() { return query(&wa::db_api::get_dynamic_global_properties).wait(); }
    bp::object get_block(uint32_t num) { return encode_optional_value(query(&wa::db_api::get_block, num).wait()); }
    fc::time_point_sec head_block_time() { return query(&wa::db_api::head_block_time).wait(); }
    ch::real_supply get_real_supply() { return query(&wa::db_api::get_real_supply).wait(); }
    uint64_t get_new_asset_per_block() { return to_safe_value(query(&wa::db_api::get_new_asset_per_block).wait()); }
    uint64_t get_new_asset_by_block(uint32_t block_num) { return to_safe_value(query(&wa::db_api::get_asset_per_block_by_block_num, block_num).wait()); }
    uint64_t get_miner_pay(fc::time_point_sec block_time) { return to_safe_value(query(&wa::db_api::get_miner_pay_from_fees_by_block_time, block_time).wait()); }
    bp::list get_actual_votes() { return to_list(query(&wa::db_api::get_actual_votes).wait()); }

    // account
    uint64_t get_account_count() { return query(&wa::db_api::get_account_count).wait(); }
    bp::object get_account(const std::string& name) { return encode_optional_value(query(&wa::db_api::get_account_by_name, name).wait()); }
    bp::dict lookup_accounts(const std::string& lowerbound, uint32_t limit) { return to_dict(query(&wa::db_api::lookup_accounts, lowerbound, limit).wait()); }
    bp::list search_accounts(const std::string& term, const std::string& order, graphene::db::object_id_type id, uint32_t limit) { return to_list(query(&wa::db_api::search_accounts, term, order, id, limit).wait()); }
    bp::list get_accounts(const bp::list& ids) { return to_optional_list(query(&wa::db_api::get_accounts, vector_from_list<ch::account_id_type>(ids)).wait()); }
    bp::list list_account_balances(const std::string& account) { return to_list(exec(&wa::wallet_api::list_account_balances, account).wait()); }
    ch::signed_transaction create_account(const std::string &brainkey, const std::string &name, const std::string &registrar, bool broadcast)
        { return exec(&wa::wallet_api::create_account_with_brain_key, brainkey, name, registrar, broadcast).wait(); }
    ch::signed_transaction register_account(const std::string &name, const ch::public_key_type &owner, const ch::public_key_type &active, const ch::public_key_type &memo,
        const std::string &registrar, bool broadcast) { return exec(&wa::wallet_api::register_account_with_keys, name, owner, active, memo, registrar, broadcast).wait(); }
    ch::signed_transaction register_multisig_account(const std::string &name, const ch::authority &owner, const ch::authority &active, const ch::public_key_type &memo,
        const std::string &registrar, bool broadcast) { return exec(&wa::wallet_api::register_multisig_account, name, owner, active, memo, registrar, broadcast).wait(); }
    ch::signed_transaction update_account(const std::string &name, const ch::public_key_type &owner, const ch::public_key_type &active, const ch::public_key_type &memo,
        bool broadcast) { return exec(&wa::wallet_api::update_account_keys, name, static_cast<std::string>(owner), static_cast<std::string>(active), static_cast<std::string>(memo), broadcast).wait(); }
    ch::signed_transaction update_multisig_account(const std::string &name, const ch::authority &owner, const ch::authority &active, const ch::public_key_type &memo,
        bool broadcast) { return exec(&wa::wallet_api::update_account_keys_to_multisig, name, owner, active, memo, broadcast).wait(); }
    ch::signed_transaction transfer(const std::string &from, const std::string &to, double amount, const std::string &symbol, const std::string &memo, bool broadcast)
        { return exec(&wa::wallet_api::transfer, from, to, fc::to_string(amount), symbol, memo, broadcast).wait(); }

    // asset
    bp::list list_assets(const std::string& lowerbound, uint32_t limit) { return to_list(query(&wa::db_api::list_assets, lowerbound, limit).wait()); }
    bp::list get_assets(const bp::list& ids) { return to_optional_list(query(&wa::db_api::get_assets, vector_from_list<ch::asset_id_type>(ids)).wait()); }
    ch::signed_transaction create_monitored_asset(const std::string &issuer, const std::string &symbol, uint8_t precision, const std::string &description, uint32_t feed_lifetime_sec, uint8_t minimum_feeds,
       bool broadcast) { return exec(&wa::wallet_api::create_monitored_asset, issuer, symbol, precision, description, feed_lifetime_sec, minimum_feeds, broadcast).wait(); }
    ch::signed_transaction update_monitored_asset(const std::string &symbol, const std::string &description, uint32_t feed_lifetime_sec, uint8_t minimum_feeds, bool broadcast)
       { return exec(&wa::wallet_api::update_monitored_asset, symbol, description, feed_lifetime_sec, minimum_feeds, broadcast).wait(); }
    ch::signed_transaction create_user_issued_asset(const std::string& issuer, const std::string& symbol, uint8_t precision, const std::string& description, uint64_t max_supply, const ch::price& core_exchange_rate,
       bool exchangeable, bool fixed_max_supply, bool broadcast) { return exec(&wa::wallet_api::create_user_issued_asset, issuer, symbol, precision, description, max_supply, core_exchange_rate, exchangeable, fixed_max_supply, broadcast).wait(); }
    ch::signed_transaction update_user_issued_asset(const std::string& symbol, const std::string& issuer, const std::string& description, uint64_t max_supply, const ch::price& core_exchange_rate,
       bool exchangeable, bool broadcast) { return exec(&wa::wallet_api::update_user_issued_asset, symbol, issuer, description, max_supply, core_exchange_rate, exchangeable, broadcast).wait(); }
    ch::signed_transaction issue_asset(const std::string& account, double amount, const std::string& symbol, const std::string& memo, bool broadcast)
       { return exec(&wa::wallet_api::issue_asset, account, fc::to_string(amount), symbol, memo, broadcast).wait(); }
    ch::signed_transaction fund_asset_pools(const std::string& account, double uia_amount, const std::string& uia_symbol, double dct_amount, const std::string& dct_symbol, bool broadcast)
       { return exec(&wa::wallet_api::fund_asset_pools, account, fc::to_string(uia_amount), uia_symbol, fc::to_string(dct_amount), dct_symbol, broadcast).wait(); }
    ch::signed_transaction reserve_asset(const std::string& account, double amount, const std::string& symbol, bool broadcast)
       { return exec(&wa::wallet_api::reserve_asset, account, fc::to_string(amount), symbol, broadcast).wait(); }
    ch::signed_transaction claim_fees(double uia_amount, const std::string& uia_symbol, double dct_amount, const std::string& dct_symbol, bool broadcast)
       { return exec(&wa::wallet_api::claim_fees, fc::to_string(uia_amount), uia_symbol, fc::to_string(dct_amount), dct_symbol, broadcast).wait(); }
    ch::signed_transaction publish_asset_feed(const std::string& account, const std::string& symbol, const ch::price_feed& feed, bool broadcast)
       { return exec(&wa::wallet_api::publish_asset_feed, account, symbol, feed, broadcast).wait(); }

    // miner
    uint64_t get_miner_count() { return query(&wa::db_api::get_miner_count).wait(); }
    bp::dict list_miners(const std::string& lowerbound, uint32_t limit) { return to_dict(query(&wa::db_api::lookup_miner_accounts, lowerbound, limit).wait()); }
    bp::list get_miners(const bp::list& ids) { return to_optional_list(query(&wa::db_api::get_miners, vector_from_list<ch::miner_id_type>(ids)).wait()); }
    bp::object get_miner_by_account(ch::account_id_type id) { return encode_optional_value(query(&wa::db_api::get_miner_by_account, id).wait()); }
    ch::signed_transaction create_miner(const std::string &account, const std::string &url, bool broadcast)
        { return exec(&wa::wallet_api::create_miner, account, url, broadcast).wait(); }
    ch::signed_transaction update_miner(const std::string &miner, const std::string &url, const ch::public_key_type &signing_key, bool broadcast)
        { return exec(&wa::wallet_api::update_miner, miner, url, static_cast<std::string>(signing_key), broadcast).wait(); }

    // non fungible token
    bp::list list_non_fungible_tokens(const std::string& lowerbound, uint32_t limit) { return to_list(query(&wa::db_api::list_non_fungible_tokens, lowerbound, limit).wait()); }
    bp::list get_non_fungible_tokens(const bp::list& ids) { return to_optional_list(query(&wa::db_api::get_non_fungible_tokens, vector_from_list<ch::non_fungible_token_id_type>(ids)).wait()); }
    bp::list list_non_fungible_token_data(const ch::non_fungible_token_id_type& nft) { return to_list(query(&wa::db_api::list_non_fungible_token_data, nft).wait()); }
    bp::dict get_non_fungible_token_summary(ch::account_id_type account) { return to_dict(query(&wa::db_api::get_non_fungible_token_summary, account).wait()); }
    bp::list get_non_fungible_token_balances(const std::string& account, const bp::list& nfts) { return to_list(exec(&wa::wallet_api::get_non_fungible_token_balances, account, set_from_list<std::string>(nfts)).wait()); }
    ch::signed_transaction create_non_fungible_token(const std::string& issuer, const std::string& symbol, const std::string& description, const bp::list& definitions, uint32_t max_supply, bool fixed_max_supply, bool transferable,
        bool broadcast) { return exec(&wa::wallet_api::create_non_fungible_token, issuer, symbol, description, vector_from_list<ch::non_fungible_token_data_type>(definitions), max_supply, fixed_max_supply, transferable, broadcast).wait(); }
    ch::signed_transaction update_non_fungible_token(const std::string& issuer, const std::string& symbol, const std::string& description, uint32_t max_supply, bool fixed_max_supply, bool broadcast)
        { return exec(&wa::wallet_api::update_non_fungible_token, issuer, symbol, description, max_supply, fixed_max_supply, broadcast).wait(); }
    ch::signed_transaction issue_non_fungible_token(const std::string& account, const std::string& symbol, const bp::list& data, const std::string& memo, bool broadcast)
        { return exec(&wa::wallet_api::issue_non_fungible_token, account, symbol, vector_from_list<fc::variant>(data), memo, broadcast).wait(); }
    ch::signed_transaction transfer_non_fungible_token_data(const std::string& account, const ch::non_fungible_token_data_id_type& nft_data_id, const std::string& memo, bool broadcast)
        { return exec(&wa::wallet_api::transfer_non_fungible_token_data, account, nft_data_id, memo, broadcast).wait(); }
    ch::signed_transaction burn_non_fungible_token_data(const ch::non_fungible_token_data_id_type& nft_data_id, bool broadcast)
        { return exec(&wa::wallet_api::burn_non_fungible_token_data, nft_data_id, broadcast).wait(); }
    ch::signed_transaction update_non_fungible_token_data(const std::string& modifier, const ch::non_fungible_token_data_id_type& nft_data_id, const bp::list& data, bool broadcast)
        { return exec(&wa::wallet_api::update_non_fungible_token_data, modifier, nft_data_id, vector_from_list<std::pair<std::string, fc::variant>>(data), broadcast).wait(); }

    // network broadcast
    void broadcast_transaction(const ch::signed_transaction& trx) { broadcast(&wa::net_api::broadcast_transaction, trx).wait(); }
    void broadcast_block(const ch::signed_block& block) { broadcast(&wa::net_api::broadcast_block, block).wait(); }
};

} // dcore

#if defined(__GNUC__) && __GNUC__ <= 7 && __GNUC_MINOR__ <= 4
#pragma GCC visibility push(default)
#endif

BOOST_PYTHON_MODULE(dcore)
{
    bp::docstring_options doc_options(true, true, false);

    fc::configure_logging(fc::logging_config());
    bp::def("configure_logging", dcore::configure_logging);
    bp::def("default_logging", dcore::default_logging);
    bp::def("about", decent::get_about_info);

    dcore::register_common_types();
    dcore::register_account();
    dcore::register_asset();
    dcore::register_chain();
    dcore::register_miner();
    dcore::register_nft();
    dcore::register_operation();

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
        .def_readonly("boost_version", &decent::about_info::boost_version)
        .def_readonly("openssl_version", &decent::about_info::openssl_version)
        .def_readonly("cryptopp_version", &decent::about_info::cryptopp_version)
        .def_readonly("build", &decent::about_info::build)
    ;

    bp::class_<wa::wallet_info>("Info", bp::no_init)
        .def("__repr__", dcore::object_repr<wa::wallet_info>)
        .def_readonly("head_block_num", &wa::wallet_info::head_block_num)
        .def_readonly("head_block_id", &wa::wallet_info::head_block_id)
        .def_readonly("head_block_age", &wa::wallet_info::head_block_age)
        .def_readonly("next_maintenance_time", &wa::wallet_info::next_maintenance_time)
        .def_readonly("chain_id", &wa::wallet_info::chain_id)
        .def_readonly("participation", &wa::wallet_info::participation)
        .add_property("active_miners", dcore::encode_list<wa::wallet_info, std::vector<ch::miner_id_type>, &wa::wallet_info::active_miners>)
    ;

    bp::class_<ch::configuration>("Configuration", bp::no_init)
        .def("__repr__", dcore::object_repr<ch::configuration>)
        .def_readonly("graphene_symbol", &ch::configuration::graphene_symbol)
        .def_readonly("graphene_min_account_name_length", &ch::configuration::graphene_min_account_name_length)
        .def_readonly("graphene_max_account_name_length", &ch::configuration::graphene_max_account_name_length)
        .def_readonly("graphene_min_asset_symbol_length", &ch::configuration::graphene_min_asset_symbol_length)
        .def_readonly("graphene_max_asset_symbol_length", &ch::configuration::graphene_max_asset_symbol_length)
        .def_readonly("graphene_max_share_supply", &ch::configuration::graphene_max_share_supply)
        .def_readonly("graphene_max_sig_check_depth", &ch::configuration::graphene_max_sig_check_depth)
        .def_readonly("graphene_min_transaction_size_limit", &ch::configuration::graphene_min_transaction_size_limit)
        .def_readonly("graphene_min_block_interval", &ch::configuration::graphene_min_block_interval)
        .def_readonly("graphene_max_block_interval", &ch::configuration::graphene_max_block_interval)
        .def_readonly("graphene_default_block_interval", &ch::configuration::graphene_default_block_interval)
        .def_readonly("graphene_default_max_transaction_size", &ch::configuration::graphene_default_max_transaction_size)
        .def_readonly("graphene_default_max_block_size", &ch::configuration::graphene_default_max_block_size)
        .def_readonly("graphene_default_max_time_until_expiration", &ch::configuration::graphene_default_max_time_until_expiration)
        .def_readonly("graphene_default_maintenance_interval", &ch::configuration::graphene_default_maintenance_interval)
        .def_readonly("graphene_default_maintenance_skip_slots", &ch::configuration::graphene_default_maintenance_skip_slots)
        .def_readonly("graphene_min_undo_history", &ch::configuration::graphene_min_undo_history)
        .def_readonly("graphene_max_undo_history", &ch::configuration::graphene_max_undo_history)
        .def_readonly("graphene_min_block_size_limit", &ch::configuration::graphene_min_block_size_limit)
        .def_readonly("graphene_blockchain_precision", &ch::configuration::graphene_blockchain_precision)
        .def_readonly("graphene_blockchain_precision_digits", &ch::configuration::graphene_blockchain_precision_digits)
        .def_readonly("graphene_max_instance_id", &ch::configuration::graphene_max_instance_id)
        .def_readonly("graphene_100_percent", &ch::configuration::graphene_100_percent)
        .def_readonly("graphene_1_percent", &ch::configuration::graphene_1_percent)
        .def_readonly("graphene_default_price_feed_lifetime", &ch::configuration::graphene_default_price_feed_lifetime)
        .def_readonly("graphene_default_max_authority_membership", &ch::configuration::graphene_default_max_authority_membership)
        .def_readonly("graphene_default_max_asset_feed_publishers", &ch::configuration::graphene_default_max_asset_feed_publishers)
        .def_readonly("graphene_default_max_miners", &ch::configuration::graphene_default_max_miners)
        .def_readonly("graphene_default_max_proposal_lifetime_sec", &ch::configuration::graphene_default_max_proposal_lifetime_sec)
        .def_readonly("graphene_default_miner_proposal_review_period_sec", &ch::configuration::graphene_default_miner_proposal_review_period_sec)
        .def_readonly("graphene_default_cashback_vesting_period_sec", &ch::configuration::graphene_default_cashback_vesting_period_sec)
        .def_readonly("graphene_default_cashback_vesting_threshold", &ch::configuration::graphene_default_cashback_vesting_threshold)
        .def_readonly("graphene_default_max_assert_opcode", &ch::configuration::graphene_default_max_assert_opcode)
        .def_readonly("graphene_max_url_length", &ch::configuration::graphene_max_url_length)
        .def_readonly("graphene_default_miner_pay_per_block", &ch::configuration::graphene_default_miner_pay_per_block)
        .def_readonly("graphene_default_miner_pay_vesting_seconds", &ch::configuration::graphene_default_miner_pay_vesting_seconds)
        .def_readonly("graphene_miner_account", &ch::configuration::graphene_miner_account)
        .def_readonly("graphene_null_account", &ch::configuration::graphene_null_account)
        .def_readonly("graphene_temp_account", &ch::configuration::graphene_temp_account)
    ;

    bp::class_<wa::extended_asset, bp::bases<ch::asset>>("BalanceEx", bp::no_init)
        .def("__repr__", dcore::object_repr<wa::extended_asset>)
        .def_readonly("pretty_amount", &wa::extended_asset::pretty_amount)
    ;

    bp::class_<dcore::Wallet, boost::noncopyable>("Wallet", bp::init<>())
        .def("__bool__", &dcore::Wallet::is_new)
        .add_property("locked", &dcore::Wallet::is_locked)
        .add_property("connected", &dcore::Wallet::is_connected)
        .add_property("filename", &dcore::Wallet::get_filename)
        .def("connect", &dcore::Wallet::connect, (bp::arg("wallet_file"), bp::arg("server") = "", bp::arg("user") = "", bp::arg("password") = ""))
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
        .def("get_configuration", &dcore::Wallet::get_configuration)
        .def("get_chain_properties", &dcore::Wallet::get_chain_properties)
        .def("get_global_properties", &dcore::Wallet::get_global_properties)
        .def("get_dynamic_global_properties", &dcore::Wallet::get_dynamic_global_properties)
        .def("get_block", &dcore::Wallet::get_block, (bp::arg("num")))
        .def("head_block_time", &dcore::Wallet::head_block_time)
        .def("get_real_supply", &dcore::Wallet::get_real_supply)
        .def("get_new_asset_per_block", &dcore::Wallet::get_new_asset_per_block)
        .def("get_new_asset_by_block", &dcore::Wallet::get_new_asset_by_block, (bp::arg("block_num")))
        .def("get_miner_pay", &dcore::Wallet::get_miner_pay, (bp::arg("block_time")))
        .def("get_actual_votes", &dcore::Wallet::get_actual_votes)
        .def("get_account_count", &dcore::Wallet::get_account_count)
        .def("get_account", &dcore::Wallet::get_account, (bp::arg("name")))
        .def("lookup_accounts", &dcore::Wallet::lookup_accounts, (bp::arg("lowerbound"), bp::arg("limit")))
        .def("search_accounts", &dcore::Wallet::search_accounts, (bp::arg("term"), bp::arg("order"), bp::arg("id"), bp::arg("limit")))
        .def("list_account_balances", &dcore::Wallet::list_account_balances, (bp::arg("account")))
        .def("get_accounts", &dcore::Wallet::get_accounts, (bp::arg("ids")))
        .def("create_account", &dcore::Wallet::create_account,
            (bp::arg("brainkey"), bp::arg("name"), bp::arg("registrar"), bp::arg("broadcast") = false))
        .def("register_account", &dcore::Wallet::register_account,
            (bp::arg("name"), bp::arg("owner"), bp::arg("active"), bp::arg("memo"), bp::arg("registrar"), bp::arg("broadcast") = false))
        .def("register_multisig_account", &dcore::Wallet::register_multisig_account,
            (bp::arg("name"), bp::arg("owner"), bp::arg("active"), bp::arg("memo"), bp::arg("registrar"), bp::arg("broadcast") = false))
        .def("update_account", &dcore::Wallet::update_account,
            (bp::arg("name"), bp::arg("owner"), bp::arg("active"), bp::arg("memo"), bp::arg("broadcast") = false))
        .def("update_multisig_account", &dcore::Wallet::update_multisig_account,
            (bp::arg("name"), bp::arg("owner"), bp::arg("active"), bp::arg("memo"), bp::arg("broadcast") = false))
        .def("transfer", &dcore::Wallet::transfer,
            (bp::arg("from"), bp::arg("to"), bp::arg("amount"), bp::arg("symbol"), bp::arg("memo"), bp::arg("broadcast") = false))
        .def("list_assets", &dcore::Wallet::list_assets, (bp::arg("lowerbound"), bp::arg("limit")))
        .def("get_assets", &dcore::Wallet::get_assets, (bp::arg("ids")))
        .def("create_monitored_asset", &dcore::Wallet::create_monitored_asset,
            (bp::arg("issuer"), bp::arg("symbol"), bp::arg("precision"), bp::arg("description"), bp::arg("feed_lifetime_sec"), bp::arg("minimum_feeds"), bp::arg("broadcast") = false))
        .def("update_monitored_asset", &dcore::Wallet::update_monitored_asset,
            (bp::arg("symbol"), bp::arg("description"), bp::arg("feed_lifetime_sec"), bp::arg("minimum_feeds"), bp::arg("broadcast") = false))
        .def("create_user_issued_asset", &dcore::Wallet::create_user_issued_asset,
            (bp::arg("issuer"), bp::arg("symbol"), bp::arg("precision"), bp::arg("description"), bp::arg("max_supply"), bp::arg("core_exchange_rate"),
             bp::arg("exchangeable"), bp::arg("fixed_max_supply"), bp::arg("broadcast") = false))
        .def("update_user_issued_asset", &dcore::Wallet::update_user_issued_asset,
            (bp::arg("symbol"), bp::arg("issuer"), bp::arg("description"), bp::arg("max_supply"), bp::arg("core_exchange_rate"), bp::arg("exchangeable"), bp::arg("broadcast") = false))
        .def("issue_asset", &dcore::Wallet::issue_asset,
            (bp::arg("account"), bp::arg("amount"), bp::arg("symbol"), bp::arg("memo"), bp::arg("broadcast") = false))
        .def("fund_asset_pools", &dcore::Wallet::fund_asset_pools,
            (bp::arg("account"), bp::arg("uia_amount"), bp::arg("uia_symbol"), bp::arg("dct_amount"), bp::arg("dct_symbol"), bp::arg("broadcast") = false))
        .def("reserve_asset", &dcore::Wallet::reserve_asset,
            (bp::arg("account"), bp::arg("amount"), bp::arg("symbol"), bp::arg("broadcast") = false))
        .def("claim_fees", &dcore::Wallet::claim_fees,
            (bp::arg("uia_amount"), bp::arg("uia_symbol"), bp::arg("dct_amount"), bp::arg("dct_symbol"), bp::arg("broadcast") = false))
        .def("publish_asset_feed", &dcore::Wallet::publish_asset_feed,
            (bp::arg("account"), bp::arg("symbol"), bp::arg("feed"), bp::arg("broadcast") = false))
        .def("get_miner_count", &dcore::Wallet::get_miner_count)
        .def("list_miners", &dcore::Wallet::list_miners, (bp::arg("lowerbound"), bp::arg("limit")))
        .def("get_miners", &dcore::Wallet::get_miners, (bp::arg("ids")))
        .def("get_miner_by_account", &dcore::Wallet::get_miner_by_account, (bp::arg("id")))
        .def("create_miner", &dcore::Wallet::create_miner, (bp::arg("account"), bp::arg("url"), bp::arg("broadcast") = false))
        .def("update_miner", &dcore::Wallet::update_miner, (bp::arg("miner"), bp::arg("url"), bp::arg("signing_key"), bp::arg("broadcast") = false))
        .def("list_non_fungible_tokens", &dcore::Wallet::list_non_fungible_tokens, (bp::arg("lowerbound"), bp::arg("limit")))
        .def("get_non_fungible_tokens", &dcore::Wallet::get_non_fungible_tokens, (bp::arg("ids")))
        .def("list_non_fungible_token_data", &dcore::Wallet::list_non_fungible_token_data, (bp::arg("nft")))
        .def("get_non_fungible_token_summary", &dcore::Wallet::get_non_fungible_token_summary, (bp::arg("account")))
        .def("get_non_fungible_token_balances", &dcore::Wallet::get_non_fungible_token_balances, (bp::arg("account"), (bp::arg("nfts"))))
        .def("create_non_fungible_token", &dcore::Wallet::create_non_fungible_token,
            (bp::arg("issuer"), bp::arg("symbol"), bp::arg("description"), bp::arg("definitions"), bp::arg("max_supply"),
             bp::arg("fixed_max_supply"), bp::arg("transferable"), bp::arg("broadcast") = false))
        .def("update_non_fungible_token", &dcore::Wallet::update_non_fungible_token,
            (bp::arg("issuer"), bp::arg("symbol"), bp::arg("description"), bp::arg("max_supply"), bp::arg("fixed_max_supply"), bp::arg("broadcast") = false))
        .def("issue_non_fungible_token", &dcore::Wallet::issue_non_fungible_token, (bp::arg("account"), bp::arg("symbol"), bp::arg("data"), bp::arg("memo"), bp::arg("broadcast") = false))
        .def("transfer_non_fungible_token_data", &dcore::Wallet::transfer_non_fungible_token_data, (bp::arg("account"), bp::arg("nft_data_id"), bp::arg("memo"), bp::arg("broadcast") = false))
        .def("burn_non_fungible_token_data", &dcore::Wallet::burn_non_fungible_token_data, (bp::arg("nft_data_id"), bp::arg("broadcast") = false))
        .def("update_non_fungible_token_data", &dcore::Wallet::update_non_fungible_token_data, (bp::arg("modifier"), bp::arg("nft_data_id"), bp::arg("data"), bp::arg("broadcast") = false))
        .def("broadcast_transaction", &dcore::Wallet::broadcast_transaction, (bp::arg("trx")))
        .def("broadcast_block", &dcore::Wallet::broadcast_block, (bp::arg("block")))
    ;
}
