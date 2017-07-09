#ifndef SAM_TESTING_H
#define SAM_TESTING_H

class Testing
{
public:
    Testing();

    bool test();
    bool testStateSerializationWrite();
    bool testStateSerializationRead();
};

#endif // SAM_TESTING_H
