#include "module.hpp"
#include <graphene/db/object.hpp>
#include <graphene/chain/protocol/block.hpp>
#include <graphene/chain/protocol/fee_schedule.hpp>

namespace dcore {

template<typename T>
void register_hash(const char* name)
{
    bp::class_<T>(name, bp::init<>())
        .def(bp::init<std::string>())
        .def("__repr__", object_repr<T>)
        .def("__str__", &T::operator std::string)
        .def("__hash__", object_hash<T>)
    ;
}

template<typename T>
void register_object_id(const char* name)
{
    bp::class_<T>(name, bp::init<uint64_t>())
        .def(bp::init<graphene::db::object_id_type>())
        .def("__repr__", object_repr<T>)
        .def("__str__", object_id_str<T>)
        .def("__hash__", &T::operator uint64_t)
        .def_readonly("object_id", &T::operator graphene::db::object_id_type)
    ;
};

void register_common_types()
{
    register_hash<fc::ripemd160>("RIPEMD160");
    register_hash<fc::sha256>("SHA256");
    register_hash<fc::sha224>("SHA224");

    bp::class_<fc::time_point_sec>("TimePointSec", bp::init<>())
        .def("__repr__", &fc::time_point_sec::to_iso_string)
        .def("sec_since_epoch", &fc::time_point_sec::sec_since_epoch)
    ;

    bp::class_<fc::ecc::compact_signature>("CompactSignature", bp::no_init)
        .def("__repr__", object_repr<fc::ecc::compact_signature>)
    ;

    bp::class_<fc::ecc::public_key>("PublicKey_Raw", bp::no_init)
        .def("__repr__", object_repr<fc::ecc::public_key>)
    ;

    bp::class_<graphene::chain::public_key_type>("PublicKey", bp::no_init)
        .def("__repr__", object_repr<graphene::chain::public_key_type>)
    ;

    bp::class_<graphene::chain::private_key_type>("PrivateKey", bp::no_init)
        .def("__repr__", object_repr<graphene::chain::private_key_type>)
        .def("get_public_key", &graphene::chain::private_key_type::get_public_key)
        .def("get_shared_secret", &graphene::chain::private_key_type::get_shared_secret)
    ;

    bp::class_<graphene::db::object_id_type>("ObjectId", bp::init<uint8_t, uint8_t, uint8_t>())
        .def(bp::init<std::string>())
        .def("__repr__", object_repr<graphene::db::object_id_type>)
        .def("__str__", &graphene::db::object_id_type::operator std::string)
        .def("__hash__", &graphene::db::object_id_type::operator uint64_t)
        .def_readonly("space", &graphene::db::object_id_type::space)
        .def_readonly("type", &graphene::db::object_id_type::type)
        .def_readonly("instance", &graphene::db::object_id_type::instance)
        .def("is_null", &graphene::db::object_id_type::is_null)
    ;

    register_object_id<graphene::chain::account_id_type>("AccountId");
    register_object_id<graphene::chain::asset_id_type>("AssetId");
    register_object_id<graphene::chain::miner_id_type>("MinerId");
    register_object_id<graphene::chain::proposal_id_type>("ProposalId");
    register_object_id<graphene::chain::operation_history_id_type>("OperationHistoryId");
    register_object_id<graphene::chain::withdraw_permission_id_type>("WithdrawPermissionId");
    register_object_id<graphene::chain::vesting_balance_id_type>("VestingBalanceId");
    register_object_id<graphene::chain::non_fungible_token_id_type>("NonFungibleTokenId");
    register_object_id<graphene::chain::non_fungible_token_data_id_type>("NonFungibleTokenDataId");

    register_object_id<graphene::chain::global_property_id_type>("GlobalPropertyId");
    register_object_id<graphene::chain::dynamic_global_property_id_type>("DynamicGlobalPropertyId");
    register_object_id<graphene::chain::asset_dynamic_data_id_type>("AssetDynamicDataId");
    register_object_id<graphene::chain::account_balance_id_type>("AccountBalanceId");
    register_object_id<graphene::chain::account_statistics_id_type>("AccountStatisticsId");
    register_object_id<graphene::chain::transaction_obj_id_type>("TransactionId");
    register_object_id<graphene::chain::block_summary_id_type>("BlockSummaryId");
    register_object_id<graphene::chain::account_transaction_history_id_type>("AccountTransactionHistoryId");
    register_object_id<graphene::chain::chain_property_id_type>("ChainPropertyId");
    register_object_id<graphene::chain::miner_schedule_id_type>("MinerScheduleId");
    register_object_id<graphene::chain::budget_record_id_type>("BudgetRecordId");
    register_object_id<graphene::chain::buying_id_type>("BuyingId");
    register_object_id<graphene::chain::content_id_type>("ContentId");
    register_object_id<graphene::chain::publisher_id_type>("SeederId");
    register_object_id<graphene::chain::subscription_id_type>("SubscriptionId");
    register_object_id<graphene::chain::seeding_statistics_id_type>("SeedingStatisticsId");
    register_object_id<graphene::chain::transaction_detail_id_type>("TransactionDetailId");
    register_object_id<graphene::chain::message_id_type>("MessageId");
    register_object_id<graphene::chain::transaction_history_id_type>("TransactionHistoryId");

    bp::class_<graphene::chain::block_header>("BlockHeader", bp::init<>())
        .def("__repr__", object_repr<graphene::chain::block_header>)
        .def("digest", &graphene::chain::block_header::digest)
        .def("block_num", &graphene::chain::block_header::block_num)
        .def("num_from_id", &graphene::chain::block_header::num_from_id)
        .staticmethod("num_from_id")
        .def_readwrite("previous", &graphene::chain::block_header::previous)
        .def_readwrite("timestamp", &graphene::chain::block_header::timestamp)
        .def_readwrite("miner", &graphene::chain::block_header::miner)
        .def_readwrite("transaction_merkle_root", &graphene::chain::block_header::transaction_merkle_root)
    ;

    bp::class_<graphene::chain::signed_block_header, bp::bases<graphene::chain::block_header>>("SignedBlockHeader", bp::init<>())
        .def("__repr__", object_repr<graphene::chain::signed_block_header>)
        .def("id", &graphene::chain::signed_block_header::id)
        .def("signee", &graphene::chain::signed_block_header::signee)
        .def("sign", &graphene::chain::signed_block_header::sign)
        .def("validate_signee", &graphene::chain::signed_block_header::validate_signee)
        .add_property("miner_signature", &graphene::chain::signed_block_header::miner_signature)
    ;

    bp::class_<graphene::chain::signed_block, bp::bases<graphene::chain::signed_block_header>>("SignedBlock", bp::init<>())
        .def("__repr__", object_repr<graphene::chain::signed_block>)
        .def("calculate_merkle_root", &graphene::chain::signed_block::calculate_merkle_root)
        .add_property("transactions", encode_list<graphene::chain::signed_block, std::vector<graphene::chain::processed_transaction>, &graphene::chain::signed_block::transactions>)
    ;

    bp::class_<graphene::chain::signed_block_with_info, bp::bases<graphene::chain::signed_block>>("SignedBlockInfo", bp::init<>())
        .def("__repr__", object_repr<graphene::chain::signed_block_with_info>)
        .add_property("transaction_ids", encode_list<graphene::chain::signed_block_with_info,
                                                     std::vector<graphene::chain::transaction_id_type>,
                                                     &graphene::chain::signed_block_with_info::transaction_ids>)
        .add_property("miner_reward", decode_safe_type<graphene::chain::signed_block_with_info, int64_t, &graphene::chain::signed_block_with_info::miner_reward>)
    ;

    bp::class_<graphene::chain::asset>("Balance", bp::init<>())
        .def("__repr__", object_repr<graphene::chain::asset>)
        .add_property("amount", decode_safe_type<graphene::chain::asset, int64_t, &graphene::chain::asset::amount>,
                                encode_safe_type<graphene::chain::asset, int64_t, &graphene::chain::asset::amount>)
        .add_property("asset_id", &graphene::chain::asset::asset_id)
    ;
}

} // dcore
