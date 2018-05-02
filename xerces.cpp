#include <iostream>
#include <fstream>
#include <memory>
#include <sstream>

#include <xercesc/sax/HandlerBase.hpp>
#include <xercesc/dom/DOM.hpp>
#include <xercesc/util/XMLString.hpp>
#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/framework/StdOutFormatTarget.hpp>
#include <xercesc/framework/LocalFileFormatTarget.hpp>

XERCES_CPP_NAMESPACE_USE
using namespace std;

struct XercesPlattform
{
    class CustomErrorHandler : public HandlerBase
    {

      public:
        CustomErrorHandler() {}

        void error(const SAXParseException &e)
        {
            handler(e);
        }

        void fatalError(const SAXParseException &e)
        {
            handler(e);
        }

        void warning(const SAXParseException &e)
        {
            handler(e);
        }

      private:
        void handler(const SAXParseException &e)
        {
            char *message = XMLString::transcode(e.getMessage());

            cerr << "line " << e.getLineNumber()
                 << ", column " << e.getColumnNumber()
                 << " -> " << message << "\n\n";

            XMLString::release(&message);
        }
    };

    XercesDOMParser *domParser;

    XercesPlattform()
    {
        try
        {
            XMLPlatformUtils::Initialize();
            domParser = new XercesDOMParser();
            ErrorHandler *errorHandler = errorHandler = dynamic_cast<ErrorHandler *>(new CustomErrorHandler());
            domParser->setErrorHandler(errorHandler);
        }
        catch (const XMLException &e)
        {
            char *message = XMLString::transcode(e.getMessage());
            cerr << "XML toolkit initialization error: " << message << endl;
            XMLString::release(&message);
        }
    };

    ~XercesPlattform()
    {
        XMLPlatformUtils::Terminate();
    };

    DOMDocument *readDom(const std::string &file) {

      if (domParser->loadGrammar("../people.xsd", Grammar::SchemaGrammarType) ==
          NULL) {
        cerr << "couldn't load schema" << endl;
        return nullptr;
      }

      domParser->setValidationScheme(XercesDOMParser::Val_Always);
      domParser->setDoNamespaces(true);
      domParser->setDoSchema(true);
      domParser->setValidationSchemaFullChecking(true);

      domParser->parse(file.c_str());
      if (domParser->getErrorCount() == 0) {
        cerr << "XML file validated against the schema successfully" << endl;
      } else {
        cerr << "XML file doesn't conform to the schema" << endl;
      }
      return domParser->getDocument();
    };

    void writeToStdOut(DOMDocument *doc) {
      XMLCh *LS = XMLString::transcode("LS");
      DOMImplementation *implement =
          DOMImplementationRegistry::getDOMImplementation(LS);
      XMLString::release(&LS);

      DOMLSSerializer *serializer =
          dynamic_cast<DOMImplementationLS *>(implement)->createLSSerializer();

      DOMConfiguration *domConfiguration = serializer->getDomConfig();
      domConfiguration->setParameter(XMLUni::fgDOMWRTFormatPrettyPrint, true);

      XMLFormatTarget *target = new StdOutFormatTarget();
      DOMLSOutput *output =
          dynamic_cast<DOMImplementationLS *>(implement)->createLSOutput();
      output->setByteStream(target);

      serializer->write(doc, output);

      output->release();
      serializer->release();
    };
};


void modifyElements(const std::string &file)
{
    XercesPlattform xerces;

    DOMDocument *doc(xerces.readDom(file));
    DOMElement *p = doc->getDocumentElement();

    XMLCh *temp = XMLString::transcode("age");
    DOMNodeList *list = doc->getElementsByTagName(temp);
    XMLString::release(&temp); //Freigeben von Speicher

    const XMLSize_t nodeCount = list->getLength();
    for (XMLSize_t nodeIndex = 0; nodeIndex < nodeCount; ++nodeIndex) //Kein ForEach
    {
        DOMNode *currentNode = list->item(nodeIndex);
        if (currentNode->getNodeType() != DOMNode::ELEMENT_NODE)
        {
            continue;
        }
        DOMNode *text = currentNode->getFirstChild();

        short int_age;
        char *char_age = XMLString::transcode(text->getNodeValue());
        int_age = stoi(char_age); //Kann Exceptions werfen, Nicht Typ sicher
        XMLString::release(&char_age);
       
        ++int_age;
        std::string str_age =  to_string(int_age);

        XMLCh *age_char = XMLString::transcode(str_age.c_str());
        text->setNodeValue(age_char);
        XMLString::release(&age_char);
    }
    xerces.writeToStdOut(doc);
}
    


void accessElements(const std::string &file)
{
    XercesPlattform xerces;
    DOMDocument *doc(xerces.readDom(file));

    DOMElement *p = doc->getDocumentElement();

    XMLCh *temp = XMLString::transcode("person");
    DOMNodeList *list = doc->getElementsByTagName(temp);
    XMLString::release(&temp);

    const XMLSize_t nodeCount = list->getLength();
    for (XMLSize_t nodeIndex = 0; nodeIndex < nodeCount; ++nodeIndex)
    {
        DOMNode *currentNode = list->item(nodeIndex);
        if (currentNode->getNodeType() != DOMNode::ELEMENT_NODE)
            continue;

        XMLCh *temp = XMLString::transcode("id");
        DOMElement *currentElement = dynamic_cast<xercesc::DOMElement *>(currentNode);
        const XMLCh *attribute = currentElement->getAttribute(temp);
        XMLString::release(&temp);
        char *attribute_char = XMLString::transcode(attribute);
        cout << "attribute: " << attribute_char << endl;
        XMLString::release(&attribute_char);

        for (DOMNode *n = currentNode->getFirstChild();
             n != nullptr;
             n = n->getNextSibling())
        {
            if (n->getNodeType() != DOMNode::ELEMENT_NODE)
                continue;

            char *el_name = XMLString::transcode(n->getNodeName());
            cout << el_name << ": ";
            XMLString::release(&el_name);

            DOMNode *text = n->getFirstChild();
            char *name = XMLString::transcode(text->getNodeValue());
            cout << name << endl;
            XMLString::release(&name);
        }
    }

    doc->release();
}

int main()
{
    const string file("../people.xml");

    accessElements(file);
    modifyElements(file);
}
