cmake_minimum_required(VERSION 3.18)
project(OpCoDe VERSION 1.3.2 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)

set(op_sources
    OpCoDe/Ice/IceAABB.cpp
    OpCoDe/Ice/IceContainer.cpp
    OpCoDe/Ice/IceHPoint.cpp
    OpCoDe/Ice/IceIndexedTriangle.cpp
    OpCoDe/Ice/IceMatrix3x3.cpp
    OpCoDe/Ice/IceMatrix4x4.cpp
    OpCoDe/Ice/IceOBB.cpp
    OpCoDe/Ice/IcePlane.cpp
    OpCoDe/Ice/IcePoint.cpp
    OpCoDe/Ice/IceRandom.cpp
    OpCoDe/Ice/IceRay.cpp
    OpCoDe/Ice/IceRevisitedRadix.cpp
    OpCoDe/Ice/IceSegment.cpp
    OpCoDe/Ice/IceTriangle.cpp
    OpCoDe/Ice/IceUtils.cpp
    OpCoDe/Opcode.cpp
    OpCoDe/OPC_AABBCollider.cpp
    OpCoDe/OPC_AABBTree.cpp
    OpCoDe/OPC_BaseModel.cpp
    OpCoDe/OPC_BoxPruning.cpp
    OpCoDe/OPC_Collider.cpp
    OpCoDe/OPC_Common.cpp
    OpCoDe/OPC_HybridModel.cpp
    OpCoDe/OPC_LSSCollider.cpp
    OpCoDe/OPC_MeshInterface.cpp
    OpCoDe/OPC_Model.cpp
    OpCoDe/OPC_OBBCollider.cpp
    OpCoDe/OPC_OptimizedTree.cpp
    OpCoDe/OPC_Picking.cpp
    OpCoDe/OPC_PlanesCollider.cpp
    OpCoDe/OPC_RayCollider.cpp
    OpCoDe/OPC_SphereCollider.cpp
    OpCoDe/OPC_SweepAndPrune.cpp
    OpCoDe/OPC_TreeBuilders.cpp
    OpCoDe/OPC_TreeCollider.cpp
    OpCoDe/OPC_VolumeCollider.cpp
    OpCoDe/StdAfx.cpp
)

if (NOT DEFINED BUILD_SHARED_LIBS)
    set(BUILD_SHARED_LIBS ON)
endif()

if (BUILD_SHARED_LIBS)
    add_library(OpCoDe SHARED ${op_sources})
else()
    add_library(OpCoDe STATIC ${op_sources})
endif()

target_include_directories(OpCoDe PRIVATE "${CMAKE_SOURCE_DIR}/OpCoDe")
target_precompile_headers(OpCoDe PRIVATE OpCoDe/Stdafx.h)

set_target_properties(OpCoDe PROPERTIES PREFIX "" POSITION_INDEPENDENT_CODE ON)
target_compile_definitions(OpCoDe PRIVATE _ALLOW_KEYWORD_MACROS OPCODE_EXPORTS)
target_compile_options(OpCoDe PRIVATE /fp:precise)

if (NOT BUILD_SHARED_LIBS)
    target_compile_definitions(OpCoDe PRIVATE ICE_NO_DLL)
endif()

target_compile_options(OpCoDe PRIVATE "$<$<CONFIG:Release>:/Zi>")
target_link_options(OpCoDe PRIVATE "$<$<CONFIG:Release>:/DEBUG>")
target_link_options(OpCoDe PRIVATE "$<$<CONFIG:Release>:/OPT:REF>")
target_link_options(OpCoDe PRIVATE "$<$<CONFIG:Release>:/OPT:ICF>")
