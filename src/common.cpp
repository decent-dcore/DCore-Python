#include "module.hpp"
#include <graphene/chain/protocol/block.hpp>
#include <graphene/chain/protocol/fee_schedule.hpp>

namespace dcore {

void register_common_types()
{
    register_hash<fc::ripemd160>("RIPEMD160");
    register_hash<fc::sha256>("SHA256");

    bp::class_<fc::ecc::compact_signature>("CompactSignature", bp::no_init)
        .def("__repr__", object_repr<fc::ecc::compact_signature>)
    ;

    bp::class_<fc::ecc::public_key>("PublicKey", bp::no_init)
        .def("__repr__", object_repr<fc::ecc::public_key>)
    ;

    bp::class_<fc::ecc::private_key>("PrivateKey", bp::no_init)
        .def("__repr__", object_repr<fc::ecc::private_key>)
        .def("get_public_key", &fc::ecc::private_key::get_public_key)
        .def("get_shared_secret", &fc::ecc::private_key::get_shared_secret)
    ;

    bp::class_<fc::time_point_sec>("TimePointSec", bp::init<>())
        .def("__repr__", &fc::time_point_sec::to_iso_string)
        .def("sec_since_epoch", &fc::time_point_sec::sec_since_epoch)
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

    register_object_id<graphene::chain::miner_id_type>("MinerId");

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
        //.add_property("transactions", encode_list<graphene::chain::signed_block,
        //                                          std::vector<graphene::chain::processed_transaction>,
        //                                          &graphene::chain::signed_block::processed_transaction>)
    ;

    bp::class_<graphene::chain::signed_block_with_info, bp::bases<graphene::chain::signed_block>>("SignedBlockInfo", bp::init<>())
        .def("__repr__", object_repr<graphene::chain::signed_block_with_info>)
        .add_property("transaction_ids", encode_list<graphene::chain::signed_block_with_info,
                                                     std::vector<graphene::chain::transaction_id_type>,
                                                     &graphene::chain::signed_block_with_info::transaction_ids>)
        .add_property("miner_reward", decode_safe_type<int64_t>)
    ;
}

} // dcore
