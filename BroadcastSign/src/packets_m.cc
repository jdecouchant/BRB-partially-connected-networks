//
// Generated file, do not edit! Created by nedtool 5.5 from src/packets.msg.
//

// Disable warnings about unused variables, empty switch stmts, etc:
#ifdef _MSC_VER
#  pragma warning(disable:4101)
#  pragma warning(disable:4065)
#endif

#if defined(__clang__)
#  pragma clang diagnostic ignored "-Wshadow"
#  pragma clang diagnostic ignored "-Wconversion"
#  pragma clang diagnostic ignored "-Wunused-parameter"
#  pragma clang diagnostic ignored "-Wc++98-compat"
#  pragma clang diagnostic ignored "-Wunreachable-code-break"
#  pragma clang diagnostic ignored "-Wold-style-cast"
#elif defined(__GNUC__)
#  pragma GCC diagnostic ignored "-Wshadow"
#  pragma GCC diagnostic ignored "-Wconversion"
#  pragma GCC diagnostic ignored "-Wunused-parameter"
#  pragma GCC diagnostic ignored "-Wold-style-cast"
#  pragma GCC diagnostic ignored "-Wsuggest-attribute=noreturn"
#  pragma GCC diagnostic ignored "-Wfloat-conversion"
#endif

#include <iostream>
#include <sstream>
#include <memory>
#include "packets_m.h"

namespace omnetpp {

// Template pack/unpack rules. They are declared *after* a1l type-specific pack functions for multiple reasons.
// They are in the omnetpp namespace, to allow them to be found by argument-dependent lookup via the cCommBuffer argument

// Packing/unpacking an std::vector
template<typename T, typename A>
void doParsimPacking(omnetpp::cCommBuffer *buffer, const std::vector<T,A>& v)
{
    int n = v.size();
    doParsimPacking(buffer, n);
    for (int i = 0; i < n; i++)
        doParsimPacking(buffer, v[i]);
}

template<typename T, typename A>
void doParsimUnpacking(omnetpp::cCommBuffer *buffer, std::vector<T,A>& v)
{
    int n;
    doParsimUnpacking(buffer, n);
    v.resize(n);
    for (int i = 0; i < n; i++)
        doParsimUnpacking(buffer, v[i]);
}

// Packing/unpacking an std::list
template<typename T, typename A>
void doParsimPacking(omnetpp::cCommBuffer *buffer, const std::list<T,A>& l)
{
    doParsimPacking(buffer, (int)l.size());
    for (typename std::list<T,A>::const_iterator it = l.begin(); it != l.end(); ++it)
        doParsimPacking(buffer, (T&)*it);
}

template<typename T, typename A>
void doParsimUnpacking(omnetpp::cCommBuffer *buffer, std::list<T,A>& l)
{
    int n;
    doParsimUnpacking(buffer, n);
    for (int i = 0; i < n; i++) {
        l.push_back(T());
        doParsimUnpacking(buffer, l.back());
    }
}

// Packing/unpacking an std::set
template<typename T, typename Tr, typename A>
void doParsimPacking(omnetpp::cCommBuffer *buffer, const std::set<T,Tr,A>& s)
{
    doParsimPacking(buffer, (int)s.size());
    for (typename std::set<T,Tr,A>::const_iterator it = s.begin(); it != s.end(); ++it)
        doParsimPacking(buffer, *it);
}

template<typename T, typename Tr, typename A>
void doParsimUnpacking(omnetpp::cCommBuffer *buffer, std::set<T,Tr,A>& s)
{
    int n;
    doParsimUnpacking(buffer, n);
    for (int i = 0; i < n; i++) {
        T x;
        doParsimUnpacking(buffer, x);
        s.insert(x);
    }
}

// Packing/unpacking an std::map
template<typename K, typename V, typename Tr, typename A>
void doParsimPacking(omnetpp::cCommBuffer *buffer, const std::map<K,V,Tr,A>& m)
{
    doParsimPacking(buffer, (int)m.size());
    for (typename std::map<K,V,Tr,A>::const_iterator it = m.begin(); it != m.end(); ++it) {
        doParsimPacking(buffer, it->first);
        doParsimPacking(buffer, it->second);
    }
}

template<typename K, typename V, typename Tr, typename A>
void doParsimUnpacking(omnetpp::cCommBuffer *buffer, std::map<K,V,Tr,A>& m)
{
    int n;
    doParsimUnpacking(buffer, n);
    for (int i = 0; i < n; i++) {
        K k; V v;
        doParsimUnpacking(buffer, k);
        doParsimUnpacking(buffer, v);
        m[k] = v;
    }
}

// Default pack/unpack function for arrays
template<typename T>
void doParsimArrayPacking(omnetpp::cCommBuffer *b, const T *t, int n)
{
    for (int i = 0; i < n; i++)
        doParsimPacking(b, t[i]);
}

template<typename T>
void doParsimArrayUnpacking(omnetpp::cCommBuffer *b, T *t, int n)
{
    for (int i = 0; i < n; i++)
        doParsimUnpacking(b, t[i]);
}

// Default rule to prevent compiler from choosing base class' doParsimPacking() function
template<typename T>
void doParsimPacking(omnetpp::cCommBuffer *, const T& t)
{
    throw omnetpp::cRuntimeError("Parsim error: No doParsimPacking() function for type %s", omnetpp::opp_typename(typeid(t)));
}

template<typename T>
void doParsimUnpacking(omnetpp::cCommBuffer *, T& t)
{
    throw omnetpp::cRuntimeError("Parsim error: No doParsimUnpacking() function for type %s", omnetpp::opp_typename(typeid(t)));
}

}  // namespace omnetpp

