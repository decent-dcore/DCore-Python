#include "module.hpp"
#include <graphene/chain/protocol/vote.hpp>
#include <graphene/chain/miner_object.hpp>

namespace dcore {

void register_miner()
{
    bp::class_<graphene::chain::vote_id_type>("VoteId", bp::init<>())
        .def("__repr__", object_repr<graphene::chain::vote_id_type>)
        .def("__str__", &graphene::chain::vote_id_type::operator std::string)
        .def(bp::init<const std::string&>())
        .add_property("instance", &graphene::chain::vote_id_type::instance, &graphene::chain::vote_id_type::set_instance)
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
}

} // dcore
