#pragma once
// Host numerical shim only. RTL co-simulation must validate bounded FIFOs.
#include <queue>
#include <cassert>
namespace hls {template<class T>class stream {
 std::queue<T> q;
public:
 explicit stream(const char*){}
 void write(const T&v){q.push(v);}
 T read(){assert(!q.empty());T v=q.front();q.pop();return v;}
 ~stream(){assert(q.empty());}
};}
