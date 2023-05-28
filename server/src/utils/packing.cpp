#include"utils/packing.h"

namespace CPT106_Group
{

Packing::~Packing()
{
    clear_pack();
}

void Packing::clear_pack()
{
    for(int i=0;i<3;i++)
    {
        max_packing[i].clear();
        packing[i].clear();
    }
}

void Packing::clear_pack(int floor)
{
    max_packing[floor - 1].clear();
}

bool Packing::set_pack(TRANSPORTATION new_trans,int floor,int id,int &state)
{
    if(floor <= 0 || floor > max_floor)
    {
        state = 2;
        std::cout<<"[Error] This floor does not exist"<<std::endl;
        return false;
    }
    for(auto iter = max_packing[floor - 1].begin();iter!=max_packing[floor - 1].end();++iter)
    {
        if(id == iter->first)
        {
            state = 3;
            std::cout<<"[Error] You have already create this pack"<<std::endl;
            return false;
        }
    }
    max_packing[floor - 1].emplace(id,new_trans);
    return true;
}

TRANSPORTATION Packing::query_pack(int floor,int id,int &state)
{
    if(floor <= 0 || floor > max_floor)
    {
        std::cout<<"[Error] This floor does not exist"<<std::endl;
        return TRANSPORTATION::ERROR;
    }
    for(auto iter = max_packing[floor - 1].begin();iter!=max_packing[floor - 1].end();++iter)
    {
        if(id == iter->first)
            return iter->second;
    }
    std::cout<<"[Error] Cannot find this id"<<std::endl;
    return TRANSPORTATION::ERROR;
}

void Packing::change_pack(TRANSPORTATION new_trans,int floor,int id,int &state)
{
    if(floor <= 0 || floor > max_floor)
    {
        std::cout<<"[Error] This floor does not exist"<<std::endl;
        return;
    }
    std::map<int,TRANSPORTATION>::iterator iter;
    iter = max_packing[floor - 1].find(id);
    if(iter == max_packing[floor - 1].end())
    {
        std::cout<<"[Error] Cannot find this id"<<std::endl;
        return;
    }
    else
    {
        iter->second = new_trans;
    }
    std::cout<<"change done"<<std::endl;
}

void Packing::delete_pack(int floor,int id,int &state)
{
    if(floor <= 0 || floor > max_floor)
    {
        std::cout<<"[Error] This floor does not exist"<<std::endl;
        return;
    }
    std::map<int,TRANSPORTATION>::iterator iter;
    iter = max_packing[floor - 1].find(id);
    if(iter == max_packing[floor - 1].end())
    {
        std::cout<<"[Error] Cannot find this id"<<std::endl;
        return;
    }
    else
    {
        max_packing[floor-1].erase(iter);
    }
    std::cout<<"delete done"<<std::endl;
}

bool Packing::pack_pos(Ticket new_trans,int floor,int id,int &state)
{
    if(floor <= 0 || floor > max_floor)
    {
        std::cout<<"[Error] This floor does not exist"<<std::endl;
        return false;
    }
    for(auto iter = packing[floor - 1].begin();iter!=packing[floor - 1].end();++iter)
    {
        if(id == iter->first)
        {
            std::cout<<"[Error] Someone has already stopped at the location"<<std::endl;
            return false;
        }
    }
    for(auto iter = max_packing[floor - 1].begin();iter!=max_packing[floor - 1].end();++iter)
    {
        if(id == iter->first && new_trans.transportation != iter->second)
        {
            std::cout<<"[Error] Mismatched vehicle types"<<std::endl;
            return false;
        }
        else if(id == iter->first && new_trans.transportation == iter->second)
        {
            packing[floor - 1].emplace(id,new_trans);
            return true;
        }
    }
    std::cout<<"[Error] Cannot find id"<<std::endl;
    return false;

}

TRANSPORTATION Packing::query_trans(int floor,int id,int &state)
{
    if(floor <= 0 || floor > max_floor)
    {
        std::cout<<"[Error] This floor does not exist"<<std::endl;
        return TRANSPORTATION::ERROR;
    }

    for(auto iter = packing[floor - 1].begin();iter!=packing[floor - 1].end();++iter)
    {
        if(id == iter->first)
            return iter->second.transportation;
    }
    std::cout<<"[Error] Cannot find this id"<<std::endl;
    return TRANSPORTATION::ERROR;
}

std::chrono::system_clock::time_point Packing::query_times(int floor,int id,int &state)
{
    if(floor <= 0 || floor > max_floor)
    {
        throw std::runtime_error("[Error] This floor does not exist");
    }
    for(auto iter = packing[floor - 1].begin();iter!=packing[floor - 1].end();++iter)
    {
        if(id == iter->first)
            return iter->second.get_time;
    }
    throw std::runtime_error("[Error] Cannot find this id");
}

Ticket Packing::query_ticket(int floor,int id,int &state)
{
    if(floor <= 0 || floor > max_floor)
    {
        std::cout<<"[Error]This floor does not exist"<<std::endl;
        state = 2;
        Ticket ticket;
        ticket.id = 0;
        return ticket;
    }
    for(auto iter = packing[floor - 1].begin();iter!=packing[floor - 1].end(); ++iter)
    {
        if(id == iter->first)
            return iter->second;
    }
    std::cout<<"[Error]Cannot find this id"<<std::endl;
    state = 3;
    Ticket ticket;
    ticket.id = 0;
    return ticket;
}

Ticket Packing::query_ticket(std::string vehicle,int &state)
{
    for(int i=1;i<=3;i++)
    {
        for(auto iter = packing[i - 1].begin();iter!=packing[i - 1].end(); ++iter)
        {
            if(iter->second.License_plate_number == vehicle)
            {
                state = 1;
                return iter->second;
            }
        }
    }
    std::cout<<"[Error]Cannot find this id"<<std::endl;
    state = 3;
    Ticket ticket;
    ticket.id = 0;
    return ticket;
}

int Packing::calculate_price(const std::chrono::system_clock::time_point& delta_t,const Ticket& ticket)
{
    int final_price;
    double delta_time = std::chrono::duration<double,std::milli>(delta_t - ticket.get_time).count();
    if(delta_time < 100000.0)
        final_price = 0;
    else 
        final_price = (int)(delta_time / 1e6);
    return final_price;
}

int Packing::get_all_packing_size()
{
    int all_length = 0;
    for(int i = 0;i < 3;i++)
    {
        all_length += max_packing[i].size();
    }
    return all_length;
}

int Packing::get_floor_packing_size(int floor,int &state)
{
    if(floor <= 0 || floor > max_floor)
    {
        std::cout<<"[Error] This floor does not exist"<<std::endl;
        return -1;
    }
    return max_packing[floor - 1].size();
}

int Packing::get_transportation_packing_size(int floor,TRANSPORTATION trans,int &state)
{
    int final_cnt = 0;
    if(floor <= 0 || floor > max_floor)
    {
        std::cout<<"[Error] This floor does not exist"<<std::endl;
        return -1;
    }
    for(auto iter = max_packing[floor - 1].begin();iter != max_packing[floor - 1].end();++iter)
    {
        if(iter->second == trans)
            final_cnt++;
    }
    return final_cnt;
}

int Packing::get_all_transportation_packing_size(TRANSPORTATION trans,int &state)
{
    int final_cnt = 0;
    for(int i=1;i<=3;i++)
    {
        final_cnt += get_transportation_packing_size(i,trans,state);
    }
    return final_cnt;
}

int Packing::get_transportation_packed_size(int floor,TRANSPORTATION trans,int &state)
{
    int final_cnt = 0;
    if(floor <= 0 || floor > max_floor)
    {
        std::cout<<"[Error] This floor does not exist"<<std::endl;
        return -1;
    }
    for(auto iter = packing[floor - 1].begin();iter != packing[floor - 1].end();++iter)
    {
        if(iter->second.transportation == trans)
            final_cnt++;
    }
    return final_cnt;
}

int Packing::get_transportation_delta_size(int floor,TRANSPORTATION trans,int &state)
{
    int final_cnt = 0;
    if(floor <= 0 || floor > max_floor)
    {
        std::cout<<"[Error] This floor does not exist"<<std::endl;
        state = 2;
        return -1;
    }
    final_cnt = get_transportation_packing_size(floor,trans,state) - get_transportation_packed_size(floor,trans,state);
    state = 1;
    return final_cnt;
}

void Packing::leave_pack(int floor,int id,int &state)
{
    int final_cnt = 0;
    if(floor <= 0 || floor > max_floor)
    {
        std::cout<<"[Error] This floor does not exist"<<std::endl;
        state = 2;
        return;
    }
    std::map<int,Ticket>::iterator iter;
    iter = packing[floor - 1].find(id);
    if(iter == packing[floor - 1].end())
    {
        std::cout<<"[Error] Cannot find this id"<<std::endl;
        state = 3;
        return;
    }
    else
    {
        packing[floor - 1].erase(iter);
    }
    state = 1;
    std::cout<<"leave complete"<<std::endl;
}

std::map<int,TRANSPORTATION> Packing::get_all_info_packing(int floor)
{
    return max_packing[floor - 1];
}

std::map<int,Ticket> Packing::get_all_ticking_map(int floor)
{
    return packing[floor - 1];
}

bool Packing::is_pack_here(std::map<int,TRANSPORTATION>::iterator iter,int floor)
{
    for(auto it = packing[floor - 1].begin();it != packing[floor - 1].end(); ++it)
    {
        if(it->second.id == iter->first)
        {
            return true;
        }
    }
    return false;
}

void Packing::clear_ticket(int floor,int id)
{
    packing[floor - 1].erase(id);
}

void Packing::set_max_floor(int max_floor) {
    this->max_floor = max_floor;
}

int Packing::get_max_floor() {
    return this->max_floor;
}

}