#include "geohash.h"

const char* GeoHasherBase32::charMap
{"0123456789bcdefghjkmnpqrstuvwxyz"};

const char* GeoHasherBase64::charMap
{"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/"};

const uint64_t GeoHasher::encode(const Point& p) { 
  float minLat = -LATITUDE_RANGE;
  float maxLat = LATITUDE_RANGE;
  float minLon = -LONGITUDE_RANGE;
  float maxLon = LONGITUDE_RANGE;
  float mid;
  uint64_t hash = 0;
  unsigned int numBits = 0;
  while (numBits < bitDepth) {
    hash <<= 1;
    // longitude --> even bits
    // latitude --> odd bits
    if (numBits % 2 == 0) {
      mid = (maxLon + minLon) / 2;
      if (p.lon > mid) {
        hash |= 1;        
        minLon = mid;
      } else {
        maxLon = mid;
      }
    } else {
      mid = (maxLat + minLat) / 2;
      if (p.lat > mid) {
        hash |= 1;
        minLat = mid;
      } else {
        maxLat = mid;
      }
    }

    numBits++;
  }
  return hash;
}

const Point GeoHasher::decode(const uint64_t hash) {
  auto& boundingBox = decodeBox(hash);
  const Point& minPoint = boundingBox.first;
  const Point& maxPoint = boundingBox.second;
  float lat = (minPoint.lat + maxPoint.lat) / 2;
  float lon = (minPoint.lon + maxPoint.lon) / 2;

  error = {maxPoint.lat - lat, maxPoint.lon - lon};
  return {lat, lon};
}

const pair<Point, Point> GeoHasher::decodeBox(const uint64_t hash) {
  float minLat = -LATITUDE_RANGE;
  float maxLat = LATITUDE_RANGE;
  float minLon = -LONGITUDE_RANGE;
  float maxLon = LONGITUDE_RANGE;
  float midLat, midLon;
  bool isEven;

  unsigned int numBits = 0;
  uint64_t one = 1;
  while (numBits < bitDepth) {
    midLon = (maxLon + minLon) / 2;
    midLat = (maxLat + minLat) / 2;
    isEven = numBits % 2 == 0;

    if (hash & (one << (bitDepth-numBits-1))) {
      if (isEven) {
        minLon = midLon;
      } else {
        minLat = midLat;
      } 
    } else {
      if (isEven) {
        maxLon = midLon;
      } else {
        maxLat = midLat;
      } 
    }

    numBits++;
  }

  return make_pair(Point{minLat, minLon}, Point{maxLat, maxLon});
}

const unsigned int GeoHasherBaseX::findBase(const char* map) {
  int mapLen = strlen(map);
  int base = 2;
  while ((1 << base) != mapLen) base++;
  return base;
}

const string GeoHasherBaseX::toChars(const char* map, uint64_t intHash) {
  int base = findBase(map);

  string ret {};
  int h, i;
  int curBit = 0;
  uint64_t baseMax = strlen(map)-1;

  while (curBit < bitDepth) {
    h = ((baseMax << curBit) & intHash) >> curBit;
    ret = map[h] + ret;
    curBit += base;
  }

  return ret;
}

const uint64_t GeoHasherBaseX::toInt(const char* map, string sHash) {
  int base = findBase(map);
  uint64_t intHash = 0, 
    pos;
  int curBit = 0;
  const char* mapPtr;
  char ch;

  for (int i = sHash.size()-1; i >= 0; i--) {
    mapPtr = map;
    ch = sHash[i];
    while (*mapPtr++ != ch);

    pos = mapPtr - map - 1;
    intHash |= pos << curBit;
    curBit += base;
  }

  return intHash;
}

const string GeoHasherBase32::encode(const Point& p) {
  uint64_t intHash = GeoHasher::encode(p);
  return toChars(charMap, intHash);
}

const Point GeoHasherBase32::decode(const string& s) {
  uint64_t intHash = toInt(charMap, s);
  return GeoHasher::decode(intHash);
}

const string GeoHasherBase64::encode(const Point& p) {
  uint64_t intHash = GeoHasher::encode(p);
  return toChars(charMap, intHash);
}

const Point GeoHasherBase64::decode(const string& s) {
  uint64_t intHash = toInt(charMap, s);
  return GeoHasher::decode(intHash);
}

ostream& operator<<(ostream& os, const Point& p) {
  os << '{' << p.lat << ", " << p.lon << '}';
  return os;
}

bool operator==(const Point& lhs, const Point& rhs) {
  return (abs(lhs.lat - rhs.lat) < 1e-3 &&
          abs(lhs.lon - rhs.lon) < 1e-3);
}
