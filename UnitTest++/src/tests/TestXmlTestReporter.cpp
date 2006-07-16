#include "../UnitTest++.h"
#include "../XmlTestReporter.h"

#include <sstream>

using namespace UnitTest;
using std::ostringstream;

namespace
{

#ifdef UNITTEST_USE_CUSTOM_STREAMS

// Overload to let MemoryOutStream accept std::string
MemoryOutStream& operator<<(MemoryOutStream& s, const std::string& value)
{
    s << value.c_str();
    return s;
}

#endif

struct XmlTestReporterFixture
{
	XmlTestReporterFixture()
		: reporter(output)
	{
	}

	ostringstream output;
	XmlTestReporter reporter;
};

TEST_FIXTURE(XmlTestReporterFixture, MultipleCharactersAreEscaped)
{
	TestDetails const details("TestName", "suite", "file", 123);

	reporter.ReportTestStart(details);
	reporter.ReportFailure(details, "filename.h", 4321, "\"\"\'\'&&<<>>");
	reporter.ReportTestFinish(details, 0.1f);
	reporter.ReportSummary(1, 1, 0.1f);

	char const* expected =
		"<?xml version=\"1.0\"?>"
		"<unittest-results tests=\"1\" failures=\"1\" time=\"0.1\">"
		"<test suite=\"suite\" name=\"TestName\" time=\"0.1\">"
		"<failure message=\"filename.h(4321) : "
		"&quot;&quot;&apos;&apos;&amp;&amp;&lt;&lt;&gt;&gt;\"/>"
		"</test>"
		"</unittest-results>";

	CHECK_EQUAL(expected, output.str());
}

TEST_FIXTURE(XmlTestReporterFixture, OutputIsCachedUntilReportSummaryIsCalled)
{
    TestDetails const details("test", "suite", "file", 123);

    reporter.ReportTestStart(details);
    reporter.ReportFailure(details, "file", 126, "message");
    reporter.ReportTestFinish(details, 1.0F);
    CHECK(output.str().empty());

    reporter.ReportSummary(1, 1, 1.0F);
    CHECK(!output.str().empty());
}

TEST_FIXTURE(XmlTestReporterFixture, EmptyReportSummaryFormat)
{
    reporter.ReportSummary(0, 0, 0.1f);

    const char *expected =
"<?xml version=\"1.0\"?>"
"<unittest-results tests=\"0\" failures=\"0\" time=\"0.1\">"
"</unittest-results>";

    CHECK_EQUAL(expected, output.str());
}

TEST_FIXTURE(XmlTestReporterFixture, SingleSuccessfulTestReportSummaryFormat)
{
    TestDetails const details("TestName");

    reporter.ReportTestStart(details);
    reporter.ReportSummary(1, 0, 0.1f);

    const char *expected =
"<?xml version=\"1.0\"?>"
"<unittest-results tests=\"1\" failures=\"0\" time=\"0.1\">"
"<test suite=\"DefaultSuite\" name=\"TestName\" time=\"0\"/>"
"</unittest-results>";
    
    CHECK_EQUAL(expected, output.str());
}

TEST_FIXTURE(XmlTestReporterFixture, SingleFailedTestReportSummaryFormat)
{
    TestDetails const details("A Test", "suite", "file", 123);
  
    reporter.ReportTestStart(details);
    reporter.ReportFailure(details, "A File", 4321, "A Failure");
    reporter.ReportSummary(1, 1, 0.1f);

    const char *expected =
		"<?xml version=\"1.0\"?>"
		"<unittest-results tests=\"1\" failures=\"1\" time=\"0.1\">"
		"<test suite=\"suite\" name=\"A Test\" time=\"0\">"
		"<failure message=\"A File(4321) : A Failure\"/>"
		"</test>"
		"</unittest-results>";

    CHECK_EQUAL(expected, output.str());
}

TEST_FIXTURE(XmlTestReporterFixture, FailureMessageIsXMLEscaped)
{
    TestDetails const details("TestName", "suite", "file", 123);
  
    reporter.ReportTestStart(details);
    reporter.ReportFailure(details, "filename.h", 4321, "\"\'&<>");
    reporter.ReportTestFinish(details, 0.1f);
    reporter.ReportSummary(1, 1, 0.1f);
    
    char const* expected =
		"<?xml version=\"1.0\"?>"
		"<unittest-results tests=\"1\" failures=\"1\" time=\"0.1\">"
		"<test suite=\"suite\" name=\"TestName\" time=\"0.1\">"
		"<failure message=\"filename.h(4321) : &quot;&apos;&amp;&lt;&gt;\"/>"
		"</test>"
		"</unittest-results>";

    CHECK_EQUAL(expected, output.str());
}

TEST_FIXTURE(XmlTestReporterFixture, OneFailureAndOneSuccess)
{
    TestDetails const failedDetails("FailedTest", "suite", "file", 123);
    reporter.ReportTestStart(failedDetails);
    reporter.ReportFailure(failedDetails, "fail.h", 1, "expected 1 but was 2");
    reporter.ReportTestFinish(failedDetails, 0.1f);

    TestDetails const succeededDetails("SucceededTest", "suite");
    reporter.ReportTestStart(succeededDetails);
    reporter.ReportTestFinish(succeededDetails, 1.0f);
    reporter.ReportSummary(2, 1, 1.1f);
    
    char const* expected =
		"<?xml version=\"1.0\"?>"
		"<unittest-results tests=\"2\" failures=\"1\" time=\"1.1\">"
		"<test suite=\"suite\" name=\"FailedTest\" time=\"0.1\">"
		"<failure message=\"fail.h(1) : expected 1 but was 2\"/>"
		"</test>"
		"<test suite=\"suite\" name=\"SucceededTest\" time=\"1\"/>"
		"</unittest-results>";

    CHECK_EQUAL(expected, output.str());
}

}