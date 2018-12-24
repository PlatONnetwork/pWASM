
#ifndef PLATON_MPCC_CPP
#define PLATON_MPCC_CPP

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <platon/platon.hpp>

namespace mpc {
	
#define COMMON_SPLIT_CHAR "&"
#define OWNER "__OWNER__"

#define PREFIX "__MPC__"
#define PREFIX_BONUS_RULE "__BONUS_RULE__"

#define KEY_IR "__IR__"
#define KEY_PARTIES PREFIX "__PARTIES__"
#define KEY_URLS PREFIX "__URLS__"
#define KEY_INVITORS PREFIX "__INVITOR__"
#define KEY_TASK_INFO PREFIX "__TASK__"
#define KEY_METHOD_PRICE PREFIX "__METHOD_PRICE__"

/// IR data -> IR_VALUE_TEMPLATE
#define IR_VALUE "$IR_VALUE_TEMPLATE$"

/// Starter -> INVITOR_VALUE_TEMPLATE
#define INVITOR_VALUE "$INVITOR_VALUE_TEMPLATE$"

/// Participator，use symbol '&' to split，eg：a&b&c -> PARTIES_VALUE_TEMPLATE
#define PARTIES_VALUE "$PARTIES_VALUE_TEMPLATE$"

/// Urls for participators, use symbol '&' to split.
/// URLS -> URLS_VALUE_TEMPLATE
#define URLS_VALUE "$URLS_VALUE_TEMPLATE$"

/// Rule of allocation.
/// format：k1:v1,k2:v2,k3:v3 -> PROFIT_RULES_VALUE
#define PROFIT_RULES_VALUE "$PROFIT_RULES_VALUE$"

/// Calu method's price -> METHOD_PRICE_VALUE
/// rule：${METHOD}&${VALUE},${METHOD}&${VALUE}
/// eg: func01$10000000000000000,func02$200000000000000,func03$4000000000000000000000000000
#define METHOD_PRICE_VALUE "$METHOD_PRICE_VALUE$"

/// the prefix key of map struct
#define PREFIX_RULES_MAP PREFIX "_MAP_RULE_"
#define PREFIX_RESULT_MAP PREFIX "_MAP_RESULT_"
#define PREFIX_ALLOT_MAP PREFIX "_MAP_ALLOT_"
#define PREFIX_METHOD_MAP PREFIX "_MAP_PRICE_"

class MPC : public platon::Contract {
    public:
		// define event.
        PLATON_EVENT(start_calc_event, uint64_t, const char *)
        PLATON_EVENT(set_result_event, uint64_t, const char *)
		PLATON_EVENT(set_fees_event, uint64_t, const char *)

        void init() {
            platon::setState(OWNER, platon::caller().toString()); // save msg.sender
            platon::setState(KEY_INVITORS, INVITOR_VALUE);      // save invitor
            platon::setState(KEY_PARTIES, PARTIES_VALUE);       // save partner
            platon::setState(KEY_URLS, URLS_VALUE);             // save all urls
            platon::setState(KEY_IR, IR_VALUE);                 // save ir data
            init_set_urls();                                    // save each one by id
            init_set_price();									// save price of method
        }

		/// get the owner address of contract.
        const char * get_owner() const {
            std::string _owner_ ;
            platon::getState(OWNER, _owner_);
            return _owner_.c_str();
        }

		/// trigger mpc compute.
        void start_calc(const char *method, const char *extra) {

            // check msg.send is valid
            bool _isPartner = is_partner();
            if(!_isPartner){
                platon::println("Invalid msg.sender, not belong to partners.");
                PLATON_EMIT_EVENT(start_calc_event, 0, "Invalid msg.sender, not belong to partners.");
                return;
            }
			
			// check value is greater than preprice.
			bool is_enough_res = is_enough_price(method);
			if(!is_enough_res){
				platon::println("insufficient value to the method of ", method);
				PLATON_EMIT_EVENT(start_calc_event, 0, "Insufficient value for the method.");
				return;
			}

            // gen task id and save value
            const char *task_id = gen_task_id().c_str();
            platon::println("New task id: ", task_id);
            bool save_res = save_allot_amount(task_id);
            if(!save_res) {
                platon::println("save allot amount fail");
                PLATON_EMIT_EVENT(start_calc_event, 0, "save allot amount fail");
                return;
            }

            // save task information. INFO_PREFIX + TASK_ID
            std::string task_key_str = KEY_TASK_INFO + std::string(task_id);
            std::string info_str = std::string(method) + COMMON_SPLIT_CHAR + std::string(extra);
            platon::setState(task_key_str, info_str);
            platon::println("start success..");
            PLATON_EMIT_EVENT(start_calc_event, 1, task_id);
        }

