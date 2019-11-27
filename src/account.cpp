#include "module.hpp"
#include <graphene/chain/account_object.hpp>

namespace dcore {

void register_account()
{
    bp::class_<graphene::chain::authority>("Authority", bp::init<>())
        .def("__repr__", object_repr<graphene::chain::authority>)
        .def_readwrite("weight_threshold", &graphene::chain::authority::weight_threshold)
        .add_property("account_auths",
            encode_dict<graphene::chain::authority, boost::container::flat_map<graphene::chain::account_id_type, graphene::chain::weight_type>, &graphene::chain::authority::account_auths>,
            decode_dict<graphene::chain::authority, boost::container::flat_map<graphene::chain::account_id_type, graphene::chain::weight_type>, &graphene::chain::authority::account_auths>)
        .add_property("key_auths",
            encode_dict<graphene::chain::authority, boost::container::flat_map<graphene::chain::public_key_type, graphene::chain::weight_type>, &graphene::chain::authority::key_auths>,
            decode_dict<graphene::chain::authority, boost::container::flat_map<graphene::chain::public_key_type, graphene::chain::weight_type>, &graphene::chain::authority::key_auths>)
        .def("null_authority", graphene::chain::authority::null_authority)
        .staticmethod("null_authority")
    ;

    bp::class_<graphene::chain::account_options>("AccountOptions", bp::init<>())
        .def("__repr__", object_repr<graphene::chain::account_options>)
        .def_readwrite("memo_key", &graphene::chain::account_options::memo_key)
        .def_readwrite("voting_account", &graphene::chain::account_options::voting_account)
        .def_readwrite("num_miner", &graphene::chain::account_options::num_miner)
        .def_readwrite("allow_subscription", &graphene::chain::account_options::allow_subscription)
        .def_readwrite("price_per_subscribe", &graphene::chain::account_options::price_per_subscribe)
        .def_readwrite("subscription_period", &graphene::chain::account_options::subscription_period)
        .add_property("votes",
            encode_set<graphene::chain::account_options, boost::container::flat_set<graphene::chain::vote_id_type>, &graphene::chain::account_options::votes>,
            decode_set<graphene::chain::account_options, boost::container::flat_set<graphene::chain::vote_id_type>, &graphene::chain::account_options::votes>)
    ;

    bp::class_<graphene::chain::publishing_rights>("PublishingRights", bp::init<>())
        .def("__repr__", object_repr<graphene::chain::publishing_rights>)
        .def_readwrite("is_publishing_manager", &graphene::chain::publishing_rights::is_publishing_manager)
        .add_property("publishing_rights_received",
            encode_set<graphene::chain::publishing_rights, std::set<graphene::chain::account_id_type>, &graphene::chain::publishing_rights::publishing_rights_received>,
            decode_set<graphene::chain::publishing_rights, std::set<graphene::chain::account_id_type>, &graphene::chain::publishing_rights::publishing_rights_received>)
        .add_property("publishing_rights_forwarded",
            encode_set<graphene::chain::publishing_rights, std::set<graphene::chain::account_id_type>, &graphene::chain::publishing_rights::publishing_rights_forwarded>,
            decode_set<graphene::chain::publishing_rights, std::set<graphene::chain::account_id_type>, &graphene::chain::publishing_rights::publishing_rights_forwarded>)
    ;

    bp::class_<object_wrapper<graphene::chain::account_object>, std::shared_ptr<graphene::chain::account_object>> account("Account", bp::no_init);
    object_wrapper<graphene::chain::account_object>::wrap(account)
        .def_readwrite("name", &graphene::chain::account_object::name)
        .def_readwrite("registrar", &graphene::chain::account_object::registrar)
        .def_readwrite("owner", &graphene::chain::account_object::owner)
        .def_readwrite("active", &graphene::chain::account_object::active)
        .def_readwrite("options", &graphene::chain::account_object::options)
        .def_readwrite("rights_to_publish", &graphene::chain::account_object::rights_to_publish)
        .def_readwrite("statistics", &graphene::chain::account_object::statistics)
        .def_readwrite("top_n_control_flags", &graphene::chain::account_object::top_n_control_flags)
    ;

    bp::class_<object_wrapper<graphene::chain::account_balance_object>, std::shared_ptr<graphene::chain::account_balance_object>> balance("AccountBalance", bp::no_init);
    object_wrapper<graphene::chain::account_balance_object>::wrap(balance)
        .def_readwrite("owner", &graphene::chain::account_balance_object::owner)
        .def_readwrite("asset_type", &graphene::chain::account_balance_object::asset_type)
        .add_property("balance",
            decode_safe_type<graphene::chain::account_balance_object, int64_t, &graphene::chain::account_balance_object::balance>,
            encode_safe_type<graphene::chain::account_balance_object, int64_t, &graphene::chain::account_balance_object::balance>)
    ;

    bp::class_<object_wrapper<graphene::chain::account_statistics_object>, std::shared_ptr<graphene::chain::account_statistics_object>> stats("AccountStats", bp::no_init);
    object_wrapper<graphene::chain::account_statistics_object>::wrap(stats)
        .def_readwrite("owner", &graphene::chain::account_statistics_object::owner)
        .def_readwrite("most_recent_op", &graphene::chain::account_statistics_object::most_recent_op)
        .def_readwrite("total_ops", &graphene::chain::account_statistics_object::total_ops)
        .add_property("total_core_in_orders",
            decode_safe_type<graphene::chain::account_statistics_object, int64_t, &graphene::chain::account_statistics_object::total_core_in_orders>,
            encode_safe_type<graphene::chain::account_statistics_object, int64_t, &graphene::chain::account_statistics_object::total_core_in_orders>)
        .add_property("pending_fees",
            decode_safe_type<graphene::chain::account_statistics_object, int64_t, &graphene::chain::account_statistics_object::pending_fees>,
            encode_safe_type<graphene::chain::account_statistics_object, int64_t, &graphene::chain::account_statistics_object::pending_fees>)
        .add_property("pending_vested_fees",
            decode_safe_type<graphene::chain::account_statistics_object, int64_t, &graphene::chain::account_statistics_object::pending_vested_fees>,
            encode_safe_type<graphene::chain::account_statistics_object, int64_t, &graphene::chain::account_statistics_object::pending_vested_fees>)
    ;
}

} // dcore
