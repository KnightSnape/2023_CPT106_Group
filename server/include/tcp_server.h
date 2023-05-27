#pragma once
#include"utils/packing.h"
#include"utils/person.h"
#include<fstream>
#include<sys/types.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<errno.h>
#include<unistd.h>
#include"netinet/in.h"

#define SERVER_IP "0.0.0.0"
#define SERVER_PORT 8080

namespace CPT106_Group
{

class Server
{
    public:
        Server();
        ~Server();
        void init_socket();
        void load_params();
        nlohmann::json createJson();
        std::string toJSONString(const nlohmann::json& json);
        void handle_json(const std::string& json_string);
        void send_json(const nlohmann::json& json);
        void solve_ping();
        void solve_get_available_spots(int floor);
        void solve_exit(std::string vehicle);
        void solve_entrance(std::string vehicle,std::string tips);
        void solve_add_account(std::string username,std::string password);
        void solve_query_permission(std::string username,std::string password);
        void solve_query_vehicle(std::string vehicle);
        void solve_query_spots();
        void solve_update_spots(nlohmann::json json);
        bool check_password(std::string username,std::string password);
        void update();
        void main_process();

    private:    
        int serverSocket,clientSocket;
        struct sockaddr_in serverAddr,clientAddr;
        socklen_t clientAddrLen;

        int car_cnt_;
        int trunk_cnt_;
        int van_cnt_;
        int motorcycle_cnt_;

        int car_cnt_state_;
        int trunk_cnt_state_;
        int van_cnt_state_;
        int motorcycle_cnt_state_;

        int leave_state{};
        int customer_fee{};

        int entrance_state{};
        int entrance_floor{};
        int entrance_spot{};

        int add_account_state{};

        int query_permission_state{};

        int query_vehicle_state{};
        int vehicle_floor{};
        int vehicle_spot{};
        std::string permission;

        bool is_password_correct;

        bool is_ping_available;
        bool is_show_available_spots;
        bool is_exit_available;
        bool is_entrance_available;
        bool is_add_account_available;
        bool is_query_permission_available;
        bool is_query_spots_available;
        bool is_query_vehicle_available;
        bool is_update_spots_available;

        Manager manager;
        Packing::Ptr packing_ptr_;
};


    
}