#include"tcp_server.h"

namespace CPT106_Group
{

Server::Server()
{
    packing_ptr_ = std::make_shared<CPT106_Group::Packing>();
    init_socket();
    load_params();
    main_process();
}   

Server::~Server()
{

}

void Server::init_socket()
{
    serverSocket = socket(AF_INET,SOCK_STREAM,0);
    if(serverSocket < 0)
    {
        std::cerr<<"Failed to create socket"<<std::endl;
        exit(-1);
    }
    int reuse = 1;
    setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR,(const void *)&reuse , sizeof(int));
    memset(&serverAddr,0,sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(SERVER_PORT);
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    if(bind(serverSocket, (struct sockaddr *)&serverAddr,sizeof(serverAddr)) < 0)
    {
        std::cerr<<"Failed to bind socket"<<std::endl;
        exit(-1);
    }
    if(listen(serverSocket,1) < 0)
    {
        std::cerr<<"Failed to listen on socket"<<std::endl;
        exit(-1);
    }
    std::cout<<"[Info]Server is listening on port:"<<SERVER_PORT<<std::endl;
}

void Server::load_params()
{
    std::string path = "../config/model1.json";
    std::ifstream ifs(path);

    nlohmann::json get_file = nlohmann::json::parse(ifs);

    car_cnt_ = 0;
    trunk_cnt_ = 0;
    van_cnt_ = 0;
    motorcycle_cnt_ = 0;

    car_cnt_state_ = 0;
    trunk_cnt_state_ = 0;
    van_cnt_state_ = 0;
    motorcycle_cnt_state_ = 0;

    is_password_correct = false;

    is_ping_available = false;
    is_show_available_spots = false;
    is_exit_available = false;
    is_entrance_available = false;
    is_add_account_available = false;
    is_query_permission_available = false;
    is_query_spots_available = false;
    is_update_spots_available = false;
    is_query_vehicle_available = false;

    packing_ptr_->clear_pack();
    for(int i = 1;i <= 3;i++)
    {
        int car_cnt = get_file["floor"][std::to_string(i)]["car"];
        int trunk_cnt = get_file["floor"][std::to_string(i)]["trunk"];
        int van_cnt = get_file["floor"][std::to_string(i)]["van"];
        int motorcycle_cnt = get_file["floor"][std::to_string(i)]["motorcycle"];
        for(int j = 0;j < car_cnt;j++)
        {
            packing_ptr_->set_pack(TRANSPORTATION::CAR,i,j,car_cnt_state_);
        }
        for(int j=0;j < trunk_cnt;j++)
        {
            packing_ptr_->set_pack(TRANSPORTATION::TRUNK,i,j + car_cnt,trunk_cnt_state_);
        }
        for(int j = 0;j < van_cnt;j++)
        {
            packing_ptr_->set_pack(TRANSPORTATION::VAN,i,j + car_cnt + trunk_cnt,van_cnt_state_);
        }
        for(int j = 0;j < motorcycle_cnt;j++)
        {
            packing_ptr_->set_pack(TRANSPORTATION::MOTORCYCLE,i,j + car_cnt + trunk_cnt + van_cnt,motorcycle_cnt_state_);
        }
    }

    std::string manager_path = "../config/manager.json";
    std::ifstream ifss(manager_path);

    nlohmann::json get_manager = nlohmann::json::parse(ifss);

    std::string root_name = get_manager["username"];
    std::string root_password = get_manager["password"];

    manager.set_name(root_name);
    manager.set_passwd(root_password);

    std::string customer_path = "../config/customer.json";
    std::ifstream ifsss(customer_path);

    nlohmann::json get_customer = nlohmann::json::parse(ifsss);

    int number = get_customer["number"];
    for(int i=1;i<=number;i++)
    {
        std::string customer_username = get_customer["customer" + std::to_string(i)]["username"];
        std::string customer_password = get_customer["customer" + std::to_string(i)]["password"];
        manager.add_person(customer_username,customer_password);
    }
}

void Server::handle_json(const std::string& json_string)
{
    nlohmann::json json = nlohmann::json::parse(json_string);

    std::string user_name = json["auth"]["account"];
    std::string password = json["auth"]["password"];

    bool permission = false;
    if(user_name == manager.get_name())
        permission = true;

    is_password_correct = check_password(user_name,password);
    std::string operation_name = json["operation"]["name"];
    if(operation_name == "ping")
    {
        solve_ping();
    }
    if(operation_name == "get_available_spots")
    {
        std::string floor = json["operation"]["arguments"]["floor"];
        int get_floor = std::stoi(floor);
        solve_get_available_spots(get_floor);
    }

    if(operation_name == "vehicle_exit")
    {
        if(permission == true)
        {
            std::string vehicle = json["operation"]["arguments"]["plate_number"];
            solve_exit(vehicle);
        }
        else
        {
            std::cout<<"[Error]Permission denied"<<std::endl;
            is_exit_available = true;
            leave_state = 5;
        }
    }

    if(operation_name == "vehicle_enter")
    {
        if(permission == true)
        {
            std::string vehicle = json["operation"]["arguments"]["plate_number"];
            std::string tips = json["operation"]["arguments"]["type"];
            solve_entrance(vehicle,tips);
        }
        else
        {
            std::cout<<"[Error]Permission denied"<<std::endl;
            is_entrance_available = true;
            entrance_state = 5;
        }
    }

    if(operation_name == "add_account")
    {
        if(permission == true)
        {
            std::string account = json["operation"]["arguments"]["account"];
            std::string password = json["operation"]["arguments"]["password"];
            solve_add_account(account,password);
        }
        else
        {
            std::cout<<"[Error]Permission denied"<<std::endl;
            is_add_account_available = true;
            add_account_state = 5;
        }
    }

    if(operation_name == "query_permission")
    {
        solve_query_permission(user_name,password);
    }

    if(operation_name == "query_vehicle")
    {
        std::string vehicle = json["operation"]["arguments"]["plate_number"];
        solve_query_vehicle(vehicle);
    }
    if(operation_name == "query_spots")
    {
        solve_query_spots();
    }
    if(operation_name == "update_spots")
    {
        solve_update_spots(json);
    }
}

nlohmann::json Server::createJson()
{
    nlohmann::json json;
    if(!is_password_correct)
    {
        json["state"]["succ"] = 0;
        json["state"]["reason"] = "Incorrect password";
        json["return_val"] = NULL;
        is_password_correct = false;
        return json;
    }
    if(is_ping_available)
    {
        json["state"]["succ"] = 1;
        json["state"]["reason"] = NULL;

        json["return_val"] = NULL;
        std::cout<<"[Debug]Ready to send"<<std::endl;
        is_ping_available = false;
    }
    if(is_show_available_spots)
    {
        if(car_cnt_state_ == 2 || trunk_cnt_state_ == 2 || van_cnt_state_ == 2 || motorcycle_cnt_state_ == 2)
        {
            json["state"]["succ"] = 0;
            json["state"]["reason"] = "You choose the wrong floor";

            json["return_val"] = NULL;
        }
        else
        {
            json["state"]["succ"] = 1;
            json["state"]["reason"] = NULL;
                    
            json["return_val"]["car_available"] = car_cnt_;
            json["return_val"]["motorcycle_available"] = motorcycle_cnt_;
            json["return_val"]["van_available"] = van_cnt_;
            json["return_val"]["trunk_available"] = trunk_cnt_;
        }
        std::cout<<"[Debug]Ready to send"<<std::endl;
        is_show_available_spots = false;
        car_cnt_state_ = 0;
        trunk_cnt_state_ = 0;
        van_cnt_state_ = 0;
        motorcycle_cnt_state_ = 0;
    }
    if(is_exit_available)
    {
        if(leave_state == 2)
        {
            json["state"]["succ"] = 0;
            json["state"]["reason"] = "You choose the wrong floor";

            json["return_val"] = NULL;
        }
        else if(leave_state == 3)
        {
            json["state"]["succ"] = 0;
            json["state"]["reason"] = "Maybe you don't have ticket";

            json["return_val"] = NULL;
        }
        else if(leave_state == 4)
        {
            json["state"]["succ"] = 0;
            json["state"]["reason"] = "You don't have this Customer";

            json["return_val"] = NULL;
        }
        else if(leave_state == 5)
        {
            json["state"]["succ"] = 0;
            json["state"]["reason"] = "Permission denied";

            json["return_val"] = NULL;
        }
        else if(leave_state == 1)
        {
            json["state"]["succ"] = 1;
            json["state"]["reason"] = NULL;

            json["return_val"]["fee"] = customer_fee;
        }
        std::cout<<"[Debug]Ready to send"<<std::endl;
        is_exit_available = false;
        customer_fee = 0;
        leave_state = 0;
    }
    if(is_entrance_available)
    {
        if(entrance_state == 2)
        {
            json["state"]["succ"] = 0;
            json["state"]["reason"] = "You choose the wrong floor";

            json["return_val"] = NULL;
        }
        else if(entrance_state == 3)
        {
            json["state"]["succ"] = 0;
            json["state"]["reason"] = "You choose the wrong floor";

            json["return_val"] = NULL;
        }
        else if(entrance_state == 4)
        {
            json["state"]["succ"] = 0;
            json["state"]["reason"] = "You don't have this Customer";

            json["return_val"] = NULL;
        }
        else if(entrance_state == 5)
        {
            json["state"]["succ"] = 0;
            json["state"]["reason"] = "Permission denied";

            json["return_val"] = NULL;
        }
        else if(entrance_state == 1)
        {
            json["state"]["succ"] = 1;
            json["state"]["reason"] = NULL;

            json["return_val"]["floor"] = entrance_floor;
            json["return_val"]["spot"] = entrance_spot; 
        }
        std::cout<<"[Debug]Ready to send"<<std::endl;
        is_entrance_available = false;
        entrance_floor = 0;
        entrance_spot = 0;
        entrance_state = 0;
    }
    if(is_add_account_available)
    {
        if(add_account_state == 2)
        {
            json["state"]["succ"] = 0;
            json["state"]["reason"] = "You have already register this customer";
            json["return_val"] = NULL;
        }
        else if(add_account_state == 5)
        {
            json["state"]["succ"] = 0;
            json["state"]["reason"] = "Permission denied";
            json["return_val"] = NULL;
        }
        else if(add_account_state == 1)
        {
            json["state"]["succ"] = 1;
            json["state"]["reason"] = NULL;
            json["return_val"] = NULL;
        }
        is_add_account_available = false;
    }
    if(is_query_permission_available)
    {
        if(query_permission_state == 2)
        {
            json["state"]["succ"] = 0;
            json["state"]["reason"] = "Unregistered user";
            json["return_val"] = NULL;
        } 
        else
        {
            json["state"]["succ"] = 1;
            json["state"]["reason"] = NULL;
            json["return_val"]["permission"] = permission;
        }
        is_query_permission_available = false;
    }
    if(is_query_spots_available)
    {
        json["state"]["succ"] = 1;
        json["state"]["reason"] = NULL;
        for(int i=1;i<=3;i++)
        {
            std::map<int,TRANSPORTATION> get_map = packing_ptr_->get_all_info_packing(i);
            for(auto iter = get_map.begin();iter != get_map.end();++iter)
            {
                TRANSPORTATION trans = iter->second;
                int id = iter->first;
                std::string trans_string;
                if(trans == TRANSPORTATION::CAR)
                    trans_string = "car";
                else if(trans == TRANSPORTATION::TRUNK)
                    trans_string = "trunk";
                else if(trans == TRANSPORTATION::VAN)
                    trans_string = "van";
                else if(trans == TRANSPORTATION::MOTORCYCLE)
                    trans_string = "motorcycle";
                json["return_val"]["floor"][std::to_string(i)]["spots"][std::to_string(id)] = trans_string;
            }
        }
        is_query_spots_available = false;
    }
    if(is_update_spots_available)
    {
        json["state"]["succ"] = 1;
        json["state"]["reason"] = NULL;
        json["return_val"] = NULL;
        is_update_spots_available = false;
    }
    if(is_query_vehicle_available)
    {
        if(query_vehicle_state == 3)
        {
            json["state"]["succ"] = 0;
            json["state"]["reason"] = "There is no this vehicle";
            json["return_val"] = NULL;
        }
        else if(query_vehicle_state == 1)
        {
            json["state"]["succ"] = 1;
            json["state"]["reason"] = NULL;
            json["return_val"]["floor"] = vehicle_floor;
            json["return_val"]["spot"] = vehicle_spot;   
        }
        vehicle_floor = 0;
        vehicle_spot = 0;
        query_vehicle_state = 0;
        is_query_vehicle_available = false;
    }
    is_password_correct = false;
    return json;
}

std::string Server::toJSONString(const nlohmann::json& json)
{
    return json.dump();
}

void Server::send_json(const nlohmann::json& json)
{
    std::string jsonString = toJSONString(json);
    ssize_t sendlen = send(clientSocket,jsonString.c_str(),jsonString.length(),0);
    std::cout<<"[Info]Send done"<<std::endl;
    if(sendlen < 0)
    {
        std::cerr << "Failed to send data"<<std::endl;
        exit(-1);
    }
}

void Server::solve_ping()
{
    std::cout<<"Receive ping request"<<std::endl;
    is_ping_available = true;
}

void Server::solve_get_available_spots(int floor)
{
    car_cnt_ = packing_ptr_->get_transportation_delta_size(floor,TRANSPORTATION::CAR,car_cnt_state_);
    trunk_cnt_ = packing_ptr_->get_transportation_delta_size(floor,TRANSPORTATION::TRUNK,trunk_cnt_state_);
    van_cnt_ = packing_ptr_->get_transportation_delta_size(floor,TRANSPORTATION::VAN,van_cnt_state_);
    motorcycle_cnt_ = packing_ptr_->get_transportation_delta_size(floor,TRANSPORTATION::MOTORCYCLE,motorcycle_cnt_state_);

    is_show_available_spots = true;
}

void Server::solve_exit(std::string vehicle)
{
    Ticket tic = packing_ptr_->query_ticket(vehicle,leave_state);
    if(leave_state == 3)
    {
        is_exit_available = true;
        return;
    }
    std::chrono::system_clock::time_point t = std::chrono::system_clock::now();
    customer_fee = packing_ptr_->calculate_price(t,tic);
    packing_ptr_->leave_pack(tic.floor,tic.id,leave_state);
    std::cout<<"[Info] Customer's fee is:"<<customer_fee<<std::endl;
    is_exit_available = true;
}

void Server::solve_entrance(std::string vehicle,std::string tips)
{
    TRANSPORTATION trans;
    if(tips == "car")
        trans = TRANSPORTATION::CAR;
    else if(tips == "trunk")
        trans = TRANSPORTATION::TRUNK;
    else if(tips == "van")
        trans = TRANSPORTATION::VAN;
    else if(tips == "motorcycle")
        trans = TRANSPORTATION::MOTORCYCLE;
    else    
        trans = TRANSPORTATION::ERROR;

    for(int i=1;i<=3;i++)
    {
        if(entrance_state == 1)
            continue;
        for(auto iter = packing_ptr_->get_all_info_packing(i).begin();iter != packing_ptr_->get_all_info_packing(i).end(); ++iter)
        {
            if(!packing_ptr_->is_pack_here(iter,i) && iter->second == trans)
            {
                entrance_floor = i;
                entrance_spot = iter->first;
                entrance_state = 1;
                break;
            }
        }
    }
    std::cout<<entrance_floor<<std::endl;
    std::cout<<entrance_spot<<std::endl;
    if(entrance_state == 1)
    {
        std::chrono::system_clock::time_point t = std::chrono::system_clock::now();
        Ticket tic;
        tic.transportation  = trans;
        tic.get_time = t;
        tic.id = entrance_spot;
        tic.floor = entrance_floor;
        tic.License_plate_number = vehicle;
        packing_ptr_->pack_pos(tic,entrance_floor,entrance_spot,entrance_state);
    }
    is_entrance_available = true;
}

void Server::solve_add_account(std::string username,std::string password)
{
    for(int i=0;i<manager.customers.size();i++)
    {
        if(manager.customers[i].get_name() == username)
        {
            std::cout<<"[Error]You have already create this account"<<std::endl;
            add_account_state = 2;
            return;
        }
    }
    manager.add_person(username,password);
    add_account_state = 1;
    is_add_account_available = true;
}

void Server::solve_query_permission(std::string username,std::string password)
{
    if(username == manager.get_name())
    {
        permission = "manager";
        is_query_permission_available = true;
        return;
    }
    for(int i=0;i<manager.customers.size();i++)
    {
        if(username == manager.customers[i].get_name())
        {
            permission = "customer";
            is_query_permission_available = true;
            return;
        }
    }
    std::cout<<"[Error]Unregistered name"<<std::endl;
    query_permission_state = 2;
    is_query_permission_available = true;
}

void Server::solve_query_spots()
{
    is_query_spots_available = true;
}

void Server::solve_query_vehicle(std::string vehicle)
{
    Ticket tic = packing_ptr_->query_ticket(vehicle,query_vehicle_state);
    vehicle_floor = tic.floor;
    vehicle_spot = tic.id;
    is_query_vehicle_available = true;
}

void Server::solve_update_spots(nlohmann::json json)
{
    for(int i=1;i<=3;i++)
    {
        packing_ptr_->clear_pack(i);
        for(auto iter = json["operation"]["arguments"]["floors"][std::to_string(i)]["spots"].begin(); iter != json["operation"]["arguments"]["floors"][std::to_string(i)]["spots"].end(); ++iter)
        {
            std::string trans_string = iter.value();
            TRANSPORTATION trans;
            if(trans_string == "car")
                trans = TRANSPORTATION::CAR;
            else if(trans_string == "trunk")
                trans = TRANSPORTATION::TRUNK;
            else if(trans_string == "van")
                trans = TRANSPORTATION::VAN;
            else if(trans_string == "motorcycle")
                trans = TRANSPORTATION::MOTORCYCLE;

            int state;
            std::string key = iter.key();
            packing_ptr_->set_pack(trans,i,std::stoi(key),state);
        }

        for(auto iter = packing_ptr_->get_all_ticking_map(i).begin();iter != packing_ptr_->get_all_ticking_map(i).end();++iter)
        {
            bool find_target = false;
            for(auto it = packing_ptr_->get_all_info_packing(i).begin();it != packing_ptr_->get_all_info_packing(i).end();++it)
            {
                if(iter->first == it->first && iter->second.transportation == it->second)
                {
                    find_target = true;
                    break;
                }
                else if(iter->first == iter->first && iter->second.transportation != it->second)
                {
                    find_target = true;
                    packing_ptr_->clear_ticket(i,iter->first);
                    break;
                }
            }
            if(!find_target)
            {
                packing_ptr_->clear_ticket(i,iter->first);
            }
        }
    }

    is_update_spots_available = true;
}

bool Server::check_password(std::string username,std::string password)
{
    if(username == manager.get_name())
    {
        if(password == manager.get_passwd())
            return true;
        return false;
    }
    for(int i=0;i<manager.customers.size();i++)
    {
        if(username == manager.customers[i].get_name())
        {
            if(password == manager.customers[i].get_passwd())
                return true;
            return false;
        }
    }
    return false;
}

void Server::update()
{
    clientAddrLen = sizeof(clientAddr);
    clientSocket = accept(serverSocket,(struct sockaddr *)&clientAddr, &clientAddrLen);
    if(clientSocket < 0)
    {
        std::cerr << "Failed to accept client connection"<<std::endl;
        return; 
    }

    char buffer[1024];
    size_t recvlen = recv(clientSocket,buffer,sizeof(buffer),0);
    if(recvlen < 0)
    {
        std::cerr<< "Failed to receive data"<<std::endl;
        return;
    }

    std::string jsonString(buffer,recvlen);
    handle_json(jsonString);

    nlohmann::json json = createJson();
    send_json(json);
    
    //TODO(Knight):
    close(clientSocket);

}


void Server::main_process()
{
    while(1)
    {
        update();
    }
    close(serverSocket);
}

}

