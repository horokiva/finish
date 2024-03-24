#ifndef __PROGTEST__
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <cctype>
#include <cmath>
#include <cassert>
#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <list>
#include <algorithm>
#include <functional>
#include <memory>
#endif /* __PROGTEST__ */

class CIterator;

class CLandRegister
{
public:
    struct Property{
        std::string m_City;
        std::string m_Addr;
        std::string m_Region;
        unsigned long long m_ID;
        std::string m_Owner;
        long long m_AcquisitionTimestamp;
    };

    CLandRegister();
    ~CLandRegister();

    bool                     add                           ( const std::string    & city,
                                                             const std::string    & addr,
                                                             const std::string    & region,
                                                             unsigned long long           id );

    bool                     del                           ( const std::string    & city,
                                                             const std::string    & addr );

    bool                     del                           ( const std::string    & region,
                                                             unsigned long long         id );

    bool                     getOwner                      ( const std::string    & city,
                                                             const std::string    & addr,
                                                             std::string          & owner ) const;

    bool                     getOwner                      ( const std::string    & region,
                                                             unsigned long long           id,
                                                             std::string          & owner ) const;

    bool                     newOwner                      ( const std::string    & city,
                                                             const std::string    & addr,
                                                             const std::string    & owner );

    bool                     newOwner                      ( const std::string    & region,
                                                             unsigned long long         id,
                                                             const std::string    & owner );

    size_t                   count                         ( const std::string    & owner ) const;

    CIterator                listByAddr                    () const;

    CIterator                listByOwner                   ( const std::string    & owner ) const;
    size_t findProperty(const std::string& city, const std::string& addr) const;
    size_t findProperty(const std::string& region, unsigned long long id) const;
private:
    friend class CIterator;
    std::vector<Property> properties;
    std::vector<Property> sortedByCityAddress;
    std::vector<Property> sortedByRegionID;
    std::vector<Property> sortedByOwnerAcquisitionTimestamp;
    size_t m_NextAcquisitionOrder = 1;
};

class CIterator
{
public:
    CIterator(const CLandRegister &landRegister, std::vector<CLandRegister::Property> sortedProperties);
    ~CIterator();

    bool                     atEnd                         () const;
    void                     next                          ();
    std::string              city                          () const;
    std::string              addr                          () const;
    std::string              region                        () const;
    unsigned                 id                            () const;
    std::string              owner                         () const;
private:
    friend class CLandRegister;

    const CLandRegister &landRegister;
    size_t currentIndex;
    std::vector<CLandRegister::Property> sortedProperties;
};

CLandRegister::CLandRegister() {}

CLandRegister::~CLandRegister() {}

CIterator::CIterator(const CLandRegister& landRegister, std::vector<CLandRegister::Property> sortedProperties)
        : landRegister(landRegister), currentIndex(0), sortedProperties(sortedProperties) {}

CIterator::~CIterator() {}

bool CLandRegister::add(const std::string& city, const std::string& addr, const std::string& region, unsigned long long id)
{
    if (findProperty(city, addr) != properties.size() || findProperty(region, id) != properties.size()) {
        return false; // Property already exists
    }

    Property newProperty {city, addr, region, id, ""};
    properties.push_back(newProperty);

    auto cmpByCityAddress = [&](const Property& p1, const Property& p2) {
        return std::tie(p1.m_City, p1.m_Addr) < std::tie(p2.m_City, p2.m_Addr);
    };
    auto cmpByRegionID = [&](const Property& p1, const Property& p2) {
        return std::tie(p1.m_Region, p1.m_ID) < std::tie(p2.m_Region, p2.m_ID);
    };
    auto cmpByOwnerAcquisitionTimestamp = [&](const Property& p1, const Property& p2) {
        return p1.m_AcquisitionTimestamp < p2.m_AcquisitionTimestamp;
    };

    auto posCityAddress = std::lower_bound(sortedByCityAddress.begin(), sortedByCityAddress.end(), newProperty, cmpByCityAddress);
    auto posRegionID = std::lower_bound(sortedByRegionID.begin(), sortedByRegionID.end(), newProperty, cmpByRegionID);
    auto posOwnerAcquisitionTimestamp = std::lower_bound(sortedByOwnerAcquisitionTimestamp.begin(), sortedByOwnerAcquisitionTimestamp.end(), newProperty, cmpByOwnerAcquisitionTimestamp);

    sortedByCityAddress.insert(posCityAddress, newProperty);
    sortedByRegionID.insert(posRegionID, newProperty);
    sortedByOwnerAcquisitionTimestamp.insert(posOwnerAcquisitionTimestamp, newProperty);

    return true;
}

