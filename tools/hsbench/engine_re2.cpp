
#include "engine_re2.h"

#include "util/make_unique.h"

#include <algorithm>
#include <chrono>
#include <limits>

#ifndef __FUNCTION_NAME__
#ifdef WIN32
#define __FUNCTION_NAME__ __FUNCTION__
#else
#define __FUNCTION_NAME__ __func__
#endif
#endif

#define UNIMPLEMENT                                                            \
    throw std::logic_error(std::string()+__FUNCTION_NAME__+" is not implemented.");

EngineRE2Context::EngineRE2Context() {}

EngineRE2Context::~EngineRE2Context() {}

EngineRE2::EngineRE2(std::unique_ptr<RE2::Set> db_in) : db(std::move(db_in)) {}

EngineRE2::~EngineRE2() {}

std::unique_ptr<EngineContext> EngineRE2::makeContext() const {
    return std::unique_ptr<EngineContext>();
}

void EngineRE2::scan(const char *data, unsigned len, unsigned blockId,
                     ResultEntry &results, EngineContext &ectx) const {
    assert(data);
    std::string subject(data, len);
    
    auto start = std::chrono::high_resolution_clock::now();
    if (db->Match(subject, nullptr)) {
        results.matches++;
    }
    auto end = std::chrono::high_resolution_clock::now();
    double elapsed = std::chrono::duration<double>(end - start).count();
    //if (elapsed > 3.0) {
    //    std::cout << "ID:" << blockId << " elapsed: " << elapsed
    //              << " length: " << len << std::endl <<  "data: " << std::string(data, len)
    //              << std::endl;
    //}
}

void EngineRE2::scan_vectored(const char *const *data, const unsigned int *len,
                              unsigned int count, unsigned int streamId,
                              ResultEntry &result,
                              EngineContext &ectx) const {UNIMPLEMENT}

std::unique_ptr<EngineStream> EngineRE2::streamOpen(EngineContext &ectx,
                                                    unsigned id) const {
    UNIMPLEMENT
}

void EngineRE2::streamClose(std::unique_ptr<EngineStream> stream,
                            ResultEntry &result) const {
    UNIMPLEMENT
}

void EngineRE2::streamCompressExpand(EngineStream &stream,
                                     std::vector<char> &temp) const {
    UNIMPLEMENT
}

void EngineRE2::streamScan(EngineStream &stream, const char *data,
                           unsigned int len, unsigned int id,
                           ResultEntry &result) const {
    UNIMPLEMENT
}

void EngineRE2::printStats() const {
    printf("RE2 \n"); 
}

void EngineRE2::sqlStats(SqlDB &db) const {}

static bool readExpressionRe2(const std::string &input, std::string *expr) {
    if (input[0] != '/') {
        return false;
    }
    size_t end = input.find_last_of("/");
    if (end == std::string::npos) {
        return false;
    }
    *expr = input.substr(1, end - 1);
    std::string flag = input.substr(end + 1);
    return true;
}

std::unique_ptr<EngineRE2> buildEngineRe2(const ExpressionMap &expressions,
                                          const std::string &name,
                                          const std::string &sigs_name) {
    RE2::Options opt;
    opt.set_max_mem(std::numeric_limits<std::int64_t>::max());

    auto db = ue2::make_unique<RE2::Set>(opt, RE2::RE2::Anchor::UNANCHORED);
    for (const auto &m : expressions) {
        std::string pattern;
        if (!readExpressionRe2(m.second, &pattern)) {
            return nullptr;
        }
        std::string error;
        if (db->Add(pattern, &error) < 0) {
            std::cerr << error << std::endl;
            return nullptr;
        }
    }
    if (!db->Compile()) {
        return nullptr;
    }
    return ue2::make_unique<EngineRE2>(std::move(db));
}
