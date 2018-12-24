//
// Created by zhou.yang on 2018/11/7.
//

#pragma once

#include <boost/endian/conversion.hpp>
#include "RLP.h"
/**
 * @brief Transaction coding operation
 * 
 */
namespace platon {

    /**
     * @brief Specified type encoding
     * 
     * @param stream RLP stream
     * @param d int8_t type
     */
    inline void txEncode(RLPStream &stream, int8_t d){
        d = boost::endian::endian_reverse(d);

        bytesConstRef ref((byte*)&d, sizeof(d));
        stream.append(ref);
    }

    /**
     * @brief Specified type encoding
     * 
     * @param stream RLP stream
     * @param d uint16_t type
     */
    inline void txEncode(RLPStream &stream, uint16_t d){
        d = boost::endian::endian_reverse(d);

        bytesConstRef ref((byte*)&d, sizeof(d));
        stream.append(ref);
    }

    /**
     * @brief Specified type encoding
     * 
     * @param stream RLP stream
     * @param d int16_t type
     */
    inline void txEncode(RLPStream &stream, int16_t d){
        d = boost::endian::endian_reverse(d);

        bytesConstRef ref((byte*)&d, sizeof(d));
        stream.append(ref);
    }

    /**
     * @brief Specified type encoding
     * 
     * @param stream RLP stream
     * @param d uint32_t type
     */
    inline void txEncode(RLPStream &stream, uint32_t d){
        d = boost::endian::endian_reverse(d);

        bytesConstRef ref((byte*)&d, sizeof(d));
        stream.append(ref);
    }

    /**
     * @brief Specified type encoding
     * 
     * @param stream RLP stream
     * @param d int32_t type
     */
    inline void txEncode(RLPStream &stream, int32_t d){
        d = boost::endian::endian_reverse(d);

        bytesConstRef ref((byte*)&d, sizeof(d));
        stream.append(ref);
    }

#ifdef WASM_COMPILE
    /**
     * @brief Specified type encoding
     * 
     * @param stream RLP stream
     * @param d int type
     */
    inline void txEncode(RLPStream &stream, int d){
        d = boost::endian::endian_reverse((int32_t)d);

        bytesConstRef ref((byte*)&d, sizeof(d));
        stream.append(ref);
    }
#endif

    /**
     * @brief Specified type encoding
     * 
     * @param stream RLP stream
     * @param d uint64_t type
     */
    inline void txEncode(RLPStream &stream, uint64_t d){
        d = boost::endian::endian_reverse(d);

        bytesConstRef ref((byte*)&d, sizeof(d));
        stream.append(ref);
    }

    /**
     * @brief Specified type encoding
     * 
     * @param stream RLP stream
     * @param d int64_t type
     */
    inline void txEncode(RLPStream &stream, int64_t d){
        d = boost::endian::endian_reverse(d);

        bytesConstRef ref((byte*)&d, sizeof(d));
        stream.append(ref);
    }

    /**
     * @brief Specified type encoding
     * 
     * @param stream RLP stream
     * @param d std::string type
     */
    inline void txEncode(RLPStream &stream, const std::string & d){
        stream.append(d);
    }

    /**
     * @brief Specified type encoding
     * 
     * @param stream RLP stream
     * @param d Char pointer type
     */
    inline void txEncode(RLPStream &stream, const char *d){
        stream.append(std::string(d));
    }

    /**
     * @brief Empty implementation
     * 
     * @param stream 
     */
    inline void txEncode(RLPStream &stream){
    }



    /**
     * @brief Serialize to RLPStream
     * 
     * @tparam Arg Starting element type
     * @tparam Args Variable parameter type
     * @param stream RLP stream
     * @param a Starting parameter
     * @param args Variable parameter
     */
    template<typename Arg, typename... Args>
    void txEncode(RLPStream &stream, Arg&& a, Args&&... args ) {
        txEncode(stream, a);
        txEncode(stream, args...);
    }
}
