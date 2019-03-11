
#pragma once

#include "lib/graft/graft_macros.h"

#include "lib/graft/reflective-rapidjson/reflector-boosthana.h"
#include "lib/graft/reflective-rapidjson/serializable.h"
#include "lib/graft/reflective-rapidjson/types.h"

#include <boost/hana.hpp>

#define GRAFT_DEFINE_IO_STRUCT(__S__, ...) \
    struct __S__ : public ReflectiveRapidJSON::JsonSerializable<__S__> { \
	BOOST_HANA_DEFINE_STRUCT(__S__, __VA_ARGS__); \
    }

#define GRAFT_DEFINE_IO_STRUCT_INITED(__S__, ...) \
    struct __S__ : public ReflectiveRapidJSON::JsonSerializable<__S__> { \
        __S__() : INIT_PAIRS(__VA_ARGS__) {} \
	BOOST_HANA_DEFINE_STRUCT(__S__, TN_PAIRS(__VA_ARGS__)); \
    }

/*
 *  Mapping of supported C++ types to supported JSON types
 *  ========================================================================
 *                           C++ type                        |  JSON type
 *  ---------------------------------------------------------+--------------
 *   custom structures/classes                               | object
 *   bool                                                    | true/false
 *   signed and unsigned integral types                      | number
 *   float and double                                        | number
 *   enum and enum class                                     | number
 *   std::string                                             | string
 *   const char *                                            | string
 *   iteratable lists (std::vector, std::list, ...)          | array
 *   sets (std::set, std::unordered_set, std::multiset, ...) | array
 *   std::tuple                                              | array
 *   std::unique_ptr, std::shared_ptr                        | depends/null
 *   std::map, std::unordered_map                            | object
 *   JsonSerializable                                        | object
 *  ---------------------------------------------------------+--------------
 *
 *  Example of structure definitions:
 *  =================================
 *
 *  GRAFT_DEFINE_IO_STRUCT(Payment,
 *      (uint64, amount),
 *      (uint32, block_height),
 *      (std::string, payment_id),
 *      (std::string, tx_hash),
 *      (uint32, unlock_time)
 * );
 *
 * or initialized with default values
 *
 *  GRAFT_DEFINE_IO_STRUCT_INITED(Payment,
 *      (uint64, amount, 999),
 *      (uint32, block_height, 10000),
 *      (std::string, payment_id, "abc"),
 *      (std::string, tx_hash, "def"),
 *      (uint32, unlock_time, 555555)
 * );
 *
 * GRAFT_DEFINE_IO_STRUCT(Payments,
 *     (std::vector<Payment>, payments)
 * );
 */

namespace graft::serializer
{
    class JsonParseError : public std::runtime_error
    {
    public:
        JsonParseError(const rapidjson::ParseResult &pr)
            : std::runtime_error( std::string("Json parse error, code: ") + std::to_string(pr.Code())
                                  + ", offset: " + std::to_string(pr.Offset()))
        { }
    };
} //namespace graft::serializer
