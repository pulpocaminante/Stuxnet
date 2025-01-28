#define BOOST_TEST_MODULE DebugTests
#define BOOST_TEST_LOG_COMPACT_ON_SUCCESS

#include <boost/test/unit_test.hpp>

#include "../ExportInterface.hpp"
#include "../WMIConnection.hpp"
#include "Shell32Wrapper.hpp"
#include "wmi.h"

std::wstring encodedFileContentsWrite;
std::wstring encodedFileContentsRead;


BOOST_AUTO_TEST_CASE(Query_Success) {

    // Arrange
    COMWrapper ICom;
    WMIConnection IWmi(ICom);
    QueryInterface IQuery(ICom, IWmi);
    std::wstring sQuery = L"SELECT * FROM Win32_LogicalDisk";
    std::vector<std::map<std::wstring, std::any>> vOut;
    
    // Act
    HRESULT hres = IQuery.Query(sQuery, vOut);
    
	// Assert
    BOOST_CHECK(SUCCEEDED(hres));
    BOOST_CHECK(!vOut.empty());
}

BOOST_AUTO_TEST_CASE(Query_Fail) {
    
    // Arrange
    COMWrapper ICom;
    WMIConnection IWmi(ICom);
    QueryInterface IQuery(ICom, IWmi);
    std::wstring sQuery = L"Invalid WQL query";
    std::vector<std::map<std::wstring, std::any>> vOut;

    // Act
    HRESULT hres = IQuery.Query(sQuery, vOut);
    BOOST_CHECK(FAILED(hres));
    BOOST_CHECK(vOut.empty());
}

BOOST_AUTO_TEST_CASE(Query_NoResults) {

    // Arrange
    COMWrapper ICom;
    WMIConnection IWmi(ICom);
    QueryInterface IQuery(ICom, IWmi);
    std::wstring sQuery = L"SELECT * FROM Win32_LogicalDisk WHERE DriveType = 0";
    std::vector<std::map<std::wstring, std::any>> vOut;

    // Act
    
    HRESULT hres = IQuery.Query(sQuery, vOut);

	// Assert
    BOOST_CHECK(SUCCEEDED(hres));
    BOOST_CHECK(vOut.empty());
    
}

BOOST_AUTO_TEST_CASE(ExportInterfaceLeak_Check)
{
    // Arrange
    IExport IFind;

    // Act
    LPVOID address = IFind.LoadAndFindSingleExport("sdh.le2ll3l", "ScheuextWlEexlE");

    // Assert
    BOOST_REQUIRE_NE(address, nullptr);
}

BOOST_AUTO_TEST_CASE(RailfenceCipherIntegrity_Check)
{
    // Arrange
    IExport IFind;
    const wchar_t* ciphertext = L"RutOcrneOeierTStC2\\y";
    wchar_t plaintext[256];

    // Act
    IFind.railfence_wdecipher(5, ciphertext, plaintext);
    
    // Assert
    wchar_t expected_plaintext[] = L"ROOT\\SecurityCenter2";
    BOOST_REQUIRE_MESSAGE(wcscmp(plaintext, expected_plaintext) == 0, "plaintext and expected_plaintext are not equal.");
}

BOOST_AUTO_TEST_CASE(InitializeCOM_Check)
{
    // Arrange
    COMWrapper ICom;

    // Assert
    BOOST_REQUIRE_EQUAL(ICom.IReady, TRUE);
}

BOOST_AUTO_TEST_CASE(InitializeWMI_Check)
{
    // Arrange
    COMWrapper ICom;
    WMIConnection IWmi(ICom);
    
    // Assert
    BOOST_REQUIRE_EQUAL(IWmi.IReady, TRUE);
    BOOST_REQUIRE_EQUAL(ICom.IReady, TRUE);
}

BOOST_AUTO_TEST_CASE(Base64ReadWriteIntegrity_Check)
{
    // Arrange
    Railfence ciph;
    encodedFileContentsWrite.clear();
    encodedFileContentsRead.clear();

    // Act
    std::string sFilePathIn = "C:\\GOG Games\\DOOM\\DOOM.WAD";
    std::string sFilePathOut = "C:\\GOG Games\\DOOM\\DOOM_CPY.WAD";

    ciph.FileToBase64UTF16(sFilePathIn, encodedFileContentsWrite);
    ciph.Base64UTF16ToFile(encodedFileContentsWrite, sFilePathOut);

    std::ifstream fileIn(sFilePathIn, std::ios::binary | std::ios::ate);
    std::ifstream fileOut(sFilePathOut, std::ios::binary | std::ios::ate);
    std::streamsize sizeIn = fileIn.tellg();
    std::streamsize sizeOut = fileOut.tellg();

    // Assert
    BOOST_REQUIRE_EQUAL(sizeIn, sizeOut);
    BOOST_REQUIRE_MESSAGE(encodedFileContentsWrite == encodedFileContentsRead, "File contents are not equal.");
}

