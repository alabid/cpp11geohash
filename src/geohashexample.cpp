#include <cassert>
#include "geohash.h"

int main() {
  Point TestPoints[] = {{37.8324, 112.558},
                        {63.2345, 25.5323},
                        {-10.23, -170.23},
                        {11.12, 50.324},
                        {23.34, -150.343},
                        {10.22, -179.92}};

  cout << "encoding and decoding the following points..." << endl;
  for (const Point& p: TestPoints) {
    cout << p << endl;
    GeoHasher gh;
    const uint64_t hash = gh.encode(p);
    assert(p == gh.decode(hash));
    
    GeoHasherBase32 gh32;
    const string s32 = gh32.encode(p);
    assert(p == gh32.decode(s32));
    
    GeoHasherBase64 gh64;
    const string s64 = gh64.encode(p);
    assert(p == gh64.decode(s64));
  }
}

