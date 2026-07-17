// DTLTOGraphBuilder
//------------------------------------------------------------------------------

// Includes
//------------------------------------------------------------------------------
#include "DTLTOGraphBuilder.h"

// FBuildCore
#include "Tools/FBuild/FBuildCore/BFF/Functions/Function.h"
#include "Tools/FBuild/FBuildCore/FLog.h"
#include "Tools/FBuild/FBuildCore/Graph/AliasNode.h"
#include "Tools/FBuild/FBuildCore/Graph/CompilerNode.h"
#include "Tools/FBuild/FBuildCore/Graph/Node.h"
#include "Tools/FBuild/FBuildCore/Graph/NodeGraph.h"
#include "Tools/FBuild/FBuildCore/Graph/ObjectListNode.h"

// Core
#include "Core/Env/Assert.h"
#include "Core/FileIO/PathUtils.h"
#include "Core/Reflection/ReflectedProperty.h"
#include "Core/Reflection/ReflectionInfo.h"
#include "Core/Strings/AStackString.h"

// CONSTRUCTOR
//------------------------------------------------------------------------------
DTLTOGraphBuilder::DTLTOGraphBuilder( NodeGraph & nodeGraph )
    : m_NodeGraph( nodeGraph )
{
}

// BuildGraph
//------------------------------------------------------------------------------
Node * DTLTOGraphBuilder::BuildGraph( const DTLTOData & data, const AString & aliasName )
{
    if ( data.m_CommonArgs.IsEmpty() ) // m_CommonArgs[0] is the compiler
    {
        FLOG_ERROR( "DTLTO: no compiler specified" );
        return nullptr;
    }
    if ( data.m_Jobs.IsEmpty() )
    {
        FLOG_ERROR( "DTLTO: no jobs to build" );
        return nullptr;
    }
    if ( m_NodeGraph.FindNode( aliasName ) )
    {
        FLOG_ERROR( "DTLTO: target '%s' already exists", aliasName.Get() );
        return nullptr;
    }

    CompilerNode * compiler = CreateCompilerNode( data.m_CommonArgs[ 0 ] );
    if ( compiler == nullptr )
    {
        return nullptr; // CreateCompilerNode will have emitted an error
    }

    StackArray<Node *> jobNodes;
    for ( const DTLTOData::Job & job : data.m_Jobs )
    {
        Node * objectList = CreateObjectListForJob( data, job, compiler );
        if ( objectList == nullptr )
        {
            return nullptr; // CreateObjectListForJob will have emitted an error
        }
        jobNodes.Append( objectList );
    }

    // group all jobs under a single root to build
    AliasNode * root = m_NodeGraph.CreateNode<AliasNode>( aliasName );

    const ReflectedProperty * targetsProp =
        root->GetReflectionInfoV()->GetReflectedProperty( AStackString( "Targets" ) );
    ASSERT( targetsProp );
    targetsProp->GetPtrToArray<Node *>( root )->Append( jobNodes );

    if ( !root->Initialize( m_NodeGraph, nullptr, Function::Find( AStackString( "Alias" ) ) ) )
    {
        return nullptr;
    }

    return root;
}

// CreateCompilerNode
//------------------------------------------------------------------------------
CompilerNode * DTLTOGraphBuilder::CreateCompilerNode( const AString & compilerExe )
{
    const AStackString compilerName( "Compiler-DTLTO" );
    if ( Node * existing = m_NodeGraph.FindNode( compilerName ) )
    {
        return existing->CastTo<CompilerNode>();
    }

    CompilerNode * compiler = m_NodeGraph.CreateNode<CompilerNode>( compilerName );
    const ReflectionInfo * ri = compiler->GetReflectionInfoV();
    VERIFY( ri->SetProperty( compiler, "Executable", compilerExe ) );
    VERIFY( ri->SetProperty( compiler, "CompilerFamily", AStackString( "custom" ) ) ); // no host-side -E on bitcode
    VERIFY( ri->SetProperty( compiler, "SimpleDistributionMode", true ) );
    VERIFY( ri->SetProperty( compiler, "AllowDistribution", true ) );

    if ( !compiler->Initialize( m_NodeGraph, nullptr, Function::Find( AStackString( "Compiler" ) ) ) )
    {
        return nullptr;
    }
    return compiler;
}

// CreateObjectListForJob
//------------------------------------------------------------------------------
Node * DTLTOGraphBuilder::CreateObjectListForJob( const DTLTOData & data,
                                                  const DTLTOData::Job & job,
                                                  CompilerNode * compiler )
{


    AString inputPath;
    if ( job.m_Args.IsEmpty() == false )
    {
        inputPath = job.m_Args[ 0 ]; //  job.m_Args[ 0 ] is the input bitcode (???)
    }

    // ObjectListNode * objectList = m_NodeGraph.CreateNode<ObjectListNode>( listName );
    // const ReflectionInfo * ri = objectList->GetReflectionInfoV();
    // VERIFY( ri->SetProperty( objectList, "Compiler", compiler->GetName() ) );
    // VERIFY( ri->SetProperty( objectList, "CompilerOptions", compilerOptions ) );
    // VERIFY( ri->SetProperty( objectList, "CompilerOutputPath", outputDir ) );
    // VERIFY( ri->SetProperty( objectList, "AllowDistribution", true ) );

    return nullptr;
}
