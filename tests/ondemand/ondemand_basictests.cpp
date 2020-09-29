#include <cinttypes>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <string>
#include <vector>
#include <cmath>
#include <set>
#include <sstream>
#include <utility>
#include <ciso646>
#include <unistd.h>

#include "simdjson.h"
#include "cast_tester.h"
#include "test_macros.h"

// const size_t AMAZON_CELLPHONES_NDJSON_DOC_COUNT = 793;
#define SIMDJSON_SHOW_DEFINE(x) printf("%s=%s\n", #x, STRINGIFY(x))

using namespace simdjson;
using namespace simdjson::builtin;

namespace number_tests {

  // ulp distance
  // Marc B. Reynolds, 2016-2019
  // Public Domain under http://unlicense.org, see link for details.
  // adapted by D. Lemire
  inline uint64_t f64_ulp_dist(double a, double b) {
    uint64_t ua, ub;
    memcpy(&ua, &a, sizeof(ua));
    memcpy(&ub, &b, sizeof(ub));
    if ((int64_t)(ub ^ ua) >= 0)
      return (int64_t)(ua - ub) >= 0 ? (ua - ub) : (ub - ua);
    return ua + ub + 0x80000000;
  }


  bool small_integers() {
    std::cout << __func__ << std::endl;
    ondemand::parser parser;
    for (int m = 10; m < 20; m++) {
      for (int i = -1024; i < 1024; i++) {
        padded_string json = std::to_string(i);
        auto doc = parser.iterate(json);
        int64_t actual;
        ASSERT_SUCCESS(doc.get(actual));
        if (actual != i) {
          std::cerr << "JSON '" << json << "' parsed to " << actual << " instead of " << i << std::endl;
          return false;
        }
      }
    }
    return true;
  }

  bool powers_of_two() {
    std::cout << __func__ << std::endl;
    char buf[1024];
    ondemand::parser parser;
    uint64_t maxulp = 0;
    for (int i = -1075; i < 1024; ++i) {// large negative values should be zero.
      double expected = pow(2, i);
      size_t n = snprintf(buf, sizeof(buf), "%.*e", std::numeric_limits<double>::max_digits10 - 1, expected);
      if (n >= sizeof(buf)) { abort(); }
      fflush(NULL);
      double actual;
      padded_string json(buf, n);
      auto doc = parser.iterate(json);
      auto error = doc.get(actual);
      if (error) { std::cerr << error << std::endl; return false; }
      uint64_t ulp = f64_ulp_dist(actual,expected);
      if(ulp > maxulp) maxulp = ulp;
      if(ulp > 0) {
        std::cerr << "JSON '" << json << " parsed to ";
        fprintf( stderr," %18.18g instead of %18.18g\n", actual, expected); // formatting numbers is easier with printf
        SIMDJSON_SHOW_DEFINE(FLT_EVAL_METHOD);
        return false;
      }
    }
    return true;
  }

  static const double testing_power_of_ten[] = {
      1e-307, 1e-306, 1e-305, 1e-304, 1e-303, 1e-302, 1e-301, 1e-300, 1e-299,
      1e-298, 1e-297, 1e-296, 1e-295, 1e-294, 1e-293, 1e-292, 1e-291, 1e-290,
      1e-289, 1e-288, 1e-287, 1e-286, 1e-285, 1e-284, 1e-283, 1e-282, 1e-281,
      1e-280, 1e-279, 1e-278, 1e-277, 1e-276, 1e-275, 1e-274, 1e-273, 1e-272,
      1e-271, 1e-270, 1e-269, 1e-268, 1e-267, 1e-266, 1e-265, 1e-264, 1e-263,
      1e-262, 1e-261, 1e-260, 1e-259, 1e-258, 1e-257, 1e-256, 1e-255, 1e-254,
      1e-253, 1e-252, 1e-251, 1e-250, 1e-249, 1e-248, 1e-247, 1e-246, 1e-245,
      1e-244, 1e-243, 1e-242, 1e-241, 1e-240, 1e-239, 1e-238, 1e-237, 1e-236,
      1e-235, 1e-234, 1e-233, 1e-232, 1e-231, 1e-230, 1e-229, 1e-228, 1e-227,
      1e-226, 1e-225, 1e-224, 1e-223, 1e-222, 1e-221, 1e-220, 1e-219, 1e-218,
      1e-217, 1e-216, 1e-215, 1e-214, 1e-213, 1e-212, 1e-211, 1e-210, 1e-209,
      1e-208, 1e-207, 1e-206, 1e-205, 1e-204, 1e-203, 1e-202, 1e-201, 1e-200,
      1e-199, 1e-198, 1e-197, 1e-196, 1e-195, 1e-194, 1e-193, 1e-192, 1e-191,
      1e-190, 1e-189, 1e-188, 1e-187, 1e-186, 1e-185, 1e-184, 1e-183, 1e-182,
      1e-181, 1e-180, 1e-179, 1e-178, 1e-177, 1e-176, 1e-175, 1e-174, 1e-173,
      1e-172, 1e-171, 1e-170, 1e-169, 1e-168, 1e-167, 1e-166, 1e-165, 1e-164,
      1e-163, 1e-162, 1e-161, 1e-160, 1e-159, 1e-158, 1e-157, 1e-156, 1e-155,
      1e-154, 1e-153, 1e-152, 1e-151, 1e-150, 1e-149, 1e-148, 1e-147, 1e-146,
      1e-145, 1e-144, 1e-143, 1e-142, 1e-141, 1e-140, 1e-139, 1e-138, 1e-137,
      1e-136, 1e-135, 1e-134, 1e-133, 1e-132, 1e-131, 1e-130, 1e-129, 1e-128,
      1e-127, 1e-126, 1e-125, 1e-124, 1e-123, 1e-122, 1e-121, 1e-120, 1e-119,
      1e-118, 1e-117, 1e-116, 1e-115, 1e-114, 1e-113, 1e-112, 1e-111, 1e-110,
      1e-109, 1e-108, 1e-107, 1e-106, 1e-105, 1e-104, 1e-103, 1e-102, 1e-101,
      1e-100, 1e-99,  1e-98,  1e-97,  1e-96,  1e-95,  1e-94,  1e-93,  1e-92,
      1e-91,  1e-90,  1e-89,  1e-88,  1e-87,  1e-86,  1e-85,  1e-84,  1e-83,
      1e-82,  1e-81,  1e-80,  1e-79,  1e-78,  1e-77,  1e-76,  1e-75,  1e-74,
      1e-73,  1e-72,  1e-71,  1e-70,  1e-69,  1e-68,  1e-67,  1e-66,  1e-65,
      1e-64,  1e-63,  1e-62,  1e-61,  1e-60,  1e-59,  1e-58,  1e-57,  1e-56,
      1e-55,  1e-54,  1e-53,  1e-52,  1e-51,  1e-50,  1e-49,  1e-48,  1e-47,
      1e-46,  1e-45,  1e-44,  1e-43,  1e-42,  1e-41,  1e-40,  1e-39,  1e-38,
      1e-37,  1e-36,  1e-35,  1e-34,  1e-33,  1e-32,  1e-31,  1e-30,  1e-29,
      1e-28,  1e-27,  1e-26,  1e-25,  1e-24,  1e-23,  1e-22,  1e-21,  1e-20,
      1e-19,  1e-18,  1e-17,  1e-16,  1e-15,  1e-14,  1e-13,  1e-12,  1e-11,
      1e-10,  1e-9,   1e-8,   1e-7,   1e-6,   1e-5,   1e-4,   1e-3,   1e-2,
      1e-1,   1e0,    1e1,    1e2,    1e3,    1e4,    1e5,    1e6,    1e7,
      1e8,    1e9,    1e10,   1e11,   1e12,   1e13,   1e14,   1e15,   1e16,
      1e17,   1e18,   1e19,   1e20,   1e21,   1e22,   1e23,   1e24,   1e25,
      1e26,   1e27,   1e28,   1e29,   1e30,   1e31,   1e32,   1e33,   1e34,
      1e35,   1e36,   1e37,   1e38,   1e39,   1e40,   1e41,   1e42,   1e43,
      1e44,   1e45,   1e46,   1e47,   1e48,   1e49,   1e50,   1e51,   1e52,
      1e53,   1e54,   1e55,   1e56,   1e57,   1e58,   1e59,   1e60,   1e61,
      1e62,   1e63,   1e64,   1e65,   1e66,   1e67,   1e68,   1e69,   1e70,
      1e71,   1e72,   1e73,   1e74,   1e75,   1e76,   1e77,   1e78,   1e79,
      1e80,   1e81,   1e82,   1e83,   1e84,   1e85,   1e86,   1e87,   1e88,
      1e89,   1e90,   1e91,   1e92,   1e93,   1e94,   1e95,   1e96,   1e97,
      1e98,   1e99,   1e100,  1e101,  1e102,  1e103,  1e104,  1e105,  1e106,
      1e107,  1e108,  1e109,  1e110,  1e111,  1e112,  1e113,  1e114,  1e115,
      1e116,  1e117,  1e118,  1e119,  1e120,  1e121,  1e122,  1e123,  1e124,
      1e125,  1e126,  1e127,  1e128,  1e129,  1e130,  1e131,  1e132,  1e133,
      1e134,  1e135,  1e136,  1e137,  1e138,  1e139,  1e140,  1e141,  1e142,
      1e143,  1e144,  1e145,  1e146,  1e147,  1e148,  1e149,  1e150,  1e151,
      1e152,  1e153,  1e154,  1e155,  1e156,  1e157,  1e158,  1e159,  1e160,
      1e161,  1e162,  1e163,  1e164,  1e165,  1e166,  1e167,  1e168,  1e169,
      1e170,  1e171,  1e172,  1e173,  1e174,  1e175,  1e176,  1e177,  1e178,
      1e179,  1e180,  1e181,  1e182,  1e183,  1e184,  1e185,  1e186,  1e187,
      1e188,  1e189,  1e190,  1e191,  1e192,  1e193,  1e194,  1e195,  1e196,
      1e197,  1e198,  1e199,  1e200,  1e201,  1e202,  1e203,  1e204,  1e205,
      1e206,  1e207,  1e208,  1e209,  1e210,  1e211,  1e212,  1e213,  1e214,
      1e215,  1e216,  1e217,  1e218,  1e219,  1e220,  1e221,  1e222,  1e223,
      1e224,  1e225,  1e226,  1e227,  1e228,  1e229,  1e230,  1e231,  1e232,
      1e233,  1e234,  1e235,  1e236,  1e237,  1e238,  1e239,  1e240,  1e241,
      1e242,  1e243,  1e244,  1e245,  1e246,  1e247,  1e248,  1e249,  1e250,
      1e251,  1e252,  1e253,  1e254,  1e255,  1e256,  1e257,  1e258,  1e259,
      1e260,  1e261,  1e262,  1e263,  1e264,  1e265,  1e266,  1e267,  1e268,
      1e269,  1e270,  1e271,  1e272,  1e273,  1e274,  1e275,  1e276,  1e277,
      1e278,  1e279,  1e280,  1e281,  1e282,  1e283,  1e284,  1e285,  1e286,
      1e287,  1e288,  1e289,  1e290,  1e291,  1e292,  1e293,  1e294,  1e295,
      1e296,  1e297,  1e298,  1e299,  1e300,  1e301,  1e302,  1e303,  1e304,
      1e305,  1e306,  1e307,  1e308};



