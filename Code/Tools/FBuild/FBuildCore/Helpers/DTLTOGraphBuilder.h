// DTLTOGraphBuilder - Synthesize a NodeGraph from parsed LLVM DTLTO data
//------------------------------------------------------------------------------
#pragma once

// Includes
//------------------------------------------------------------------------------
#include "Tools/FBuild/FBuildCore/Helpers/DTLTOData.h"

#include "Core/Containers/Array.h"
#include "Core/Strings/AString.h"

// Forward Declarations
//------------------------------------------------------------------------------
class CompilerNode;
class Node;
class NodeGraph;

// DTLTOGraphBuilder
//------------------------------------------------------------------------------
class DTLTOGraphBuilder
{
public:
    explicit DTLTOGraphBuilder( NodeGraph & nodeGraph );

    DTLTOGraphBuilder( const DTLTOGraphBuilder & other ) = delete;
    DTLTOGraphBuilder & operator=( const DTLTOGraphBuilder & other ) = delete;

    // One ObjectList per job, grouped under an Alias.
    Node * BuildGraph( const DTLTOData & data, const AString & aliasName );

private:
    CompilerNode * CreateCompilerNode( const AString & compilerExe );

    NodeGraph & m_NodeGraph;
};

//------------------------------------------------------------------------------