namespace {
template <class T> inline
typename std::enable_if<std::is_polymorphic<T>::value && std::is_base_of<omnetpp::cObject,T>::value, void *>::type
toVoidPtr(T* t)
{
    return (void *)(static_cast<const omnetpp::cObject *>(t));
}

template <class T> inline
typename std::enable_if<std::is_polymorphic<T>::value && !std::is_base_of<omnetpp::cObject,T>::value, void *>::type
toVoidPtr(T* t)
{
    return (void *)dynamic_cast<const void *>(t);
}

template <class T> inline
typename std::enable_if<!std::is_polymorphic<T>::value, void *>::type
toVoidPtr(T* t)
{
    return (void *)static_cast<const void *>(t);
}

}

namespace inet {

// forward
template<typename T, typename A>
std::ostream& operator<<(std::ostream& out, const std::vector<T,A>& vec);

// Template rule to generate operator<< for shared_ptr<T>
template<typename T>
inline std::ostream& operator<<(std::ostream& out,const std::shared_ptr<T>& t) { return out << t.get(); }

// Template rule which fires if a struct or class doesn't have operator<<
template<typename T>
inline std::ostream& operator<<(std::ostream& out,const T&) {return out;}

// operator<< for std::vector<T>
template<typename T, typename A>
inline std::ostream& operator<<(std::ostream& out, const std::vector<T,A>& vec)
{
    out.put('{');
    for(typename std::vector<T,A>::const_iterator it = vec.begin(); it != vec.end(); ++it)
    {
        if (it != vec.begin()) {
            out.put(','); out.put(' ');
        }
        out << *it;
    }
    out.put('}');

    char buf[32];
    sprintf(buf, " (size=%u)", (unsigned int)vec.size());
    out.write(buf, strlen(buf));
    return out;
}

Register_Class(BriefPacket)

BriefPacket::BriefPacket() : ::inet::FieldsChunk()
{
    this->setChunkLength(B(1));

}

BriefPacket::BriefPacket(const BriefPacket& other) : ::inet::FieldsChunk(other)
{
    copy(other);
}

BriefPacket::~BriefPacket()
{
    delete [] this->path;
    delete [] this->data;
}

BriefPacket& BriefPacket::operator=(const BriefPacket& other)
{
    if (this == &other) return *this;
    ::inet::FieldsChunk::operator=(other);
    copy(other);
    return *this;
}

void BriefPacket::copy(const BriefPacket& other)
{
    this->msgType = other.msgType;
    this->broadcasterId = other.broadcasterId;
    this->msgId = other.msgId;
    this->echoOrReadySender = other.echoOrReadySender;
    delete [] this->path;
    this->path = (other.path_arraysize==0) ? nullptr : new int[other.path_arraysize];
    path_arraysize = other.path_arraysize;
    for (size_t i = 0; i < path_arraysize; i++) {
        this->path[i] = other.path[i];
    }
    this->linkSenderId = other.linkSenderId;
    this->ackedEchoForREADY_ECHO = other.ackedEchoForREADY_ECHO;
    this->includeData = other.includeData;
    this->dataSize = other.dataSize;
    delete [] this->data;
    this->data = (other.data_arraysize==0) ? nullptr : new char[other.data_arraysize];
    data_arraysize = other.data_arraysize;
    for (size_t i = 0; i < data_arraysize; i++) {
        this->data[i] = other.data[i];
    }
    this->debugId = other.debugId;
}

void BriefPacket::parsimPack(omnetpp::cCommBuffer *b) const
{
    ::inet::FieldsChunk::parsimPack(b);
    doParsimPacking(b,this->msgType);
    doParsimPacking(b,this->broadcasterId);
    doParsimPacking(b,this->msgId);
    doParsimPacking(b,this->echoOrReadySender);
    b->pack(path_arraysize);
    doParsimArrayPacking(b,this->path,path_arraysize);
    doParsimPacking(b,this->linkSenderId);
    doParsimPacking(b,this->ackedEchoForREADY_ECHO);
    doParsimPacking(b,this->includeData);
    doParsimPacking(b,this->dataSize);
    b->pack(data_arraysize);
    doParsimArrayPacking(b,this->data,data_arraysize);
    doParsimPacking(b,this->debugId);
}

void BriefPacket::parsimUnpack(omnetpp::cCommBuffer *b)
{
    ::inet::FieldsChunk::parsimUnpack(b);
    doParsimUnpacking(b,this->msgType);
    doParsimUnpacking(b,this->broadcasterId);
    doParsimUnpacking(b,this->msgId);
    doParsimUnpacking(b,this->echoOrReadySender);
    delete [] this->path;
    b->unpack(path_arraysize);
    if (path_arraysize == 0) {
        this->path = nullptr;
    } else {
        this->path = new int[path_arraysize];
        doParsimArrayUnpacking(b,this->path,path_arraysize);
    }
    doParsimUnpacking(b,this->linkSenderId);
    doParsimUnpacking(b,this->ackedEchoForREADY_ECHO);
    doParsimUnpacking(b,this->includeData);
    doParsimUnpacking(b,this->dataSize);
    delete [] this->data;
    b->unpack(data_arraysize);
    if (data_arraysize == 0) {
        this->data = nullptr;
    } else {
        this->data = new char[data_arraysize];
        doParsimArrayUnpacking(b,this->data,data_arraysize);
    }
    doParsimUnpacking(b,this->debugId);
}

int BriefPacket::getMsgType() const
{
    return this->msgType;
}

void BriefPacket::setMsgType(int msgType)
{
    handleChange();
    this->msgType = msgType;
}

int BriefPacket::getBroadcasterId() const
{
    return this->broadcasterId;
}

void BriefPacket::setBroadcasterId(int broadcasterId)
{
    handleChange();
    this->broadcasterId = broadcasterId;
}

int BriefPacket::getMsgId() const
{
    return this->msgId;
}

void BriefPacket::setMsgId(int msgId)
{
    handleChange();
    this->msgId = msgId;
}

int BriefPacket::getEchoOrReadySender() const
{
    return this->echoOrReadySender;
}

void BriefPacket::setEchoOrReadySender(int echoOrReadySender)
{
    handleChange();
    this->echoOrReadySender = echoOrReadySender;
}

size_t BriefPacket::getPathArraySize() const
{
    return path_arraysize;
}

int BriefPacket::getPath(size_t k) const
{
    if (k >= path_arraysize) throw omnetpp::cRuntimeError("Array of size path_arraysize indexed by %lu", (unsigned long)k);
    return this->path[k];
}

void BriefPacket::setPathArraySize(size_t newSize)
{
    handleChange();
    int *path2 = (newSize==0) ? nullptr : new int[newSize];
    size_t minSize = path_arraysize < newSize ? path_arraysize : newSize;
    for (size_t i = 0; i < minSize; i++)
        path2[i] = this->path[i];
    for (size_t i = minSize; i < newSize; i++)
        path2[i] = 0;
    delete [] this->path;
    this->path = path2;
    path_arraysize = newSize;
}

void BriefPacket::setPath(size_t k, int path)
{
    if (k >= path_arraysize) throw omnetpp::cRuntimeError("Array of size  indexed by %lu", (unsigned long)k);
    handleChange();
    this->path[k] = path;
}

void BriefPacket::insertPath(size_t k, int path)
{
    handleChange();
    if (k > path_arraysize) throw omnetpp::cRuntimeError("Array of size  indexed by %lu", (unsigned long)k);
    size_t newSize = path_arraysize + 1;
    int *path2 = new int[newSize];
    size_t i;
    for (i = 0; i < k; i++)
        path2[i] = this->path[i];
    path2[k] = path;
    for (i = k + 1; i < newSize; i++)
        path2[i] = this->path[i-1];
    delete [] this->path;
    this->path = path2;
    path_arraysize = newSize;
}

void BriefPacket::insertPath(int path)
{
    insertPath(path_arraysize, path);
}

void BriefPacket::erasePath(size_t k)
{
    if (k >= path_arraysize) throw omnetpp::cRuntimeError("Array of size  indexed by %lu", (unsigned long)k);
    handleChange();
    size_t newSize = path_arraysize - 1;
    int *path2 = (newSize == 0) ? nullptr : new int[newSize];
    size_t i;
    for (i = 0; i < k; i++)
        path2[i] = this->path[i];
    for (i = k; i < newSize; i++)
        path2[i] = this->path[i+1];
    delete [] this->path;
    this->path = path2;
    path_arraysize = newSize;
}

int BriefPacket::getLinkSenderId() const
{
    return this->linkSenderId;
}

void BriefPacket::setLinkSenderId(int linkSenderId)
{
    handleChange();
    this->linkSenderId = linkSenderId;
}

int BriefPacket::getAckedEchoForREADY_ECHO() const
{
    return this->ackedEchoForREADY_ECHO;
}

void BriefPacket::setAckedEchoForREADY_ECHO(int ackedEchoForREADY_ECHO)
{
    handleChange();
    this->ackedEchoForREADY_ECHO = ackedEchoForREADY_ECHO;
}

bool BriefPacket::getIncludeData() const
{
    return this->includeData;
}

void BriefPacket::setIncludeData(bool includeData)
{
    handleChange();
    this->includeData = includeData;
}

int BriefPacket::getDataSize() const
{
    return this->dataSize;
}

void BriefPacket::setDataSize(int dataSize)
{
    handleChange();
    this->dataSize = dataSize;
}

size_t BriefPacket::getDataArraySize() const
{
    return data_arraysize;
}

char BriefPacket::getData(size_t k) const
{
    if (k >= data_arraysize) throw omnetpp::cRuntimeError("Array of size data_arraysize indexed by %lu", (unsigned long)k);
    return this->data[k];
}

void BriefPacket::setDataArraySize(size_t newSize)
{
    handleChange();
    char *data2 = (newSize==0) ? nullptr : new char[newSize];
    size_t minSize = data_arraysize < newSize ? data_arraysize : newSize;
    for (size_t i = 0; i < minSize; i++)
        data2[i] = this->data[i];
    for (size_t i = minSize; i < newSize; i++)
        data2[i] = 0;
    delete [] this->data;
    this->data = data2;
    data_arraysize = newSize;
}

void BriefPacket::setData(size_t k, char data)
{
    if (k >= data_arraysize) throw omnetpp::cRuntimeError("Array of size  indexed by %lu", (unsigned long)k);
    handleChange();
    this->data[k] = data;
}

void BriefPacket::insertData(size_t k, char data)
{
    handleChange();
    if (k > data_arraysize) throw omnetpp::cRuntimeError("Array of size  indexed by %lu", (unsigned long)k);
    size_t newSize = data_arraysize + 1;
    char *data2 = new char[newSize];
    size_t i;
    for (i = 0; i < k; i++)
        data2[i] = this->data[i];
    data2[k] = data;
    for (i = k + 1; i < newSize; i++)
        data2[i] = this->data[i-1];
    delete [] this->data;
    this->data = data2;
    data_arraysize = newSize;
}

void BriefPacket::insertData(char data)
{
    insertData(data_arraysize, data);
}

void BriefPacket::eraseData(size_t k)
{
    if (k >= data_arraysize) throw omnetpp::cRuntimeError("Array of size  indexed by %lu", (unsigned long)k);
    handleChange();
    size_t newSize = data_arraysize - 1;
    char *data2 = (newSize == 0) ? nullptr : new char[newSize];
    size_t i;
    for (i = 0; i < k; i++)
        data2[i] = this->data[i];
    for (i = k; i < newSize; i++)
        data2[i] = this->data[i+1];
    delete [] this->data;
    this->data = data2;
    data_arraysize = newSize;
}

int BriefPacket::getDebugId() const
{
    return this->debugId;
}

void BriefPacket::setDebugId(int debugId)
{
    handleChange();
    this->debugId = debugId;
}

class BriefPacketDescriptor : public omnetpp::cClassDescriptor
{
  private:
    mutable const char **propertynames;
    enum FieldConstants {
        FIELD_msgType,
        FIELD_broadcasterId,
        FIELD_msgId,
        FIELD_echoOrReadySender,
        FIELD_path,
        FIELD_linkSenderId,
        FIELD_ackedEchoForREADY_ECHO,
        FIELD_includeData,
        FIELD_dataSize,
        FIELD_data,
        FIELD_debugId,
    };
  public:
    BriefPacketDescriptor();
    virtual ~BriefPacketDescriptor();

