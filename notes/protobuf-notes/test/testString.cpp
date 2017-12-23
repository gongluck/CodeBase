#include <iostream>
#include "person.pb.h"

using namespace std;

int main()
{
    string str;

    GOOGLE_PROTOBUF_VERIFY_VERSION;

    Person obj;
    obj.set_name("gongluck");
    obj.set_id(1);
    *obj.mutable_email() = "http://blog.csdn.net/gongluck93";
    obj.SerializeToString(&str);

    Person obj2;
    obj2.ParseFromString(str);
    cout << "name = " << obj2.name() << endl;
    cout << "id = " << obj2.id() << endl;
    cout << "email = " << obj2.email() << endl;

    google::protobuf::ShutdownProtobufLibrary();

    return 0;
}
