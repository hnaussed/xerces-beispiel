#include <iostream>
#include <memory>
#include <string>
#include "people.hxx"

using namespace std;

void accessElements(const string &file)
{
  unique_ptr<people_t> ppl(people(file));

  people_t::person_sequence &persons(ppl->person());

  for (auto person : persons)
  {
    cout << "name:   " << person.first_name() << " ";

    if (person.middle_name().present())
    {
      cout << person.middle_name().get() << " ";
    }

    cout << person.last_name() << endl;

    cout << "gender: " << person.gender() << endl
         << "age:    " << person.age() << endl
         << "id:     " << person.id() << endl
         << endl;
  }
}

void modifyElement(const string &file)
{
  unique_ptr<people_t> ppl(people(file));

  people_t::person_sequence &persons(ppl->person());

  for (auto person : persons)
  {
    person.age()++;
  }

  person_t &john(persons[0]);
  person_t &jane(persons[1]);

  john.middle_name("Mary");
  jane.middle_name().reset();

  persons.push_back(john);

  xml_schema::namespace_infomap map;
  map[""].name = "";
  map[""].schema = "people.xsd";

  people(cout, *ppl, map);
}

void createElement()
{
  people_t ppl;
  people_t::person_sequence &persons(ppl.person());

  persons.push_back(
      person_t("John",         // first-name
               "Doe",          // last-name
               gender_t::male, // gender
               32,             // age
               1));

  persons.push_back(
      person_t("Jane",           // first-name
               "Doe",            // last-name
               gender_t::female, // gender
               28,               // age
               2));              // id

  person_t &jane(persons.back());
  jane.middle_name("Mary");

  xml_schema::namespace_infomap map;
  map[""].name = "";
  map[""].schema = "people.xsd";

  people(cout, ppl, map);
}

int main(int argc, char *argv[])
{
  std::string file = "../people.xml";
  cout << "Access Elements" << endl;
  accessElements(file);
  cout << "Modify Elements" << endl;
  modifyElement(file);
  cout << "Create Elements" << endl;
  createElement();
}