    virtual bool doesSupport(omnetpp::cObject *obj) const override;
    virtual const char **getPropertyNames() const override;
    virtual const char *getProperty(const char *propertyname) const override;
    virtual int getFieldCount() const override;
    virtual const char *getFieldName(int field) const override;
    virtual int findField(const char *fieldName) const override;
    virtual unsigned int getFieldTypeFlags(int field) const override;
    virtual const char *getFieldTypeString(int field) const override;
    virtual const char **getFieldPropertyNames(int field) const override;
    virtual const char *getFieldProperty(int field, const char *propertyname) const override;
    virtual int getFieldArraySize(void *object, int field) const override;

    virtual const char *getFieldDynamicTypeString(void *object, int field, int i) const override;
    virtual std::string getFieldValueAsString(void *object, int field, int i) const override;
    virtual bool setFieldValueAsString(void *object, int field, int i, const char *value) const override;

    virtual const char *getFieldStructName(int field) const override;
    virtual void *getFieldStructValuePointer(void *object, int field, int i) const override;
};

Register_ClassDescriptor(BriefPacketDescriptor)

BriefPacketDescriptor::BriefPacketDescriptor() : omnetpp::cClassDescriptor(omnetpp::opp_typename(typeid(inet::BriefPacket)), "inet::FieldsChunk")
{
    propertynames = nullptr;
}

BriefPacketDescriptor::~BriefPacketDescriptor()
{
    delete[] propertynames;
}

bool BriefPacketDescriptor::doesSupport(omnetpp::cObject *obj) const
{
    return dynamic_cast<BriefPacket *>(obj)!=nullptr;
}

const char **BriefPacketDescriptor::getPropertyNames() const
{
    if (!propertynames) {
        static const char *names[] = {  nullptr };
        omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
        const char **basenames = basedesc ? basedesc->getPropertyNames() : nullptr;
        propertynames = mergeLists(basenames, names);
    }
    return propertynames;
}

const char *BriefPacketDescriptor::getProperty(const char *propertyname) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? basedesc->getProperty(propertyname) : nullptr;
}

