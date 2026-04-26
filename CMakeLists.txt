cmake_minimum_required(VERSION 3.10)

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
add_subdirectory(thirdparty/plog)

# Package
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
find_package(PkgConfig REQUIRED)

pkg_check_modules(TSDUCK REQUIRED IMPORTED_TARGET tsduck)
pkg_check_modules(PCSCLITE REQUIRED IMPORTED_TARGET libpcsclite)

# library
add_library(
	${PROJECT_NAME}
	SHARED
	src/BonDriver_LinuxDantto4k.cpp
	thirdparty/dantto4k/src/acasCard.cpp
	thirdparty/dantto4k/src/acasHandler.cpp
	thirdparty/dantto4k/src/accessControlDescriptor.cpp
	thirdparty/dantto4k/src/adtsConverter.cpp
	thirdparty/dantto4k/src/aes.cpp
	thirdparty/dantto4k/src/aribEncoder.cpp
	thirdparty/dantto4k/src/aribUtil.cpp
	thirdparty/dantto4k/src/b24Color.cpp
	thirdparty/dantto4k/src/b24SubtitleConvertor.cpp
	thirdparty/dantto4k/src/caMessage.cpp
	thirdparty/dantto4k/src/casProxyClient.cpp
	thirdparty/dantto4k/src/compressedIPPacket.cpp
	thirdparty/dantto4k/src/config.cpp
	thirdparty/dantto4k/src/contentCopyControlDescriptor.cpp
	thirdparty/dantto4k/src/damt.cpp
	thirdparty/dantto4k/src/dantto4k.cpp
	thirdparty/dantto4k/src/dataTransmissionMessage.cpp
	thirdparty/dantto4k/src/dataUnit.cpp
	thirdparty/dantto4k/src/dcct.cpp
	thirdparty/dantto4k/src/ddmt.cpp
	thirdparty/dantto4k/src/ecm.cpp
	thirdparty/dantto4k/src/emt.cpp
	thirdparty/dantto4k/src/eventPackageDescriptor.cpp
	thirdparty/dantto4k/src/extensionHeaderScrambling.cpp
	thirdparty/dantto4k/src/fragmentAssembler.cpp
	thirdparty/dantto4k/src/indexItem.cpp
	thirdparty/dantto4k/src/ipv6.cpp
	thirdparty/dantto4k/src/m2SectionMessage.cpp
	thirdparty/dantto4k/src/m2ShortSectionMessage.cpp
	thirdparty/dantto4k/src/mhAit.cpp
	thirdparty/dantto4k/src/mhApplicationBoundaryAndPermissionDescriptor.cpp
	thirdparty/dantto4k/src/mhApplicationDescriptor.cpp
	thirdparty/dantto4k/src/mhAudioComponentDescriptor.cpp
	thirdparty/dantto4k/src/mhAutostartPriorityDescriptor.cpp
	thirdparty/dantto4k/src/mhBit.cpp
	thirdparty/dantto4k/src/mhBroadcasterNameDescriptor.cpp
	thirdparty/dantto4k/src/mhCaContractInformation.cpp
	thirdparty/dantto4k/src/mhCacheControlInfoDescriptor.cpp
	thirdparty/dantto4k/src/mhCdt.cpp
	thirdparty/dantto4k/src/mhContentDescriptor.cpp
	thirdparty/dantto4k/src/mhDataComponentDescriptor.cpp
	thirdparty/dantto4k/src/mhEit.cpp
	thirdparty/dantto4k/src/mhEventGroupDescriptor.cpp
	thirdparty/dantto4k/src/mhExtendedEventDescriptor.cpp
	thirdparty/dantto4k/src/mhExternalApplicationControlDescriptor.cpp
	thirdparty/dantto4k/src/mhLinkageDescriptor.cpp
	thirdparty/dantto4k/src/mhLogoTransmissionDescriptor.cpp
	thirdparty/dantto4k/src/mhParentalRatingDescriptor.cpp
	thirdparty/dantto4k/src/mhRandomizedLatencyDescriptor.cpp
	thirdparty/dantto4k/src/mhSdt.cpp
	thirdparty/dantto4k/src/mhSeriesDescriptor.cpp
	thirdparty/dantto4k/src/mhServiceDescriptor.cpp
	thirdparty/dantto4k/src/mhServiceListDescriptor.cpp
	thirdparty/dantto4k/src/mhShortEventDescriptor.cpp
	thirdparty/dantto4k/src/mhSiParameterDescriptor.cpp
	thirdparty/dantto4k/src/mhSimpleApplicationLocationDescriptor.cpp
	thirdparty/dantto4k/src/mhStreamIdentificationDescriptor.cpp
	thirdparty/dantto4k/src/mhTot.cpp
	thirdparty/dantto4k/src/mhTransportProtocolDescriptor.cpp
	thirdparty/dantto4k/src/mmtDescriptorFactory.cpp
	thirdparty/dantto4k/src/mmtDescriptors.cpp
	thirdparty/dantto4k/src/mmtGeneralLocationInfo.cpp
	thirdparty/dantto4k/src/mmtStream.cpp
	thirdparty/dantto4k/src/mmtTableFactory.cpp
	thirdparty/dantto4k/src/mmtTlvDemuxer.cpp
	thirdparty/dantto4k/src/mmtp.cpp
	thirdparty/dantto4k/src/mpt.cpp
	thirdparty/dantto4k/src/mpu.cpp
	thirdparty/dantto4k/src/mpuApplicationProcessor.cpp
	thirdparty/dantto4k/src/mpuAudioProcessor.cpp
	thirdparty/dantto4k/src/mpuExtendedTimestampDescriptor.cpp
	thirdparty/dantto4k/src/mpuProcessorFactory.cpp
	thirdparty/dantto4k/src/mpuSubtitleProcessor.cpp
	thirdparty/dantto4k/src/mpuTimestampDescriptor.cpp
	thirdparty/dantto4k/src/mpuVideoProcessor.cpp
	thirdparty/dantto4k/src/multimediaServiceInformationDescriptor.cpp
	thirdparty/dantto4k/src/networkNameDescriptor.cpp
	thirdparty/dantto4k/src/nit.cpp
	thirdparty/dantto4k/src/ntp.cpp
	thirdparty/dantto4k/src/paMessage.cpp
	thirdparty/dantto4k/src/pesPacket.cpp
	thirdparty/dantto4k/src/plt.cpp
	thirdparty/dantto4k/src/pugixml.cpp
	thirdparty/dantto4k/src/relatedBroadcasterDescriptor.cpp
	thirdparty/dantto4k/src/remoteControlKeyDescriptor.cpp
	thirdparty/dantto4k/src/remuxerHandler.cpp
	thirdparty/dantto4k/src/serviceListDescriptor.cpp
	thirdparty/dantto4k/src/signalingMessage.cpp
	thirdparty/dantto4k/src/smartCard.cpp
	thirdparty/dantto4k/src/stream.cpp
	thirdparty/dantto4k/src/systemManagementDescriptor.cpp
	thirdparty/dantto4k/src/timeUtil.cpp
	thirdparty/dantto4k/src/timebase.cpp
	thirdparty/dantto4k/src/tlv.cpp
	thirdparty/dantto4k/src/tlvDescriptorFactory.cpp
	thirdparty/dantto4k/src/tlvDescriptors.cpp
	thirdparty/dantto4k/src/tlvTableFactory.cpp
	thirdparty/dantto4k/src/transmissionControlSignal.cpp
	thirdparty/dantto4k/src/ttml.cpp
	thirdparty/dantto4k/src/videoComponentDescriptor.cpp
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
	thirdparty/dantto4k/src
	thirdparty/dantto4k/thirdparty/asio/asio/include
)

# Compile Options
target_compile_options(
	${PROJECT_NAME}
	PRIVATE 
	-Wall 
	-maes 
	-msse4.1
)

# target_compile_definitions(
# 	${PROJECT_NAME}
# 	PRIVATE
# 	PLOG_OMIT_LOG_DEFINES
# 	# PCSCLITE_NO_PCSC_TYPEDEFS
# )

# Link Library
target_link_libraries(
	${PROJECT_NAME}
	PRIVATE
	Threads::Threads
	plog::plog
	PkgConfig::TSDUCK
	PkgConfig::PCSCLITE
)
