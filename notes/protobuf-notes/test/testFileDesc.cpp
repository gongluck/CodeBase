#include <unistd.h>
#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "person.pb.h"

using namespace std;

int main()
{
    int fd = open("./testFileDesc.xxx", O_CREAT|O_TRUNC|O_RDWR, 0664);

    GOOGLE_PROTOBUF_VERIFY_VERSION;

    Person obj;
    obj.set_name("gongluck");
    obj.set_id(1);
    *obj.mutable_email() = "http://blog.csdn.net/gongluck93";
    obj.SerializeToFileDescriptor(fd);
    fsync(fd);
    lseek(fd, 0, SEEK_SET);

    Person obj2;
    obj2.ParseFromFileDescriptor(fd);
    cout << "name = " << obj2.name() << endl;
    cout << "id = " << obj2.id() << endl;
    cout << "email = " << obj2.email() << endl;

    google::protobuf::ShutdownProtobufLibrary();

    return 0;
}