int BriefPacketDescriptor::getFieldCount() const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? 11+basedesc->getFieldCount() : 11;
}

unsigned int BriefPacketDescriptor::getFieldTypeFlags(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldTypeFlags(field);
        field -= basedesc->getFieldCount();
    }
    static unsigned int fieldTypeFlags[] = {
        FD_ISEDITABLE,    // FIELD_msgType
        FD_ISEDITABLE,    // FIELD_broadcasterId
        FD_ISEDITABLE,    // FIELD_msgId
        FD_ISEDITABLE,    // FIELD_echoOrReadySender
        FD_ISARRAY | FD_ISEDITABLE,    // FIELD_path
        FD_ISEDITABLE,    // FIELD_linkSenderId
        FD_ISEDITABLE,    // FIELD_ackedEchoForREADY_ECHO
        FD_ISEDITABLE,    // FIELD_includeData
        FD_ISEDITABLE,    // FIELD_dataSize
        FD_ISARRAY | FD_ISEDITABLE,    // FIELD_data
        FD_ISEDITABLE,    // FIELD_debugId
    };
    return (field >= 0 && field < 11) ? fieldTypeFlags[field] : 0;
}

const char *BriefPacketDescriptor::getFieldName(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldName(field);
        field -= basedesc->getFieldCount();
    }
    static const char *fieldNames[] = {
        "msgType",
        "broadcasterId",
        "msgId",
        "echoOrReadySender",
        "path",
        "linkSenderId",
        "ackedEchoForREADY_ECHO",
        "includeData",
        "dataSize",
        "data",
        "debugId",
    };
    return (field >= 0 && field < 11) ? fieldNames[field] : nullptr;
}

