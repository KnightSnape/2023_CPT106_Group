#include"utils/person.h"

namespace CPT106_Group
{

Person::Person(int id,std::string name)
{
    this->id = id;
    this->name = name;
}

void Person::set_id(int id)
{
    this->id = id;
}

void Person::set_name(std::string name)
{
    this->name = name;
}

void Person::set_passwd(std::string passwd)
{
    this->passwd = passwd;
}

int Person::get_id()
{
    return this->id;
}

std::string Person::get_name()
{
    return this->name;
}

std::string Person::get_passwd()
{
    return this->passwd;
}

void Customer::set_vehicle(std::string vehicle)
{
    this->vehicle = vehicle;
}

std::string Customer::get_vehicle()
{
    return this->vehicle;
}

void Customer::search_packing_platform_callback(int floor,TRANSPORTATION trans,int &state)
{
    int cnt = packing_ptr_->get_transportation_delta_size(floor,trans,state);
    std::cout<<"[Info]Get the Remaining parking spaces:"<<cnt<<std::endl;
}

std::vector<Ticket> Customer::get_ticket()
{
    return this->ticket;
}

std::vector<std::map<int,TRANSPORTATION> > Manager::get_list_browser()
{
    std::vector<std::map<int,TRANSPORTATION> > final_list;
    for(int i=1;i<=3;i++)
    {
        std::map<int,TRANSPORTATION> temp_map;
        temp_map = packing_ptr_->get_all_info_packing(i);
        final_list.push_back(temp_map);
    }
    return final_list;
}

void Manager::add_pack(TRANSPORTATION trans,int floor,int id,int &state)
{
    if(!packing_ptr_->set_pack(trans,floor,id,state))
    {
        std::cout<<"Add Failed."<<std::endl;
    }
}

void Manager::modify_pack(TRANSPORTATION trans,int floor,int id,int &state)
{
    packing_ptr_->change_pack(trans,floor,id,state);
}

void Manager::add_person(std::string name,std::string passwd)
{
    Customer customer;
    for(int i = 0;i < customers.size();i++)
    {
        if(customers[i].get_name() == name)
        {
            std::cout<<"[Error] You have added this name"<<std::endl;
            return;
        }
    }
    customer.set_id(++customer_cnt);
    customer.set_name(name);
    customer.set_passwd(passwd);
    customer.packing_ptr_ = this->packing_ptr_;
    customers.push_back(customer);
}

void Manager::change_person_passwd(std::string name,std::string new_passwd)
{
    for(int i = 0;i < customers.size();i++)
    {
        if(customers[i].get_name() == name)
        {
            customers[i].set_passwd(new_passwd);
            std::cout<<"[Info]Change password done"<<std::endl;
            return;
        }
    }
    std::cout<<"[Error]Cannot find this name"<<std::endl;
    return;
}

void Manager::search_packing_platform_callback(int floor,TRANSPORTATION trans,int &state)
{
    int cnt = packing_ptr_->get_transportation_delta_size(floor,trans,state);
    std::cout<<"[Info]Get the Remaining parking spaces:"<<cnt<<std::endl;
}

int Manager::search_person(std::string name,std::string passwd)
{
    for(int i = 0;i < customers.size(); i++)
    {
        if(customers[i].get_name() == name && customers[i].get_passwd() == passwd)
        {
            return i;
        }
    }
    return -1;
}

int Manager::search_person(std::string vehicle)
{
    for(int i = 0;i < customers.size(); i++)
    {
        if(customers[i].get_vehicle() == vehicle)
        {
            return i;
        }
    }
    return -1;

}



}