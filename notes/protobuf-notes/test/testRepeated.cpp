#include <iostream>
#include <fstream>
#include "person.pb.h"

using namespace std;

int main()
{
    fstream file("testStream.xxx", ios::in|ios::out|ios::trunc|ios::binary);

    GOOGLE_PROTOBUF_VERIFY_VERSION;

    AddressBook obj;

    Person* p1 = obj.add_people();
    p1->set_name("gongluck");
    p1->set_id(1);
    *(p1->mutable_email()) = "http://blog.csdn.net/gongluck93";

    Person* p2 = obj.add_people();
    p2->set_name("panzhikun");
    p2->set_id(2);
    *(p2->mutable_email()) = "panzhikun@gg.com";

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
    }

    google::protobuf::ShutdownProtobufLibrary();

    file.close();

    return 0;
}
