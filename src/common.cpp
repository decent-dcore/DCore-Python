#include "module.hpp"
#include <graphene/db/object.hpp>
#include <graphene/chain/protocol/block.hpp>
#include <graphene/chain/protocol/fee_schedule.hpp>
#include <graphene/chain/budget_record_object.hpp>
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
}

template<typename T, typename R, fc::safe<R> (T::* func)()>
R safe_value(T& obj)
{
   return ((&obj)->*func)().value;
}

std::string key_to_str(const graphene::chain::private_key_type& key)
{
    return graphene::utilities::key_to_wif(key);
}

bp::object key_from_str(const std::string& key)
{
    return encode_optional_value<graphene::chain::private_key_type>(graphene::utilities::wif_to_key(key));
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

graphene::chain::signature_type sign_transaction(graphene::chain::signed_transaction& trx,
                                                 const graphene::chain::private_key_type& key,
                                                 const graphene::chain::chain_id_type& chain_id)
{
    return trx.sign(key, chain_id);
}

graphene::chain::memo_data::message_type get_message(const graphene::chain::memo_data& memo)
{
    return memo.message;
}

void set_message(graphene::chain::memo_data& memo, const graphene::chain::memo_data::message_type& msg)
{
    memo.message = msg;
}

struct memo_converter
{
    static PyObject* convert(const graphene::chain::memo_data::message_type& memo)
    {
        if(!memo.empty())
            return PyByteArray_FromStringAndSize(memo.data(), memo.size());
        Py_RETURN_NONE;
    }

    static void* convertible(PyObject* obj)
    {
        return obj == Py_None || PyByteArray_Check(obj) ? obj : nullptr;
    }

    static void construct(PyObject* obj, bp::converter::rvalue_from_python_stage1_data* data)
    {
        void* storage = ((boost::python::converter::rvalue_from_python_storage<graphene::chain::memo_data::message_type>*)data)->storage.bytes;
        if(PyByteArray_Check(obj)) {
            const char* memo = PyByteArray_AsString(obj);
            new (storage)graphene::chain::memo_data::message_type(memo, memo + PyByteArray_Size(obj));
        }
        else
            new (storage)graphene::chain::memo_data::message_type();

        data->convertible = storage;
    }
};

struct variant_converter
{
    static PyObject* convert(const fc::variant& v)
    {
        if(v.is_bool())
            return PyBool_FromLong(static_cast<long>(v.as_int64()));
        else if(v.is_int64())
            return PyLong_FromLongLong(v.as_int64());
        else if(v.is_uint64())
            return PyLong_FromUnsignedLongLong(v.as_uint64());
        else if(v.is_string())
            return PyUnicode_FromString(v.as_string().c_str());
        Py_RETURN_NONE;
    }

    static void* convertible(PyObject* obj)
    {
        return obj == Py_None || PyBool_Check(obj) || PyLong_Check(obj) || PyUnicode_Check(obj) ? obj : nullptr;
    }

    static void construct(PyObject* obj, bp::converter::rvalue_from_python_stage1_data* data)
    {
        void* storage = ((boost::python::converter::rvalue_from_python_storage<fc::variant>*)data)->storage.bytes;
        if(PyBool_Check(obj))
            new (storage)fc::variant(obj == Py_True);
        else if(PyLong_Check(obj)) {
            int overflow = 0;
            int64_t v = PyLong_AsLongLongAndOverflow(obj, &overflow);
            if(overflow)
                new (storage)fc::variant(PyLong_AsUnsignedLongLong(obj));
            else
                new (storage)fc::variant(v);
        }
        else if(PyUnicode_Check(obj))
            new (storage)fc::variant(PyUnicode_AsUTF8(obj));
        else
            new (storage)fc::variant();

        data->convertible = storage;
    }
};

void register_common_types()
{
    std::string scopeName = bp::extract<std::string>(bp::scope().attr("__name__"))() + ".Exception";
    exception_class = PyErr_NewException(scopeName.c_str(), nullptr, nullptr);
    bp::scope().attr("Exception") = bp::handle<>(bp::borrowed(exception_class));
    bp::register_exception_translator<fc::exception>(exception_translator);

    bp::to_python_converter<fc::variant, variant_converter>();
    bp::converter::registry::push_back(variant_converter::convertible, variant_converter::construct, bp::type_id<fc::variant>());

    bp::to_python_converter<graphene::chain::memo_data::message_type, memo_converter>();
    bp::converter::registry::push_back(memo_converter::convertible, memo_converter::construct, bp::type_id<graphene::chain::memo_data::message_type>());

    register_hash<fc::ripemd160>("RIPEMD160");
    register_hash<fc::sha256>("SHA256");

    bp::class_<fc::uint128>("UInt128", bp::init<>())
        .def(bp::init<uint64_t>())
        .def(bp::init<const std::string&>())
        .def("__repr__", object_repr<fc::uint128>)
        .def("__str__", &fc::uint128::operator std::string)
        .def_readwrite("hi", &fc::uint128::hi)
        .def_readwrite("lo", &fc::uint128::lo)
    ;

    bp::class_<fc::time_point_sec>("TimePointSec", bp::init<>())
        .def(bp::init<uint32_t>())
        .def("__repr__", &fc::time_point_sec::to_iso_string)
        .def("sec_since_epoch", &fc::time_point_sec::sec_since_epoch)
    ;

    bp::class_<fc::ecc::compact_signature>("CompactSignature", bp::no_init)
        .def("__repr__", object_repr<fc::ecc::compact_signature>)
        .def("__len__", &fc::ecc::compact_signature::size)
    ;

    bp::class_<graphene::chain::public_key_type>("PublicKey", bp::init<bp::optional<std::string>>())
        .def("__repr__", object_repr<graphene::chain::public_key_type>)
        .def("__str__", &graphene::chain::public_key_type::operator std::string)
    ;

    bp::class_<graphene::chain::private_key_type>("PrivateKey", bp::no_init)
        .def("__repr__", object_repr<graphene::chain::private_key_type>)
        .def("__str__", key_to_str)
        .def("get_public_key", &graphene::chain::private_key_type::get_public_key)
        .def("get_shared_secret", &graphene::chain::private_key_type::get_shared_secret)
        .def("sign_compact", &graphene::chain::private_key_type::sign_compact)
        .def("derive_el_gamal_key", derive_el_gamal_key)
        .def("from_string", key_from_str)
        .staticmethod("from_string")
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

    bp::class_<graphene::chain::memo_data>("Memo", bp::init<>())
        .def(bp::init<const std::string&, const graphene::chain::private_key_type&, const graphene::chain::public_key_type&, bp::optional<uint64_t>>())
        .def("__repr__", object_repr<graphene::chain::memo_data>)
        .def_readwrite("sender", &graphene::chain::memo_data::from)
        .def_readwrite("receiver", &graphene::chain::memo_data::to)
        .def_readwrite("nonce", &graphene::chain::memo_data::nonce)
        .add_property("message", get_message, set_message)
        .def("get_message", &graphene::chain::memo_data::get_message)
        .def("encrypt_message", graphene::chain::memo_data::encrypt_message)
        .staticmethod("encrypt_message")
        .def("decrypt_message", graphene::chain::memo_data::decrypt_message)
        .staticmethod("decrypt_message")
        .def("generate_nonce", graphene::chain::memo_data::generate_nonce)
        .staticmethod("generate_nonce")
    ;

    bp::class_<graphene::chain::transaction>("Transaction", bp::init<>())
        .def("__repr__", object_repr<graphene::chain::transaction>)
        .def("validate", &graphene::chain::transaction::validate)
        .def("digest", &graphene::chain::transaction::digest)
        .def("signature_digest", &graphene::chain::transaction::sig_digest)
        .def("set_reference_block", &graphene::chain::transaction::set_reference_block)
        .def_readwrite("ref_block_num", &graphene::chain::transaction::ref_block_num)
        .def_readwrite("ref_block_prefix", &graphene::chain::transaction::ref_block_prefix)
        .def_readwrite("expiration", &graphene::chain::transaction::expiration)
        .add_property("id", &graphene::chain::transaction::id)
        .add_property("operations",
            encode_list<graphene::chain::transaction, std::vector<graphene::chain::operation>, &graphene::chain::transaction::operations>,
            decode_list<graphene::chain::transaction, std::vector<graphene::chain::operation>, &graphene::chain::transaction::operations>)
    ;

    bp::class_<graphene::chain::signed_transaction, bp::bases<graphene::chain::transaction>>("SignedTransaction", bp::init<>())
        .def("__repr__", object_repr<graphene::chain::signed_transaction>)
        .def("sign", sign_transaction)
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
        .def("unit_price", graphene::chain::price::unit_price)
        .staticmethod("unit_price")
    ;

    bp::class_<graphene::chain::real_supply>("RealSupply", bp::no_init)
        .def("__repr__", object_repr<graphene::chain::real_supply>)
        .def("total", safe_value<graphene::chain::real_supply, int64_t, &graphene::chain::real_supply::total>)
        .add_property("account_balances", decode_safe_type<graphene::chain::real_supply, int64_t, &graphene::chain::real_supply::account_balances>)
        .add_property("vesting_balances", decode_safe_type<graphene::chain::real_supply, int64_t, &graphene::chain::real_supply::vesting_balances>)
        .add_property("escrows", decode_safe_type<graphene::chain::real_supply, int64_t, &graphene::chain::real_supply::escrows>)
        .add_property("pools", decode_safe_type<graphene::chain::real_supply, int64_t, &graphene::chain::real_supply::pools>)
    ;
}

} // dcore
