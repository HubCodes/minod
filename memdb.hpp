#ifndef _MEMDB_HPP
#define _MEMDB_HPP

class Memdb final {
public:
	explicit Memdb();
	~Memdb();

	void set(const char* buffer, const int keysize, const int valuesize);
	char* get(const char* key, int& valuesize);
};

#endif