bool CLandRegister::del(const std::string& city, const std::string& addr)
{
    size_t index = findProperty(city, addr);
    if (index == properties.size()) {
        return false; // Property not found
    }

    properties.erase(properties.begin() + index);
    return true;
}

bool CLandRegister::del(const std::string &region, unsigned long long id)
{
    size_t index = findProperty(region, id);
    if (index == properties.size()) {
        return false; // Property not found
    }

    properties.erase(properties.begin() + index);
    return true;
}

bool CLandRegister::getOwner(const std::string& city, const std::string& addr, std::string& owner) const
{
    size_t index = findProperty(city, addr);
    if (index == properties.size()) {
        return false; // Property not found
    }

    owner = properties[index].m_Owner;
    return true;
}

bool CLandRegister::getOwner(const std::string& region, unsigned long long id, std::string& owner) const
{
    size_t index = findProperty(region, id);
    if (index == properties.size()) {
        return false; // Property not found
    }

    owner = properties[index].m_Owner ;
    return true;
}

bool CLandRegister::newOwner(const std::string& city, const std::string& addr, const std::string& owner)
{
    size_t index = findProperty(city, addr);
    if (index == properties.size() || properties[index].m_Owner == owner) {
        return false; // Property not found or already owned by the same owner
    }

    properties[index].m_Owner = owner;
    properties[index].m_AcquisitionTimestamp = m_NextAcquisitionOrder++;

    return true;
}

bool CLandRegister::newOwner(const std::string& region, unsigned long long id, const std::string& owner)
{
    size_t index = findProperty(region, id);
    if (index == properties.size() || properties[index].m_Owner == owner) {
        return false; // Property not found
    }

    properties[index].m_Owner = owner;
    properties[index].m_AcquisitionTimestamp = m_NextAcquisitionOrder++;

    return true;
}

size_t CLandRegister::count(const std::string& owner) const
{
    return std::count_if(properties.begin(), properties.end(),
                         [owner](const Property& p) {
                             return strcasecmp(p.m_Owner.c_str(), owner.c_str()) == 0;
                         });
}

CIterator CLandRegister::listByAddr() const
{
    std::vector<Property> sortedProperties = sortedByCityAddress;

    CIterator iterator(*this, sortedProperties);
    return iterator;
}

CIterator CLandRegister::listByOwner(const std::string& owner) const
{
    std::vector<Property> ownedProperties;

    for (const auto& property : properties) {
        if (strcasecmp(property.m_Owner.c_str(), owner.c_str()) == 0) {
            ownedProperties.push_back(property);
        }
    }

    std::sort(ownedProperties.begin(), ownedProperties.end(), [](const Property& a, const Property& b) {
        return a.m_AcquisitionTimestamp < b.m_AcquisitionTimestamp;
    });

    CIterator iterator(*this, ownedProperties);
    return iterator;
}

size_t CLandRegister::findProperty(const std::string& city, const std::string& addr) const
{
    auto it = std::find_if(properties.begin(), properties.end(),
                           [&](const Property& p) { return p.m_City == city && p.m_Addr == addr; });

    return std::distance(properties.begin(), it);
}

size_t CLandRegister::findProperty(const std::string& region, unsigned long long id) const
{
    auto it = std::find_if(properties.begin(), properties.end(),
                           [region, id](const Property& p) { return p.m_Region == region && p.m_ID == id; });

    return std::distance(properties.begin(), it);
}

bool CIterator::atEnd() const
{
    return currentIndex >= sortedProperties.size();
}

void CIterator::next()
{
    if(!atEnd())
    {
        currentIndex++;
    }
}

std::string CIterator::city() const
{
    return (!atEnd()) ? sortedProperties[currentIndex].m_City : "";
}

std::string CIterator::addr() const
{
    return (!atEnd()) ? sortedProperties[currentIndex].m_Addr : "";
}