int BriefPacketDescriptor::findField(const char *fieldName) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    int base = basedesc ? basedesc->getFieldCount() : 0;
    if (fieldName[0] == 'm' && strcmp(fieldName, "msgType") == 0) return base+0;
    if (fieldName[0] == 'b' && strcmp(fieldName, "broadcasterId") == 0) return base+1;
    if (fieldName[0] == 'm' && strcmp(fieldName, "msgId") == 0) return base+2;
    if (fieldName[0] == 'e' && strcmp(fieldName, "echoOrReadySender") == 0) return base+3;
    if (fieldName[0] == 'p' && strcmp(fieldName, "path") == 0) return base+4;
    if (fieldName[0] == 'l' && strcmp(fieldName, "linkSenderId") == 0) return base+5;
    if (fieldName[0] == 'a' && strcmp(fieldName, "ackedEchoForREADY_ECHO") == 0) return base+6;
    if (fieldName[0] == 'i' && strcmp(fieldName, "includeData") == 0) return base+7;
    if (fieldName[0] == 'd' && strcmp(fieldName, "dataSize") == 0) return base+8;
    if (fieldName[0] == 'd' && strcmp(fieldName, "data") == 0) return base+9;
    if (fieldName[0] == 'd' && strcmp(fieldName, "debugId") == 0) return base+10;
    return basedesc ? basedesc->findField(fieldName) : -1;
}