        /// The result recored, and allot profit to participators.
        void set_result(const char *taskId, uint64_t status, const char *data) {
            // check msg.send is valid
            bool _isPartner = is_partner();
            if(!_isPartner){
                platon::println("set_result->Invalid msg.sender, not belong to partners.");
                PLATON_EMIT_EVENT(start_calc_event, 0, "Invalid msg.sender, not belong to partners.");
                return;
            }

            std::string key_str = PREFIX_RESULT_MAP + std::string(taskId);
            // check result set
            std::string res_value_str;
            platon::getState(key_str, res_value_str);
            if(res_value_str.length() > 1){
                platon::println("set_result-> set already. Can not reset again.");
                PLATON_EMIT_EVENT(start_calc_event, 0, "set already. Can not reset again.");
                return;
            }

            // save result
            std::string status_str = toString(status);
            std::string data_str = std::string(data);
            std::string value_str = status_str + COMMON_SPLIT_CHAR + data_str;
            platon::println("save result, key: ", key_str, " value:", value_str);
            platon::setState(key_str, value_str);

            // divide the money. equal division.
            if(status != 1){    // 1 success 0 fail.
                platon::println("set_result-> status must equal 1.");
                PLATON_EMIT_EVENT(start_calc_event, 0, "status must equal 1.");
                return;
            }
            platon::u256 amount_i = get_saved_amount(taskId);
            const char *partners_c = get_participants();
            std::string partner_str = std::string(partners_c);
            std::vector<std::string> partner_vec = split(partner_str, COMMON_SPLIT_CHAR);
			
            platon::u256 count_i(partner_vec.size());
			
            platon::println("part per：", count_i.convert_to<std::string>());
            platon::u256 each_one_i = amount_i / count_i;
            platon::println("avg ：", each_one_i.convert_to<std::string>());

            // transfer to partner
            for(unsigned int i = 0; i < partner_vec.size(); ++i) {
                std::string p_str = partner_vec[i];
                platon::println("transfer to:", p_str, " value:", each_one_i.convert_to<std::string>());
				platon::Address _addr(p_str, true);
                platon::callTransfer(_addr, each_one_i);
            }
			platon::println("Set result success.");
            PLATON_EMIT_EVENT(set_result_event, 1, "Setting result success.");
        }

		/// k1:v1,k2:v2,k3:v3 
		void set_fees(const char *fees) {
			platon::println("into set_fees...");
			PLATON_EMIT_EVENT(set_fees_event, 1, "set fees done.");
		}
		
        const char * get_ir_data() const {
            std::string irdata;
            platon::getState(KEY_IR, irdata);
            return irdata.c_str();
        }

        const char * get_participants() const {
            std::string parties;
            platon::getState(KEY_PARTIES, parties);
            platon::println("get_participants:", parties);
            return parties.c_str();
        }

        const char * get_urls() const {
            std::string urls;
            platon::getState(KEY_URLS, urls);
            return urls.c_str();
        }

        const char * get_url_by_id(const char *id) const {
            std::string url;
            std::string key_str = KEY_URLS + std::string(id);
            platon::getState(key_str, url);
            return url.c_str();
        }

        const char * get_result(const char *task_id) const {
            std::string id_str = std::string(task_id);
            std::string key_str = PREFIX_RESULT_MAP + std::string(id_str);
            std::string result;
            platon::getState(key_str, result);
			// std::string value_str = status_str + COMMON_SPLIT_CHAR + data_str;
			std::vector<std::string> partner_vec = split(result, COMMON_SPLIT_CHAR);
			if(partner_vec.size() != 2) {
				return "";
			}
			return partner_vec[1].c_str();
        }

		uint64_t get_status(const char *task_id) const {
			std::string id_str = std::string(task_id);
            std::string key_str = PREFIX_RESULT_MAP + std::string(id_str);
            std::string result;
            platon::getState(key_str, result);
			std::vector<std::string> partner_vec = split(result, COMMON_SPLIT_CHAR);
			if(partner_vec.size() != 2) {
				return 0;
			}
			uint64_t st = stouint64(partner_vec[0]);
			return st;
		}

