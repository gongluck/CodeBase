#include <iostream>
#include <fstream>
#include "person.pb.h"

using namespace std;

int main()
{
    fstream file("testStream.xxx", ios::in|ios::out|ios::trunc|ios::binary);

    GOOGLE_PROTOBUF_VERIFY_VERSION;

    Person obj;
    obj.set_name("gongluck");
    obj.set_id(1);
    *obj.mutable_email() = "http://blog.csdn.net/gongluck93";
    obj.SerializeToOstream(&file);
    file.flush();
    file.seekg(0, ios::beg);

    Person obj2;
    obj2.ParseFromIstream(&file);
    cout << "name = " << obj2.name() << endl;
    cout << "id = " << obj2.id() << endl;
    cout << "email = " << obj2.email() << endl;

    google::protobuf::ShutdownProtobufLibrary();

    file.close();

    return 0;
}
