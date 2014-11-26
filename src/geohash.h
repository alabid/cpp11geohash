#ifndef GEOHASH_H
#define GEOHASH_H

#include <cstdint>
#include <cstring>
#include <string>
#include <utility>
#include <iostream>
#include <cmath>
using namespace std;

struct Point {
  Point() {}
  Point(double la, double lo): lat(la), lon(lo) {}
  friend ostream& operator<<(ostream& os, const Point& p);

  double lat;
  double lon;
};

bool operator==(const Point& lhs, const Point& rhs);

class GeoHasher {
public:
  GeoHasher(unsigned int bitD = 52): bitDepth{bitD} {}
  const uint64_t encode(const Point& p);
  const Point decode(const uint64_t hash);
  void setBitDepth(unsigned int num) {
    bitDepth = num;
  }
protected:
  const Point getError() {
    return error;
  }

  unsigned int bitDepth;
  // error.lat == error for latitude
  // error.lon == error for longitude
  Point error;
  static const int LATITUDE_RANGE {90};
  static const int LONGITUDE_RANGE {180};
private:
  const pair<Point, Point> decodeBox(const uint64_t);
};

class GeoHasherBaseX: public GeoHasher {
public:
  GeoHasherBaseX(): GeoHasher() {}
protected:
  const string toChars(const char*, uint64_t);
  const uint64_t toInt(const char*, string);
private:
  const unsigned int findBase(const char*);
};

class GeoHasherBase32: public GeoHasherBaseX {
public:
  GeoHasherBase32(): GeoHasherBaseX() {}
  const string encode(const Point&);
  const Point decode(const string&);
private:
  static const char* charMap;
};

class GeoHasherBase64: public GeoHasherBaseX {
public:
  GeoHasherBase64(): GeoHasherBaseX() {}
  const string encode(const Point&);
  const Point decode(const string&);
private:
  static const char* charMap;
};

#endif
