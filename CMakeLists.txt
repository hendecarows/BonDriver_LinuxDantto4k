cmake_minimum_required(VERSION 3.16)

# Project name
project(BonDriver_LinuxDantto4k LANGUAGES CXX)

# System Information (for debug)
message(STATUS "System Information")
message(STATUS "System = ${CMAKE_SYSTEM_NAME}")
message(STATUS "C Compiler = ${CMAKE_C_COMPILER_ID}")
message(STATUS "C++ Compiler = ${CMAKE_CXX_COMPILER_ID}")

# Build Type Configuration
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
	message(STATUS "Setting build type to 'Release' as none was specified.")
	set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build." FORCE)
	set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release")
endif()

# Other Library
set(THREADS_PREFER_PTHREAD_FLAG ON)

include(FetchContent)

FetchContent_Declare(
	plog
	GIT_REPOSITORY https://github.com/SergiusTheBest/plog
	GIT_TAG        1.1.11
	GIT_SHALLOW    true
)
FetchContent_MakeAvailable(plog)

FetchContent_Declare(
	dantto4k
	GIT_REPOSITORY https://github.com/nekohkr/dantto4k
	GIT_TAG        v1.0.1
)
FetchContent_MakeAvailable(dantto4k)

find_package(Threads REQUIRED)
find_package(PkgConfig REQUIRED)

pkg_check_modules(TSDUCK REQUIRED IMPORTED_TARGET tsduck)
pkg_check_modules(PCSCLITE REQUIRED IMPORTED_TARGET libpcsclite)

