#include <stdlib.h>
#include <string.h>
#include <string>
#include <platon/platon.hpp>


namespace platon {
    class Hello : public Contract {
        public:
            Hello(){}
            virtual void init() {
                println("init success...");
            }
            PLATON_EVENT(hello, const char *)
        public:
            void print(const char *msg)  {
                println(msg);
                PLATON_EMIT_EVENT(hello, msg);
            }
    };

}

PLATON_ABI(platon::Hello, print)
//platon autogen begin
extern "C" { 
void print(const char * msg) {
platon::Hello Hello_platon;
Hello_platon.print(msg);
}
void init() {
platon::Hello Hello_platon;
Hello_platon.init();
}

}
//platon autogen end