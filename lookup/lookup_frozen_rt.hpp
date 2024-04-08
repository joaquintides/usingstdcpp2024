#define FROZEN_LETITGO_HAS_STRING_VIEW
#include <frozen/string.h>
#include <frozen/unordered_set.h>

static frozen::string entities[]=
{
#include INPUT_DATA
};
std::size_t entities_size=sizeof(entities)/sizeof(entities[0]);

static auto c=frozen::make_unordered_set(entities);

#include <string_view>

bool found(std::string_view x)
{
  return c.find(x)!=c.end();
}
