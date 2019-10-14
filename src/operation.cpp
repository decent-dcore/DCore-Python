#include "module.hpp"
#include <graphene/chain/protocol/operations.hpp>
#include <graphene/chain/protocol/fee_schedule.hpp>

namespace dcore {

template<typename T>
bp::object decode_operation(const graphene::chain::operation& op)
{
    return op.which() == graphene::chain::operation::tag<T>::value ? bp::object(op.get<T>()) : bp::object();
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