const char *BriefPacketDescriptor::getFieldTypeString(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldTypeString(field);
        field -= basedesc->getFieldCount();
    }
    static const char *fieldTypeStrings[] = {
        "int",    // FIELD_msgType
        "int",    // FIELD_broadcasterId
        "int",    // FIELD_msgId
        "int",    // FIELD_echoOrReadySender
        "int",    // FIELD_path
        "int",    // FIELD_linkSenderId
        "int",    // FIELD_ackedEchoForREADY_ECHO
        "bool",    // FIELD_includeData
        "int",    // FIELD_dataSize
        "char",    // FIELD_data
        "int",    // FIELD_debugId
    };
    return (field >= 0 && field < 11) ? fieldTypeStrings[field] : nullptr;
}

const char **BriefPacketDescriptor::getFieldPropertyNames(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldPropertyNames(field);
        field -= basedesc->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    }
}

const char *BriefPacketDescriptor::getFieldProperty(int field, const char *propertyname) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldProperty(field, propertyname);
        field -= basedesc->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    }
}

int BriefPacketDescriptor::getFieldArraySize(void *object, int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldArraySize(object, field);
        field -= basedesc->getFieldCount();
    }
    BriefPacket *pp = (BriefPacket *)object; (void)pp;
    switch (field) {
        case FIELD_path: return pp->getPathArraySize();
        case FIELD_data: return pp->getDataArraySize();
        default: return 0;
    }
}