BOOST_AUTO_TEST_SUITE(WMIFS_Suite)
BOOST_AUTO_TEST_CASE(CreateDrive_Success)
{
    // Arrange
    COMWrapper ICom;
    WMIConnection IWmi(ICom);
    ClassFactory IFactory(ICom, IWmi);
    WMIFSInterface IWMIFS(ICom, IWmi, IFactory);

    std::wstring sDriveName = L"Test";
    std::wstring sFileContent = L"Some file content";
    
    HRESULT hres = IWMIFS.CreateDrive(sDriveName, &sFileContent);
    BOOST_CHECK(SUCCEEDED(hres));
}

BOOST_AUTO_TEST_CASE(CreateDrive_Fail)
{
    // Arrange
    COMWrapper ICom;
    WMIConnection IWmi(ICom);
    ClassFactory IFactory(ICom, IWmi);
    WMIFSInterface IWMIFS(ICom, IWmi, IFactory);

    std::wstring sDriveName = L"Inva11111111111111111111111111111111lidDri#$*Jdfs-fsd-veName";
    std::wstring sFileContent = L"Some file content";
    HRESULT hres = IWMIFS.CreateDrive(sDriveName, &sFileContent);
    BOOST_CHECK(FAILED(hres));
}

BOOST_AUTO_TEST_CASE(CreateDriveInstance_Success) {
    // Arrange
    COMWrapper ICom;
    WMIConnection IWmi(ICom);
    ClassFactory IFactory(ICom, IWmi);
    WMIFSInterface IWMIFS(ICom, IWmi, IFactory);
    HRESULT hresCreate = NULL;
    HRESULT hresCreateInstance = NULL;
    HRESULT hresDelete = NULL;

    // Act
    std::wstring sDriveName = L"Test3";
	hresCreate = IWMIFS.CreateDrive(sDriveName, nullptr);
    hresCreateInstance = IWMIFS.CreateDriveInstance(sDriveName);
	hresDelete = IWMIFS.DeleteDrive(sDriveName);

    // Assert
	BOOST_CHECK_MESSAGE(SUCCEEDED(hresCreate), "Drive creation failed.");
	BOOST_CHECK_MESSAGE(SUCCEEDED(hresCreateInstance), "Instance creation failed.");
	BOOST_CHECK_MESSAGE(SUCCEEDED(hresDelete), "Drive deletion failed.");
}

BOOST_AUTO_TEST_CASE(CreateDriveInstance_Fail_GetObject) {
    // Arrange
    COMWrapper ICom;
    WMIConnection IWmi(ICom);
    ClassFactory IFactory(ICom, IWmi);
    WMIFSInterface IWMIFS(ICom, IWmi, IFactory);

    // Act
    std::wstring sDriveName = L"Inva@lid#--)/.DriveN$ame";
    HRESULT hres = IWMIFS.CreateDriveInstance(sDriveName);

    // Assert
    BOOST_CHECK(FAILED(hres));
}

BOOST_AUTO_TEST_CASE(CreateDriveInstance_Fail_GetObject2) {
    // Arrange
    COMWrapper ICom;
    WMIConnection IWmi(ICom);
    ClassFactory IFactory(ICom, IWmi);
    WMIFSInterface IWMIFS(ICom, IWmi, IFactory);

    // Act
    std::wstring sDriveName = L"LegalYetInvalidName";
    HRESULT hres = IWMIFS.CreateDriveInstance(sDriveName);

    // Assert
    BOOST_CHECK(FAILED(hres));
}

BOOST_AUTO_TEST_CASE(WMIFSWriteFile_Fail_FilePath)
{
    // Arrange
    COMWrapper ICom;
    WMIConnection IWmi(ICom);
    ClassFactory IFactory(ICom, IWmi);
    WMIFSInterface IWMIFS(ICom, IWmi, IFactory);
    Railfence ciph;
    HRESULT hres;
    std::wstring fileContents;

    // Act
    std::string sFilePath = "fds333fs3dfsfs3df";
    hres = ciph.FileToBase64UTF16(sFilePath, fileContents);
    
    // Assert
	BOOST_CHECK(FAILED(hres));
    
    // Act
    hres = IWMIFS.WriteFile(L"Test", L"FileName", fileContents);
    
    // Assert
    BOOST_CHECK(FAILED(hres));
}

