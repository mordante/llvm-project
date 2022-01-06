#include <format>
#include <string_view>

int main() {
  // XXX extend to all types in use and move to commit upfront
  // this allows to validate the code bloat before and after.
  {
    std::string_view sv{"hello world"};
    std::format("{}{}|{}{}{}{}{}{}|{}{}{}{}{}{}|{}{}{}|{}{}|{}", true, '*',

                (signed char)(42), (short)(42), (int)(42), (long)(42), (long long)(42), (__int128_t)(42),

                (unsigned char)(42), (unsigned short)(42), (unsigned int)(42), (unsigned long)(42),
                (unsigned long long)(42), (__uint128_t)(42),

                (float)(42), (double)(42), (long double)(42),

                "hello world", sv,

                nullptr);
  }
  {
    std::wstring_view sv{L"hello world"};
    std::format(L"{}{}|{}{}{}{}{}{}|{}{}{}{}{}{}|{}{}{}|{}{}|{}", true, L'*',

                (signed char)(42), (short)(42), (int)(42), (long)(42), (long long)(42), (__int128_t)(42),

                (unsigned char)(42), (unsigned short)(42), (unsigned int)(42), (unsigned long)(42),
                (unsigned long long)(42), (__uint128_t)(42),

                (float)(42), (double)(42), (long double)(42),

                L"hello world", sv,

                nullptr);
  }
}
