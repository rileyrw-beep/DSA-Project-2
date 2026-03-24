
#ifndef DSA_PROJECT_2_DATA_STRUCTURE_HPP
#define DSA_PROJECT_2_DATA_STRUCTURE_HPP

template <typename T>
class DataStructure {

public:
    DataStructure() = default;
    virtual ~DataStructure() = default;

    virtual bool insert(T val) = 0;
    virtual bool remove(T val) = 0;
    virtual bool search(T val) = 0;

};


#endif //DSA_PROJECT_2_DATA_STRUCTURE_HPP