  bool powers_of_ten() {
    std::cout << __func__ << std::endl;
    char buf[1024];
    ondemand::parser parser;

    bool is_pow_correct{1e-308 == std::pow(10,-308)};
    int start_point = is_pow_correct ? -10000 : -307;
    if(!is_pow_correct) {
      std::cout << "On your system, the pow function is busted. Sorry about that. " << std::endl;
    }
    for (int i = start_point; i <= 308; ++i) {// large negative values should be zero.
      size_t n = snprintf(buf, sizeof(buf), "1e%d", i);
      if (n >= sizeof(buf)) { abort(); }
      fflush(NULL);
      padded_string json(buf, n);
      auto doc = parser.iterate(json);
      double actual;
      auto error = doc.get(actual);
      if (error) { std::cerr << error << std::endl; return false; }
      double expected = ((i >= -307) ? testing_power_of_ten[i + 307]: std::pow(10, i));
      int ulp = (int) f64_ulp_dist(actual, expected);
      if(ulp > 0) {
        std::cerr << "JSON '" << json << " parsed to ";
        fprintf( stderr," %18.18g instead of %18.18g\n", actual, expected); // formatting numbers is easier with printf
        SIMDJSON_SHOW_DEFINE(FLT_EVAL_METHOD);
        return false;
      }
    }
    printf("Powers of 10 can be parsed.\n");
    return true;
  }
  bool run() {
    return small_integers() &&
           powers_of_two() &&
           powers_of_ten();
  }
}


namespace parse_api_tests {
  using namespace std;
  using namespace simdjson;
  using namespace simdjson::dom;

  const padded_string BASIC_JSON = "[1,2,3]"_padded;
  const padded_string BASIC_NDJSON = "[1,2,3]\n[4,5,6]"_padded;
  const padded_string EMPTY_NDJSON = ""_padded;

  bool parser_iterate() {
    std::cout << "Running " << __func__ << std::endl;
    ondemand::parser parser;
    auto doc = parser.iterate(BASIC_JSON);
    ASSERT_SUCCESS( doc.get_array() );
    return true;
  }

// #if SIMDJSON_EXCEPTIONS
//   bool parser_iterate_exception() {
//     std::cout << "Running " << __func__ << std::endl;
//     ondemand::parser parser;
//     auto doc = parser.iterate(BASIC_JSON);
//     SIMDJSON_UNUSED ondemand::array array = doc;
//     return true;
//   }
// #endif

  bool run() {
    return parser_iterate() &&
// #if SIMDJSON_EXCEPTIONS
//            parser_iterate_exception() &&
// #endif
           true;
  }
}

namespace dom_api_tests {
  using namespace std;
  using namespace simdjson;
  using namespace simdjson::dom;

  bool object_iterator() {
    std::cout << "Running " << __func__ << std::endl;
    auto json = R"({ "a": 1, "b": 2, "c": 3 })"_padded;
    const char* expected_key[] = { "a", "b", "c" };
    uint64_t expected_value[] = { 1, 2, 3 };

    ondemand::parser parser;
    auto doc = parser.iterate(json);
    ondemand::object object;
    ASSERT_SUCCESS( doc.get(object) );
    int i = 0;
    for (auto [ field, error ] : object) {
      ASSERT_SUCCESS(error);
      ASSERT( field.key() == expected_key[i] , "Keys not equal" );
      // ASSERT_EQUAL( field.key(), expected_key[i] );
      ASSERT_EQUAL( field.value().get<uint64_t>().first, expected_value[i] );
      i++;
    }
    ASSERT_EQUAL( i*sizeof(uint64_t), sizeof(expected_value) );
    return true;
  }

