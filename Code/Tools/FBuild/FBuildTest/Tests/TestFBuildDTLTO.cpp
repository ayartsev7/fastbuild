// TestFBuildDTLTO.cpp
//------------------------------------------------------------------------------

// Includes
//------------------------------------------------------------------------------
#include "FBuildTest.h"

// TestFBuildDTLTO
//------------------------------------------------------------------------------
TEST_GROUP( TestFBuildDTLTO, FBuildTest )
{
};

//------------------------------------------------------------------------------
TEST_CASE( TestFBuildDTLTO, InitializeFromDTLTO )
{
    const char * testDir = "../tmp/Test/FBuildDTLTO/";
    const char * jsonFile = "../tmp/Test/FBuildDTLTO/dist-file.json";
    EnsureDirExists( testDir );
    MakeFile( jsonFile, R"({
        "common": {
            "linker_output": "app.exe",
            "args": [ "clang.exe" ],
            "inputs": []
        },
        "jobs": [
            {
                "args": [ "input.obj" ],
                "inputs": [ "input.obj" ],
                "outputs": [ "input.1.native.o" ]
            }
        ]
    })" );

    FBuildTestOptions options;
    options.m_DTLTOFile = jsonFile;
    options.m_SaveDBOnCompletion = true;

    FBuildForTest fBuild( options );
    TEST_ASSERT( fBuild.Initialize() );

    const Node * all = fBuild.GetNode( "all" );
    TEST_ASSERT( all );
    TEST_ASSERT( all->GetType() == Node::ALIAS_NODE );
    TEST_ASSERT( all->GetStaticDependencies().GetSize() == 1 );
    TEST_ASSERT( fBuild.GetOptions().m_SaveDBOnCompletion == false );
}

//------------------------------------------------------------------------------
