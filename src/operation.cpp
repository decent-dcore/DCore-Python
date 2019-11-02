#include "module.hpp"
#include <graphene/chain/protocol/operations.hpp>
#include <graphene/chain/protocol/fee_schedule.hpp>

namespace dcore {

template<typename V, typename T>
bp::object decode_static_variant(const V& v)
{
    return v.which() == V::template tag<T>::value ? bp::object(v.template get<T>()) : bp::object();
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
        .def(bp::init<const graphene::chain::asset_create_operation&>())
        .def(bp::init<const graphene::chain::asset_issue_operation&>())
        .def(bp::init<const graphene::chain::asset_publish_feed_operation&>())
        .def(bp::init<const graphene::chain::miner_create_operation&>())
        .def(bp::init<const graphene::chain::miner_update_operation&>())
        .def(bp::init<const graphene::chain::miner_update_global_parameters_operation&>())
        .def(bp::init<const graphene::chain::proposal_create_operation&>())
        .def(bp::init<const graphene::chain::proposal_update_operation&>())
        .def(bp::init<const graphene::chain::proposal_delete_operation&>())
        .def(bp::init<const graphene::chain::withdraw_permission_create_operation&>())
        .def(bp::init<const graphene::chain::withdraw_permission_update_operation&>())
        .def(bp::init<const graphene::chain::withdraw_permission_claim_operation&>())
        .def(bp::init<const graphene::chain::withdraw_permission_delete_operation&>())
        .def(bp::init<const graphene::chain::vesting_balance_create_operation&>())
        .def(bp::init<const graphene::chain::vesting_balance_withdraw_operation&>())
        .def(bp::init<const graphene::chain::custom_operation&>())
        .def(bp::init<const graphene::chain::assert_operation&>())
        .def(bp::init<const graphene::chain::non_fungible_token_create_definition_operation&>())
        .def(bp::init<const graphene::chain::non_fungible_token_update_definition_operation&>())
        .def(bp::init<const graphene::chain::non_fungible_token_issue_operation&>())
        .def(bp::init<const graphene::chain::non_fungible_token_transfer_operation&>())
        .def(bp::init<const graphene::chain::non_fungible_token_update_data_operation&>())
        .def("__repr__", object_repr<graphene::chain::operation>)
        .def("validate", graphene::chain::operation_validate)
        .add_property("transfer", decode_static_variant<graphene::chain::operation, graphene::chain::transfer_operation>)
        .add_property("account_create", decode_static_variant<graphene::chain::operation, graphene::chain::account_create_operation>)
        .add_property("account_update", decode_static_variant<graphene::chain::operation, graphene::chain::account_update_operation>)
        .add_property("asset_create", decode_static_variant<graphene::chain::operation, graphene::chain::asset_create_operation>)
        .add_property("asset_issue", decode_static_variant<graphene::chain::operation, graphene::chain::asset_issue_operation>)
        .add_property("asset_publish_feed", decode_static_variant<graphene::chain::operation, graphene::chain::asset_publish_feed_operation>)
        .add_property("miner_create", decode_static_variant<graphene::chain::operation, graphene::chain::miner_create_operation>)
        .add_property("miner_update", decode_static_variant<graphene::chain::operation, graphene::chain::miner_update_operation>)
        .add_property("update_global_parameters", decode_static_variant<graphene::chain::operation, graphene::chain::miner_update_global_parameters_operation>)
        .add_property("proposal_create", decode_static_variant<graphene::chain::operation, graphene::chain::proposal_create_operation>)
        .add_property("proposal_update", decode_static_variant<graphene::chain::operation, graphene::chain::proposal_update_operation>)
        .add_property("proposal_delete", decode_static_variant<graphene::chain::operation, graphene::chain::proposal_delete_operation>)
        .add_property("withdraw_permission_create", decode_static_variant<graphene::chain::operation, graphene::chain::withdraw_permission_create_operation>)
        .add_property("withdraw_permission_update", decode_static_variant<graphene::chain::operation, graphene::chain::withdraw_permission_update_operation>)
        .add_property("withdraw_permission_claim", decode_static_variant<graphene::chain::operation, graphene::chain::withdraw_permission_claim_operation>)
        .add_property("withdraw_permission_delete", decode_static_variant<graphene::chain::operation, graphene::chain::withdraw_permission_delete_operation>)
        .add_property("vesting_balance_create", decode_static_variant<graphene::chain::operation, graphene::chain::vesting_balance_create_operation>)
        .add_property("vesting_balance_withdraw", decode_static_variant<graphene::chain::operation, graphene::chain::vesting_balance_withdraw_operation>)
        .add_property("custom", decode_static_variant<graphene::chain::operation, graphene::chain::custom_operation>)
        .add_property("assert", decode_static_variant<graphene::chain::operation, graphene::chain::assert_operation>)
        .add_property("non_fungible_token_create", decode_static_variant<graphene::chain::operation, graphene::chain::non_fungible_token_create_definition_operation>)
        .add_property("non_fungible_token_update", decode_static_variant<graphene::chain::operation, graphene::chain::non_fungible_token_update_definition_operation>)
        .add_property("non_fungible_token_issue", decode_static_variant<graphene::chain::operation, graphene::chain::non_fungible_token_issue_operation>)
        .add_property("non_fungible_token_transfer", decode_static_variant<graphene::chain::operation, graphene::chain::non_fungible_token_transfer_operation>)
        .add_property("non_fungible_token_data_update", decode_static_variant<graphene::chain::operation, graphene::chain::non_fungible_token_update_data_operation>)
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

    bp::class_<graphene::chain::withdraw_permission_create_operation>("CreateWithdrawPermission", bp::init<>())
        .def("__repr__", object_repr<graphene::chain::withdraw_permission_create_operation>)
        .def_readwrite("fee", &graphene::chain::withdraw_permission_create_operation::fee)
        .def_readwrite("from_account", &graphene::chain::withdraw_permission_create_operation::withdraw_from_account)
        .def_readwrite("authorized_account", &graphene::chain::withdraw_permission_create_operation::authorized_account)
        .def_readwrite("limit", &graphene::chain::withdraw_permission_create_operation::withdrawal_limit)
        .def_readwrite("period_sec", &graphene::chain::withdraw_permission_create_operation::withdrawal_period_sec)
        .def_readwrite("periods_until_expiration", &graphene::chain::withdraw_permission_create_operation::periods_until_expiration)
        .def_readwrite("period_start_time", &graphene::chain::withdraw_permission_create_operation::period_start_time)
   ;

    bp::class_<graphene::chain::withdraw_permission_update_operation>("UpdateWithdrawPermission", bp::init<>())
        .def("__repr__", object_repr<graphene::chain::withdraw_permission_update_operation>)
        .def_readwrite("fee", &graphene::chain::withdraw_permission_update_operation::fee)
        .def_readwrite("from_account", &graphene::chain::withdraw_permission_update_operation::withdraw_from_account)
        .def_readwrite("authorized_account", &graphene::chain::withdraw_permission_update_operation::authorized_account)
        .def_readwrite("permission", &graphene::chain::withdraw_permission_update_operation::permission_to_update)
        .def_readwrite("limit", &graphene::chain::withdraw_permission_update_operation::withdrawal_limit)
        .def_readwrite("period_sec", &graphene::chain::withdraw_permission_update_operation::withdrawal_period_sec)
        .def_readwrite("periods_until_expiration", &graphene::chain::withdraw_permission_update_operation::periods_until_expiration)
        .def_readwrite("period_start_time", &graphene::chain::withdraw_permission_update_operation::period_start_time)
    ;

    bp::class_<graphene::chain::withdraw_permission_claim_operation>("ClaimWithdrawPermission", bp::init<>())
        .def("__repr__", object_repr<graphene::chain::withdraw_permission_claim_operation>)
        .def_readwrite("fee", &graphene::chain::withdraw_permission_claim_operation::fee)
        .def_readwrite("from_account", &graphene::chain::withdraw_permission_claim_operation::withdraw_from_account)
        .def_readwrite("to_account", &graphene::chain::withdraw_permission_claim_operation::withdraw_to_account)
        .def_readwrite("permission", &graphene::chain::withdraw_permission_claim_operation::withdraw_permission)
        .def_readwrite("amount", &graphene::chain::withdraw_permission_claim_operation::amount_to_withdraw)
        .add_property("memo",
            decode_optional_type<graphene::chain::withdraw_permission_claim_operation, graphene::chain::memo_data, &graphene::chain::withdraw_permission_claim_operation::memo>,
            encode_optional_type<graphene::chain::withdraw_permission_claim_operation, graphene::chain::memo_data, &graphene::chain::withdraw_permission_claim_operation::memo>)
    ;

    bp::class_<graphene::chain::withdraw_permission_delete_operation>("DeleteWithdrawPermission", bp::init<>())
        .def("__repr__", object_repr<graphene::chain::withdraw_permission_delete_operation>)
        .def_readwrite("fee", &graphene::chain::withdraw_permission_delete_operation::fee)
        .def_readwrite("from_account", &graphene::chain::withdraw_permission_delete_operation::withdraw_from_account)
        .def_readwrite("authorized_account", &graphene::chain::withdraw_permission_delete_operation::authorized_account)
        .def_readwrite("permission", &graphene::chain::withdraw_permission_delete_operation::withdrawal_permission)
    ;

    {
        bp::scope vesting_balance = bp::class_<graphene::chain::vesting_balance_create_operation>("CreateVestingBalance", bp::init<>())
            .def("__repr__", object_repr<graphene::chain::vesting_balance_create_operation>)
            .def_readwrite("fee", &graphene::chain::vesting_balance_create_operation::fee)
            .def_readwrite("creator", &graphene::chain::vesting_balance_create_operation::creator)
            .def_readwrite("owner", &graphene::chain::vesting_balance_create_operation::owner)
            .def_readwrite("amount", &graphene::chain::vesting_balance_create_operation::amount)
            .def_readwrite("policy", &graphene::chain::vesting_balance_create_operation::policy)
        ;

        bp::scope policy = bp::class_<graphene::chain::vesting_policy_initializer>("Policy", bp::no_init)
            .def(bp::init<const graphene::chain::linear_vesting_policy_initializer&>())
            .def(bp::init<const graphene::chain::cdd_vesting_policy_initializer&>())
            .def("__repr__", object_repr<graphene::chain::vesting_policy_initializer>)
            .add_property("linear", decode_static_variant<graphene::chain::vesting_policy_initializer, graphene::chain::linear_vesting_policy_initializer>)
            .add_property("cdd", decode_static_variant<graphene::chain::vesting_policy_initializer, graphene::chain::cdd_vesting_policy_initializer>)
        ;

        bp::class_<graphene::chain::linear_vesting_policy_initializer>("Linear", bp::init<>())
            .def("__repr__", object_repr<graphene::chain::linear_vesting_policy_initializer>)
            .def_readwrite("begin", &graphene::chain::linear_vesting_policy_initializer::begin_timestamp)
            .def_readwrite("vesting_cliff_seconds", &graphene::chain::linear_vesting_policy_initializer::vesting_cliff_seconds)
            .def_readwrite("vesting_duration_seconds", &graphene::chain::linear_vesting_policy_initializer::vesting_duration_seconds)
        ;

        bp::class_<graphene::chain::cdd_vesting_policy_initializer>("Cdd", bp::init<>())
            .def("__repr__", object_repr<graphene::chain::cdd_vesting_policy_initializer>)
            .def_readwrite("start_claim", &graphene::chain::cdd_vesting_policy_initializer::start_claim)
            .def_readwrite("vesting_seconds", &graphene::chain::cdd_vesting_policy_initializer::vesting_seconds)
        ;
    }

    bp::class_<graphene::chain::vesting_balance_withdraw_operation>("WithdrawVestingBalance", bp::init<>())
        .def("__repr__", object_repr<graphene::chain::vesting_balance_withdraw_operation>)
        .def_readwrite("fee", &graphene::chain::vesting_balance_withdraw_operation::fee)
        .def_readwrite("vesting_balance", &graphene::chain::vesting_balance_withdraw_operation::vesting_balance)
        .def_readwrite("owner", &graphene::chain::vesting_balance_withdraw_operation::owner)
        .def_readwrite("amount", &graphene::chain::vesting_balance_withdraw_operation::amount)
    ;

    {
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

    {
        bp::scope assrt = bp::class_<graphene::chain::assert_operation>("Assert", bp::init<>())
            .def("__repr__", object_repr<graphene::chain::assert_operation>)
            .def_readwrite("fee", &graphene::chain::assert_operation::fee)
            .def_readwrite("payer", &graphene::chain::assert_operation::fee_paying_account)
            .add_property("required_auths",
                encode_set<graphene::chain::assert_operation, boost::container::flat_set<graphene::chain::account_id_type>, &graphene::chain::assert_operation::required_auths>,
                decode_set<graphene::chain::assert_operation, boost::container::flat_set<graphene::chain::account_id_type>, &graphene::chain::assert_operation::required_auths>)
            .add_property("predicates",
                encode_list<graphene::chain::assert_operation, std::vector<graphene::chain::predicate>, &graphene::chain::assert_operation::predicates>,
                decode_list<graphene::chain::assert_operation, std::vector<graphene::chain::predicate>, &graphene::chain::assert_operation::predicates>)
        ;

        bp::scope predicate = bp::class_<graphene::chain::predicate>("Predicate", bp::no_init)
            .def(bp::init<const graphene::chain::account_name_eq_lit_predicate&>())
            .def(bp::init<const graphene::chain::asset_symbol_eq_lit_predicate&>())
            .def(bp::init<const graphene::chain::block_id_predicate&>())
            .def("__repr__", object_repr<graphene::chain::predicate>)
            .add_property("linear", decode_static_variant<graphene::chain::predicate, graphene::chain::account_name_eq_lit_predicate>)
            .add_property("cdd", decode_static_variant<graphene::chain::predicate, graphene::chain::asset_symbol_eq_lit_predicate>)
            .add_property("cdd", decode_static_variant<graphene::chain::predicate, graphene::chain::block_id_predicate>)
        ;

        bp::class_<graphene::chain::account_name_eq_lit_predicate>("AccountNameEquals", bp::init<>())
            .def("__repr__", object_repr<graphene::chain::account_name_eq_lit_predicate>)
            .def_readwrite("account", &graphene::chain::account_name_eq_lit_predicate::account_id)
            .def_readwrite("name", &graphene::chain::account_name_eq_lit_predicate::name)
        ;

        bp::class_<graphene::chain::asset_symbol_eq_lit_predicate>("AssetNameEquals", bp::init<>())
            .def("__repr__", object_repr<graphene::chain::asset_symbol_eq_lit_predicate>)
            .def_readwrite("asset", &graphene::chain::asset_symbol_eq_lit_predicate::asset_id)
            .def_readwrite("symbol", &graphene::chain::asset_symbol_eq_lit_predicate::symbol)
        ;

        bp::class_<graphene::chain::block_id_predicate>("BlockIdEquals", bp::init<>())
            .def("__repr__", object_repr<graphene::chain::block_id_predicate>)
            .def_readwrite("id", &graphene::chain::block_id_predicate::id)
        ;
    }

    bp::class_<graphene::chain::non_fungible_token_create_definition_operation>("CreateNonFungibleToken", bp::init<>())
        .def("__repr__", object_repr<graphene::chain::non_fungible_token_create_definition_operation>)
        .def_readwrite("fee", &graphene::chain::non_fungible_token_create_definition_operation::fee)
        .def_readwrite("symbol", &graphene::chain::non_fungible_token_create_definition_operation::symbol)
        .def_readwrite("options", &graphene::chain::non_fungible_token_create_definition_operation::options)
        .add_property("definitions",
            encode_list<graphene::chain::non_fungible_token_create_definition_operation, graphene::chain::non_fungible_token_data_definitions, &graphene::chain::non_fungible_token_create_definition_operation::definitions>,
            decode_list<graphene::chain::non_fungible_token_create_definition_operation, graphene::chain::non_fungible_token_data_definitions, &graphene::chain::non_fungible_token_create_definition_operation::definitions>)
        .def_readwrite("transferable", &graphene::chain::non_fungible_token_create_definition_operation::transferable)
    ;

    bp::class_<graphene::chain::non_fungible_token_update_definition_operation>("UpdateNonFungibleToken", bp::init<>())
        .def("__repr__", object_repr<graphene::chain::non_fungible_token_update_definition_operation>)
        .def_readwrite("fee", &graphene::chain::non_fungible_token_update_definition_operation::fee)
        .def_readwrite("issuer", &graphene::chain::non_fungible_token_update_definition_operation::current_issuer)
        .def_readwrite("non_fungible_token", &graphene::chain::non_fungible_token_update_definition_operation::nft_id)
        .def_readwrite("options", &graphene::chain::non_fungible_token_update_definition_operation::options)
    ;

    bp::class_<graphene::chain::non_fungible_token_issue_operation>("IssueNonFungibleToken", bp::init<>())
        .def("__repr__", object_repr<graphene::chain::non_fungible_token_issue_operation>)
        .def_readwrite("fee", &graphene::chain::non_fungible_token_issue_operation::fee)
        .def_readwrite("issuer", &graphene::chain::non_fungible_token_issue_operation::issuer)
        .def_readwrite("receiver", &graphene::chain::non_fungible_token_issue_operation::to)
        .def_readwrite("non_fungible_token", &graphene::chain::non_fungible_token_issue_operation::nft_id)
        .add_property("data",
            encode_list<graphene::chain::non_fungible_token_issue_operation, fc::variants, &graphene::chain::non_fungible_token_issue_operation::data>,
            decode_list<graphene::chain::non_fungible_token_issue_operation, fc::variants, &graphene::chain::non_fungible_token_issue_operation::data>)
        .add_property("memo",
            decode_optional_type<graphene::chain::non_fungible_token_issue_operation, graphene::chain::memo_data, &graphene::chain::non_fungible_token_issue_operation::memo>,
            encode_optional_type<graphene::chain::non_fungible_token_issue_operation, graphene::chain::memo_data, &graphene::chain::non_fungible_token_issue_operation::memo>)
    ;

    bp::class_<graphene::chain::non_fungible_token_transfer_operation>("TransferNonFungibleToken", bp::init<>())
        .def("__repr__", object_repr<graphene::chain::non_fungible_token_transfer_operation>)
        .def_readwrite("fee", &graphene::chain::non_fungible_token_transfer_operation::fee)
        .def_readwrite("sender", &graphene::chain::non_fungible_token_transfer_operation::from)
        .def_readwrite("receiver", &graphene::chain::non_fungible_token_transfer_operation::to)
        .def_readwrite("non_fungible_token_data", &graphene::chain::non_fungible_token_transfer_operation::nft_data_id)
        .add_property("memo",
            decode_optional_type<graphene::chain::non_fungible_token_transfer_operation, graphene::chain::memo_data, &graphene::chain::non_fungible_token_transfer_operation::memo>,
            encode_optional_type<graphene::chain::non_fungible_token_transfer_operation, graphene::chain::memo_data, &graphene::chain::non_fungible_token_transfer_operation::memo>)
    ;

    bp::class_<graphene::chain::non_fungible_token_update_data_operation>("UpadateNonFungibleTokenData", bp::init<>())
        .def("__repr__", object_repr<graphene::chain::non_fungible_token_update_data_operation>)
        .def_readwrite("fee", &graphene::chain::non_fungible_token_update_data_operation::fee)
        .def_readwrite("modifier", &graphene::chain::non_fungible_token_update_data_operation::modifier)
        .def_readwrite("non_fungible_token_data", &graphene::chain::non_fungible_token_update_data_operation::nft_data_id)
        .add_property("data",
            encode_list<graphene::chain::non_fungible_token_update_data_operation, std::vector<std::pair<std::string, fc::variant>>, &graphene::chain::non_fungible_token_update_data_operation::data>,
            decode_list<graphene::chain::non_fungible_token_update_data_operation, std::vector<std::pair<std::string, fc::variant>>, &graphene::chain::non_fungible_token_update_data_operation::data>)
    ;
}

} // dcore
