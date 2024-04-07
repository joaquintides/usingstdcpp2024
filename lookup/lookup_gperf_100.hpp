/* C++ code produced by gperf version 3.1 */
/* Command-line: gperf -I -L C++ gperf_data_100.txt  */
/* Computed positions: -k'1-2,4-5' */

#if !((' ' == 32) && ('!' == 33) && ('"' == 34) && ('#' == 35) \
      && ('%' == 37) && ('&' == 38) && ('\'' == 39) && ('(' == 40) \
      && (')' == 41) && ('*' == 42) && ('+' == 43) && (',' == 44) \
      && ('-' == 45) && ('.' == 46) && ('/' == 47) && ('0' == 48) \
      && ('1' == 49) && ('2' == 50) && ('3' == 51) && ('4' == 52) \
      && ('5' == 53) && ('6' == 54) && ('7' == 55) && ('8' == 56) \
      && ('9' == 57) && (':' == 58) && (';' == 59) && ('<' == 60) \
      && ('=' == 61) && ('>' == 62) && ('?' == 63) && ('A' == 65) \
      && ('B' == 66) && ('C' == 67) && ('D' == 68) && ('E' == 69) \
      && ('F' == 70) && ('G' == 71) && ('H' == 72) && ('I' == 73) \
      && ('J' == 74) && ('K' == 75) && ('L' == 76) && ('M' == 77) \
      && ('N' == 78) && ('O' == 79) && ('P' == 80) && ('Q' == 81) \
      && ('R' == 82) && ('S' == 83) && ('T' == 84) && ('U' == 85) \
      && ('V' == 86) && ('W' == 87) && ('X' == 88) && ('Y' == 89) \
      && ('Z' == 90) && ('[' == 91) && ('\\' == 92) && (']' == 93) \
      && ('^' == 94) && ('_' == 95) && ('a' == 97) && ('b' == 98) \
      && ('c' == 99) && ('d' == 100) && ('e' == 101) && ('f' == 102) \
      && ('g' == 103) && ('h' == 104) && ('i' == 105) && ('j' == 106) \
      && ('k' == 107) && ('l' == 108) && ('m' == 109) && ('n' == 110) \
      && ('o' == 111) && ('p' == 112) && ('q' == 113) && ('r' == 114) \
      && ('s' == 115) && ('t' == 116) && ('u' == 117) && ('v' == 118) \
      && ('w' == 119) && ('x' == 120) && ('y' == 121) && ('z' == 122) \
      && ('{' == 123) && ('|' == 124) && ('}' == 125) && ('~' == 126))
/* The character set is not based on ISO-646.  */
#error "gperf generated tables don't work with this execution character set. Please report a bug to <bug-gperf@gnu.org>."
#endif

#include <string.h>

#define TOTAL_KEYWORDS 100
#define MIN_WORD_LENGTH 2
#define MAX_WORD_LENGTH 20
#define MIN_HASH_VALUE 2
#define MAX_HASH_VALUE 166
/* maximum key range = 165, duplicates = 0 */

class Perfect_Hash
{
private:
  static inline unsigned int hash (const char *str, size_t len);
public:
  static const char *in_word_set (const char *str, size_t len);
};

