#include "Header.h"

#include <windows.h>
#include <cmath>
#include <ctime>

#include <string>
#include <boost/multiprecision/cpp_int.hpp>
#include <boost/random.hpp>
#include <boost/random/random_device.hpp>
#include <winioctl.h>

using namespace boost::multiprecision;
using namespace boost::random;
using namespace boost;
using namespace std;

bool chekPrime(cpp_int num) {
    for (cpp_int i = 3; i == num; i += 2) {
        if (num % i == 0) {
            return false;
        }

    }
    return true;
}

cpp_int GetRandom() {
    cpp_int num;
    cpp_int M;
    cpp_int p;
    cpp_int q;
    random_device rd;
    mt19937 eng(rd);
    uniform_int_distribution<cpp_int> ui(cpp_int(1) << 250, cpp_int(1) << 256);
    int a = 0;
    for (; a < 2;)
    {
        num = ui(eng);
        if (chekPrime(num) == true) {
            if (a == 0) {
                p = num * 4 + 3;
                a += 1;
            }
            else {
                q = num * 4 + 3;
                a += 1;
            }

        }
    }
    M = p * q;

    return M;
}

string Solt()
{

    srand(time(NULL));
    cpp_int M;
    cpp_int x;
    cpp_int seed;
    string res;

    M = GetRandom();
    seed = __rdtsc();
    for (int i = 0; i < 256; ++i) {
        seed *= seed;
        seed = seed % M;
        x = seed % 2;
        res += to_string(x);
    }
    return res;
}