        const char* get_fee(const char* method) const {
            platon::u256 fee;
            std::string key_str = KEY_METHOD_PRICE + std::string(method);
            platon::getState(key_str, fee);
            platon::println("call get_fee. - ", fee.convert_to<std::string>());
            return fee.convert_to<std::string>().c_str();
        }

        const char * get_invitor() const {
            std::string invitor;
            platon::getState(KEY_INVITORS, invitor);
            return invitor.c_str();
        }

    private:
        std::string gen_task_id() const {
            uint64_t nonce_i = getCallerNonce();
            std::string nonce_str = toString(nonce_i);
            platon::h160 caller_h = platon::caller();
            std::string caller_h_str = caller_h.toString();
            platon::println("get nonce: ", nonce_str);
			platon::println("get caller_h_str: ", caller_h_str);
            std::string src_str = nonce_str + caller_h_str;
            char *t_src;
            strcpy(t_src, src_str.c_str());
            
            platon::h256 sha3_h = platon::sha3((platon::byte*)t_src, strlen(t_src));
            std::string sha3_h_str = sha3_h.toString();
            platon::println("task id : ", sha3_h_str);
            return sha3_h_str;
        }

        bool is_partner() {
            platon::h160 caller_h = platon::caller();
            std::string caller_h_str = caller_h.toString();
            std::string partner_str;
            platon::getState(KEY_PARTIES, partner_str);
            platon::println("Participator ：", partner_str);
            platon::println("Caller ：", caller_h_str);
            std::string::size_type idx = caller_h_str.find("0x");
            if(idx == std::string::npos){
                caller_h_str = "0x" + caller_h_str;
            }
            platon::println("Caller End：", caller_h_str);
            std::vector<std::string> partner_vec = split(partner_str, COMMON_SPLIT_CHAR);
            for(unsigned int i = 0 ; i < partner_vec.size(); ++i) {
                platon::println("v_caller:", partner_vec[i]);
                if(strcmp(caller_h_str.c_str(), partner_vec[i].c_str()) == 0){
                    return true;
                }
            }
            return false;
        }
		
		/// blance is engouth for caller to pay the method's price.
		bool is_enough_price(const char *method) {
			// the price of method.
			platon::u256 fee;
            std::string key_str = KEY_METHOD_PRICE + std::string(method);
			platon::println("is_enough_price : key_str", key_str);
            platon::getState(key_str, fee);
			platon::println("is_enough_price:call get_fee. - ", fee.convert_to<std::string>());
			// the pricee of value to caller.
			platon::u256 _value = platon::callValue();
			platon::println("is_enough_price:call get_value. - ", _value.convert_to<std::string>());
			if(_value >= fee) {
				platon::println("is_enough_price: return true.");
				return true;
			}
			platon::println("is_enough_price: return false.");
			return false;
        }
		
        /// transfer of value, from user's account to contract's account.
        bool save_allot_amount(const char *task_id) {
			// platon::balance().convert_to<std::string>().c_str();
			platon::u256 value_u = platon::callValue();
            std::string key_str = PREFIX_ALLOT_MAP + std::string(task_id);
            
			std::string value_str = value_u.convert_to<std::string>();
            platon::println("save_allot_amount: ", key_str, " - ", value_str);
            platon::setState(key_str, value_u);
            return true;
        }

        platon::u256 get_saved_amount(const char *task_id) {
            std::string key_str = PREFIX_ALLOT_MAP + std::string(task_id);
            platon::u256 value_u;
            platon::getState(key_str, value_u);
            platon::println("get_saved_amout: ", value_u.convert_to<std::string>());
            return value_u;
        }