  bool array_iterator() {
    std::cout << "Running " << __func__ << std::endl;
    auto json = R"([ 1, 10, 100 ])"_padded;
    uint64_t expected_value[] = { 1, 10, 100 };

    ondemand::parser parser;
    auto doc = parser.iterate(json);
    ondemand::array array;
    ASSERT_SUCCESS( doc.get(array) );
    int i=0;
    for (auto value : array) {
      uint64_t v;
      ASSERT_SUCCESS( value.get(v) );
      ASSERT_EQUAL( v, expected_value[i] );
      i++;
    }
    ASSERT_EQUAL( i*sizeof(uint64_t), sizeof(expected_value) );
    return true;
  }

//   bool object_iterator_empty() {
//     std::cout << "Running " << __func__ << std::endl;
//     auto json = R"({})"_padded;
//     int i = 0;

//     ondemand::parser parser;
//     ondemand::object object;
//     ASSERT_SUCCESS( parser.iterate(json).get(object) );
//     for (SIMDJSON_UNUSED auto field : object) {
//       TEST_FAIL("Unexpected field");
//       i++;
//     }
//     ASSERT_EQUAL(i, 0);
//     return true;
//   }

//   bool array_iterator_empty() {
//     std::cout << "Running " << __func__ << std::endl;
//     auto json = R"([])"_padded;
//     int i=0;

//     ondemand::parser parser;
//     ondemand::array array;
//     ASSERT_SUCCESS( parser.iterate(json).get(array) );
//     for (SIMDJSON_UNUSED auto value : array) {
//       TEST_FAIL("Unexpected value");
//       i++;
//     }
//     ASSERT_EQUAL(i, 0);
//     return true;
//   }

//   bool string_value() {
//     std::cout << "Running " << __func__ << std::endl;
//     auto json = R"([ "hi", "has backslash\\" ])"_padded;
//     ondemand::parser parser;
//     ondemand::array array;
//     ASSERT_SUCCESS( parser.iterate(json).get(array) );

//     auto iter = array.begin();
//     std::string_view val;
//     ASSERT_SUCCESS( (*iter).get(val) );
//     ASSERT_EQUAL( val, "hi" );

//     ++iter;
//     ASSERT_SUCCESS( (*iter).get(val) );
//     ASSERT_EQUAL( val, "has backslash\\" );

//     return true;
//   }

//   bool numeric_values() {
//     std::cout << "Running " << __func__ << std::endl;
//     auto json = R"([ 0, 1, -1, 1.1 ])"_padded;
//     ondemand::parser parser;
//     ondemand::array array;
//     ASSERT_SUCCESS( parser.iterate(json).get(array) );

//     auto iter = array.begin();
//     ASSERT_EQUAL( (*iter).get<uint64_t>().first, 0 );
//     ASSERT_EQUAL( (*iter).get<int64_t>().first, 0 );
//     ASSERT_EQUAL( (*iter).get<double>().first, 0 );
//     ++iter;
//     ASSERT_EQUAL( (*iter).get<uint64_t>().first, 1 );
//     ASSERT_EQUAL( (*iter).get<int64_t>().first, 1 );
//     ASSERT_EQUAL( (*iter).get<double>().first, 1 );
//     ++iter;
//     ASSERT_EQUAL( (*iter).get<int64_t>().first, -1 );
//     ASSERT_EQUAL( (*iter).get<double>().first, -1 );
//     ++iter;
//     ASSERT_EQUAL( (*iter).get<double>().first, 1.1 );
//     return true;
//   }

//   bool boolean_values() {
//     std::cout << "Running " << __func__ << std::endl;
//     auto json = R"([ true, false ])"_padded;
//     ondemand::parser parser;
//     ondemand::array array;
//     ASSERT_SUCCESS( parser.iterate(json).get(array) );

//     auto val = array.begin();
//     ASSERT_EQUAL( (*val).get<bool>().first, true );
//     ++val;
//     ASSERT_EQUAL( (*val).get<bool>().first, false );
//     return true;
//   }

//   bool null_value() {
//     std::cout << "Running " << __func__ << std::endl;
//     auto json = R"([ null ])"_padded;
//     ondemand::parser parser;
//     ondemand::array array;
//     ASSERT_SUCCESS( parser.iterate(json).get(array) );

//     auto val = array.begin();
//     ASSERT_EQUAL( !(*val).is_null(), 0 );
//     return true;
//   }

//   bool document_object_index() {
//     std::cout << "Running " << __func__ << std::endl;
//     auto json = R"({ "a": 1, "b": 2, "c/d": 3})"_padded;
//     ondemand::parser parser;
//     ondemand::object object;
//     ASSERT_SUCCESS( parser.iterate(json).get(object) );
//     ASSERT_EQUAL( object["a"].get<uint64_t>().first, 1 );
//     ASSERT_EQUAL( object["b"].get<uint64_t>().first, 2 );
//     ASSERT_EQUAL( object["c/d"].get<uint64_t>().first, 3 );
//     // Check all three again in backwards order, to ensure we can go backwards
//     ASSERT_EQUAL( object["c/d"].get<uint64_t>().first, 3 );
//     ASSERT_EQUAL( object["b"].get<uint64_t>().first, 2 );
//     ASSERT_EQUAL( object["a"].get<uint64_t>().first, 1 );

//     simdjson::error_code error;
//     SIMDJSON_UNUSED element val;
// #ifndef _LIBCPP_VERSION // should work everywhere but with libc++, must include the <ciso646> header.
//     std::tie(val,error) = object["d"];
//     ASSERT_ERROR( error, NO_SUCH_FIELD );
//     std::tie(std::ignore,error) = object["d"];
//     ASSERT_ERROR( error, NO_SUCH_FIELD );
// #endif
//     // tie(val, error) = object["d"]; fails with "no viable overloaded '='" on Apple clang version 11.0.0	    tie(val, error) = doc["d"];
//     object["d"].tie(val, error);
//     ASSERT_ERROR( error, NO_SUCH_FIELD );
//     ASSERT_ERROR( object["d"].get(val), NO_SUCH_FIELD );
//     ASSERT_ERROR( object["d"], NO_SUCH_FIELD );
//     return true;
//   }

//   bool object_index() {
//     std::cout << "Running " << __func__ << std::endl;
//     auto json = R"({ "obj": { "a": 1, "b": 2, "c/d": 3 } })"_padded;
//     ondemand::parser parser;
//     ondemand::document doc;
//     ASSERT_SUCCESS( parser.iterate(json).get(doc) );
//     ASSERT_EQUAL( doc["obj"]["a"].get<uint64_t>().first, 1);

//     object obj;
//     ASSERT_SUCCESS( doc.get(obj) );
//     ASSERT_EQUAL( obj["obj"]["a"].get<uint64_t>().first, 1);

//     ASSERT_SUCCESS( obj["obj"].get(obj) );
//     ASSERT_EQUAL( obj["a"].get<uint64_t>().first, 1 );
//     ASSERT_EQUAL( obj["b"].get<uint64_t>().first, 2 );
//     ASSERT_EQUAL( obj["c/d"].get<uint64_t>().first, 3 );
//     // Check all three again in backwards order, to ensure we can go backwards
//     ASSERT_EQUAL( obj["c/d"].get<uint64_t>().first, 3 );
//     ASSERT_EQUAL( obj["b"].get<uint64_t>().first, 2 );
//     ASSERT_EQUAL( obj["a"].get<uint64_t>().first, 1 );

//     SIMDJSON_UNUSED element val;
//     ASSERT_ERROR( doc["d"].get(val), NO_SUCH_FIELD);
//     return true;
//   }

//   bool twitter_count() {
//     std::cout << "Running " << __func__ << std::endl;
//     // Prints the number of results in twitter.json
//     ondemand::parser parser;
//     uint64_t result_count;
//     ASSERT_SUCCESS( parser.load(TWITTER_JSON)["search_metadata"]["count"].get(result_count) );
//     ASSERT_EQUAL( result_count, 100 );
//     return true;
//   }

//   bool twitter_default_profile() {
//     std::cout << "Running " << __func__ << std::endl;
//     // Print users with a default profile.
//     set<string_view> default_users;
//     ondemand::parser parser;
//     ondemand::array tweets;
//     ASSERT_SUCCESS( parser.load(TWITTER_JSON)["statuses"].get(tweets) );
//     for (auto tweet : tweets) {
//       object user;
//       ASSERT_SUCCESS( tweet["user"].get(user) );
//       bool default_profile;
//       ASSERT_SUCCESS( user["default_profile"].get(default_profile) );
//       if (default_profile) {
//         std::string_view screen_name;
//         ASSERT_SUCCESS( user["screen_name"].get(screen_name) );
//         default_users.insert(screen_name);
//       }
//     }
//     ASSERT_EQUAL( default_users.size(), 86 );
//     return true;
//   }

//   bool twitter_image_sizes() {
//     std::cout << "Running " << __func__ << std::endl;
//     // Print image names and sizes
//     set<pair<uint64_t, uint64_t>> image_sizes;
//     simdjson::error_code error;
//     ondemand::parser parser;
//     ondemand::array tweets;
//     ASSERT_SUCCESS( parser.load(TWITTER_JSON)["statuses"].get(tweets) );
//     for (auto tweet : tweets) {
//       ondemand::array media;
//       if (not (error = tweet["entities"]["media"].get(media))) {
//         for (auto image : media) {
//           object sizes;
//           ASSERT_SUCCESS( image["sizes"].get(sizes) );
//           for (auto size : sizes) {
//             uint64_t width, height;
//             ASSERT_SUCCESS( size.value["w"].get(width) );
//             ASSERT_SUCCESS( size.value["h"].get(height) );
//             image_sizes.insert(make_pair(width, height));
//           }
//         }
//       }
//     }
//     ASSERT_EQUAL( image_sizes.size(), 15 );
//     return true;
//   }

// #if SIMDJSON_EXCEPTIONS

//   bool object_iterator_exception() {
//     std::cout << "Running " << __func__ << std::endl;
//     auto json = R"({ "a": 1, "b": 2, "c": 3 })"_padded;
//     const char* expected_key[] = { "a", "b", "c" };
//     uint64_t expected_value[] = { 1, 2, 3 };
//     int i = 0;

//     ondemand::parser parser;
//     for (auto [key, value] : ondemand::object(parser.iterate(json))) {
//       ASSERT_EQUAL( key, expected_key[i]);
//       ASSERT_EQUAL( uint64_t(value), expected_value[i] );
//       i++;
//     }
//     ASSERT_EQUAL( i*sizeof(uint64_t), sizeof(expected_value) );
//     return true;
//   }

//   bool array_iterator_exception() {
//     std::cout << "Running " << __func__ << std::endl;
//     auto json = R"([ 1, 10, 100 ])"_padded;
//     uint64_t expected_value[] = { 1, 10, 100 };
//     int i=0;

//     ondemand::parser parser;
//     for (uint64_t value : parser.iterate(json)) {
//       ASSERT_EQUAL( value, expected_value[i] );
//       i++;
//     }
//     ASSERT_EQUAL( i*sizeof(uint64_t), sizeof(expected_value) );
//     return true;
//   }

//   bool string_value_exception() {
//     std::cout << "Running " << __func__ << std::endl;
//     ondemand::parser parser;
//     ASSERT_EQUAL( (const char *)parser.iterate(R"("hi")"_padded), "hi" );
//     ASSERT_EQUAL( string_view(parser.iterate(R"("hi")"_padded)), "hi" );
//     ASSERT_EQUAL( (const char *)parser.iterate(R"("has backslash\\")"_padded), "has backslash\\");
//     ASSERT_EQUAL( string_view(parser.iterate(R"("has backslash\\")"_padded)), "has backslash\\" );
//     return true;
//   }

//   bool numeric_values_exception() {
//     std::cout << "Running " << __func__ << std::endl;
//     ondemand::parser parser;

//     ASSERT_EQUAL( uint64_t(parser.iterate("0"_padded)), 0);
//     ASSERT_EQUAL( int64_t(parser.iterate("0"_padded)), 0);
//     ASSERT_EQUAL( double(parser.iterate("0"_padded)), 0);

//     ASSERT_EQUAL( uint64_t(parser.iterate("1"_padded)), 1);
//     ASSERT_EQUAL( int64_t(parser.iterate("1"_padded)), 1);
//     ASSERT_EQUAL( double(parser.iterate("1"_padded)), 1);

//     ASSERT_EQUAL( int64_t(parser.iterate("-1"_padded)), -1);
//     ASSERT_EQUAL( double(parser.iterate("-1"_padded)), -1);

//     ASSERT_EQUAL( double(parser.iterate("1.1"_padded)), 1.1);

//     return true;
//   }

//   bool boolean_values_exception() {
//     std::cout << "Running " << __func__ << std::endl;
//     ondemand::parser parser;

//     ASSERT_EQUAL( bool(parser.iterate("true"_padded)), true);

//     ASSERT_EQUAL( bool(parser.iterate("false"_padded)), false);

//     return true;
//   }

//   bool null_value_exception() {
//     std::cout << "Running " << __func__ << std::endl;
//     ondemand::parser parser;

//     ASSERT_EQUAL( bool(parser.iterate("null"_padded).is_null()), true );

//     return true;
//   }

//   bool document_object_index_exception() {
//     std::cout << "Running " << __func__ << std::endl;
//     auto json = R"({ "a": 1, "b": 2, "c": 3})"_padded;
//     ondemand::parser parser;
//     auto obj = parser.iterate(json);

//     ASSERT_EQUAL(uint64_t(obj["a"]), 1);

//     return true;
//   }

//   bool object_index_exception() {
//     std::cout << "Running " << __func__ << std::endl;
//     auto json = R"({ "obj": { "a": 1, "b": 2, "c": 3 } })"_padded;
//     ondemand::parser parser;
//     object obj = parser.iterate(json)["obj"];

//     ASSERT_EQUAL( uint64_t(obj["a"]), 1);

//     return true;
//   }

//   bool twitter_count_exception() {
//     std::cout << "Running " << __func__ << std::endl;
//     // Prints the number of results in twitter.json
//     ondemand::parser parser;
//     element doc = parser.load(TWITTER_JSON);
//     uint64_t result_count = doc["search_metadata"]["count"];
//     if (result_count != 100) { cerr << "Expected twitter.json[metadata_count][count] = 100, got " << result_count << endl; return false; }
//     return true;
//   }

//   bool twitter_default_profile_exception() {
//     std::cout << "Running " << __func__ << std::endl;
//     // Print users with a default profile.
//     set<string_view> default_users;
//     ondemand::parser parser;
//     element doc = parser.load(TWITTER_JSON);
//     for (object tweet : doc["statuses"].get<ondemand::array>()) {
//       object user = tweet["user"];
//       if (user["default_profile"]) {
//         default_users.insert(user["screen_name"]);
//       }
//     }
//     if (default_users.size() != 86) { cerr << "Expected twitter.json[statuses][user] to contain 86 default_profile users, got " << default_users.size() << endl; return false; }
//     return true;
//   }

//   bool twitter_image_sizes_exception() {
//     std::cout << "Running " << __func__ << std::endl;
//     // Print image names and sizes
//     set<pair<uint64_t, uint64_t>> image_sizes;
//     ondemand::parser parser;
//     for (object tweet : parser.load(TWITTER_JSON)["statuses"]) {
//       auto media = tweet["entities"]["media"];
//       if (!media.error()) {
//         for (object image : media) {
//           for (auto size : object(image["sizes"])) {
//             image_sizes.insert(make_pair(size.value["w"], size.value["h"]));
//           }
//         }
//       }
//     }
//     ASSERT_EQUAL( image_sizes.size(), 15 );
//     return true;
//   }

// #endif

