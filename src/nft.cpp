#include "module.hpp"
#include <graphene/chain/non_fungible_token_object.hpp>

namespace dcore {

void register_nft()
{
    typedef decltype(graphene::chain::non_fungible_token_data_type::nobody) nft_data_modification;
    bp::enum_<nft_data_modification>("NonFungibleTokenDataModification")
        .value("Nobody", nft_data_modification::nobody)
        .value("Issuer", nft_data_modification::issuer)
        .value("Owner", nft_data_modification::owner)
        .value("Both", nft_data_modification::both)
    ;

    typedef decltype(graphene::chain::non_fungible_token_data_type::string) nft_data_type;
    bp::enum_<nft_data_type>("NonFungibleTokenDataType")
        .value("String", nft_data_type::string)
        .value("Integer", nft_data_type::integer)
        .value("Boolean", nft_data_type::boolean)
    ;

    typedef std::pair<std::string, fc::variant> nft_data_value;
    bp::class_<nft_data_value>("NonFungibleTokenDataValue", bp::init<>())
        .def(bp::init<std::string, fc::variant>())
        .def("__repr__", object_repr<nft_data_value>)
        .def_readwrite("name", &nft_data_value::first)
        .def_readwrite("value", &nft_data_value::second)
    ;

    bp::class_<graphene::chain::non_fungible_token_data_type>("NonFungibleTokenDataDefinition", bp::init<>())
        .def("__repr__", object_repr<graphene::chain::non_fungible_token_data_type>)
        .def_readwrite("unique", &graphene::chain::non_fungible_token_data_type::unique)
        .def_readwrite("modifiable", &graphene::chain::non_fungible_token_data_type::modifiable)
        .def_readwrite("type", &graphene::chain::non_fungible_token_data_type::type)
        .add_property("name",
            decode_optional_type<graphene::chain::non_fungible_token_data_type, std::string, &graphene::chain::non_fungible_token_data_type::name>,
            encode_optional_type<graphene::chain::non_fungible_token_data_type, std::string, &graphene::chain::non_fungible_token_data_type::name>)
    ;

    bp::class_<graphene::chain::non_fungible_token_options>("NonFungibleTokenOptions", bp::init<>())
        .def("__repr__", object_repr<graphene::chain::non_fungible_token_options>)
        .def_readwrite("issuer", &graphene::chain::non_fungible_token_options::issuer)
        .def_readwrite("max_supply", &graphene::chain::non_fungible_token_options::max_supply)
        .def_readwrite("fixed_max_supply", &graphene::chain::non_fungible_token_options::fixed_max_supply)
        .def_readwrite("description", &graphene::chain::non_fungible_token_options::description)
    ;

    bp::class_<object_wrapper<graphene::chain::non_fungible_token_object>, std::shared_ptr<graphene::chain::non_fungible_token_object>> asset("NonFungibleToken", bp::no_init);
    object_wrapper<graphene::chain::non_fungible_token_object>::wrap(asset)
        .def_readwrite("symbol", &graphene::chain::non_fungible_token_object::symbol)
        .def_readwrite("options", &graphene::chain::non_fungible_token_object::options)
        .add_property("definitions",
            encode_list<graphene::chain::non_fungible_token_object, graphene::chain::non_fungible_token_data_definitions, &graphene::chain::non_fungible_token_object::definitions>,
            decode_list<graphene::chain::non_fungible_token_object, graphene::chain::non_fungible_token_data_definitions, &graphene::chain::non_fungible_token_object::definitions>)
        .def_readwrite("transferable", &graphene::chain::non_fungible_token_object::transferable)
        .def_readwrite("current_supply", &graphene::chain::non_fungible_token_object::current_supply)
    ;

    bp::class_<object_wrapper<graphene::chain::non_fungible_token_data_object>, std::shared_ptr<graphene::chain::non_fungible_token_data_object>> data("NonFungibleTokenData", bp::no_init);
    object_wrapper<graphene::chain::non_fungible_token_data_object>::wrap(data)
        .def_readwrite("nft_id", &graphene::chain::non_fungible_token_data_object::nft_id)
        .def_readwrite("owner", &graphene::chain::non_fungible_token_data_object::owner)
        .add_property("data",
            encode_list<graphene::chain::non_fungible_token_data_object, fc::variants, &graphene::chain::non_fungible_token_data_object::data>,
            decode_list<graphene::chain::non_fungible_token_data_object, fc::variants, &graphene::chain::non_fungible_token_data_object::data>)
    ;
}

} // dcore
