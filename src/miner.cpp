#include "module.hpp"
#include <graphene/app/database_api.hpp>

namespace dcore {

void register_miner()
{
    bp::class_<graphene::chain::vote_id_type>("VoteId", bp::init<>())
        .def("__repr__", object_repr<graphene::chain::vote_id_type>)
        .def("__str__", &graphene::chain::vote_id_type::operator std::string)
        .def(bp::init<const std::string&>())
        .add_property("instance", &graphene::chain::vote_id_type::instance, &graphene::chain::vote_id_type::set_instance)
    ;

    bp::class_<graphene::app::miner_voting_info>("VotingInfo", bp::init<>())
        .def("__repr__", object_repr<graphene::app::miner_voting_info>)
        .def_readwrite("id", &graphene::app::miner_voting_info::id)
        .def_readwrite("name", &graphene::app::miner_voting_info::name)
        .def_readwrite("url", &graphene::app::miner_voting_info::url)
        .def_readwrite("total_votes", &graphene::app::miner_voting_info::total_votes)
        .def_readwrite("voted", &graphene::app::miner_voting_info::voted)
    ;

    typedef std::pair<graphene::chain::account_id_type, uint64_t> votes_gained;
    bp::class_<object_wrapper<graphene::chain::miner_object>, std::shared_ptr<graphene::chain::miner_object>> miner("Miner", bp::no_init);
    {
        bp::scope s = miner;
        bp::class_<votes_gained>("VotesGained", bp::init<>())
            .def("__repr__", object_repr<votes_gained>)
            .def_readwrite("account", &votes_gained::first)
            .def_readwrite("votes", &votes_gained::second)
        ;
    }

    object_wrapper<graphene::chain::miner_object>::wrap(miner)
        .def_readwrite("account", &graphene::chain::miner_object::miner_account)
        .def_readwrite("last_aslot", &graphene::chain::miner_object::last_aslot)
        .def_readwrite("signing_key", &graphene::chain::miner_object::signing_key)
        .def_readwrite("vote_id", &graphene::chain::miner_object::vote_id)
        .add_property("pay_vb",
            decode_optional_type<graphene::chain::miner_object, graphene::chain::vesting_balance_id_type, &graphene::chain::miner_object::pay_vb>,
            encode_optional_type<graphene::chain::miner_object, graphene::chain::vesting_balance_id_type, &graphene::chain::miner_object::pay_vb>)
        .def_readwrite("total_votes", &graphene::chain::miner_object::total_votes)
        .def_readwrite("url", &graphene::chain::miner_object::url)
        .def_readwrite("total_missed", &graphene::chain::miner_object::total_missed)
        .def_readwrite("last_confirmed_block_num", &graphene::chain::miner_object::last_confirmed_block_num)
        .def_readwrite("vote_ranking", &graphene::chain::miner_object::vote_ranking)
        .add_property("votes_gained",
            encode_list<graphene::chain::miner_object, std::vector<votes_gained>, &graphene::chain::miner_object::votes_gained>,
            decode_list<graphene::chain::miner_object, std::vector<votes_gained>, &graphene::chain::miner_object::votes_gained>)
    ;

    {
        bp::scope policy = bp::class_<graphene::chain::vesting_policy>("VestingPolicy", bp::no_init)
            .def(bp::init<const graphene::chain::linear_vesting_policy&>())
            .def(bp::init<const graphene::chain::cdd_vesting_policy&>())
            .def("__repr__", object_repr<graphene::chain::vesting_policy>)
            .add_property("linear", decode_static_variant<graphene::chain::vesting_policy, graphene::chain::linear_vesting_policy>)
            .add_property("cdd", decode_static_variant<graphene::chain::vesting_policy, graphene::chain::cdd_vesting_policy>)
        ;

        bp::class_<graphene::chain::linear_vesting_policy>("Linear", bp::init<>())
            .def("__repr__", object_repr<graphene::chain::linear_vesting_policy>)
            .def_readwrite("begin", &graphene::chain::linear_vesting_policy::begin_timestamp)
            .def_readwrite("vesting_cliff_seconds", &graphene::chain::linear_vesting_policy::vesting_cliff_seconds)
            .def_readwrite("vesting_duration_seconds", &graphene::chain::linear_vesting_policy::vesting_duration_seconds)
            .add_property("balance", decode_safe_type<graphene::chain::linear_vesting_policy, int64_t, &graphene::chain::linear_vesting_policy::begin_balance>,
                                     encode_safe_type<graphene::chain::linear_vesting_policy, int64_t, &graphene::chain::linear_vesting_policy::begin_balance>)
        ;

        bp::class_<graphene::chain::cdd_vesting_policy>("Cdd", bp::init<>())
            .def("__repr__", object_repr<graphene::chain::cdd_vesting_policy>)
            .def_readwrite("vesting_seconds", &graphene::chain::cdd_vesting_policy::vesting_seconds)
            .def_readwrite("coin_seconds_earned", &graphene::chain::cdd_vesting_policy::coin_seconds_earned)
            .def_readwrite("start_claim", &graphene::chain::cdd_vesting_policy::start_claim)
            .def_readwrite("coin_seconds_earned_last_update", &graphene::chain::cdd_vesting_policy::coin_seconds_earned_last_update)
        ;
    }

    bp::class_<object_wrapper<graphene::chain::vesting_balance_object>, std::shared_ptr<graphene::chain::vesting_balance_object>> balance("VestingBalance", bp::no_init);
    object_wrapper<graphene::chain::vesting_balance_object>::wrap(balance)
        .def_readwrite("owner", &graphene::chain::vesting_balance_object::owner)
        .def_readwrite("balance", &graphene::chain::vesting_balance_object::balance)
        .def_readwrite("policy", &graphene::chain::vesting_balance_object::policy)
        .def("get_allowed_withdraw", &graphene::chain::vesting_balance_object::get_allowed_withdraw)
    ;
}

} // dcore
