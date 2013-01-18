#include "xUnit++/xUnit++.h"
#include "XmlReporter.h"
#include "tinyxml2.h"

using xUnitpp::Utilities::XmlReporter;

SUITE("XmlReporter")
{

FACT("XmlReporter generates valid xml with no tests")
{
    std::stringstream out;

    XmlReporter reporter(out);
    reporter.ReportAllTestsComplete(0, 0, 0, 0);

    Assert.Equal(tinyxml2::XMLError::XML_SUCCESS, tinyxml2::XMLDocument().Parse(out.str().c_str()));
}

}