BOOST_AUTO_TEST_CASE(WMIFSWriteFile_Fail_FileDrive)
{
    // Arrange
    COMWrapper ICom;
    WMIConnection IWmi(ICom);
    ClassFactory IFactory(ICom, IWmi);
    WMIFSInterface IWMIFS(ICom, IWmi, IFactory);
    Railfence ciph;
    HRESULT hres;
    std::wstring fileContents;
    
    // Act
    std::string sFilePath = "C:\\GOG Games\\DOOM\\DOOM.WAD";


    ciph.FileToBase64UTF16(sFilePath, fileContents);

    hres = IWMIFS.WriteFile(L"$@#$@#RSFDSGHDJDDSfdssdfsdf", L"FileName", fileContents);

    // Assert
    BOOST_CHECK(FAILED(hres));
}

BOOST_AUTO_TEST_CASE(WMIFSReadWrite_Success)
{
    // Arrange
    COMWrapper ICom;
    WMIConnection IWmi(ICom);
    ClassFactory IFactory(ICom, IWmi);
    WMIFSInterface IWMIFS(ICom, IWmi, IFactory);
    Railfence ciph;
    HRESULT hresWrite;
    HRESULT hresRead;
    HRESULT hresCreate;
    HRESULT hresDelete;
    std::wstring fileContentsWrite;
    std::wstring fileContentsRead;
    
    std::string sFilePath = "C:\\GOG Games\\DOOM\\unins000.exe";
    ciph.FileToBase64UTF16(sFilePath, fileContentsWrite);

    // Act
    hresCreate = IWMIFS.CreateDrive(L"Test2", NULL);
    hresWrite = IWMIFS.WriteFile(L"Test2", L"Test", fileContentsWrite);
    hresRead = IWMIFS.ReadFile(L"Test2", L"Test", fileContentsRead);
    hresDelete = IWMIFS.DeleteDrive(L"Test2");
    
    // Assert
	BOOST_CHECK_MESSAGE(SUCCEEDED(hresWrite), "Write failed");
	BOOST_CHECK_MESSAGE(SUCCEEDED(hresRead), "Read failed");
	BOOST_CHECK_MESSAGE(SUCCEEDED(hresCreate), "Create failed");
	BOOST_CHECK_MESSAGE(SUCCEEDED(hresDelete), "Delete failed");
	BOOST_CHECK_MESSAGE(fileContentsWrite == fileContentsRead, "File contents are not equal");
}

BOOST_AUTO_TEST_CASE(WMIFSReadFile_Fail_FileContents)
{
    // Arrange
    COMWrapper ICom;
    WMIConnection IWmi(ICom);
    ClassFactory IFactory(ICom, IWmi);
    WMIFSInterface IWMIFS(ICom, IWmi, IFactory);
    Railfence ciph;
    HRESULT hres;
    std::wstring contents = L"";
    
    // Act

    hres = IWMIFS.ReadFile(L"Tes423423t", L"Te$@$@$@st", contents);


    // Assert
    BOOST_CHECK(FAILED(hres));
}

BOOST_AUTO_TEST_CASE(WMIFSReadFile_Fail_FileDrive)
{
    // Arrange
    COMWrapper ICom;
    WMIConnection IWmi(ICom);
    ClassFactory IFactory(ICom, IWmi);
    WMIFSInterface IWMIFS(ICom, IWmi, IFactory);
    Railfence ciph;
    HRESULT hres;
    std::wstring fileContents;
    
    // Act

    hres = IWMIFS.ReadFile(L"Tes123t$@#$@@#$", L"Test", encodedFileContentsRead);


    // Assert
    BOOST_CHECK(FAILED(hres));
}

BOOST_AUTO_TEST_CASE(WMIFSDeleteDrive_Success)
{
	// Arrange
	COMWrapper ICom;
	WMIConnection IWmi(ICom);
	ClassFactory IFactory(ICom, IWmi);
	WMIFSInterface IWMIFS(ICom, IWmi, IFactory);
	HRESULT hres;
    std::wstring fileContents = L"";
    

	// Act
    hres = IWMIFS.CreateDrive(L"Test", &fileContents);
    
    // Assert
	BOOST_CHECK(SUCCEEDED(hres));
    
    // Act
	hres = IWMIFS.DeleteDrive(L"Test");

	// Assert
    BOOST_CHECK(SUCCEEDED(hres));
}

BOOST_AUTO_TEST_CASE(WMIFSDeleteDrive_Fail)
{
	// Arrange
	COMWrapper ICom;
	WMIConnection IWmi(ICom);
	ClassFactory IFactory(ICom, IWmi);
	WMIFSInterface IWMIFS(ICom, IWmi, IFactory);
	HRESULT hres;

	// Act
	hres = IWMIFS.DeleteDrive(L"Te31223123123st");

	// Assert
	BOOST_CHECK(FAILED(hres));
}

BOOST_AUTO_TEST_SUITE_END(WMIFS_Suite)
