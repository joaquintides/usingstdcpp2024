#include <string_view>
#include "hd_constexpr_perfect_set.hpp"

static constexpr std::string_view entities[]=
{
#include INPUT_DATA
};
constexpr std::size_t entities_size=sizeof(entities)/sizeof(entities[0]);

constexpr hd::constexpr_perfect_set<
  std::string_view,entities_size,hd::mulxp3_string_hash
> c(entities);

bool found(std::string_view x)
{
  return c.find(x)!=c.end();
}