  bool run() {
    return
           object_iterator() &&
           array_iterator() &&
//            object_iterator_empty() &&
//            array_iterator_empty() &&
//            string_value() &&
//            numeric_values() &&
//            boolean_values() &&
//            null_value() &&
//            document_object_index() &&
//            object_index() &&
//            twitter_count() &&
//            twitter_default_profile() &&
//            twitter_image_sizes() &&
// #if SIMDJSON_EXCEPTIONS
//            object_iterator_exception() &&
//            array_iterator_exception() &&
//            string_value_exception() &&
//            numeric_values_exception() &&
//            boolean_values_exception() &&
//            null_value_exception() &&
//            document_object_index() &&
//            twitter_count_exception() &&
//            twitter_default_profile_exception() &&
//            twitter_image_sizes_exception() &&
// #endif
           true;
  }
}

// namespace type_tests {
//   using namespace simdjson;
//   using namespace std;

//   const padded_string ALL_TYPES_JSON = R"(
//     {
//       "array": [],

//       "object": {},

//       "string": "foo",

//       "0": 0,
//       "1": 1,
//       "-1": -1,
//       "9223372036854775807": 9223372036854775807,
//       "-9223372036854775808": -9223372036854775808,

//       "9223372036854775808": 9223372036854775808,
//       "18446744073709551615": 18446744073709551615,

