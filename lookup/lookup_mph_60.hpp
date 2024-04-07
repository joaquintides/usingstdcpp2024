#include <mph>

constexpr auto entities=std::array{
mph::pair{"gel",0},
mph::pair{"bigcirc",1},
mph::pair{"ne",2},
mph::pair{"euro",3},
mph::pair{"bbrk",4},
mph::pair{"fork",5},
mph::pair{"sqsub",6},
mph::pair{"leqq",7},
mph::pair{"horbar",8},
mph::pair{"lbrack",9},
mph::pair{"nLeftrig",10},
mph::pair{"Dfr",11},
mph::pair{"kappa",12},
mph::pair{"utdot",13},
mph::pair{"precneqq",14},
mph::pair{"Otimes",15},
mph::pair{"Pi",16},
mph::pair{"nrtrie",17},
mph::pair{"bull",18},
mph::pair{"plusdu",19},
mph::pair{"Zeta",20},
mph::pair{"gtquest",21},
mph::pair{"atilde",22},
mph::pair{"rdquo",23},
mph::pair{"dopf",24},
mph::pair{"rightrig",25},
mph::pair{"yacy",26},
mph::pair{"Coproduc",27},
mph::pair{"yfr",28},
mph::pair{"nleftrig",29},
mph::pair{"ltrie",30},
mph::pair{"QUOT",31},
mph::pair{"zcaron",32},
mph::pair{"opar",33},
mph::pair{"thicksim",34},
mph::pair{"nopf",35},
mph::pair{"Ycy",36},
mph::pair{"intlarhk",37},
mph::pair{"imagline",38},
mph::pair{"bsolb",39},
mph::pair{"sim",40},
mph::pair{"twoheadl",41},
mph::pair{"dlcrop",42},
mph::pair{"nexist",43},
mph::pair{"dfisht",44},
mph::pair{"nlE",45},
mph::pair{"pscr",46},
mph::pair{"Pcy",47},
mph::pair{"fpartint",48},
mph::pair{"boxV",49},
mph::pair{"circlear",50},
mph::pair{"profsurf",51},
mph::pair{"vangrt",52},
mph::pair{"pr",53},
mph::pair{"eDot",54},
mph::pair{"lnapprox",55},
mph::pair{"RuleDela",56},
mph::pair{"ccupssm",57},
mph::pair{"CapitalD",58},
mph::pair{"zeetrf",59},
};
constexpr std::size_t entities_size=sizeof(entities)/sizeof(entities[0]);

#include <string_view>

bool found(std::string_view x)
{
  return *mph::hash<entities>(x)<entities_size;
}