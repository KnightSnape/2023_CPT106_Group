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
        /**
         *
         * @param new_trans
         * @param floor
         * @param id
         * @return
         */
        bool set_pack(TRANSPORTATION new_trans,int floor,int id,int &state);
        /**
         *
         * @param floor
         * @param id
         * @return
         */
        TRANSPORTATION query_pack(int floor,int id,int &state);
        /**
         *
         * @param new_trans
         * @param floor
         * @param id
         */
        void change_pack(TRANSPORTATION new_trans,int floor,int id,int &state);
        /**
         *
         * @param floor
         * @param id
         */
        void delete_pack(int floor,int id,int &state);
        /**
         *
         * @param new_trans
         * @param floor
         * @param id
         * @return
         */
        bool pack_pos(Ticket new_trans,int floor,int id,int &state);
        /**
         *
         * @param floor
         * @param id
         * @return
         */
        TRANSPORTATION query_trans(int floor,int id,int &state);
        /**
         *
         * @param floor
         * @param id
         * @return
         */
        std::chrono::system_clock::time_point query_times(int floor,int id,int &state);
        /**
         *
         * @param floor
         * @param id
         * @return
         */
        Ticket query_ticket(int floor,int id,int &state);

        Ticket query_ticket(std::string vehicle,int &state);
        /**
         *
         * @param delta_t
         * @param ticket
         * @return
         */
        int calculate_price(const std::chrono::system_clock::time_point& delta_t,const Ticket& ticket);
        /**
         *
         * @return
         */
        int get_all_packing_size();
        /**
         *
         * @param floor
         * @return
         */
        int get_floor_packing_size(int floor,int &state);
        /**
         *
         * @param floor
         * @param trans
         * @return
         */
        int get_transportation_packing_size(int floor,TRANSPORTATION trans,int &state);
        /**
         *
         * @param trans
         * @return
         */
        int get_all_transportation_packing_size(TRANSPORTATION trans,int &state);
        /**
         *
         * @param floor
         * @param trans
         * @return
         */
        int get_transportation_packed_size(int floor,TRANSPORTATION trans,int &state);
        /**
         *
         * @param floor
         * @param trans
         * @return
         */
        int get_transportation_delta_size(int floor,TRANSPORTATION trans,int &state);
        /**
         *
         * @param floor
         * @param id
         */
        void leave_pack(int floor,int id,int &state);

        /**
         *
         * @param floor
         * @return
         */
        std::map<int,TRANSPORTATION> get_all_info_packing(int floor);

        /**
         *
         * @param floor
         * @return
         */
        std::map<int,Ticket> get_all_ticking_map(int floor);

        bool is_pack_here(std::map<int,TRANSPORTATION>::iterator iter,int floor);

        void clear_ticket(int floor,int id);


    private:
        std::map<int,Ticket> packing[3];
        std::map<int,TRANSPORTATION> max_packing[3];
};





}