//       "0.0": 0.0,
//       "0.1": 0.1,
//       "1e0": 1e0,
//       "1e100": 1e100,

//       "true": true,
//       "false": false,

//       "null": null
//     }
//   )"_padded;

//   template<typename T>
//   bool test_cast(simdjson_result<ondemand::element> result, T expected) {
//     cast_tester<T> tester;
//     std::cout << "  test_cast<" << typeid(T).name() << "> expecting " << expected << std::endl;
//     // Grab the element out and check success
//     ondemand::element element = result.first;

//     RUN_TEST( tester.test_get_t(element, expected) );
//     RUN_TEST( tester.test_get_t(result, expected) );
//     RUN_TEST( tester.test_get(element, expected) );
//     RUN_TEST( tester.test_get(result, expected) );
//     // RUN_TEST( tester.test_named_get(element, expected) );
//     // RUN_TEST( tester.test_named_get(result, expected) );
//     RUN_TEST( tester.test_is(element, true) );
//     RUN_TEST( tester.test_is(result, true) );
//     // RUN_TEST( tester.test_named_is(element, true) );
//     // RUN_TEST( tester.test_named_is(result, true) );
// #if SIMDJSON_EXCEPTIONS
//     RUN_TEST( tester.test_implicit_cast(element, expected) );
//     RUN_TEST( tester.test_implicit_cast(result, expected) );
// #endif

//     return true;
//   }

//   template<typename T>
//   bool test_cast(simdjson_result<ondemand::element> result) {
//     cast_tester<T> tester;
//     std::cout << "  test_cast<" << typeid(T).name() << ">" << std::endl;
//     // Grab the element out and check success
//     ondemand::element element = result.first;

//     RUN_TEST( tester.test_get_t(element) );
//     RUN_TEST( tester.test_get_t(result) );
//     RUN_TEST( tester.test_get(element) );
//     RUN_TEST( tester.test_get(result) );
//     RUN_TEST( tester.test_named_get(element) );
//     RUN_TEST( tester.test_named_get(result) );
//     RUN_TEST( tester.test_is(element, true) );
//     RUN_TEST( tester.test_is(result, true) );
//     RUN_TEST( tester.test_named_is(element, true) );
//     RUN_TEST( tester.test_named_is(result, true) );
// #if SIMDJSON_EXCEPTIONS
//     RUN_TEST( tester.test_implicit_cast(element) );
//     RUN_TEST( tester.test_implicit_cast(result) );
// #endif

//     return true;
//   }

//   //
//   // Test that we get errors when we cast to the wrong type
//   //
//   template<typename T>
//   bool test_cast_error(simdjson_result<ondemand::element> result, simdjson::error_code expected_error) {
//     std::cout << "  test_cast_error<" << typeid(T).name() << "> expecting error '" << expected_error << "'" << std::endl;
//     ondemand::element element = result.first;

//     cast_tester<T> tester;

//     RUN_TEST( tester.test_get_error(element, expected_error) );
//     RUN_TEST( tester.test_get_error(result, expected_error) );
//     RUN_TEST( tester.test_named_get_error(element, expected_error) );
//     RUN_TEST( tester.test_named_get_error(result, expected_error) );
//     RUN_TEST( tester.test_is(element, false) );
//     RUN_TEST( tester.test_is(result, false) );
//     RUN_TEST( tester.test_named_is(element, false) );
//     RUN_TEST( tester.test_named_is(result, false) );
// #if SIMDJSON_EXCEPTIONS
//     RUN_TEST( tester.test_implicit_cast_error(element, expected_error) );
//     RUN_TEST( tester.test_implicit_cast_error(result, expected_error) );
// #endif

//     return true;
//   }

//   bool test_type(simdjson_result<ondemand::element> result, ondemand::element_type expected_type) {
//     std::cout << "  test_type() expecting " << expected_type << std::endl;
//     ondemand::element element = result.first;
//     ondemand::element_type actual_type;
//     auto error = result.type().get(actual_type);
//     ASSERT_SUCCESS(error);
//     ASSERT_EQUAL(actual_type, expected_type);

//     actual_type = element.type();
//     ASSERT_SUCCESS(error);
//     ASSERT_EQUAL(actual_type, expected_type);

// #if SIMDJSON_EXCEPTIONS

//     try {

//       actual_type = result.type();
//       ASSERT_EQUAL(actual_type, expected_type);

//     } catch(simdjson_error &e) {
//       std::cerr << e.error() << std::endl;
//       return false;
//     }

// #endif // SIMDJSON_EXCEPTIONS

//     return true;
//   }

//   bool test_is_null(simdjson_result<ondemand::element> result, bool expected_is_null) {
//     std::cout << "  test_is_null() expecting " << expected_is_null << std::endl;
//     // Grab the element out and check success
//     ondemand::element element = result.first;
//     ASSERT_EQUAL(result.is_null(), expected_is_null);

//     ASSERT_EQUAL(element.is_null(), expected_is_null);

//     return true;
//   }

//   bool cast_array() {
//     std::cout << "Running " << __func__ << std::endl;

//     ondemand::parser parser;
//     simdjson_result<ondemand::element> result = parser.iterate(ALL_TYPES_JSON)["array"];

//     return true
//       && test_type(result, ondemand::element_type::ARRAY)
//       && test_cast<ondemand::array>(result)
//       && test_cast_error<ondemand::object>(result, INCORRECT_TYPE)
//       && test_cast_error<std::string_view>(result, INCORRECT_TYPE)
//       && test_cast_error<const char *>(result, INCORRECT_TYPE)
//       && test_cast_error<int64_t>(result, INCORRECT_TYPE)
//       && test_cast_error<uint64_t>(result, INCORRECT_TYPE)
//       && test_cast_error<double>(result, INCORRECT_TYPE)
//       && test_cast_error<bool>(result, INCORRECT_TYPE)
//       && test_is_null(result, false);
//   }

//   bool cast_object() {
//     std::cout << "Running " << __func__ << std::endl;

//     ondemand::parser parser;
//     simdjson_result<ondemand::element> result = parser.iterate(ALL_TYPES_JSON)["object"];

