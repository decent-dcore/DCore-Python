#include "module.hpp"
#include <graphene/chain/protocol/operations.hpp>
#include <graphene/chain/protocol/fee_schedule.hpp>

namespace dcore {

template<typename T>
bp::object decode_operation(const graphene::chain::operation& op)
{
    return op.which() == graphene::chain::operation::tag<T>::value ? bp::object(op.get<T>()) : bp::object();
}

bp::list encode_proposed_operations(const graphene::chain::proposal_create_operation &op)
{
    bp::list l;
    for(const auto& v : op.proposed_ops)
        l.append(v.op);
    return l;
}

void decode_proposed_operations(graphene::chain::proposal_create_operation& op, const bp::list &l)
{
    auto len = bp::len(l);
    op.proposed_ops.resize(len);
    while(len--) {
        bp::extract<graphene::chain::operation> v(l[len]);
        op.proposed_ops[len].op = v();
    }
}

template<typename T>
graphene::chain::memo_data::message_type get_data(const T& op)
{
    return op.data;
}

template<typename T>
void set_data(T& op, const graphene::chain::memo_data::message_type& data)
{
    op.data = data;
}

bp::object get_messaging_payload(const graphene::chain::custom_operation& op)
{
    if(op.id == graphene::chain::custom_operation::custom_operation_subtype_messaging) {
        graphene::chain::message_payload pl;
        op.get_messaging_payload(pl);
        return bp::object(pl);
    }

    return bp::object();
}

void register_operation()
{
    bp::scope op = bp::class_<graphene::chain::operation>("Operation", bp::no_init)
        .def(bp::init<const graphene::chain::transfer_operation&>())
        .def(bp::init<const graphene::chain::account_create_operation&>())
        .def(bp::init<const graphene::chain::account_update_operation&>())
        .def(bp::init<const graphene::chain::custom_operation&>())
        .def("__repr__", object_repr<graphene::chain::operation>)
        .def("validate", graphene::chain::operation_validate)
        .add_property("transfer", decode_operation<graphene::chain::transfer_operation>)
        .add_property("account_create", decode_operation<graphene::chain::account_create_operation>)
        .add_property("account_update", decode_operation<graphene::chain::account_update_operation>)
        .add_property("asset_create", decode_operation<graphene::chain::asset_create_operation>)
        .add_property("asset_issue", decode_operation<graphene::chain::asset_issue_operation>)
        .add_property("asset_publish_feed", decode_operation<graphene::chain::asset_publish_feed_operation>)
        .add_property("miner_create", decode_operation<graphene::chain::miner_create_operation>)
        .add_property("miner_update", decode_operation<graphene::chain::miner_update_operation>)
        .add_property("update_global_parameters", decode_operation<graphene::chain::miner_update_global_parameters_operation>)
        .add_property("proposal_create", decode_operation<graphene::chain::proposal_create_operation>)
        .add_property("proposal_update", decode_operation<graphene::chain::proposal_update_operation>)
        .add_property("proposal_delete", decode_operation<graphene::chain::proposal_delete_operation>)
        .add_property("custom", decode_operation<graphene::chain::custom_operation>)
    ;

    bp::class_<graphene::chain::operation_result>("Result", bp::no_init)
        .def("__repr__", object_repr<graphene::chain::operation_result>)
    ;

    bp::class_<graphene::chain::transfer_operation>("Transfer", bp::init<>())
        .def("__repr__", object_repr<graphene::chain::transfer_operation>)
        .def_readwrite("fee", &graphene::chain::transfer_operation::fee)
        .def_readwrite("sender", &graphene::chain::transfer_operation::from)
        .def_readwrite("receiver", &graphene::chain::transfer_operation::to)
        .def_readwrite("amount", &graphene::chain::transfer_operation::amount)
        .add_property("memo",
            decode_optional_type<graphene::chain::transfer_operation, graphene::chain::memo_data, &graphene::chain::transfer_operation::memo>,
            encode_optional_type<graphene::chain::transfer_operation, graphene::chain::memo_data, &graphene::chain::transfer_operation::memo>)
    ;

    bp::class_<graphene::chain::account_create_operation>("CreateAccount", bp::init<>())
        .def("__repr__", object_repr<graphene::chain::account_create_operation>)
        .def_readwrite("fee", &graphene::chain::account_create_operation::fee)
        .def_readwrite("registrar", &graphene::chain::account_create_operation::registrar)
        .def_readwrite("name", &graphene::chain::account_create_operation::name)
        .def_readwrite("owner", &graphene::chain::account_create_operation::owner)
        .def_readwrite("active", &graphene::chain::account_create_operation::active)
        .def_readwrite("options", &graphene::chain::account_create_operation::options)
    ;

    bp::class_<graphene::chain::account_update_operation>("UpdateAccount", bp::init<>())
        .def("__repr__", object_repr<graphene::chain::account_update_operation>)
        .def_readwrite("fee", &graphene::chain::account_update_operation::fee)
        .def_readwrite("account", &graphene::chain::account_update_operation::account)
        .add_property("owner",
            decode_optional_type<graphene::chain::account_update_operation, graphene::chain::authority, &graphene::chain::account_update_operation::owner>,
            encode_optional_type<graphene::chain::account_update_operation, graphene::chain::authority, &graphene::chain::account_update_operation::owner>)
        .add_property("active",
            decode_optional_type<graphene::chain::account_update_operation, graphene::chain::authority, &graphene::chain::account_update_operation::active>,
            encode_optional_type<graphene::chain::account_update_operation, graphene::chain::authority, &graphene::chain::account_update_operation::active>)
        .add_property("options",
            decode_optional_type<graphene::chain::account_update_operation, graphene::chain::account_options, &graphene::chain::account_update_operation::new_options>,
            encode_optional_type<graphene::chain::account_update_operation, graphene::chain::account_options, &graphene::chain::account_update_operation::new_options>)
    ;

    bp::class_<graphene::chain::asset_create_operation>("CreateAsset", bp::init<>())
        .def("__repr__", object_repr<graphene::chain::asset_create_operation>)
        .def_readwrite("fee", &graphene::chain::asset_create_operation::fee)
        .def_readwrite("issuer", &graphene::chain::asset_create_operation::issuer)
        .def_readwrite("symbol", &graphene::chain::asset_create_operation::symbol)
        .def_readwrite("precision", &graphene::chain::asset_create_operation::precision)
        .def_readwrite("description", &graphene::chain::asset_create_operation::description)
        .def_readwrite("options", &graphene::chain::asset_create_operation::options)
        .add_property("monitored_asset_options",
            decode_optional_type<graphene::chain::asset_create_operation, graphene::chain::monitored_asset_options, &graphene::chain::asset_create_operation::monitored_asset_opts>,
            encode_optional_type<graphene::chain::asset_create_operation, graphene::chain::monitored_asset_options, &graphene::chain::asset_create_operation::monitored_asset_opts>)
    ;

    bp::class_<graphene::chain::asset_issue_operation>("IssueAsset", bp::init<>())
        .def("__repr__", object_repr<graphene::chain::asset_issue_operation>)
        .def_readwrite("fee", &graphene::chain::asset_issue_operation::fee)
        .def_readwrite("issuer", &graphene::chain::asset_issue_operation::issuer)
        .def_readwrite("asset", &graphene::chain::asset_issue_operation::asset_to_issue)
        .def_readwrite("receiver", &graphene::chain::asset_issue_operation::issue_to_account)
        .add_property("memo",
            decode_optional_type<graphene::chain::asset_issue_operation, graphene::chain::memo_data, &graphene::chain::asset_issue_operation::memo>,
            encode_optional_type<graphene::chain::asset_issue_operation, graphene::chain::memo_data, &graphene::chain::asset_issue_operation::memo>)
    ;

    bp::class_<graphene::chain::asset_publish_feed_operation>("PublishAssetFeed", bp::init<>())
        .def("__repr__", object_repr<graphene::chain::asset_publish_feed_operation>)
        .def_readwrite("fee", &graphene::chain::asset_publish_feed_operation::fee)
        .def_readwrite("publisher", &graphene::chain::asset_publish_feed_operation::publisher)
        .def_readwrite("asset", &graphene::chain::asset_publish_feed_operation::asset_id)
        .def_readwrite("feed", &graphene::chain::asset_publish_feed_operation::feed)
    ;

    bp::class_<graphene::chain::miner_create_operation>("CreateMiner", bp::init<>())
        .def("__repr__", object_repr<graphene::chain::miner_create_operation>)
        .def_readwrite("fee", &graphene::chain::miner_create_operation::fee)
        .def_readwrite("miner_account", &graphene::chain::miner_create_operation::miner_account)
        .def_readwrite("url", &graphene::chain::miner_create_operation::url)
        .def_readwrite("block_signing_key", &graphene::chain::miner_create_operation::block_signing_key)
    ;

    bp::class_<graphene::chain::miner_update_operation>("UpdateMiner", bp::init<>())
        .def("__repr__", object_repr<graphene::chain::miner_update_operation>)
        .def_readwrite("fee", &graphene::chain::miner_update_operation::fee)
        .def_readwrite("miner", &graphene::chain::miner_update_operation::miner)
        .def_readwrite("miner_account", &graphene::chain::miner_update_operation::miner_account)
        .add_property("url",
            decode_optional_type<graphene::chain::miner_update_operation, std::string, &graphene::chain::miner_update_operation::new_url>,
            encode_optional_type<graphene::chain::miner_update_operation, std::string, &graphene::chain::miner_update_operation::new_url>)
        .add_property("block_signing_key",
            decode_optional_type<graphene::chain::miner_update_operation, graphene::chain::public_key_type, &graphene::chain::miner_update_operation::new_signing_key>,
            encode_optional_type<graphene::chain::miner_update_operation, graphene::chain::public_key_type, &graphene::chain::miner_update_operation::new_signing_key>)
    ;

    bp::class_<graphene::chain::miner_update_global_parameters_operation>("UpdateGlobalParameters", bp::init<>())
        .def("__repr__", object_repr<graphene::chain::miner_update_global_parameters_operation>)
        .def_readwrite("fee", &graphene::chain::miner_update_global_parameters_operation::fee)
        .def_readwrite("parameters", &graphene::chain::miner_update_global_parameters_operation::new_parameters)
    ;

    bp::class_<graphene::chain::proposal_create_operation>("CreateProposal", bp::init<>())
        .def("__repr__", object_repr<graphene::chain::proposal_create_operation>)
        .def_readwrite("fee", &graphene::chain::proposal_create_operation::fee)
        .def_readwrite("payer", &graphene::chain::proposal_create_operation::fee_paying_account)
        .add_property("proposed_operations", encode_proposed_operations, decode_proposed_operations)
        .def_readwrite("expiration_time", &graphene::chain::proposal_create_operation::expiration_time)
        .add_property("review_period_seconds",
            decode_optional_type<graphene::chain::proposal_create_operation, uint32_t, &graphene::chain::proposal_create_operation::review_period_seconds>,
            encode_optional_type<graphene::chain::proposal_create_operation, uint32_t, &graphene::chain::proposal_create_operation::review_period_seconds>)
    ;

    bp::class_<graphene::chain::proposal_update_operation>("UpdateProposal", bp::init<>())
        .def("__repr__", object_repr<graphene::chain::proposal_update_operation>)
        .def_readwrite("fee", &graphene::chain::proposal_update_operation::fee)
        .def_readwrite("payer", &graphene::chain::proposal_update_operation::fee_paying_account)
        .def_readwrite("proposal", &graphene::chain::proposal_update_operation::proposal)
        .add_property("active_approvals_to_add",
            encode_set<graphene::chain::proposal_update_operation, boost::container::flat_set<graphene::chain::account_id_type>, &graphene::chain::proposal_update_operation::active_approvals_to_add>,
            decode_set<graphene::chain::proposal_update_operation, boost::container::flat_set<graphene::chain::account_id_type>, &graphene::chain::proposal_update_operation::active_approvals_to_add>)
        .add_property("active_approvals_to_remove",
            encode_set<graphene::chain::proposal_update_operation, boost::container::flat_set<graphene::chain::account_id_type>, &graphene::chain::proposal_update_operation::active_approvals_to_remove>,
            decode_set<graphene::chain::proposal_update_operation, boost::container::flat_set<graphene::chain::account_id_type>, &graphene::chain::proposal_update_operation::active_approvals_to_remove>)
        .add_property("owner_approvals_to_add",
            encode_set<graphene::chain::proposal_update_operation, boost::container::flat_set<graphene::chain::account_id_type>, &graphene::chain::proposal_update_operation::owner_approvals_to_add>,
            decode_set<graphene::chain::proposal_update_operation, boost::container::flat_set<graphene::chain::account_id_type>, &graphene::chain::proposal_update_operation::owner_approvals_to_add>)
        .add_property("owner_approvals_to_remove",
            encode_set<graphene::chain::proposal_update_operation, boost::container::flat_set<graphene::chain::account_id_type>, &graphene::chain::proposal_update_operation::owner_approvals_to_remove>,
            decode_set<graphene::chain::proposal_update_operation, boost::container::flat_set<graphene::chain::account_id_type>, &graphene::chain::proposal_update_operation::owner_approvals_to_remove>)
        .add_property("key_approvals_to_add",
            encode_set<graphene::chain::proposal_update_operation, boost::container::flat_set<graphene::chain::public_key_type>, &graphene::chain::proposal_update_operation::key_approvals_to_add>,
            decode_set<graphene::chain::proposal_update_operation, boost::container::flat_set<graphene::chain::public_key_type>, &graphene::chain::proposal_update_operation::key_approvals_to_add>)
        .add_property("key_approvals_to_remove",
            encode_set<graphene::chain::proposal_update_operation, boost::container::flat_set<graphene::chain::public_key_type>, &graphene::chain::proposal_update_operation::key_approvals_to_remove>,
            decode_set<graphene::chain::proposal_update_operation, boost::container::flat_set<graphene::chain::public_key_type>, &graphene::chain::proposal_update_operation::key_approvals_to_remove>)
    ;

    bp::class_<graphene::chain::proposal_delete_operation>("DeleteProposal", bp::init<>())
        .def("__repr__", object_repr<graphene::chain::proposal_delete_operation>)
        .def_readwrite("fee", &graphene::chain::proposal_delete_operation::fee)
        .def_readwrite("payer", &graphene::chain::proposal_delete_operation::fee_paying_account)
        .def_readwrite("using_owner_authority", &graphene::chain::proposal_delete_operation::using_owner_authority)
        .def_readwrite("proposal", &graphene::chain::proposal_delete_operation::proposal)
    ;

    bp::scope custom = bp::class_<graphene::chain::custom_operation>("Custom", bp::init<>())
        .def("__repr__", object_repr<graphene::chain::custom_operation>)
        .def_readwrite("fee", &graphene::chain::custom_operation::fee)
        .def_readwrite("payer", &graphene::chain::custom_operation::payer)
        .add_property("required_auths",
            encode_set<graphene::chain::custom_operation, boost::container::flat_set<graphene::chain::account_id_type>, &graphene::chain::custom_operation::required_auths>,
            decode_set<graphene::chain::custom_operation, boost::container::flat_set<graphene::chain::account_id_type>, &graphene::chain::custom_operation::required_auths>)
        .def_readwrite("id", &graphene::chain::custom_operation::id)
        .add_property("data", get_data<graphene::chain::custom_operation>, set_data<graphene::chain::custom_operation>)
        .add_property("message_payload", get_messaging_payload, &graphene::chain::custom_operation::set_messaging_payload)
    ;

    custom.attr("SUBTYPE_MESSAGING") = static_cast<uint16_t>(graphene::chain::custom_operation::custom_operation_subtype_messaging);

    bp::scope msg = bp::class_<graphene::chain::message_payload>("MessagePayload", bp::init<>())
        .def("__repr__", object_repr<graphene::chain::message_payload>)
        .def_readwrite("sender", &graphene::chain::message_payload::from)
        .def_readwrite("key", &graphene::chain::message_payload::pub_from)
        .add_property("receivers",
            encode_list<graphene::chain::message_payload, std::vector<graphene::chain::message_payload_receivers_data>, &graphene::chain::message_payload::receivers_data>,
            decode_list<graphene::chain::message_payload, std::vector<graphene::chain::message_payload_receivers_data>, &graphene::chain::message_payload::receivers_data>)
    ;

    bp::class_<graphene::chain::message_payload_receivers_data>("Data", bp::init<>())
        .def(bp::init<const std::string&, const graphene::chain::private_key_type&, const graphene::chain::public_key_type&, graphene::chain::account_id_type, bp::optional<uint64_t>>())
        .def("__repr__", object_repr<graphene::chain::message_payload_receivers_data>)
        .def_readwrite("receiver", &graphene::chain::message_payload_receivers_data::to)
        .def_readwrite("key", &graphene::chain::message_payload_receivers_data::pub_to)
        .def_readwrite("nonce", &graphene::chain::message_payload_receivers_data::nonce)
        .add_property("data", get_data<graphene::chain::message_payload_receivers_data>, set_data<graphene::chain::message_payload_receivers_data>)
        .def("get_message", &graphene::chain::message_payload_receivers_data::get_message)
    ;
}

} // dcore
