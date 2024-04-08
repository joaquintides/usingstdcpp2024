#include <string_view>
#include "hd_perfect_set.hpp"

static constexpr std::string_view entities[]=
{
#include INPUT_DATA
};
std::size_t entities_size=sizeof(entities)/sizeof(entities[0]);

static hd::perfect_set<std::string_view,hd::mulxp3_string_hash> c(
    &entities[0],&entities[entities_size]);

bool found(std::string_view x)
{
  return c.find(x)!=c.end();
}