//     return true
//       && test_type(result, ondemand::element_type::OBJECT)
//       && test_cast_error<ondemand::array>(result, INCORRECT_TYPE)
//       && test_cast<ondemand::object>(result)
//       && test_cast_error<std::string_view>(result, INCORRECT_TYPE)
//       && test_cast_error<const char *>(result, INCORRECT_TYPE)
//       && test_cast_error<int64_t>(result, INCORRECT_TYPE)
//       && test_cast_error<uint64_t>(result, INCORRECT_TYPE)
//       && test_cast_error<double>(result, INCORRECT_TYPE)
//       && test_cast_error<bool>(result, INCORRECT_TYPE)
//       && test_is_null(result, false);
//   }

//   bool cast_string() {
//     std::cout << "Running " << __func__ << std::endl;

//     ondemand::parser parser;
//     simdjson_result<ondemand::element> result = parser.iterate(ALL_TYPES_JSON)["string"];

//     return true
//       && test_type(result, ondemand::element_type::STRING)
//       && test_cast_error<ondemand::array>(result, INCORRECT_TYPE)
//       && test_cast_error<ondemand::object>(result, INCORRECT_TYPE)
//       && test_cast<std::string_view>(result, "foo")
//       && test_cast<const char *>(result, "foo")
//       && test_cast_error<int64_t>(result, INCORRECT_TYPE)
//       && test_cast_error<uint64_t>(result, INCORRECT_TYPE)
//       && test_cast_error<double>(result, INCORRECT_TYPE)
//       && test_cast_error<bool>(result, INCORRECT_TYPE)
//       && test_is_null(result, false);
//   }

//   bool cast_int64(const char *key, int64_t expected_value) {
//     std::cout << "Running " << __func__ << "(" << key << ")" << std::endl;

//     ondemand::parser parser;
//     simdjson_result<ondemand::element> result = parser.iterate(ALL_TYPES_JSON)[key];
//     return true
//       && test_type(result, ondemand::element_type::INT64)
//       && test_cast_error<ondemand::array>(result, INCORRECT_TYPE)
//       && test_cast_error<ondemand::object>(result, INCORRECT_TYPE)
//       && test_cast_error<std::string_view>(result, INCORRECT_TYPE)
//       && test_cast_error<const char *>(result, INCORRECT_TYPE)
//       && test_cast<int64_t>(result, expected_value)
//       && (expected_value >= 0 ?
//           test_cast<uint64_t>(result, expected_value) :
//           test_cast_error<uint64_t>(result, NUMBER_OUT_OF_RANGE))
//       && test_cast<double>(result, static_cast<double>(expected_value))
//       && test_cast_error<bool>(result, INCORRECT_TYPE)
//       && test_is_null(result, false);
//   }

//   bool cast_uint64(const char *key, uint64_t expected_value) {
//     std::cout << "Running " << __func__ << "(" << key << ")" << std::endl;

//     ondemand::parser parser;
//     simdjson_result<ondemand::element> result = parser.iterate(ALL_TYPES_JSON)[key];

//     return true
//       && test_type(result, ondemand::element_type::UINT64)
//       && test_cast_error<ondemand::array>(result, INCORRECT_TYPE)
//       && test_cast_error<ondemand::object>(result, INCORRECT_TYPE)
//       && test_cast_error<std::string_view>(result, INCORRECT_TYPE)
//       && test_cast_error<const char *>(result, INCORRECT_TYPE)
//       && test_cast_error<int64_t>(result, NUMBER_OUT_OF_RANGE)
//       && test_cast<uint64_t>(result, expected_value)
//       && test_cast<double>(result, static_cast<double>(expected_value))
//       && test_cast_error<bool>(result, INCORRECT_TYPE)
//       && test_is_null(result, false);
//   }

//   bool cast_double(const char *key, double expected_value) {
//     std::cout << "Running " << __func__ << "(" << key << ")" << std::endl;

//     ondemand::parser parser;
//     simdjson_result<ondemand::element> result = parser.iterate(ALL_TYPES_JSON)[key];
//     return true
//       && test_type(result, ondemand::element_type::DOUBLE)
//       && test_cast_error<ondemand::array>(result, INCORRECT_TYPE)
//       && test_cast_error<ondemand::object>(result, INCORRECT_TYPE)
//       && test_cast_error<std::string_view>(result, INCORRECT_TYPE)
//       && test_cast_error<const char *>(result, INCORRECT_TYPE)
//       && test_cast_error<int64_t>(result, INCORRECT_TYPE)
//       && test_cast_error<uint64_t>(result, INCORRECT_TYPE)
//       && test_cast<double>(result, expected_value)
//       && test_cast_error<bool>(result, INCORRECT_TYPE)
//       && test_is_null(result, false);
//   }

//   bool cast_bool(const char *key, bool expected_value) {
//     std::cout << "Running " << __func__ << "(" << key << ")" << std::endl;

//     ondemand::parser parser;
//     simdjson_result<ondemand::element> result = parser.iterate(ALL_TYPES_JSON)[key];

//     return true
//       && test_type(result, ondemand::element_type::BOOL)
//       && test_cast_error<ondemand::array>(result, INCORRECT_TYPE)
//       && test_cast_error<ondemand::object>(result, INCORRECT_TYPE)
//       && test_cast_error<std::string_view>(result, INCORRECT_TYPE)
//       && test_cast_error<const char *>(result, INCORRECT_TYPE)
//       && test_cast_error<int64_t>(result, INCORRECT_TYPE)
//       && test_cast_error<uint64_t>(result, INCORRECT_TYPE)
//       && test_cast_error<double>(result, INCORRECT_TYPE)
//       && test_cast<bool>(result, expected_value)
//       && test_is_null(result, false);
//   }

//   bool cast_null() {
//     std::cout << "Running " << __func__ << std::endl;

//     ondemand::parser parser;
//     simdjson_result<ondemand::element> result = parser.iterate(ALL_TYPES_JSON)["null"];
//     return true
//       && test_type(result, ondemand::element_type::NULL_VALUE)
//       && test_cast_error<ondemand::array>(result, INCORRECT_TYPE)
//       && test_cast_error<ondemand::object>(result, INCORRECT_TYPE)
//       && test_cast_error<std::string_view>(result, INCORRECT_TYPE)
//       && test_cast_error<const char *>(result, INCORRECT_TYPE)
//       && test_cast_error<int64_t>(result, INCORRECT_TYPE)
//       && test_cast_error<uint64_t>(result, INCORRECT_TYPE)
//       && test_cast_error<double>(result, INCORRECT_TYPE)
//       && test_cast_error<bool>(result, INCORRECT_TYPE)
//       && test_is_null(result, true);
//   }

//   bool run() {
//     return cast_array() &&

//            cast_object() &&

//            cast_string() &&

//            cast_int64("0", 0) &&
//            cast_int64("1", 1) &&
//            cast_int64("-1", -1) &&
//            cast_int64("9223372036854775807", 9223372036854775807LL) &&
//            cast_int64("-9223372036854775808", -1 - 9223372036854775807LL) &&

//            cast_uint64("9223372036854775808", 9223372036854775808ULL) &&
//            cast_uint64("18446744073709551615", 18446744073709551615ULL) &&

//            cast_double("0.0", 0.0) &&
//            cast_double("0.1", 0.1) &&
//            cast_double("1e0", 1e0) &&
//            cast_double("1e100", 1e100) &&

//            cast_bool("true", true) &&
//            cast_bool("false", false) &&

//            cast_null() &&

//            true;
//   }

// }


