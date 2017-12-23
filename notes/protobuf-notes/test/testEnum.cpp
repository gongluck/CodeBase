#include <iostream>
#include <fstream>
#include "person.pb.h"

using namespace std;
using namespace Test;

int main()
{
    fstream file("testStream.xxx", ios::in|ios::out|ios::trunc|ios::binary);

    GOOGLE_PROTOBUF_VERIFY_VERSION;

    AddressBook obj;

    Person* p1 = obj.add_people();
    p1->set_name("gongluck");
    p1->set_id(1);
    *(p1->mutable_email()) = "http://blog.csdn.net/gongluck93";

    Person::PhoneNumber* phone1 = p1->add_phones();
    phone1->set_number("110");
    phone1->set_type(Person::MOBLIE);
    Person::PhoneNumber* phone2 = p1->add_phones();
    phone2->set_number("120");
    phone2->set_type(Person::WORK);

    obj.SerializeToOstream(&file);
    file.flush();
    file.seekg(0, ios::beg);

    AddressBook obj2;
    obj2.ParseFromIstream(&file);
    for(int i= 0; i< obj.people_size(); ++i)
    {
        Person per = obj2.people(i);
        cout << "name = " << per.name() << endl;
        cout << "id = " << per.id() << endl;
        cout << "email = " << per.email() << endl;
        for(int j=0; j< per.phones_size(); ++j)
        {
            Person::PhoneNumber phonenum= per.phones(j);
            switch(phonenum.type())
            {
            case Person::MOBLIE:
                cout << "mobile : " ;
                break;
            case Person::WORK:
                cout << "work : ";
                break;
            case Person::HOME:
                cout << "home : ";
                break;
            default:
                cout << "Not Know : ";
                break;
            }
            cout << phonenum.number() << endl;
        }
    }

    google::protobuf::ShutdownProtobufLibrary();

    file.close();

    return 0;
}
