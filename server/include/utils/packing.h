#pragma once

#include"utility.h"

namespace CPT106_Group
{

class Packing
{
    public:
        typedef std::shared_ptr<CPT106_Group::Packing> Ptr;

        Packing() = default;
        ~Packing();

        void clear_pack();

        void clear_pack(int floor);

        bool set_pack(TRANSPORTATION new_trans,int floor,int id,int &state);

        TRANSPORTATION query_pack(int floor,int id,int &state);

        void change_pack(TRANSPORTATION new_trans,int floor,int id,int &state);

        void delete_pack(int floor,int id,int &state);

        bool pack_pos(Ticket new_trans,int floor,int id,int &state);

        TRANSPORTATION query_trans(int floor,int id,int &state);

        bool query_vehicle(std::string vehicle);

        std::chrono::system_clock::time_point query_times(int floor,int id,int &state);

        Ticket query_ticket(int floor,int id,int &state);

        Ticket query_ticket(std::string vehicle,int &state);

        int calculate_price(const std::chrono::system_clock::time_point& delta_t,const Ticket& ticket);

        int get_all_packing_size();

        int get_floor_packing_size(int floor,int &state);

        int get_transportation_packing_size(int floor,TRANSPORTATION trans,int &state);

        int get_all_transportation_packing_size(TRANSPORTATION trans,int &state);

        int get_transportation_packed_size(int floor,TRANSPORTATION trans,int &state);

        int get_transportation_delta_size(int floor,TRANSPORTATION trans,int &state);

        void leave_pack(int floor,int id,int &state);

        std::map<int,TRANSPORTATION> get_all_info_packing(int floor);

        std::map<int,Ticket> get_all_ticking_map(int floor);

        bool is_pack_here(std::map<int,TRANSPORTATION>::iterator iter,int floor);

        void clear_ticket(int floor,int id);

        void set_max_floor(int max_floor);

        int get_max_floor();


    private:
        int max_floor;
        std::map<int,Ticket> packing[101];
        std::map<int,TRANSPORTATION> max_packing[101];


};





}