// namespace validate_tests {
//   bool test_validate() {
//     std::cout << "Running " << __func__ << std::endl;
//     const std::string test = R"({ "foo" : 1, "bar" : [ 1, 2, 3 ], "baz": { "a": 1, "b": 2, "c": 3 } })";
//     if(!simdjson::validate_utf8(test.data(), test.size())) {
//       return false;
//     }
//     return true;
//   }
//   bool test_range() {
//     std::cout << "Running " << __func__ << std::endl;
//     for(size_t len = 0; len <= 128; len++) {
//       std::vector<uint8_t> source(len,' ');
//       if(!simdjson::validate_utf8((const char*)source.data(), source.size())) { return false; }
//     }
//     return true;
//   }

//   bool test_bad_validate() {
//     std::cout << "Running " << __func__ << std::endl;
//     const std::string test = "\x80\x81";
//     if(simdjson::validate_utf8(test.data(), test.size())) {
//       return false;
//     }
//     return true;
//   }
//   bool test_issue1169() {
//     std::cout << "Running " << __func__ << std::endl;
//     std::vector<uint8_t> source(64,' ');
//     for(size_t idx = 0; idx < 64; idx++) {
//       source[idx] = 255;
//       if(simdjson::validate_utf8((const char*)source.data(), source.size())) { return false; }
//       source[idx] = 0;
//     }
//     return true;
//   }
//   bool test_issue1169_long() {
//     std::cout << "Running " << __func__ << std::endl;
//     for(size_t len = 1; len <= 128; len++) {
//       std::vector<uint8_t> source(len,' ');
//       source[len-1] = 255;
//       if(simdjson::validate_utf8((const char*)source.data(), source.size())) { return false; }
//     }
//     return true;
//   }
//   bool test_random() {
//     std::cout << "Running " << __func__ << std::endl;
//     std::vector<uint8_t> source(64,' ');
//     const simdjson::implementation *impl_fallback = simdjson::available_implementations["fallback"];
//     if(!impl_fallback) { return true; }
//     for(size_t i = 0; i < 10000; i++) {
//       std::vector<uint8_t>& s(source);
//       s[i%64] ^= uint8_t(1235 * i);
//       const bool active_ok = simdjson::active_implementation->validate_utf8((const char*)s.data(), s.size());
//       const bool fallback_ok = impl_fallback->validate_utf8((const char*)s.data(), s.size());
//       if(active_ok != fallback_ok) { return false; }
//       s[i%64] ^= uint8_t(1235 * i);
//     }
//     return true;
//   }
//   bool run() {
//     return test_range() &&
//            test_issue1169_long() &&
//            test_issue1169() &&
//            test_random() &&
//            test_validate() &&
//            test_bad_validate();
//   }
// }



// namespace minify_tests {

//   bool check_minification(const char * input, size_t length, const char * expected, size_t expected_length) {
//     std::unique_ptr<char[]> buffer{new(std::nothrow) char[length]};
//     if(buffer.get() == nullptr) {
//       std::cerr << "cannot alloc "  << std::endl;
//       return false;
//     }
//     size_t newlength{};
//     ASSERT_SUCCESS( simdjson::minify(input, length, buffer.get(), newlength) );
//     ASSERT_EQUAL( newlength, expected_length);
//     for(size_t i = 0; i < newlength; i++) {
//       ASSERT_EQUAL( buffer.get()[i], expected[i]);
//     }
//     return true;
//   }
//   bool test_single_quote() {
//     std::cout << "Running " << __func__ << std::endl;
//     const std::string test = "\"";
//     char output[1];
//     size_t newlength;
//     auto e = simdjson::minify(test.data(), 1, output, newlength);
//     if(e) {
//       std::cout << "got an error (expected) : " << e << std::endl;
//       return true; // we have an error as expected
//     }
//     std::cerr << "This should be an error : " << e << std::endl;
//     return false;
//   }

//   bool test_minify() {
//     std::cout << "Running " << __func__ << std::endl;
//     const std::string test = R"({ "foo" : 1, "bar" : [ 1, 2, 3 ], "baz": { "a": 1, "b": 2, "c": 3 } })";
//     const std::string minified(R"({"foo":1,"bar":[1,2,3],"baz":{"a":1,"b":2,"c":3}})");
//     return check_minification(test.c_str(), test.size(), minified.c_str(), minified.size());
//   }
//   bool test_minify_array() {
//     std::cout << "Running " << __func__ << std::endl;
//     std::string test("[ 1,    2,    3]");
//     std::string minified("[1,2,3]");
//     return check_minification(test.c_str(), test.size(), minified.c_str(), minified.size());
//   }
//   bool test_minify_object() {
//     std::cout << "Running " << __func__ << std::endl;
//     std::string test(R"({ "foo   " : 1, "b  ar" : [ 1, 2, 3 ], "baz": { "a": 1, "b": 2, "c": 3 } })");
//     std::string minified(R"({"foo   ":1,"b  ar":[1,2,3],"baz":{"a":1,"b":2,"c":3}})");
//     return check_minification(test.c_str(), test.size(), minified.c_str(), minified.size());
//   }
//   bool run() {
//     return test_single_quote() &&
//            test_minify() &&
//            test_minify_array() &&
//            test_minify_object();
//   }
// }


// namespace format_tests {
//   using namespace simdjson;
//   using namespace simdjson::dom;
//   using namespace std;
//   const padded_string DOCUMENT = R"({ "foo" : 1, "bar" : [ 1, 2, 3 ], "baz": { "a": 1, "b": 2, "c": 3 } })"_padded;
//   const string MINIFIED(R"({"foo":1,"bar":[1,2,3],"baz":{"a":1,"b":2,"c":3}})");
//   bool assert_minified(ostringstream &actual, const std::string &expected=MINIFIED) {
//     if (actual.str() != expected) {
//       cerr << "Failed to correctly minify " << DOCUMENT << endl;
//       cerr << "Expected: " << expected << endl;
//       cerr << "Actual:   " << actual.str() << endl;
//       return false;
//     }
//     return true;
//   }

//   bool print_parser_iterate() {
//     std::cout << "Running " << __func__ << std::endl;
//     ondemand::parser parser;
//     ondemand::document doc;
//     ASSERT_SUCCESS( parser.iterate(DOCUMENT).get(doc) );
//     ostringstream s;
//     s << doc;
//     return assert_minified(s);
//   }
//   bool print_minify_parser_iterate() {
//     std::cout << "Running " << __func__ << std::endl;
//     ondemand::parser parser;
//     ondemand::document doc;
//     ASSERT_SUCCESS( parser.iterate(DOCUMENT).get(doc) );
//     ostringstream s;
//     s << minify(doc);
//     return assert_minified(s);
//   }

//   bool print_element() {
//     std::cout << "Running " << __func__ << std::endl;
//     ondemand::parser parser;
//     ondemand::element value;
//     ASSERT_SUCCESS( parser.iterate(DOCUMENT)["foo"].get(value) );
//     ostringstream s;
//     s << value;
//     return assert_minified(s, "1");
//   }
//   bool print_minify_element() {
//     std::cout << "Running " << __func__ << std::endl;
//     ondemand::parser parser;
//     ondemand::element value;
//     ASSERT_SUCCESS( parser.iterate(DOCUMENT)["foo"].get(value) );
//     ostringstream s;
//     s << minify(value);
//     return assert_minified(s, "1");
//   }

//   bool print_array() {
//     std::cout << "Running " << __func__ << std::endl;
//     ondemand::parser parser;
//     ondemand::array array;
//     ASSERT_SUCCESS( parser.iterate(DOCUMENT)["bar"].get(array) );
//     ostringstream s;
//     s << array;
//     return assert_minified(s, "[1,2,3]");
//   }
//   bool print_minify_array() {
//     std::cout << "Running " << __func__ << std::endl;
//     ondemand::parser parser;
//     ondemand::array array;
//     ASSERT_SUCCESS( parser.iterate(DOCUMENT)["bar"].get(array) );
//     ostringstream s;
//     s << minify(array);
//     return assert_minified(s, "[1,2,3]");
//   }

