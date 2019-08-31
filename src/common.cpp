#include "module.hpp"
#include <graphene/db/object.hpp>
#include <graphene/chain/protocol/block.hpp>
#include <graphene/chain/protocol/fee_schedule.hpp>
#include <graphene/utilities/key_conversion.hpp>
#include <graphene/utilities/keys_generator.hpp>
#include <decent/encrypt/encryptionutils.hpp>

namespace dcore {

PyObject *exception_class = nullptr;

void exception_translator(const fc::exception &e)
{
    PyErr_SetString(exception_class, e.to_detail_string().c_str());
}

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

std::string key_to_str(const graphene::chain::private_key_type& key)
{
    return graphene::utilities::key_to_wif(key);
}

std::string derive_el_gamal_key(const graphene::chain::private_key_type& key)
{
    return decent::encrypt::generate_private_el_gamal_key_from_secret(key.get_secret()).to_string();
}

decent::encrypt::DIntegerString generate_el_gamal_key()
{
    return decent::encrypt::generate_private_el_gamal_key();
}

std::string get_public_el_gamal_key(const decent::encrypt::DIntegerString &el_gamal)
{
    return decent::encrypt::get_public_el_gamal_key(el_gamal).to_string();
}

void register_common_types()
{
    std::string scopeName = bp::extract<std::string>(bp::scope().attr("__name__"))() + ".Exception";
    exception_class = PyErr_NewException(scopeName.c_str(), nullptr, nullptr);
    bp::scope().attr("Exception") = bp::handle<>(bp::borrowed(exception_class));
    bp::register_exception_translator<fc::exception>(exception_translator);

    register_hash<fc::ripemd160>("RIPEMD160");
    register_hash<fc::sha256>("SHA256");
    register_hash<fc::sha224>("SHA224");

    bp::class_<fc::uint128>("UInt128", bp::init<>())
        .def(bp::init<uint64_t>())
        .def(bp::init<const std::string&>())
        .def("__repr__", object_repr<fc::uint128>)
        .def("__str__", &fc::uint128::operator std::string)
        .def_readwrite("hi", &fc::uint128::hi)
        .def_readwrite("lo", &fc::uint128::lo)
    ;

    bp::class_<fc::time_point_sec>("TimePointSec", bp::init<>())
        .def("__repr__", &fc::time_point_sec::to_iso_string)
        .def("sec_since_epoch", &fc::time_point_sec::sec_since_epoch)
    ;

    bp::class_<fc::ecc::compact_signature>("CompactSignature", bp::no_init)
        .def("__repr__", object_repr<fc::ecc::compact_signature>)
        .def("__len__", &fc::ecc::compact_signature::size)
    ;

    bp::class_<fc::ecc::public_key>("PublicKey_Raw", bp::no_init)
        .def("__repr__", object_repr<fc::ecc::public_key>)
    ;

    bp::class_<graphene::chain::public_key_type>("PublicKey", bp::init<bp::optional<std::string>>())
        .def(bp::init<const fc::ecc::public_key>())
        .def("__repr__", object_repr<graphene::chain::public_key_type>)
        .def("__str__", &graphene::chain::public_key_type::operator std::string)
        .def("raw", &graphene::chain::public_key_type::operator fc::ecc::public_key)
    ;

    bp::class_<graphene::chain::private_key_type>("PrivateKey", bp::no_init)
        .def("__repr__", object_repr<graphene::chain::private_key_type>)
        .def("__str__", key_to_str)
        .def("get_public_key", &graphene::chain::private_key_type::get_public_key)
        .def("get_shared_secret", &graphene::chain::private_key_type::get_shared_secret)
        .def("sign_compact", &graphene::chain::private_key_type::sign_compact)
        .def("derive_el_gamal_key", derive_el_gamal_key)
        .def("generate", &graphene::chain::private_key_type::generate)
        .staticmethod("generate")
        .def("regenerate", &graphene::chain::private_key_type::regenerate)
        .staticmethod("regenerate")
        .def("generate_from_seed", &graphene::chain::private_key_type::generate_from_seed)
        .staticmethod("generate_from_seed")
    ;

    bp::def("generate_brain_key", &graphene::utilities::generate_brain_key);
    bp::def("derive_private_key", &graphene::utilities::derive_private_key, (bp::arg("brainkey"), bp::arg("sequence") = 0));

    bp::class_<decent::encrypt::DIntegerString>("ElGamalKey", bp::no_init)
        .def("__repr__", object_repr<decent::encrypt::DIntegerString>)
        .def("__str__", bp::make_getter(&decent::encrypt::DIntegerString::s))
        .def("get_public_key", get_public_el_gamal_key)
        .def("generate", generate_el_gamal_key)
        .staticmethod("generate")
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

    bp::class_<graphene::chain::operation>("Operation", bp::no_init)
        .def("__repr__", object_repr<graphene::chain::operation>)
    ;

    bp::class_<graphene::chain::operation_result>("OperationResult", bp::no_init)
        .def("__repr__", object_repr<graphene::chain::operation_result>)
    ;

    bp::class_<graphene::chain::transaction>("Transaction", bp::init<>())
        .def("__repr__", object_repr<graphene::chain::transaction>)
        .def("digest", &graphene::chain::transaction::digest)
        .def("signature_digest", &graphene::chain::transaction::sig_digest)
        .def_readwrite("ref_block_num", &graphene::chain::transaction::ref_block_num)
        .def_readwrite("ref_block_prefix", &graphene::chain::transaction::ref_block_prefix)
        .def_readwrite("expiration", &graphene::chain::transaction::expiration)
        .add_property("operations",
            encode_list<graphene::chain::transaction, std::vector<graphene::chain::operation>, &graphene::chain::transaction::operations>,
            decode_list<graphene::chain::transaction, std::vector<graphene::chain::operation>, &graphene::chain::transaction::operations>)
    ;

    bp::class_<graphene::chain::signed_transaction, bp::bases<graphene::chain::transaction>>("SignedTransaction", bp::init<>())
        .def("__repr__", object_repr<graphene::chain::signed_transaction>)
        .add_property("signatures",
            encode_list<graphene::chain::signed_transaction, std::vector<graphene::chain::signature_type>, &graphene::chain::signed_transaction::signatures>,
            decode_list<graphene::chain::signed_transaction, std::vector<graphene::chain::signature_type>, &graphene::chain::signed_transaction::signatures>)
    ;

    bp::class_<graphene::chain::processed_transaction, bp::bases<graphene::chain::signed_transaction>>("ProcessedTransaction", bp::init<>())
        .def("__repr__", object_repr<graphene::chain::processed_transaction>)
        .def("merkle_digest", &graphene::chain::processed_transaction::merkle_digest)
        .add_property("operation_results",
            encode_list<graphene::chain::processed_transaction, std::vector<graphene::chain::operation_result>, &graphene::chain::processed_transaction::operation_results>,
            decode_list<graphene::chain::processed_transaction, std::vector<graphene::chain::operation_result>, &graphene::chain::processed_transaction::operation_results>)
    ;

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
        .def_readwrite("miner_signature", &graphene::chain::signed_block_header::miner_signature)
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
        .def_readwrite("asset_id", &graphene::chain::asset::asset_id)
    ;

    bp::class_<graphene::chain::price>("Price", bp::init<>())
        .def("__repr__", object_repr<graphene::chain::price>)
        .def_readwrite("base", &graphene::chain::price::base)
        .def_readwrite("quote", &graphene::chain::price::quote)
    ;
}

} // dcore