inline unsigned int
Perfect_Hash::hash (const char *str, size_t len)
{
  static unsigned char asso_values[] =
    {
      167, 167, 167, 167, 167, 167, 167, 167, 167, 167,
      167, 167, 167, 167, 167, 167, 167, 167, 167, 167,
      167, 167, 167, 167, 167, 167, 167, 167, 167, 167,
      167, 167, 167, 167, 167, 167, 167, 167, 167, 167,
      167, 167, 167, 167, 167, 167, 167, 167, 167, 167,
      167,  50,  45, 167, 167, 167, 167, 167, 167, 167,
      167, 167, 167, 167, 167,  80, 167,  10,  45, 167,
      167,  35, 167, 167, 167, 167,  60, 167,  35,  60,
       55,  20,   0,   5,  15,  35,   0, 167, 167,   0,
        5, 167, 167, 167, 167, 167, 167,  20,  10,  10,
       50,   5,  45,  40,  60,   0,   0,  45,   5,  50,
       10,  45,   0,  65,   0,   5,   0,  10,  20,  35,
      167,  30,  30, 167, 167, 167, 167, 167, 167, 167,
      167, 167, 167, 167, 167, 167, 167, 167, 167, 167,
      167, 167, 167, 167, 167, 167, 167, 167, 167, 167,
      167, 167, 167, 167, 167, 167, 167, 167, 167, 167,
      167, 167, 167, 167, 167, 167, 167, 167, 167, 167,
      167, 167, 167, 167, 167, 167, 167, 167, 167, 167,
      167, 167, 167, 167, 167, 167, 167, 167, 167, 167,
      167, 167, 167, 167, 167, 167, 167, 167, 167, 167,
      167, 167, 167, 167, 167, 167, 167, 167, 167, 167,
      167, 167, 167, 167, 167, 167, 167, 167, 167, 167,
      167, 167, 167, 167, 167, 167, 167, 167, 167, 167,
      167, 167, 167, 167, 167, 167, 167, 167, 167, 167,
      167, 167, 167, 167, 167, 167, 167, 167, 167, 167,
      167, 167, 167, 167, 167, 167
    };
  unsigned int hval = len;

  switch (hval)
    {
      default:
        hval += asso_values[static_cast<unsigned char>(str[4])];
      /*FALLTHROUGH*/
      case 4:
        hval += asso_values[static_cast<unsigned char>(str[3])];
      /*FALLTHROUGH*/
      case 3:
      case 2:
        hval += asso_values[static_cast<unsigned char>(str[1])];
      /*FALLTHROUGH*/
      case 1:
        hval += asso_values[static_cast<unsigned char>(str[0])];
        break;
    }
  return hval;
}

const char *
Perfect_Hash::in_word_set (const char *str, size_t len)
{
  static const char * wordlist[] =
    {
      "", "",
      "pr",
      "", "", "", "", "",
      "sim",
      "pscr",
      "",
      "prurel",
      "ee",
      "Ycy",
      "npar",
      "ltrie",
      "nrtrie",
      "ne",
      "nlE",
      "ltcc",
      "rceil",
      "nltrie",
      "npolint",
      "lnapprox",
      "",
      "bepsi",
      "nexist",
      "bigcirc",
      "precneqq",
      "bull",
      "uplus",
      "",
      "ccupssm",
      "cap",
      "Zeta",
      "bsolb",
      "easter",
      "", "",
      "caps",
      "circlearrowleft",
      "zeetrf",
      "",
      "intlarhk",
      "ntriangleright",
      "ijlig",
      "",
      "ge",
      "gel",
      "opar",
      "CapitalDifferentialD",
      "lbrack",
      "",
      "fpartint",
      "eDot",
      "gimel",
      "",
      "Pi",
      "profsurf",
      "boxV",
      "utdot",
      "lesges",
      "gtquest",
      "boxtimes",
      "euro",
      "imacr",
      "plusdu",
      "GreaterTilde",
      "Pcy",
      "bbrk",
      "RightArrow",
      "RuleDelayed",
      "lbrksld",
      "amp",
      "QUOT",
      "nleftrightarrow",
      "rightrightarrows",
      "ShortUpArrow",
      "yfr",
      "leqq",
      "lharu",
      "atilde",
      "", "",
      "yacy",
      "sqsup",
      "vangrt",
      "", "",
      "copy",
      "kappa",
      "zcaron",
      "",
      "Dfr",
      "nLtv",
      "sqsub",
      "NotRightTriangle",
      "", "",
      "gsim",
      "OpenCurlyDoubleQuote",
      "ffllig",
      "",
      "imagline",
      "nopf",
      "Acirc",
      "dlcrop",
      "", "",
      "Coproduct",
      "rdquo",
      "rdquor",
      "",
      "boxminus",
      "",
      "blk34",
      "twoheadleftarrow",
      "", "",
      "boxh",
      "",
      "Otimes",
      "",
      "thicksim",
      "",
      "NotGreater",
      "Updownarrow",
      "", "", "",
      "nLeftrightarrow",
      "", "", "", "", "",
      "origof",
      "", "",
      "fork",
      "",
      "horbar",
      "", "",
      "dopf",
      "", "", "", "", "", "", "", "", "",
      "", "", "", "", "", "", "", "", "",
      "", "", "",
      "dfisht"
    };

  if (len <= MAX_WORD_LENGTH && len >= MIN_WORD_LENGTH)
    {
      unsigned int key = hash (str, len);

      if (key <= MAX_HASH_VALUE)
        {
          const char *s = wordlist[key];

          if (*str == *s && !strcmp (str + 1, s + 1))
            return s;
        }
    }
  return 0;
}

#include <string_view>

bool found(std::string_view x)
{
  return Perfect_Hash::in_word_set(x.data(),x.size());
}