//   bool print_object() {
//     std::cout << "Running " << __func__ << std::endl;
//     ondemand::parser parser;
//     ondemand::object object;
//     ASSERT_SUCCESS( parser.iterate(DOCUMENT)["baz"].get(object) );
//     ostringstream s;
//     s << object;
//     return assert_minified(s, R"({"a":1,"b":2,"c":3})");
//   }
//   bool print_minify_object() {
//     std::cout << "Running " << __func__ << std::endl;
//     ondemand::parser parser;
//     ondemand::object object;
//     ASSERT_SUCCESS( parser.iterate(DOCUMENT)["baz"].get(object) );
//     ostringstream s;
//     s << minify(object);
//     return assert_minified(s, R"({"a":1,"b":2,"c":3})");
//   }

// #if SIMDJSON_EXCEPTIONS

//   bool print_parser_iterate_exception() {
//     std::cout << "Running " << __func__ << std::endl;
//     ondemand::parser parser;
//     ostringstream s;
//     s << parser.iterate(DOCUMENT);
//     return assert_minified(s);
//   }
//   bool print_minify_parser_iterate_exception() {
//     std::cout << "Running " << __func__ << std::endl;
//     ondemand::parser parser;
//     ostringstream s;
//     s << minify(parser.iterate(DOCUMENT));
//     return assert_minified(s);
//   }

//   bool print_element_result_exception() {
//     std::cout << "Running " << __func__ << std::endl;
//     ondemand::parser parser;
//     ostringstream s;
//     s << parser.iterate(DOCUMENT)["foo"];
//     return assert_minified(s, "1");
//   }
//   bool print_minify_element_result_exception() {
//     std::cout << "Running " << __func__ << std::endl;
//     ondemand::parser parser;
//     ostringstream s;
//     s << minify(parser.iterate(DOCUMENT)["foo"]);
//     return assert_minified(s, "1");
//   }

//   bool print_element_exception() {
//     std::cout << "Running " << __func__ << std::endl;
//     ondemand::parser parser;
//     element value = parser.iterate(DOCUMENT)["foo"];
//     ostringstream s;
//     s << value;
//     return assert_minified(s, "1");
//   }
//   bool print_minify_element_exception() {
//     std::cout << "Running " << __func__ << std::endl;
//     ondemand::parser parser;
//     element value = parser.iterate(DOCUMENT)["foo"];
//     ostringstream s;
//     s << minify(value);
//     return assert_minified(s, "1");
//   }

//   bool print_array_result_exception() {
//     std::cout << "Running " << __func__ << std::endl;
//     ondemand::parser parser;
//     ostringstream s;
//     s << parser.iterate(DOCUMENT)["bar"].get<ondemand::array>();
//     return assert_minified(s, "[1,2,3]");
//   }
//   bool print_minify_array_result_exception() {
//     std::cout << "Running " << __func__ << std::endl;
//     ondemand::parser parser;
//     ostringstream s;
//     s << minify(parser.iterate(DOCUMENT)["bar"].get<ondemand::array>());
//     return assert_minified(s, "[1,2,3]");
//   }

//   bool print_object_result_exception() {
//     std::cout << "Running " << __func__ << std::endl;
//     ondemand::parser parser;
//     ostringstream s;
//     s << parser.iterate(DOCUMENT)["baz"].get<ondemand::object>();
//     return assert_minified(s, R"({"a":1,"b":2,"c":3})");
//   }
//   bool print_minify_object_result_exception() {
//     std::cout << "Running " << __func__ << std::endl;
//     ondemand::parser parser;
//     ostringstream s;
//     s << minify(parser.iterate(DOCUMENT)["baz"].get<ondemand::object>());
//     return assert_minified(s, R"({"a":1,"b":2,"c":3})");
//   }

//   bool print_array_exception() {
//     std::cout << "Running " << __func__ << std::endl;
//     ondemand::parser parser;
//     ondemand::array array = parser.iterate(DOCUMENT)["bar"];
//     ostringstream s;
//     s << array;
//     return assert_minified(s, "[1,2,3]");
//   }
//   bool print_minify_array_exception() {
//     std::cout << "Running " << __func__ << std::endl;
//     ondemand::parser parser;
//     ondemand::array array = parser.iterate(DOCUMENT)["bar"];
//     ostringstream s;
//     s << minify(array);
//     return assert_minified(s, "[1,2,3]");
//   }

//   bool print_object_exception() {
//     std::cout << "Running " << __func__ << std::endl;
//     ondemand::parser parser;
//     ondemand::object object = parser.iterate(DOCUMENT)["baz"];
//     ostringstream s;
//     s << object;
//     return assert_minified(s, R"({"a":1,"b":2,"c":3})");
//   }
//   bool print_minify_object_exception() {
//     std::cout << "Running " << __func__ << std::endl;
//     ondemand::parser parser;
//     ondemand::object object = parser.iterate(DOCUMENT)["baz"];
//     ostringstream s;
//     s << minify(object);
//     return assert_minified(s, R"({"a":1,"b":2,"c":3})");
//   }
// #endif // SIMDJSON_EXCEPTIONS

//   bool run() {
//     return print_parser_iterate() && print_minify_parser_iterate() &&
//            print_element() && print_minify_element() &&
//            print_array() && print_minify_array() &&
//            print_object() && print_minify_object() &&
// #if SIMDJSON_EXCEPTIONS
//            print_parser_iterate_exception() && print_minify_parser_iterate_exception() &&
//            print_element_result_exception() && print_minify_element_result_exception() &&
//            print_array_result_exception() && print_minify_array_result_exception() &&
//            print_object_result_exception() && print_minify_object_result_exception() &&
//            print_element_exception() && print_minify_element_exception() &&
//            print_array_exception() && print_minify_array_exception() &&
//            print_object_exception() && print_minify_object_exception() &&
// #endif
//            true;
//   }
// }


int main(int argc, char *argv[]) {
  std::cout << std::unitbuf;
  int c;
  while ((c = getopt(argc, argv, "a:")) != -1) {
    switch (c) {
    case 'a': {
      const simdjson::implementation *impl = simdjson::available_implementations[optarg];
      if (!impl) {
        fprintf(stderr, "Unsupported architecture value -a %s\n", optarg);
        return EXIT_FAILURE;
      }
      simdjson::active_implementation = impl;
      break;
    }
    default:
      fprintf(stderr, "Unexpected argument %c\n", c);
      return EXIT_FAILURE;
    }
  }

  // this is put here deliberately to check that the documentation is correct (README),
  // should this fail to compile, you should update the documentation:
  if (simdjson::active_implementation->name() == "unsupported") {
    printf("unsupported CPU\n");
  }
  // We want to know what we are testing.
  std::cout << "Running tests against this implementation: " << simdjson::active_implementation->name();
  std::cout << "(" << simdjson::active_implementation->description() << ")" << std::endl;
  std::cout << "------------------------------------------------------------" << std::endl;

  std::cout << "Running basic tests." << std::endl;
  if (
    //   validate_tests::run() &&
    //   minify_tests::run() &&
      parse_api_tests::run() &&
      dom_api_tests::run() &&
    //   type_tests::run() &&
    //   format_tests::run() &&
      number_tests::run() 
  ) {
    std::cout << "Basic tests are ok." << std::endl;
    return EXIT_SUCCESS;
  } else {
    return EXIT_FAILURE;
  }
}
