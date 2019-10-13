#include "module.hpp"
#include <graphene/chain/protocol/operations.hpp>
#include <graphene/chain/protocol/fee_schedule.hpp>

namespace dcore {

template<typename T>
bp::object decode_operation(const graphene::chain::operation& op)
{
    return op.which() == graphene::chain::operation::tag<T>::value ? bp::object(op.get<T>()) : bp::object();
}

void register_operation()
{
    bp::scope op = bp::class_<graphene::chain::operation>("Operation", bp::no_init)
        .def(bp::init<const graphene::chain::transfer_operation&>())
        .def("__repr__", object_repr<graphene::chain::operation>)
        .def("validate", graphene::chain::operation_validate)
        .add_property("transfer", decode_operation<graphene::chain::transfer_operation>)
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

    bp::class_<graphene::chain::operation_result>("Result", bp::no_init)
        .def("__repr__", object_repr<graphene::chain::operation_result>)
    ;
}

} // dcore
