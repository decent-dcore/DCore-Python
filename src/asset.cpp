#include "module.hpp"
#include <graphene/chain/asset_object.hpp>

namespace dcore {

template<typename T, typename V, const V T::* instance>
bp::object decode_extension_type(const graphene::chain::asset_options& obj)
{
    auto it = obj.extensions.find(T());
    return it == obj.extensions.end() ? bp::object() : bp::object(((*it).template get<T>().*instance));
}

template<typename T, typename V, V T::* instance>
void encode_extension_type(graphene::chain::asset_options& obj, const V &v)
{
    auto it = obj.extensions.find(T());
    if(it == obj.extensions.end())
        it = obj.extensions.insert(T()).first;
    ((*it).template get<T>().*instance) = v;
}

void register_asset()
{
    bp::class_<graphene::chain::price_feed>("PriceFeed", bp::init<>())
        .def("__repr__", object_repr<graphene::chain::price_feed>)
        .def_readwrite("core_exchange_rate", &graphene::chain::price_feed::core_exchange_rate)
    ;

    typedef std::pair<fc::time_point_sec, graphene::chain::price_feed> price_feed_time;
    bp::class_<price_feed_time>("PriceFeedTime", bp::init<>())
        .def("__repr__", object_repr<price_feed_time>)
        .def_readwrite("time", &price_feed_time::first)
        .def_readwrite("feed", &price_feed_time::second)
    ;

    bp::class_<graphene::chain::monitored_asset_options>("MonitoredAssetOptions", bp::init<>())
        .def("__repr__", object_repr<graphene::chain::monitored_asset_options>)
        .add_property("feeds",
            encode_dict<graphene::chain::monitored_asset_options, boost::container::flat_map<graphene::chain::account_id_type, price_feed_time>, &graphene::chain::monitored_asset_options::feeds>,
            decode_dict<graphene::chain::monitored_asset_options, boost::container::flat_map<graphene::chain::account_id_type, price_feed_time>, &graphene::chain::monitored_asset_options::feeds>)
        .def_readwrite("current_feed", &graphene::chain::monitored_asset_options::current_feed)
        .def_readwrite("current_feed_publication_time", &graphene::chain::monitored_asset_options::current_feed_publication_time)
        .def_readwrite("feed_lifetime_sec", &graphene::chain::monitored_asset_options::feed_lifetime_sec)
        .def_readwrite("minimum_feeds", &graphene::chain::monitored_asset_options::minimum_feeds)
    ;

    bp::class_<graphene::chain::asset_options>("AssetOptions", bp::init<>())
        .def("__repr__", object_repr<graphene::chain::asset_options>)
        .add_property("max_supply",
            decode_safe_type<graphene::chain::asset_options, int64_t, &graphene::chain::asset_options::max_supply>,
            encode_safe_type<graphene::chain::asset_options, int64_t, &graphene::chain::asset_options::max_supply>)
        .def_readwrite("core_exchange_rate", &graphene::chain::asset_options::core_exchange_rate)
        .def_readwrite("is_exchangeable", &graphene::chain::asset_options::is_exchangeable)
        .add_property("is_fixed_max_supply",
            decode_extension_type<graphene::chain::asset_options::fixed_max_supply_struct, bool, &graphene::chain::asset_options::fixed_max_supply_struct::is_fixed_max_supply>,
            encode_extension_type<graphene::chain::asset_options::fixed_max_supply_struct, bool, &graphene::chain::asset_options::fixed_max_supply_struct::is_fixed_max_supply>)
    ;

    bp::class_<object_wrapper<graphene::chain::asset_object>, std::shared_ptr<graphene::chain::asset_object>> asset("Asset", bp::no_init);
    object_wrapper<graphene::chain::asset_object>::wrap(asset)
        .def_readwrite("symbol", &graphene::chain::asset_object::symbol)
        .def_readwrite("precision", &graphene::chain::asset_object::precision)
        .def_readwrite("issuer", &graphene::chain::asset_object::issuer)
        .def_readwrite("description", &graphene::chain::asset_object::description)
        .add_property("monitored_asset_opts",
            decode_optional_type<graphene::chain::asset_object, graphene::chain::monitored_asset_options, &graphene::chain::asset_object::monitored_asset_opts>,
            encode_optional_type<graphene::chain::asset_object, graphene::chain::monitored_asset_options, &graphene::chain::asset_object::monitored_asset_opts>)
        .def_readwrite("options", &graphene::chain::asset_object::options)
        .def_readwrite("dynamic_asset_data_id", &graphene::chain::asset_object::dynamic_asset_data_id)
    ;

    bp::class_<object_wrapper<graphene::chain::asset_dynamic_data_object>, std::shared_ptr<graphene::chain::asset_dynamic_data_object>> data("AssetDynamicData", bp::no_init);
    object_wrapper<graphene::chain::asset_dynamic_data_object>::wrap(data)
        .add_property("current_supply",
            decode_safe_type<graphene::chain::asset_dynamic_data_object, int64_t, &graphene::chain::asset_dynamic_data_object::current_supply>,
            encode_safe_type<graphene::chain::asset_dynamic_data_object, int64_t, &graphene::chain::asset_dynamic_data_object::current_supply>)
        .add_property("asset_pool",
            decode_safe_type<graphene::chain::asset_dynamic_data_object, int64_t, &graphene::chain::asset_dynamic_data_object::asset_pool>,
            encode_safe_type<graphene::chain::asset_dynamic_data_object, int64_t, &graphene::chain::asset_dynamic_data_object::asset_pool>)
        .add_property("core_pool",
            decode_safe_type<graphene::chain::asset_dynamic_data_object, int64_t, &graphene::chain::asset_dynamic_data_object::core_pool>,
            encode_safe_type<graphene::chain::asset_dynamic_data_object, int64_t, &graphene::chain::asset_dynamic_data_object::core_pool>)
    ;
}

} // dcore
