// SPDX-License-Identifier: MIT
/*
 * BonDriver for dantto4k (BonDriver_LinuxDantto4k.cpp)
 *
 */

#include "BonDriver_LinuxDantto4k.hpp"

#include <stdint.h>
#include <unistd.h>
#include <dlfcn.h>

#include <cassert>
#include <cstddef>
#include <cstring>
#include <format>
#include <stdexcept>
#include <string>
#include <utility>

#include <plog/Log.h>
#include <plog/Initializers/ConsoleInitializer.h>
#include <plog/Formatters/FuncMessageFormatter.h>

#include "acasHandler.h"
#include "runtime_error.hpp"

namespace BonDriver_LinuxDantto4k {

BonDriver::BonDriver(config::Config& config)
{
	try {
		// INIファイルの読み込み
		// [bondriver]
		auto sct_bon = config.Get("bondriver");
		auto loglevel = sct_bon.GetInt("logLevel", 3);
		if (!plog::get()) {
			plog::init<plog::FuncMessageFormatter>(static_cast<plog::Severity>(loglevel), plog::streamStdErr);
			PLOGD << "plog init = " << plog::get()->getInstance();
		} else {
			plog::get()->setMaxSeverity(static_cast<plog::Severity>(loglevel));
		}
		current_path_ = sct_bon.Get("currentPath");
		bondriver_path_ = sct_bon.Get("bondriverPath");
		mmts_to_ts_channel_spaces_ = sct_bon.GetUIntValues("mmtsToTsChannelSpace");

		PLOGD << "logLevel = " << loglevel;
		PLOGD << "currentPath = " << current_path_;
		PLOGD << "bondriverPath = " << bondriver_path_;
		PLOGD << "mmtsToTsChannelSpace = " << mmts_to_ts_channel_spaces_;

		if (bondriver_path_.is_relative()) {
			bondriver_path_ = 	std::filesystem::canonical(current_path_ / bondriver_path_);
		}
		PLOGD << "absolute bondriverPath = " << bondriver_path_;

		// [acas]
		std::string cas_proxy_server;
		std::string smart_card_reader_name;
		if (config.Exists("acas")) {
			auto sct_acas = config.Get("acas");
			cas_proxy_server = sct_acas.GetStr("casProxyServer", "");
			smart_card_reader_name = sct_acas.GetStr("smartCardReaderName", "");
		}
		PLOGD << "casProxyServer = " << cas_proxy_server;
		PLOGD << "smartCardReaderName = " << smart_card_reader_name;

		std::unique_ptr<AcasHandler> acas_handler = std::make_unique<AcasHandler>();
		std::unique_ptr<ISmartCard> smart_card;

		if (cas_proxy_server.empty()) {
			smart_card = std::make_unique<LocalSmartCard>();
		} else {
			auto parsed = casproxy::parseAddress(cas_proxy_server);
			if (!parsed) {
				throw RuntimeError(std::format("Invalid CasProxyServer address = {}", cas_proxy_server));;
			}
			smart_card = std::make_unique<RemoteSmartCard>(parsed->first, parsed->second);
		}

		smart_card->setSmartCardReaderName(smart_card_reader_name);
		acas_handler->setSmartCard(std::move(smart_card));
		mmtlv_demuxer_.setCasHandler(std::move(acas_handler));

		// [audio]
		if (config.Exists("audio")) {
			auto sct_audio = config.Get("audio");
			disable_adts_conversion_ = sct_audio.GetBool("disableADTSConversion", false);
		}
		PLOGD << "disableADTSConversion = " << disable_adts_conversion_;
	} catch(const std::exception& e) {
		PLOGE << e.what();
		throw;
	}

	try {
		// BonDriverのロード
		bondriver_module_ = dlopen(bondriver_path_.string().c_str(), RTLD_LAZY);
		auto errmsg = dlerror();
		if (!bondriver_module_) {
			throw RuntimeError(std::format("dlopen error {}: {}", bondriver_path_.string(), errmsg));
		}

		auto create_bondriver = reinterpret_cast<IBonDriver * (*)()>(dlsym(bondriver_module_, "CreateBonDriver"));
		errmsg = dlerror();
		if (errmsg) {
			throw RuntimeError(std::format("dlsym error {}: {}", bondriver_path_.string(), errmsg));
		}

		bondriver_ = create_bondriver();
		if (!bondriver_) {
			throw RuntimeError(
				std::format("CreateBonDriver error IBonDriver = {}", static_cast<void*>(bondriver_))
			);
		}

		bondriver2_ = dynamic_cast<IBonDriver2*>(bondriver_);
		if (!bondriver2_) {
			throw RuntimeError(
				std::format("CreateBonDriver error IBonDriver = {} IBonDriver2 = {}", static_cast<void*>(bondriver_), static_cast<void*>(bondriver2_))
			);
		}
	} catch(const RuntimeError& e) {
		PLOGE << e.what();
		if (bondriver_module_) {
			dlclose(bondriver_module_);
			bondriver_module_ = nullptr;
		}
		throw;
	}

	remuxer_handler_.setOutputCallback(
		[this](const uint8_t* data, size_t size) {
			assert(size == 188);
			remux_output_.insert(remux_output_.end(), data, data + size);
		}
	);
	mmtlv_demuxer_.setDemuxerHandler(remuxer_handler_);
}

BonDriver::~BonDriver()
{
	CloseTuner();
}

const bool BonDriver::OpenTuner(void)
{
	PLOGV << __func__;
	std::lock_guard<std::mutex> lock(mtx_);

	return bondriver2_->OpenTuner();
}

void BonDriver::CloseTuner(void)
{
	PLOGV << __func__;
	std::lock_guard<std::mutex> lock(mtx_);

	if (bondriver2_) {
		PLOGD << "bondriver2_" << bondriver2_;
		bondriver2_->CloseTuner();
		bondriver2_ = nullptr;
		bondriver_ = nullptr;
	}

	if (bondriver_module_) {
		PLOGD << "bondriver_module_" << bondriver_module_;
		dlclose(bondriver_module_);
		bondriver_module_ = nullptr;
	}
}

const bool BonDriver::SetChannel(const uint8_t bCh)
{
	return SetChannel(0, bCh);
}

const float BonDriver::GetSignalLevel(void)
{
	return bondriver2_->GetSignalLevel();
}

const uint32_t BonDriver::WaitTsStream(const uint32_t dwTimeOut)
{
	return bondriver2_->WaitTsStream(dwTimeOut);
}

const uint32_t BonDriver::GetReadyCount(void)
{
	return bondriver2_->GetReadyCount();
}

const bool BonDriver::GetTsStream(uint8_t *pDst, uint32_t *pdwSize, uint32_t *pdwRemain)
{
	if (!is_mmts_to_ts_enabled_) {
		return bondriver2_->GetTsStream(pDst, pdwSize, pdwRemain);
	}

	uint8_t *pSrc = nullptr;
	auto code = GetTsStream(&pSrc, pdwSize, pdwRemain);
	if (*pdwSize) {
		std::memcpy(pDst, pSrc, *pdwSize);
	}

	return code;
}

const bool BonDriver::GetTsStream(uint8_t **ppDst, uint32_t *pdwSize, uint32_t *pdwRemain)
{
	std::lock_guard<std::mutex> lock(mtx_);

	if (!is_mmts_to_ts_enabled_) {
		return bondriver2_->GetTsStream(ppDst, pdwSize, pdwRemain);
	}

	auto code = false;
	auto ppdst = ppDst;
	auto psize = pdwSize;
	auto premain = pdwRemain;

	do {
		code = bondriver2_->GetTsStream(ppdst, psize, premain);
		if (code) {
			input_buffer_.insert(input_buffer_.end(), *ppdst, *ppdst + *psize);
		}
	} while(code && *premain != 0);

	MmtTlv::Common::ReadStream input(input_buffer_);
	while (!input.isEof()) {
		MmtTlv::DemuxStatus status = mmtlv_demuxer_.demux(input);

		if (status == MmtTlv::DemuxStatus::NotEnoughBuffer) {
			break;
		}
	}

	input_buffer_.erase(input_buffer_.begin(), input_buffer_.begin() + (input_buffer_.size() - input.leftBytes()));
	if (remux_output_.size() < 188 * 1024) {
		return false;
	}
	output_buffer_ = std::move(remux_output_);

	*ppDst = output_buffer_.data();
	*pdwSize = static_cast<uint32_t>(output_buffer_.size());
	*pdwRemain = 0;

	return true;
}

void BonDriver::PurgeTsStream(void)
{
	PLOGV << __func__;
	std::lock_guard<std::mutex> lock(mtx_);

	input_buffer_.clear();
	remux_output_.clear();
	mmtlv_demuxer_.clear();

	bondriver2_->PurgeTsStream();
}

void BonDriver::Release(void)
{
	PLOGV << __func__;
	DestroyInstance();
}

const char16_t *BonDriver::GetTunerName(void)
{
	return bondriver2_->GetTunerName();
}

const bool BonDriver::IsTunerOpening(void)
{
	std::lock_guard<std::mutex> lock(mtx_);

	return bondriver2_->IsTunerOpening();
}

const char16_t *BonDriver::EnumTuningSpace(const uint32_t dwSpace)
{
	return bondriver2_->EnumTuningSpace(dwSpace);
}

const char16_t *BonDriver::EnumChannelName(const uint32_t dwSpace, const uint32_t dwChannel)
{
	return bondriver2_->EnumChannelName(dwSpace, dwChannel);
}

const bool BonDriver::SetChannel(const uint32_t dwSpace, const uint32_t dwChannel)
{
	PLOGV << __func__;
	PLOGD << "dwSpace = " << dwSpace << " dwChannel = " << dwChannel;
	std::lock_guard<std::mutex> lock(mtx_);

	input_buffer_.clear();
	remux_output_.clear();
	mmtlv_demuxer_.clear();

	auto code = bondriver2_->SetChannel(dwSpace, dwChannel);
	if (!code) {
		PLOGI << "failed to set channel dwSpace = " << dwSpace << " dwChannel = " << dwChannel;;
		return code;
	}

	const auto& spaces = mmts_to_ts_channel_spaces_;
	if (spaces.empty()) {
		PLOGD << "no channel space";
		is_mmts_to_ts_enabled_ = true;
	} else if (std::ranges::find(spaces, dwSpace) != spaces.end()) {
		PLOGD << "found channel space = " << dwSpace << " in spaces = " << spaces;
		is_mmts_to_ts_enabled_ = true;
	} else {
		PLOGD << "not found channel space = " << dwSpace << " in spaces = " << spaces;
		is_mmts_to_ts_enabled_ = false;
	}
	PLOGD << "MMTS to TS convert = " << is_mmts_to_ts_enabled_;

	return code;
}

const uint32_t BonDriver::GetCurSpace(void)
{
	return bondriver2_-> GetCurSpace();
}

const uint32_t BonDriver::GetCurChannel(void)
{
	return bondriver2_-> GetCurChannel();
}

std::mutex BonDriver::instance_mtx_;
BonDriver *BonDriver::instance_ = nullptr;

BonDriver * BonDriver::GetInstance()
{
	std::lock_guard<std::mutex> lock(instance_mtx_);

	if (!instance_) {
		try {
			Dl_info dli;
			if (!::dladdr(reinterpret_cast<void *>(GetInstance), &dli)) {
				return nullptr;
			}

			std::filesystem::path p = dli.dli_fname;
			if (p.stem().empty() || p.extension() != ".so") {
				return nullptr;
			}

			// 以下の順で読み込む
			// (1)BonDriver_LinuxDantto4k.ini
			// (2)BonDriver_LinuxDantto4k.so.ini
			config::Config config;
			if (!config.Load(p.replace_extension(".ini"))) {
				if (!config.Load(p.replace_extension(".so.ini"))) {
					return nullptr;
				}
			}

			// [bondriver]セクションのcurrentPathキーにBonDriver_LinuxDantto4k.soのパスを追加
			config.Set("bondriver", "currentPath", p.parent_path().string());

			instance_ = new BonDriver(config);
		} catch (...) {
			return nullptr;
		}
	}

	return instance_;
}

void BonDriver::DestroyInstance()
{
	std::lock_guard<std::mutex> lock(instance_mtx_);

	if (instance_) {
		delete instance_;
		instance_ = nullptr;
	}

	return;
}

extern "C" IBonDriver * CreateBonDriver()
{
	return BonDriver::GetInstance();
}

} // namespace BonDriver_LinuxDantto4k