std::string CIterator::owner() const
{
    return (!atEnd()) ? sortedProperties[currentIndex].m_Owner : "";
}

std::string CIterator::region() const
{
    return (!atEnd()) ? sortedProperties[currentIndex].m_Region : "";
}

unsigned CIterator::id() const
{
    return (!atEnd()) ? sortedProperties[currentIndex].m_ID : 0;
}


#ifndef __PROGTEST__
static void test0 ()
{
    CLandRegister x;
    std::string owner;

    assert ( x . add ( "Prague", "Thakurova", "Dejvice", 12345 ) );
    assert ( x . add ( "Prague", "Evropska", "Vokovice", 12345 ) );
    assert ( x . add ( "Prague", "Technicka", "Dejvice", 9873 ) );
    assert ( x . add ( "Plzen", "Evropska", "Plzen mesto", 78901 ) );
    assert ( x . add ( "Liberec", "Evropska", "Librec", 4552 ) );
    CIterator i0 = x . listByAddr ();
    assert ( ! i0 . atEnd ()
             && i0 . city () == "Liberec"
             && i0 . addr () == "Evropska"
             && i0 . region () == "Librec"
             && i0 . id () == 4552
             && i0 . owner () == "" );
    i0 . next ();
    assert ( ! i0 . atEnd ()
             && i0 . city () == "Plzen"
             && i0 . addr () == "Evropska"
             && i0 . region () == "Plzen mesto"
             && i0 . id () == 78901
             && i0 . owner () == "" );
    i0 . next ();
    assert ( ! i0 . atEnd ()
             && i0 . city () == "Prague"
             && i0 . addr () == "Evropska"
             && i0 . region () == "Vokovice"
             && i0 . id () == 12345
             && i0 . owner () == "" );
    i0 . next ();
    assert ( ! i0 . atEnd ()
             && i0 . city () == "Prague"
             && i0 . addr () == "Technicka"
             && i0 . region () == "Dejvice"
             && i0 . id () == 9873
             && i0 . owner () == "" );
    i0 . next ();
    assert ( ! i0 . atEnd ()
             && i0 . city () == "Prague"
             && i0 . addr () == "Thakurova"
             && i0 . region () == "Dejvice"
             && i0 . id () == 12345
             && i0 . owner () == "" );
    i0 . next ();
    assert ( i0 . atEnd () );

    assert ( x . count ( "" ) == 5 );
    CIterator i1 = x . listByOwner ( "" );
    assert ( ! i1 . atEnd ()
             && i1 . city () == "Prague"
             && i1 . addr () == "Thakurova"
             && i1 . region () == "Dejvice"
             && i1 . id () == 12345
             && i1 . owner () == "" );
    i1 . next ();
    assert ( ! i1 . atEnd ()
             && i1 . city () == "Prague"
             && i1 . addr () == "Evropska"
             && i1 . region () == "Vokovice"
             && i1 . id () == 12345
             && i1 . owner () == "" );
    i1 . next ();
    assert ( ! i1 . atEnd ()
             && i1 . city () == "Prague"
             && i1 . addr () == "Technicka"
             && i1 . region () == "Dejvice"
             && i1 . id () == 9873
             && i1 . owner () == "" );
    i1 . next ();
    assert ( ! i1 . atEnd ()
             && i1 . city () == "Plzen"
             && i1 . addr () == "Evropska"
             && i1 . region () == "Plzen mesto"
             && i1 . id () == 78901
             && i1 . owner () == "" );
    i1 . next ();
    assert ( ! i1 . atEnd ()
             && i1 . city () == "Liberec"
             && i1 . addr () == "Evropska"
             && i1 . region () == "Librec"
             && i1 . id () == 4552
             && i1 . owner () == "" );
    i1 . next ();
    assert ( i1 . atEnd () );

    assert ( x . count ( "CVUT" ) == 0 );
    CIterator i2 = x . listByOwner ( "CVUT" );
    assert ( i2 . atEnd () );

    assert ( x . newOwner ( "Prague", "Thakurova", "CVUT" ) );
    assert ( x . newOwner ( "Dejvice", 9873, "CVUT" ) );
    assert ( x . newOwner ( "Plzen", "Evropska", "Anton Hrabis" ) );
    assert ( x . newOwner ( "Librec", 4552, "Cvut" ) );
    assert ( x . getOwner ( "Prague", "Thakurova", owner ) && owner == "CVUT" );
    assert ( x . getOwner ( "Dejvice", 12345, owner ) && owner == "CVUT" );
    assert ( x . getOwner ( "Prague", "Evropska", owner ) && owner == "" );
    assert ( x . getOwner ( "Vokovice", 12345, owner ) && owner == "" );
    assert ( x . getOwner ( "Prague", "Technicka", owner ) && owner == "CVUT" );
    assert ( x . getOwner ( "Dejvice", 9873, owner ) && owner == "CVUT" );
    assert ( x . getOwner ( "Plzen", "Evropska", owner ) && owner == "Anton Hrabis" );
    assert ( x . getOwner ( "Plzen mesto", 78901, owner ) && owner == "Anton Hrabis" );
    assert ( x . getOwner ( "Liberec", "Evropska", owner ) && owner == "Cvut" );
    assert ( x . getOwner ( "Librec", 4552, owner ) && owner == "Cvut" );
    CIterator i3 = x . listByAddr ();
    assert ( ! i3 . atEnd ()
             && i3 . city () == "Liberec"
             && i3 . addr () == "Evropska"
             && i3 . region () == "Librec"
             && i3 . id () == 4552
             && i3 . owner () == "Cvut" );
    i3 . next ();
    assert ( ! i3 . atEnd ()
             && i3 . city () == "Plzen"
             && i3 . addr () == "Evropska"
             && i3 . region () == "Plzen mesto"
             && i3 . id () == 78901
             && i3 . owner () == "Anton Hrabis" );
    i3 . next ();
    assert ( ! i3 . atEnd ()
             && i3 . city () == "Prague"
             && i3 . addr () == "Evropska"
             && i3 . region () == "Vokovice"
             && i3 . id () == 12345
             && i3 . owner () == "" );
    i3 . next ();
    assert ( ! i3 . atEnd ()
             && i3 . city () == "Prague"
             && i3 . addr () == "Technicka"
             && i3 . region () == "Dejvice"
             && i3 . id () == 9873
             && i3 . owner () == "CVUT" );
    i3 . next ();
    assert ( ! i3 . atEnd ()
             && i3 . city () == "Prague"
             && i3 . addr () == "Thakurova"
             && i3 . region () == "Dejvice"
             && i3 . id () == 12345
             && i3 . owner () == "CVUT" );
    i3 . next ();
    assert ( i3 . atEnd () );

    assert ( x . count ( "cvut" ) == 3 );
    CIterator i4 = x . listByOwner ( "cVuT" );
    assert ( ! i4 . atEnd ()
             && i4 . city () == "Prague"
             && i4 . addr () == "Thakurova"
             && i4 . region () == "Dejvice"
             && i4 . id () == 12345
             && i4 . owner () == "CVUT" );
    i4 . next ();
    assert ( ! i4 . atEnd ()
             && i4 . city () == "Prague"
             && i4 . addr () == "Technicka"
             && i4 . region () == "Dejvice"
             && i4 . id () == 9873
             && i4 . owner () == "CVUT" );
    i4 . next ();
    assert ( ! i4 . atEnd ()
             && i4 . city () == "Liberec"
             && i4 . addr () == "Evropska"
             && i4 . region () == "Librec"
             && i4 . id () == 4552
             && i4 . owner () == "Cvut" );
    i4 . next ();
    assert ( i4 . atEnd () );

    assert ( x . newOwner ( "Plzen mesto", 78901, "CVut" ) );
    assert ( x . count ( "CVUT" ) == 4 );
    CIterator i5 = x . listByOwner ( "CVUT" );
    assert ( ! i5 . atEnd ()
             && i5 . city () == "Prague"
             && i5 . addr () == "Thakurova"
             && i5 . region () == "Dejvice"
             && i5 . id () == 12345
             && i5 . owner () == "CVUT" );
    i5 . next ();
    assert ( ! i5 . atEnd ()
             && i5 . city () == "Prague"
             && i5 . addr () == "Technicka"
             && i5 . region () == "Dejvice"
             && i5 . id () == 9873
             && i5 . owner () == "CVUT" );
    i5 . next ();
    assert ( ! i5 . atEnd ()
             && i5 . city () == "Liberec"
             && i5 . addr () == "Evropska"
             && i5 . region () == "Librec"
             && i5 . id () == 4552
             && i5 . owner () == "Cvut" );
    i5 . next ();
    assert ( ! i5 . atEnd ()
             && i5 . city () == "Plzen"
             && i5 . addr () == "Evropska"
             && i5 . region () == "Plzen mesto"
             && i5 . id () == 78901
             && i5 . owner () == "CVut" );
    i5 . next ();
    assert ( i5 . atEnd () );

    assert ( x . del ( "Liberec", "Evropska" ) );
    assert ( x . del ( "Plzen mesto", 78901 ) );
    assert ( x . count ( "cvut" ) == 2 );
    CIterator i6 = x . listByOwner ( "cVuT" );
    assert ( ! i6 . atEnd ()
             && i6 . city () == "Prague"
             && i6 . addr () == "Thakurova"
             && i6 . region () == "Dejvice"
             && i6 . id () == 12345
             && i6 . owner () == "CVUT" );
    i6 . next ();
    assert ( ! i6 . atEnd ()
             && i6 . city () == "Prague"
             && i6 . addr () == "Technicka"
             && i6 . region () == "Dejvice"
             && i6 . id () == 9873
             && i6 . owner () == "CVUT" );
    i6 . next ();
    assert ( i6 . atEnd () );

    assert ( x . add ( "Liberec", "Evropska", "Librec", 4552 ) );
}