        void init_set_urls() {
            // URLS_VALUE -> "id01$tcpSession:tcp -p 9527,id02$tcpSession:tcp -p 9527,id03$tcpSession:tcp -p 9527"
            char *values = URLS_VALUE;
            char *pattern = ",";
            // ["id01$tcpSession:tcp -p 9527", "id02$tcpSession:tcp -p 9527", "id03$tcpSession:tcp -p 9527"]
            // KEY_URLS + "id01" -> tcpSession:tcp -p 9527
            platon::println("---- begin split ------");
            std::vector<std::string> arr = split(values, pattern);
            for(unsigned int i = 0 ; i < arr.size(); ++i) {
                char *trim_res = new char[100] ;
                char *str_in = new char[100];
                strcpy(str_in, arr[i].c_str());

                str_trim(str_in, trim_res);

                std::vector<std::string> id_valule_arr = split(str_in, "$");
                if(id_valule_arr.size() != 2) {
                    platon::println("Invalid value str of urls.");
                    platon::platonThrow("Invalid value str of urls.");
                }
                // do set state
                std::string _key = KEY_URLS + id_valule_arr[0];
                std::string _value = id_valule_arr[1];
                platon::println("init urls, key : ", _key, " , values: ", _value);
                platon::setState(_key, _value);
                delete []str_in;
                delete []trim_res;
            }
        }

		void init_set_price() {
            // METHOD_PRICE_VALUE -> "func01$10000000000000000,func02$200000000000000,func03$4000000000000000000000000000"
            char *values = METHOD_PRICE_VALUE;
            char *pattern = ",";
            // ["func01$10000000000000000", "func01$10000000000000000", "func01$10000000000000000"]
            // PREFIX_METHOD_MAP + "func01" -> 10000000000000000
            platon::println("---- Begin split price ------");

			std::vector<std::string> arr = split(values, pattern);
			platon::println("---- End split price ------", arr.size());
			for(unsigned int i = 0 ; i < arr.size(); ++i) {
                char *trim_res = new char[100] ;
                char *str_in = new char[100];
                strcpy(str_in, arr[i].c_str());
                str_trim(str_in, trim_res);

                std::vector<std::string> id_valule_arr = split(str_in, "$");
                if(id_valule_arr.size() != 2) {
                    platon::println("Invalid value str of price.");
                    platon::platonThrow("Invalid value str of price.");
                }
                // do set state
                std::string key_str = KEY_METHOD_PRICE + id_valule_arr[0];
                std::string value_str = id_valule_arr[1];
                platon::println("init price, key : ", key_str, " , values: ", value_str);
				platon::u256 value_u(value_str);
                platon::setState(key_str, value_u);
                delete []str_in;
                delete []trim_res;
            }
        }

    private:
        uint64_t stouint64(const std::string &num) const {
            uint64_t res = 0;
            for (size_t i = 0; i < num.length(); i++) {
                res = res * 10 + (num[i] - '0');
            }
            platon::println("stouint64 ", num, "->", res);
            return res;
        }

        std::string toString(uint64_t num) const {
            std::string res;
            while (num != 0) {
                char c = num % 10 + '0';
                num /= 10;
                res.insert(0, 1, c);
            }
            platon::println("toString ", num, "->", res);
            return res;
        }

        std::vector <std::string> split(const std::string &str, const std::string &pattern) const {
            // const char* convert to char*
            char *strc = new char[std::strlen(str.c_str()) + 1];
            strcpy(strc, str.c_str());
            std::vector <std::string> result_vec;
            char *tmp_str = strtok(strc, pattern.c_str());
            while (tmp_str != NULL) {
                result_vec.push_back(std::string(tmp_str));
                tmp_str = strtok(NULL, pattern.c_str());
            }
            delete[] strc;
            return result_vec;
        }

        void str_trim(char *str_in, char *str_out) {
            int i, j;
            i = 0;
            j = strlen(str_in) - 1;
            while(str_in[i] == ' '){
                i++;
            }
            while(str_in[j] == ' '){
                --j;
            }
            strncpy(str_out, str_in + i, j - i + 1);
            str_out[j - i + 1] = '\0';      // end flag
        }
    };
}

PLATON_ABI(mpc::MPC, start_calc);
PLATON_ABI(mpc::MPC, get_owner);
PLATON_ABI(mpc::MPC, set_result);
PLATON_ABI(mpc::MPC, get_ir_data);
PLATON_ABI(mpc::MPC, get_participants);
PLATON_ABI(mpc::MPC, get_urls);
PLATON_ABI(mpc::MPC, get_result);
PLATON_ABI(mpc::MPC, get_status);
PLATON_ABI(mpc::MPC, get_fee);
PLATON_ABI(mpc::MPC, get_invitor);
PLATON_ABI(mpc::MPC, get_url_by_id);


#endif