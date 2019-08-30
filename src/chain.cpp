#include "module.hpp"
#include <graphene/chain/global_property_object.hpp>

namespace dcore {

template<typename T, typename V, const fc::smart_ref<V> T::* instance>
V decode_smart_ref(const T& obj)
{
    return *(obj.*instance);
}

template<typename T, typename V, fc::smart_ref<V> T::* instance>
void encode_smart_ref(T& obj, V v)
{
    (obj.*instance) = std::move(v);
}

void register_chain()
{
    bp::class_<graphene::chain::fee_parameters>("FeeParameters", bp::no_init)
        .def("__repr__", object_repr<graphene::chain::fee_parameters>)
    ;

    bp::class_<graphene::chain::fee_schedule>("FeeSchedule", bp::init<>())
        .def("__repr__", object_repr<graphene::chain::fee_schedule>)
        .add_property("parameters",
            encode_set<graphene::chain::fee_schedule, boost::container::flat_set<graphene::chain::fee_parameters>, &graphene::chain::fee_schedule::parameters>,
            decode_set<graphene::chain::fee_schedule, boost::container::flat_set<graphene::chain::fee_parameters>, &graphene::chain::fee_schedule::parameters>)
        .def_readwrite("scale", &graphene::chain::fee_schedule::scale)
    ;

    bp::class_<graphene::chain::chain_parameters>("ChainParameters", bp::init<>())
        .def("__repr__", object_repr<graphene::chain::chain_parameters>)
        .add_property("current_fees",
            decode_smart_ref<graphene::chain::chain_parameters, graphene::chain::fee_schedule, &graphene::chain::chain_parameters::current_fees>,
            encode_smart_ref<graphene::chain::chain_parameters, graphene::chain::fee_schedule, &graphene::chain::chain_parameters::current_fees>)
        .def_readwrite("block_interval", &graphene::chain::chain_parameters::block_interval)
        .def_readwrite("maintenance_interval", &graphene::chain::chain_parameters::maintenance_interval)
        .def_readwrite("maintenance_skip_slots", &graphene::chain::chain_parameters::maintenance_skip_slots)
        .def_readwrite("miner_proposal_review_period", &graphene::chain::chain_parameters::miner_proposal_review_period)
        .def_readwrite("maximum_transaction_size", &graphene::chain::chain_parameters::maximum_transaction_size)
        .def_readwrite("maximum_block_size", &graphene::chain::chain_parameters::maximum_block_size)
        .def_readwrite("maximum_time_until_expiration", &graphene::chain::chain_parameters::maximum_time_until_expiration)
        .def_readwrite("maximum_proposal_lifetime", &graphene::chain::chain_parameters::maximum_proposal_lifetime)
        .def_readwrite("maximum_asset_feed_publishers", &graphene::chain::chain_parameters::maximum_asset_feed_publishers)
        .def_readwrite("maximum_miner_count", &graphene::chain::chain_parameters::maximum_miner_count)
        .def_readwrite("maximum_authority_membership", &graphene::chain::chain_parameters::maximum_authority_membership)
        .def_readwrite("cashback_vesting_period_seconds", &graphene::chain::chain_parameters::cashback_vesting_period_seconds)
        .add_property("cashback_vesting_threshold",
            decode_safe_type<graphene::chain::chain_parameters, int64_t, &graphene::chain::chain_parameters::cashback_vesting_threshold>,
            encode_safe_type<graphene::chain::chain_parameters, int64_t, &graphene::chain::chain_parameters::cashback_vesting_threshold>)
        .def_readwrite("miner_pay_vesting_seconds", &graphene::chain::chain_parameters::miner_pay_vesting_seconds)
        .def_readwrite("max_predicate_opcode", &graphene::chain::chain_parameters::max_predicate_opcode)
        .def_readwrite("max_authority_depth", &graphene::chain::chain_parameters::max_authority_depth)
    ;

    bp::class_<object_wrapper<graphene::chain::global_property_object>, std::shared_ptr<graphene::chain::global_property_object>> gp("GlobalProperties", bp::no_init);
    object_wrapper<graphene::chain::global_property_object>::wrap(gp)
        .def_readwrite("parameters", &graphene::chain::global_property_object::parameters)
        .add_property("pending_parameters",
            decode_optional_type<graphene::chain::global_property_object, graphene::chain::chain_parameters, &graphene::chain::global_property_object::pending_parameters>,
            encode_optional_type<graphene::chain::global_property_object, graphene::chain::chain_parameters, &graphene::chain::global_property_object::pending_parameters>)
        .def_readwrite("next_available_vote_id", &graphene::chain::global_property_object::next_available_vote_id)
        .add_property("active_miners",
            encode_list<graphene::chain::global_property_object, std::vector<graphene::chain::miner_id_type>, &graphene::chain::global_property_object::active_miners>,
            decode_list<graphene::chain::global_property_object, std::vector<graphene::chain::miner_id_type>, &graphene::chain::global_property_object::active_miners>)
    ;

    bp::class_<object_wrapper<graphene::chain::dynamic_global_property_object>, std::shared_ptr<graphene::chain::dynamic_global_property_object>> dgp("DynamicGlobalProperties", bp::no_init);
    object_wrapper<graphene::chain::dynamic_global_property_object>::wrap(dgp)
        .def_readwrite("head_block_number", &graphene::chain::dynamic_global_property_object::head_block_number)
        .def_readwrite("head_block_id", &graphene::chain::dynamic_global_property_object::head_block_id)
        .def_readwrite("time", &graphene::chain::dynamic_global_property_object::time)
        .def_readwrite("current_miner", &graphene::chain::dynamic_global_property_object::current_miner)
        .def_readwrite("next_maintenance_time", &graphene::chain::dynamic_global_property_object::next_maintenance_time)
        .def_readwrite("last_budget_time", &graphene::chain::dynamic_global_property_object::last_budget_time)
        .add_property("miner_budget_from_fees",
            decode_safe_type<graphene::chain::dynamic_global_property_object, int64_t, &graphene::chain::dynamic_global_property_object::miner_budget_from_fees>,
            encode_safe_type<graphene::chain::dynamic_global_property_object, int64_t, &graphene::chain::dynamic_global_property_object::miner_budget_from_fees>)
        .add_property("unspent_fee_budget",
            decode_safe_type<graphene::chain::dynamic_global_property_object, int64_t, &graphene::chain::dynamic_global_property_object::unspent_fee_budget>,
            encode_safe_type<graphene::chain::dynamic_global_property_object, int64_t, &graphene::chain::dynamic_global_property_object::unspent_fee_budget>)
        .add_property("mined_rewards",
            decode_safe_type<graphene::chain::dynamic_global_property_object, int64_t, &graphene::chain::dynamic_global_property_object::mined_rewards>,
            encode_safe_type<graphene::chain::dynamic_global_property_object, int64_t, &graphene::chain::dynamic_global_property_object::mined_rewards>)
        .add_property("miner_budget_from_rewards",
            decode_safe_type<graphene::chain::dynamic_global_property_object, int64_t, &graphene::chain::dynamic_global_property_object::miner_budget_from_rewards>,
            encode_safe_type<graphene::chain::dynamic_global_property_object, int64_t, &graphene::chain::dynamic_global_property_object::miner_budget_from_rewards>)
        .def_readwrite("accounts_registered_this_interval", &graphene::chain::dynamic_global_property_object::accounts_registered_this_interval)
        .def_readwrite("recently_missed_count", &graphene::chain::dynamic_global_property_object::recently_missed_count)
        .def_readwrite("current_aslot", &graphene::chain::dynamic_global_property_object::current_aslot)
        .def_readwrite("recent_slots_filled", &graphene::chain::dynamic_global_property_object::recent_slots_filled)
        .def_readwrite("dynamic_flags", &graphene::chain::dynamic_global_property_object::dynamic_flags)
        .def_readwrite("last_irreversible_block_num", &graphene::chain::dynamic_global_property_object::last_irreversible_block_num)
    ;
}

} // dcore
