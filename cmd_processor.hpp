#ifndef _CMD_PROCESSOR_H
#define _CMD_PROCESSOR_H

#include <memory>

class Cmd;
class Result;

class CmdProcessor {
public:
	CmdProcessor() = delete;
	virtual ~CmdProcessor();

	virtual Result apply(const std::unique_ptr<Cmd>& cmd) = 0;
};

struct Cmd {
	enum Type {
		GET,
		SET,
	};
	struct Header {
		Type type;
		int size;
	};
	std::unique_ptr<Header> header;
	char buf[];
};

struct Result {
	enum Type {
		SUCCESS,
		FAILURE,
		PANIC,
	};
	char buf[];
};

#endif
