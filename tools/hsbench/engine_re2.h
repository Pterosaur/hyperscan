#ifndef ENGINERE2_H
#define ENGINERE2_H

#include "expressions.h"
#include "engine.h"
#include "hs_runtime.h"

#include <re2/set.h>
#include <memory>
#include <vector>

struct CompileRE2Stats {

};

class EngineRE2Context : public EngineContext {
public:
    explicit EngineRE2Context();
    ~EngineRE2Context();

	std::vector<int> v;
};

/** RE2 Engine for scanning data. */
class EngineRE2 : public Engine {
public:

	explicit EngineRE2(std::unique_ptr<RE2::Set> db);

	~EngineRE2();

    // allocate an EngineContext
    virtual std::unique_ptr<EngineContext> makeContext() const;

    // non-streaming scan
    void scan(const char *data, unsigned len, unsigned blockId,
                      ResultEntry &results, EngineContext &ectx) const;

    // vectoring scan
    void scan_vectored(const char *const *data, const unsigned int *len,
                               unsigned int count, unsigned int streamId,
                               ResultEntry &result,
                               EngineContext &ectx) const;

    // stream open
    std::unique_ptr<EngineStream> streamOpen(EngineContext &ectx,
                                                     unsigned id) const;

    // stream close
    void streamClose(std::unique_ptr<EngineStream> stream,
                             ResultEntry &result) const;

    // stream compress and expand
    void streamCompressExpand(EngineStream &stream,
                                      std::vector<char> &temp) const;

    // streaming scan
    void streamScan(EngineStream &stream, const char *data,
                            unsigned int len, unsigned int id,
                            ResultEntry &result) const;

    void printStats() const;

    void sqlStats(SqlDB &db) const;

private:
    std::unique_ptr<RE2::Set> db;
};

std::unique_ptr<EngineRE2> buildEngineRe2(const ExpressionMap &expressions,
                                          const std::string &name,
                                          const std::string &sigs_name);


#endif //ENGINERE2_H