const char *BriefPacketDescriptor::getFieldDynamicTypeString(void *object, int field, int i) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldDynamicTypeString(object,field,i);
        field -= basedesc->getFieldCount();
    }
    BriefPacket *pp = (BriefPacket *)object; (void)pp;
    switch (field) {
        default: return nullptr;
    }
}

std::string BriefPacketDescriptor::getFieldValueAsString(void *object, int field, int i) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldValueAsString(object,field,i);
        field -= basedesc->getFieldCount();
    }
    BriefPacket *pp = (BriefPacket *)object; (void)pp;
    switch (field) {
        case FIELD_msgType: return long2string(pp->getMsgType());
        case FIELD_broadcasterId: return long2string(pp->getBroadcasterId());
        case FIELD_msgId: return long2string(pp->getMsgId());
        case FIELD_echoOrReadySender: return long2string(pp->getEchoOrReadySender());
        case FIELD_path: return long2string(pp->getPath(i));
        case FIELD_linkSenderId: return long2string(pp->getLinkSenderId());
        case FIELD_ackedEchoForREADY_ECHO: return long2string(pp->getAckedEchoForREADY_ECHO());
        case FIELD_includeData: return bool2string(pp->getIncludeData());
        case FIELD_dataSize: return long2string(pp->getDataSize());
        case FIELD_data: return long2string(pp->getData(i));
        case FIELD_debugId: return long2string(pp->getDebugId());
        default: return "";
    }
}

bool BriefPacketDescriptor::setFieldValueAsString(void *object, int field, int i, const char *value) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->setFieldValueAsString(object,field,i,value);
        field -= basedesc->getFieldCount();
    }
    BriefPacket *pp = (BriefPacket *)object; (void)pp;
    switch (field) {
        case FIELD_msgType: pp->setMsgType(string2long(value)); return true;
        case FIELD_broadcasterId: pp->setBroadcasterId(string2long(value)); return true;
        case FIELD_msgId: pp->setMsgId(string2long(value)); return true;
        case FIELD_echoOrReadySender: pp->setEchoOrReadySender(string2long(value)); return true;
        case FIELD_path: pp->setPath(i,string2long(value)); return true;
        case FIELD_linkSenderId: pp->setLinkSenderId(string2long(value)); return true;
        case FIELD_ackedEchoForREADY_ECHO: pp->setAckedEchoForREADY_ECHO(string2long(value)); return true;
        case FIELD_includeData: pp->setIncludeData(string2bool(value)); return true;
        case FIELD_dataSize: pp->setDataSize(string2long(value)); return true;
        case FIELD_data: pp->setData(i,string2long(value)); return true;
        case FIELD_debugId: pp->setDebugId(string2long(value)); return true;
        default: return false;
    }
}

const char *BriefPacketDescriptor::getFieldStructName(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldStructName(field);
        field -= basedesc->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    };
}

void *BriefPacketDescriptor::getFieldStructValuePointer(void *object, int field, int i) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldStructValuePointer(object, field, i);
        field -= basedesc->getFieldCount();
    }
    BriefPacket *pp = (BriefPacket *)object; (void)pp;
    switch (field) {
        default: return nullptr;
    }
}

} // namespace inet