static void test1 () {
    CLandRegister x;
    std::string owner;

    assert (x.add("Prague", "Thakurova", "Dejvice", 12345));
    assert (x.add("Prague", "Evropska", "Vokovice", 12345));
    assert (x.add("Prague", "Technicka", "Dejvice", 9873));
    assert (!x.add("Prague", "Technicka", "Hradcany", 7344));
    assert (!x.add("Brno", "Bozetechova", "Dejvice", 9873));
    assert (!x.getOwner("Prague", "THAKUROVA", owner));
    assert (!x.getOwner("Hradcany", 7343, owner));
    CIterator i0 = x.listByAddr();
    assert (!i0.atEnd()
            && i0.city() == "Prague"
            && i0.addr() == "Evropska"
            && i0.region() == "Vokovice"
            && i0.id() == 12345
            && i0.owner() == "");
    i0.next();
    assert (!i0.atEnd()
            && i0.city() == "Prague"
            && i0.addr() == "Technicka"
            && i0.region() == "Dejvice"
            && i0.id() == 9873
            && i0.owner() == "");
    i0.next();
    assert (!i0.atEnd()
            && i0.city() == "Prague"
            && i0.addr() == "Thakurova"
            && i0.region() == "Dejvice"
            && i0.id() == 12345
            && i0.owner() == "");
    i0.next();
    assert (i0.atEnd());

    assert (x.newOwner("Prague", "Thakurova", "CVUT"));
    assert (!x.newOwner("Prague", "technicka", "CVUT"));
    assert (!x.newOwner("prague", "Technicka", "CVUT"));
    assert (!x.newOwner("dejvice", 9873, "CVUT"));
    assert (!x.newOwner("Dejvice", 9973, "CVUT"));
    assert (!x.newOwner("Dejvice", 12345, "CVUT"));
    assert (x.count("CVUT") == 1);
    CIterator i1 = x.listByOwner("CVUT");
    assert (!i1.atEnd()
            && i1.city() == "Prague"
            && i1.addr() == "Thakurova"
            && i1.region() == "Dejvice"
            && i1.id() == 12345
            && i1.owner() == "CVUT");
    i1.next();
    assert (i1.atEnd());

    assert (!x.del("Brno", "Technicka"));
    assert (!x.del("Karlin", 9873));
    assert (x.del("Prague", "Technicka"));
    assert (!x.del("Prague", "Technicka"));
    assert (!x.del("Dejvice", 9873));
    assert (x.add("Tokyo", "Nagana", "Tokyo City", 12020203993));
}

int main ( void )
{
    test0 ();
    test1 ();
    return EXIT_SUCCESS;
}
#endif /* __PROGTEST__ */