# library
set(DANTTO4K_DIR build/_deps/dantto4k-src)
add_library(
	${PROJECT_NAME}
	SHARED
	src/BonDriver_LinuxDantto4k.cpp
	${DANTTO4K_DIR}/src/acasCard.cpp
	${DANTTO4K_DIR}/src/acasHandler.cpp
	${DANTTO4K_DIR}/src/accessControlDescriptor.cpp
	${DANTTO4K_DIR}/src/adtsConverter.cpp
	${DANTTO4K_DIR}/src/aes.cpp
	${DANTTO4K_DIR}/src/aribEncoder.cpp
	${DANTTO4K_DIR}/src/aribUtil.cpp
	${DANTTO4K_DIR}/src/b24Color.cpp
	${DANTTO4K_DIR}/src/b24SubtitleConvertor.cpp
	${DANTTO4K_DIR}/src/caMessage.cpp
	${DANTTO4K_DIR}/src/casProxyClient.cpp
	${DANTTO4K_DIR}/src/compressedIPPacket.cpp
	${DANTTO4K_DIR}/src/config.cpp
	${DANTTO4K_DIR}/src/contentCopyControlDescriptor.cpp
	${DANTTO4K_DIR}/src/damt.cpp
	${DANTTO4K_DIR}/src/dantto4k.cpp
	${DANTTO4K_DIR}/src/dataTransmissionMessage.cpp
	${DANTTO4K_DIR}/src/dataUnit.cpp
	${DANTTO4K_DIR}/src/dcct.cpp
	${DANTTO4K_DIR}/src/ddmt.cpp
	${DANTTO4K_DIR}/src/ecm.cpp
	${DANTTO4K_DIR}/src/emt.cpp
	${DANTTO4K_DIR}/src/eventPackageDescriptor.cpp
	${DANTTO4K_DIR}/src/extensionHeaderScrambling.cpp
	${DANTTO4K_DIR}/src/fragmentAssembler.cpp
	${DANTTO4K_DIR}/src/indexItem.cpp
	${DANTTO4K_DIR}/src/ipv6.cpp
	${DANTTO4K_DIR}/src/m2SectionMessage.cpp
	${DANTTO4K_DIR}/src/m2ShortSectionMessage.cpp
	${DANTTO4K_DIR}/src/mhAit.cpp
	${DANTTO4K_DIR}/src/mhApplicationBoundaryAndPermissionDescriptor.cpp
	${DANTTO4K_DIR}/src/mhApplicationDescriptor.cpp
	${DANTTO4K_DIR}/src/mhAudioComponentDescriptor.cpp
	${DANTTO4K_DIR}/src/mhAutostartPriorityDescriptor.cpp
	${DANTTO4K_DIR}/src/mhBit.cpp
	${DANTTO4K_DIR}/src/mhBroadcasterNameDescriptor.cpp
	${DANTTO4K_DIR}/src/mhCaContractInformation.cpp
	${DANTTO4K_DIR}/src/mhCacheControlInfoDescriptor.cpp
	${DANTTO4K_DIR}/src/mhCdt.cpp
	${DANTTO4K_DIR}/src/mhContentDescriptor.cpp
	${DANTTO4K_DIR}/src/mhDataComponentDescriptor.cpp
	${DANTTO4K_DIR}/src/mhEit.cpp
	${DANTTO4K_DIR}/src/mhEventGroupDescriptor.cpp
	${DANTTO4K_DIR}/src/mhExtendedEventDescriptor.cpp
	${DANTTO4K_DIR}/src/mhExternalApplicationControlDescriptor.cpp
	${DANTTO4K_DIR}/src/mhLinkageDescriptor.cpp
	${DANTTO4K_DIR}/src/mhLogoTransmissionDescriptor.cpp
	${DANTTO4K_DIR}/src/mhParentalRatingDescriptor.cpp
	${DANTTO4K_DIR}/src/mhRandomizedLatencyDescriptor.cpp
	${DANTTO4K_DIR}/src/mhSdt.cpp
	${DANTTO4K_DIR}/src/mhSeriesDescriptor.cpp
	${DANTTO4K_DIR}/src/mhServiceDescriptor.cpp
	${DANTTO4K_DIR}/src/mhServiceListDescriptor.cpp
	${DANTTO4K_DIR}/src/mhShortEventDescriptor.cpp
	${DANTTO4K_DIR}/src/mhSiParameterDescriptor.cpp
	${DANTTO4K_DIR}/src/mhSimpleApplicationLocationDescriptor.cpp
	${DANTTO4K_DIR}/src/mhStreamIdentificationDescriptor.cpp
	${DANTTO4K_DIR}/src/mhTot.cpp
	${DANTTO4K_DIR}/src/mhTransportProtocolDescriptor.cpp
	${DANTTO4K_DIR}/src/mmtDescriptorFactory.cpp
	${DANTTO4K_DIR}/src/mmtDescriptors.cpp
	${DANTTO4K_DIR}/src/mmtGeneralLocationInfo.cpp
	${DANTTO4K_DIR}/src/mmtStream.cpp
	${DANTTO4K_DIR}/src/mmtTableFactory.cpp
	${DANTTO4K_DIR}/src/mmtTlvDemuxer.cpp
	${DANTTO4K_DIR}/src/mmtp.cpp
	${DANTTO4K_DIR}/src/mpt.cpp
	${DANTTO4K_DIR}/src/mpu.cpp
	${DANTTO4K_DIR}/src/mpuApplicationProcessor.cpp
	${DANTTO4K_DIR}/src/mpuAudioProcessor.cpp
	${DANTTO4K_DIR}/src/mpuExtendedTimestampDescriptor.cpp
	${DANTTO4K_DIR}/src/mpuProcessorFactory.cpp
	${DANTTO4K_DIR}/src/mpuSubtitleProcessor.cpp
	${DANTTO4K_DIR}/src/mpuTimestampDescriptor.cpp
	${DANTTO4K_DIR}/src/mpuVideoProcessor.cpp
	${DANTTO4K_DIR}/src/multimediaServiceInformationDescriptor.cpp
	${DANTTO4K_DIR}/src/networkNameDescriptor.cpp
	${DANTTO4K_DIR}/src/nit.cpp
	${DANTTO4K_DIR}/src/ntp.cpp
	${DANTTO4K_DIR}/src/paMessage.cpp
	${DANTTO4K_DIR}/src/pesPacket.cpp
	${DANTTO4K_DIR}/src/plt.cpp
	${DANTTO4K_DIR}/src/pugixml.cpp
	${DANTTO4K_DIR}/src/relatedBroadcasterDescriptor.cpp
	${DANTTO4K_DIR}/src/remoteControlKeyDescriptor.cpp
	${DANTTO4K_DIR}/src/remuxerHandler.cpp
	${DANTTO4K_DIR}/src/serviceListDescriptor.cpp
	${DANTTO4K_DIR}/src/signalingMessage.cpp
	${DANTTO4K_DIR}/src/smartCard.cpp
	${DANTTO4K_DIR}/src/stream.cpp
	${DANTTO4K_DIR}/src/systemManagementDescriptor.cpp
	${DANTTO4K_DIR}/src/timeUtil.cpp
	${DANTTO4K_DIR}/src/timebase.cpp
	${DANTTO4K_DIR}/src/tlv.cpp
	${DANTTO4K_DIR}/src/tlvDescriptorFactory.cpp
	${DANTTO4K_DIR}/src/tlvDescriptors.cpp
	${DANTTO4K_DIR}/src/tlvTableFactory.cpp
	${DANTTO4K_DIR}/src/transmissionControlSignal.cpp
	${DANTTO4K_DIR}/src/ttml.cpp
	${DANTTO4K_DIR}/src/videoComponentDescriptor.cpp
)

# Standards
# Output Name (Remove "lib" prefix)
set_target_properties(
	${PROJECT_NAME}
	PROPERTIES
	CXX_STANDARD 20
	CXX_STANDARD_REQUIRED ON
	CXX_EXTENSIONS OFF
	PREFIX ""
)

# Include Directory
target_include_directories(
	${PROJECT_NAME}
	PRIVATE
	include
	src
	${DANTTO4K_DIR}/src
	${DANTTO4K_DIR}/thirdparty/asio/asio/include
)

# Compile Options
target_compile_options(
	${PROJECT_NAME}
	PRIVATE 
	-Wall 
	-maes 
	-msse4.1
)

# Link Library
target_link_libraries(
	${PROJECT_NAME}
	PRIVATE
	Threads::Threads
	plog::plog
	PkgConfig::TSDUCK
	PkgConfig::PCSCLITE
)
