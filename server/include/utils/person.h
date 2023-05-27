#pragma once
#include"packing.h"
#include"utility.h"

namespace CPT106_Group
{

class Person
{
    public:
        Person() = default;
        Person(int id,std::string name);
        ~Person() = default;
        void set_id(int id);
        void set_name(std::string name);
        void set_passwd(std::string passwd);
        int get_id();
        std::string get_name();
        std::string get_passwd();

        Packing::Ptr packing_ptr_;
    private:
        int id;
        std::string name;
        std::string passwd;

};

class Customer : public Person
{
    public:
        Customer() = default;
        ~Customer() = default;

        void set_vehicle(std::string vehicle);
        std::string get_vehicle();
        void search_packing_platform_callback(int floor,TRANSPORTATION trans,int &state);
        std::vector<Ticket> get_ticket();

        Packing::Ptr packing_ptr_;
    private:
        std::string vehicle;
        std::vector<Ticket> ticket;


};

class Manager : public Person
{
    public:
        Manager() = default;
        ~Manager() = default;

        std::vector<std::map<int,TRANSPORTATION> > get_list_browser();
        void add_pack(TRANSPORTATION trans,int floor,int id,int &state);
        void modify_pack(TRANSPORTATION trans,int floor,int id,int &state);
        void delete_pack(int floor,int id,int &state);
        void add_person(std::string name,std::string passwd);
        void change_person_passwd(std::string name,std::string new_passwd);
        void search_packing_platform_callback(int floor,TRANSPORTATION trans,int &state);
        int search_person(std::string name,std::string passwd);
        int search_person(std::string vehicle);

        Packing::Ptr packing_ptr_;
        std::vector<Customer> customers;
    private:
        int customer_cnt;



};



}