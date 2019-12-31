#include "module.hpp"
#include <graphene/chain/protocol/operations.hpp>
#include <graphene/chain/protocol/fee_schedule.hpp>

namespace dcore {

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

template<typename T, size_t N>
struct array_converter
{
    array_converter()
    {
        bp::to_python_converter<fc::array<T, N>, array_converter<T, N>>();
        bp::converter::registry::push_back(array_converter<T, N>::convertible, array_converter<T, N>::construct, bp::type_id<fc::array<T, N>>());
    }

    static PyObject* convert(const fc::array<T, N>& a)
    {
        return PyByteArray_FromStringAndSize(reinterpret_cast<const char*>(a.begin()), a.size());
    }

    static void* convertible(PyObject* obj)
    {
        return PyByteArray_Check(obj) && PyByteArray_Size(obj) == N ? obj : nullptr;
    }

    static void construct(PyObject* obj, bp::converter::rvalue_from_python_stage1_data* data)
    {
        void* storage = ((bp::converter::rvalue_from_python_storage<fc::array<T, N>>*)data)->storage.bytes;
        fc::array<T, N>* a = new (storage)fc::array<T, N>();
        if(PyByteArray_Check(obj) && PyByteArray_Size(obj) == N)
            std::memcpy(a->begin(), PyByteArray_AsString(obj), N);

        data->convertible = storage;
    }
};

void register_operation()
{
    bp::scope op = bp::class_<graphene::chain::operation>("Operation", bp::no_init)
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
        .def(bp::init<const graphene::chain::content_submit_operation&>())
        .def(bp::init<const graphene::chain::request_to_buy_operation&>())
        .def(bp::init<const graphene::chain::leave_rating_and_comment_operation&>())
        .def(bp::init<const graphene::chain::proof_of_custody_operation&>())
        .def(bp::init<const graphene::chain::deliver_keys_operation&>())
        .def(bp::init<const graphene::chain::subscribe_operation&>())
        .def(bp::init<const graphene::chain::subscribe_by_author_operation&>())
        .def(bp::init<const graphene::chain::automatic_renewal_of_subscription_operation&>())
        .def(bp::init<const graphene::chain::report_stats_operation&>())
        .def(bp::init<const graphene::chain::set_publishing_manager_operation&>())
        .def(bp::init<const graphene::chain::set_publishing_right_operation&>())
        .def(bp::init<const graphene::chain::content_cancellation_operation&>())
        .def(bp::init<const graphene::chain::asset_fund_pools_operation&>())
        .def(bp::init<const graphene::chain::asset_reserve_operation&>())
        .def(bp::init<const graphene::chain::asset_claim_fees_operation&>())
        .def(bp::init<const graphene::chain::update_user_issued_asset_operation&>())
        .def(bp::init<const graphene::chain::update_monitored_asset_operation&>())
        .def(bp::init<const graphene::chain::ready_to_publish_operation&>())
        .def(bp::init<const graphene::chain::transfer_operation&>())
        .def(bp::init<const graphene::chain::update_user_issued_asset_advanced_operation&>())
        .def(bp::init<const graphene::chain::non_fungible_token_create_definition_operation&>())
        .def(bp::init<const graphene::chain::non_fungible_token_update_definition_operation&>())
        .def(bp::init<const graphene::chain::non_fungible_token_issue_operation&>())
        .def(bp::init<const graphene::chain::non_fungible_token_transfer_operation&>())
        .def(bp::init<const graphene::chain::non_fungible_token_update_data_operation&>())
        .def("__repr__", object_repr<graphene::chain::operation>)
        .def("validate", graphene::chain::operation_validate)
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
        .add_property("content_submit", decode_static_variant<graphene::chain::operation, graphene::chain::content_submit_operation>)
        .add_property("request_to_buy", decode_static_variant<graphene::chain::operation, graphene::chain::request_to_buy_operation>)
        .add_property("leave_rating_and_comment", decode_static_variant<graphene::chain::operation, graphene::chain::leave_rating_and_comment_operation>)
        .add_property("proof_of_custody", decode_static_variant<graphene::chain::operation, graphene::chain::proof_of_custody_operation>)
        .add_property("deliver_keys", decode_static_variant<graphene::chain::operation, graphene::chain::deliver_keys_operation>)
        .add_property("subscribe_to_author", decode_static_variant<graphene::chain::operation, graphene::chain::subscribe_operation>)
        .add_property("subscribe_by_author", decode_static_variant<graphene::chain::operation, graphene::chain::subscribe_by_author_operation>)
        .add_property("automatic_renewal_of_subscription", decode_static_variant<graphene::chain::operation, graphene::chain::automatic_renewal_of_subscription_operation>)
        .add_property("report_statistics", decode_static_variant<graphene::chain::operation, graphene::chain::report_stats_operation>)
        .add_property("set_publishing_manager", decode_static_variant<graphene::chain::operation, graphene::chain::set_publishing_manager_operation>)
        .add_property("set_publishing_right", decode_static_variant<graphene::chain::operation, graphene::chain::set_publishing_right_operation>)
        .add_property("content_cancellation", decode_static_variant<graphene::chain::operation, graphene::chain::content_cancellation_operation>)
        .add_property("asset_fund_pools", decode_static_variant<graphene::chain::operation, graphene::chain::asset_fund_pools_operation>)
        .add_property("asset_reserve", decode_static_variant<graphene::chain::operation, graphene::chain::asset_reserve_operation>)
        .add_property("asset_claim_fees", decode_static_variant<graphene::chain::operation, graphene::chain::asset_claim_fees_operation>)
        .add_property("update_user_issued_asset", decode_static_variant<graphene::chain::operation, graphene::chain::update_user_issued_asset_operation>)
        .add_property("update_monitored_asset", decode_static_variant<graphene::chain::operation, graphene::chain::update_monitored_asset_operation>)
        .add_property("ready_to_publish", decode_static_variant<graphene::chain::operation, graphene::chain::ready_to_publish_operation>)
        .add_property("transfer", decode_static_variant<graphene::chain::operation, graphene::chain::transfer_operation>)
        .add_property("update_user_issued_asset_advanced", decode_static_variant<graphene::chain::operation, graphene::chain::update_user_issued_asset_advanced_operation>)
        .add_property("non_fungible_token_create", decode_static_variant<graphene::chain::operation, graphene::chain::non_fungible_token_create_definition_operation>)
        .add_property("non_fungible_token_update", decode_static_variant<graphene::chain::operation, graphene::chain::non_fungible_token_update_definition_operation>)
        .add_property("non_fungible_token_issue", decode_static_variant<graphene::chain::operation, graphene::chain::non_fungible_token_issue_operation>)
        .add_property("non_fungible_token_transfer", decode_static_variant<graphene::chain::operation, graphene::chain::non_fungible_token_transfer_operation>)
        .add_property("non_fungible_token_data_update", decode_static_variant<graphene::chain::operation, graphene::chain::non_fungible_token_update_data_operation>)
    ;

    bp::class_<graphene::chain::operation_result>("Result", bp::no_init)
        .def("__repr__", object_repr<graphene::chain::operation_result>)
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

    {
        array_converter<int8_t, 16> u_seed_conv;
        array_converter<uint8_t, DECENT_SIZE_OF_POINT_ON_CURVE_COMPRESSED> pub_key_conv;

        bp::scope submit = bp::class_<graphene::chain::content_submit_operation>("SubmitContent", bp::init<>())
            .def("__repr__", object_repr<graphene::chain::content_submit_operation>)
            .def_readwrite("fee", &graphene::chain::content_submit_operation::fee)
            .def_readwrite("author", &graphene::chain::content_submit_operation::author)
            .add_property("co_authors",
                encode_dict<graphene::chain::content_submit_operation, std::map<graphene::chain::account_id_type, uint32_t>, &graphene::chain::content_submit_operation::co_authors>,
                decode_dict<graphene::chain::content_submit_operation, std::map<graphene::chain::account_id_type, uint32_t>, &graphene::chain::content_submit_operation::co_authors>)
            .def_readwrite("uri", &graphene::chain::content_submit_operation::URI)
            .add_property("price",
                encode_list<graphene::chain::content_submit_operation, std::vector<graphene::chain::regional_price>, &graphene::chain::content_submit_operation::price>,
                decode_list<graphene::chain::content_submit_operation, std::vector<graphene::chain::regional_price>, &graphene::chain::content_submit_operation::price>)
            .def_readwrite("size", &graphene::chain::content_submit_operation::size)
            .def_readwrite("hash", &graphene::chain::content_submit_operation::hash)
            .add_property("seeders",
                encode_list<graphene::chain::content_submit_operation, std::vector<graphene::chain::account_id_type>, &graphene::chain::content_submit_operation::seeders>,
                decode_list<graphene::chain::content_submit_operation, std::vector<graphene::chain::account_id_type>, &graphene::chain::content_submit_operation::seeders>)
            .add_property("key_parts",
                encode_list<graphene::chain::content_submit_operation, std::vector<graphene::chain::ciphertext_type>, &graphene::chain::content_submit_operation::key_parts>,
                decode_list<graphene::chain::content_submit_operation, std::vector<graphene::chain::ciphertext_type>, &graphene::chain::content_submit_operation::key_parts>)
            .def_readwrite("quorum", &graphene::chain::content_submit_operation::quorum)
            .def_readwrite("expiration", &graphene::chain::content_submit_operation::expiration)
            .def_readwrite("publishing_fee", &graphene::chain::content_submit_operation::publishing_fee)
            .def_readwrite("synopsis", &graphene::chain::content_submit_operation::synopsis)
            .add_property("custody_data",
                decode_optional_type<graphene::chain::content_submit_operation, graphene::chain::custody_data_type, &graphene::chain::content_submit_operation::cd>,
                encode_optional_type<graphene::chain::content_submit_operation, graphene::chain::custody_data_type, &graphene::chain::content_submit_operation::cd>)
        ;

        bp::class_<graphene::chain::regional_price>("RegionalPrice", bp::init<>())
            .def("__repr__", object_repr<graphene::chain::regional_price>)
            .def_readwrite("region", &graphene::chain::regional_price::region)
            .def_readwrite("price", &graphene::chain::regional_price::price)
        ;

        bp::class_<graphene::chain::custody_data_type>("CustodyData", bp::init<>())
            .def("__repr__", object_repr<graphene::chain::custody_data_type>)
            .def_readwrite("num", &graphene::chain::custody_data_type::n)
            .def_readwrite("u_seed", &graphene::chain::custody_data_type::u_seed)
            .def_readwrite("public_key", &graphene::chain::custody_data_type::pubKey)
        ;
    }

    bp::class_<graphene::chain::request_to_buy_operation>("RequestToBuy", bp::init<>())
        .def("__repr__", object_repr<graphene::chain::request_to_buy_operation>)
        .def_readwrite("fee", &graphene::chain::request_to_buy_operation::fee)
        .def_readwrite("uri", &graphene::chain::request_to_buy_operation::URI)
        .def_readwrite("consumer", &graphene::chain::request_to_buy_operation::consumer)
        .def_readwrite("price", &graphene::chain::request_to_buy_operation::price)
        .def_readwrite("region_code", &graphene::chain::request_to_buy_operation::region_code_from)
        .def_readwrite("public_key", &graphene::chain::request_to_buy_operation::pubKey)
    ;

    bp::class_<graphene::chain::leave_rating_and_comment_operation>("LeaveRatingAndComment", bp::init<>())
        .def("__repr__", object_repr<graphene::chain::leave_rating_and_comment_operation>)
        .def_readwrite("fee", &graphene::chain::leave_rating_and_comment_operation::fee)
        .def_readwrite("uri", &graphene::chain::leave_rating_and_comment_operation::URI)
        .def_readwrite("consumer", &graphene::chain::leave_rating_and_comment_operation::consumer)
        .def_readwrite("rating", &graphene::chain::leave_rating_and_comment_operation::rating)
        .def_readwrite("comment", &graphene::chain::leave_rating_and_comment_operation::comment)
    ;

    {
        bp::scope proof = bp::class_<graphene::chain::proof_of_custody_operation>("ProofOfCustody", bp::init<>())
            .def("__repr__", object_repr<graphene::chain::proof_of_custody_operation>)
            .def_readwrite("fee", &graphene::chain::proof_of_custody_operation::fee)
            .def_readwrite("seeder", &graphene::chain::proof_of_custody_operation::seeder)
            .def_readwrite("uri", &graphene::chain::proof_of_custody_operation::URI)
            .add_property("proof",
                decode_optional_type<graphene::chain::proof_of_custody_operation, graphene::chain::custody_proof_type, &graphene::chain::proof_of_custody_operation::proof>,
                encode_optional_type<graphene::chain::proof_of_custody_operation, graphene::chain::custody_proof_type, &graphene::chain::proof_of_custody_operation::proof>)
        ;

        bp::class_<graphene::chain::custody_proof_type>("Proof", bp::init<>())
            .def("__repr__", object_repr<graphene::chain::custody_proof_type>)
            .def_readwrite("reference_block", &graphene::chain::custody_proof_type::reference_block)
            .def_readwrite("seed", &graphene::chain::custody_proof_type::seed)
            .add_property("mus",
                encode_list<graphene::chain::custody_proof_type, std::vector<std::string>, &graphene::chain::custody_proof_type::mus>,
                decode_list<graphene::chain::custody_proof_type, std::vector<std::string>, &graphene::chain::custody_proof_type::mus>)
            .def_readwrite("sigma", &graphene::chain::custody_proof_type::sigma)
        ;
    }

    {
        bp::scope keys = bp::class_<graphene::chain::deliver_keys_operation>("DeliverKeys", bp::init<>())
            .def("__repr__", object_repr<graphene::chain::deliver_keys_operation>)
            .def_readwrite("fee", &graphene::chain::deliver_keys_operation::fee)
            .def_readwrite("seeder", &graphene::chain::deliver_keys_operation::seeder)
            .def_readwrite("buying", &graphene::chain::deliver_keys_operation::buying)
            .def_readwrite("proof", &graphene::chain::deliver_keys_operation::proof)
            .def_readwrite("key", &graphene::chain::deliver_keys_operation::key)
        ;

        bp::class_<graphene::chain::delivery_proof_type>("Proof", bp::init<>())
            .def("__repr__", object_repr<graphene::chain::delivery_proof_type>)
            .def_readwrite("G1", &graphene::chain::delivery_proof_type::G1)
            .def_readwrite("G2", &graphene::chain::delivery_proof_type::G2)
            .def_readwrite("G3", &graphene::chain::delivery_proof_type::G3)
            .def_readwrite("s", &graphene::chain::delivery_proof_type::s)
            .def_readwrite("r", &graphene::chain::delivery_proof_type::r)
        ;

        bp::class_<graphene::chain::ciphertext_type>("Key", bp::init<>())
            .def("__repr__", object_repr<graphene::chain::ciphertext_type>)
            .def_readwrite("C1", &graphene::chain::ciphertext_type::C1)
            .def_readwrite("D1", &graphene::chain::ciphertext_type::D1)
        ;
    }

    bp::class_<graphene::chain::subscribe_operation>("SubscribeToAuthor", bp::init<>())
        .def("__repr__", object_repr<graphene::chain::subscribe_operation>)
        .def_readwrite("fee", &graphene::chain::subscribe_operation::fee)
        .def_readwrite("consumer", &graphene::chain::subscribe_operation::from)
        .def_readwrite("author", &graphene::chain::subscribe_operation::to)
        .def_readwrite("price", &graphene::chain::subscribe_operation::price)
    ;

    bp::class_<graphene::chain::subscribe_by_author_operation>("SubscribeByAuthor", bp::init<>())
        .def("__repr__", object_repr<graphene::chain::subscribe_by_author_operation>)
        .def_readwrite("fee", &graphene::chain::subscribe_by_author_operation::fee)
        .def_readwrite("consumer", &graphene::chain::subscribe_by_author_operation::from)
        .def_readwrite("author", &graphene::chain::subscribe_by_author_operation::to)
    ;

    bp::class_<graphene::chain::automatic_renewal_of_subscription_operation>("AutomaticRenewalOfSubscription", bp::init<>())
        .def("__repr__", object_repr<graphene::chain::automatic_renewal_of_subscription_operation>)
        .def_readwrite("fee", &graphene::chain::automatic_renewal_of_subscription_operation::fee)
        .def_readwrite("consumer", &graphene::chain::automatic_renewal_of_subscription_operation::consumer)
        .def_readwrite("subscription", &graphene::chain::automatic_renewal_of_subscription_operation::subscription)
        .def_readwrite("automatic_renewal", &graphene::chain::automatic_renewal_of_subscription_operation::automatic_renewal)
    ;

    bp::class_<graphene::chain::report_stats_operation>("ReportStatistics", bp::init<>())
        .def("__repr__", object_repr<graphene::chain::report_stats_operation>)
        .def_readwrite("fee", &graphene::chain::report_stats_operation::fee)
        .def_readwrite("consumer", &graphene::chain::report_stats_operation::consumer)
        .add_property("statistics",
            encode_dict<graphene::chain::report_stats_operation, std::map<graphene::chain::account_id_type, uint64_t>, &graphene::chain::report_stats_operation::stats>,
            decode_dict<graphene::chain::report_stats_operation, std::map<graphene::chain::account_id_type, uint64_t>, &graphene::chain::report_stats_operation::stats>)
    ;

    bp::class_<graphene::chain::set_publishing_manager_operation>("SetPublishingManager", bp::init<>())
        .def("__repr__", object_repr<graphene::chain::set_publishing_manager_operation>)
        .def_readwrite("fee", &graphene::chain::set_publishing_manager_operation::fee)
        .def_readwrite("payer", &graphene::chain::set_publishing_manager_operation::from)
        .add_property("publishers",
            encode_list<graphene::chain::set_publishing_manager_operation, std::vector<graphene::chain::account_id_type>, &graphene::chain::set_publishing_manager_operation::to>,
            decode_list<graphene::chain::set_publishing_manager_operation, std::vector<graphene::chain::account_id_type>, &graphene::chain::set_publishing_manager_operation::to>)
        .def_readwrite("can_create_publishers", &graphene::chain::set_publishing_manager_operation::can_create_publishers)
    ;

    bp::class_<graphene::chain::set_publishing_right_operation>("SetPublishingRight", bp::init<>())
        .def("__repr__", object_repr<graphene::chain::set_publishing_right_operation>)
        .def_readwrite("fee", &graphene::chain::set_publishing_right_operation::fee)
        .def_readwrite("payer", &graphene::chain::set_publishing_right_operation::from)
        .add_property("publishers",
            encode_list<graphene::chain::set_publishing_right_operation, std::vector<graphene::chain::account_id_type>, &graphene::chain::set_publishing_right_operation::to>,
            decode_list<graphene::chain::set_publishing_right_operation, std::vector<graphene::chain::account_id_type>, &graphene::chain::set_publishing_right_operation::to>)
        .def_readwrite("can_publish", &graphene::chain::set_publishing_right_operation::is_publisher)
    ;

    bp::class_<graphene::chain::content_cancellation_operation>("CancelContent", bp::init<>())
        .def("__repr__", object_repr<graphene::chain::content_cancellation_operation>)
        .def_readwrite("fee", &graphene::chain::content_cancellation_operation::fee)
        .def_readwrite("author", &graphene::chain::content_cancellation_operation::author)
        .def_readwrite("uri", &graphene::chain::content_cancellation_operation::URI)
    ;

    bp::class_<graphene::chain::asset_fund_pools_operation>("FundAssetPools", bp::init<>())
        .def("__repr__", object_repr<graphene::chain::asset_fund_pools_operation>)
        .def_readwrite("fee", &graphene::chain::asset_fund_pools_operation::fee)
        .def_readwrite("sender", &graphene::chain::asset_fund_pools_operation::from_account)
        .def_readwrite("user_asset", &graphene::chain::asset_fund_pools_operation::uia_asset)
        .def_readwrite("core_asset", &graphene::chain::asset_fund_pools_operation::dct_asset)
    ;

    bp::class_<graphene::chain::asset_reserve_operation>("ReserveAsset", bp::init<>())
        .def("__repr__", object_repr<graphene::chain::asset_reserve_operation>)
        .def_readwrite("fee", &graphene::chain::asset_reserve_operation::fee)
        .def_readwrite("payer", &graphene::chain::asset_reserve_operation::payer)
        .def_readwrite("amount", &graphene::chain::asset_reserve_operation::amount_to_reserve)
    ;

    bp::class_<graphene::chain::asset_claim_fees_operation>("ClaimAssetFees", bp::init<>())
        .def("__repr__", object_repr<graphene::chain::asset_claim_fees_operation>)
        .def_readwrite("fee", &graphene::chain::asset_claim_fees_operation::fee)
        .def_readwrite("issuer", &graphene::chain::asset_claim_fees_operation::issuer)
        .def_readwrite("user_asset", &graphene::chain::asset_claim_fees_operation::uia_asset)
        .def_readwrite("core_asset", &graphene::chain::asset_claim_fees_operation::dct_asset)
    ;

    bp::class_<graphene::chain::update_user_issued_asset_operation>("UpdateAsset", bp::init<>())
        .def("__repr__", object_repr<graphene::chain::update_user_issued_asset_operation>)
        .def_readwrite("fee", &graphene::chain::update_user_issued_asset_operation::fee)
        .def_readwrite("payer", &graphene::chain::update_user_issued_asset_operation::issuer)
        .def_readwrite("asset", &graphene::chain::update_user_issued_asset_operation::asset_to_update)
        .def_readwrite("description", &graphene::chain::update_user_issued_asset_operation::new_description)
        .add_property("issuer",
            decode_optional_type<graphene::chain::update_user_issued_asset_operation, graphene::chain::account_id_type, &graphene::chain::update_user_issued_asset_operation::new_issuer>,
            encode_optional_type<graphene::chain::update_user_issued_asset_operation, graphene::chain::account_id_type, &graphene::chain::update_user_issued_asset_operation::new_issuer>)
        .def_readwrite("max_supply", &graphene::chain::update_user_issued_asset_operation::max_supply)
        .def_readwrite("core_exchange_rate", &graphene::chain::update_user_issued_asset_operation::core_exchange_rate)
        .def_readwrite("exchangeable", &graphene::chain::update_user_issued_asset_operation::is_exchangeable)
    ;

    bp::class_<graphene::chain::update_monitored_asset_operation>("UpdateMonitoredAsset", bp::init<>())
        .def("__repr__", object_repr<graphene::chain::update_monitored_asset_operation>)
        .def_readwrite("fee", &graphene::chain::update_monitored_asset_operation::fee)
        .def_readwrite("payer", &graphene::chain::update_monitored_asset_operation::issuer)
        .def_readwrite("asset", &graphene::chain::update_monitored_asset_operation::asset_to_update)
        .def_readwrite("description", &graphene::chain::update_monitored_asset_operation::new_description)
        .def_readwrite("feed_lifetime_sec", &graphene::chain::update_monitored_asset_operation::new_feed_lifetime_sec)
        .def_readwrite("minimum_feeds", &graphene::chain::update_monitored_asset_operation::new_minimum_feeds)
    ;

    bp::class_<graphene::chain::ready_to_publish_operation>("ReadyToPublish", bp::init<>())
        .def("__repr__", object_repr<graphene::chain::ready_to_publish_operation>)
        .def_readwrite("fee", &graphene::chain::ready_to_publish_operation::fee)
        .def_readwrite("seeder", &graphene::chain::ready_to_publish_operation::seeder)
        .def_readwrite("public_key", &graphene::chain::ready_to_publish_operation::pubKey)
        .def_readwrite("space", &graphene::chain::ready_to_publish_operation::space)
        .def_readwrite("price", &graphene::chain::ready_to_publish_operation::price_per_MByte)
        .def_readwrite("ipfs_id", &graphene::chain::ready_to_publish_operation::ipfs_ID)
        .add_property("region_code",
            decode_optional_type<graphene::chain::ready_to_publish_operation, std::string, &graphene::chain::ready_to_publish_operation::region_code>,
            encode_optional_type<graphene::chain::ready_to_publish_operation, std::string, &graphene::chain::ready_to_publish_operation::region_code>)
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

    bp::class_<graphene::chain::update_user_issued_asset_advanced_operation>("UpdateAssetAdvanced", bp::init<>())
        .def("__repr__", object_repr<graphene::chain::update_user_issued_asset_advanced_operation>)
        .def_readwrite("fee", &graphene::chain::update_user_issued_asset_advanced_operation::fee)
        .def_readwrite("payer", &graphene::chain::update_user_issued_asset_advanced_operation::issuer)
        .def_readwrite("asset", &graphene::chain::update_user_issued_asset_advanced_operation::asset_to_update)
        .def_readwrite("precision", &graphene::chain::update_user_issued_asset_advanced_operation::new_precision)
        .def_readwrite("fixed_max_supply", &graphene::chain::update_user_issued_asset_advanced_operation::set_fixed_max_supply)
    ;

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
