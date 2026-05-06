// BonDriver_LinuxDantto4k.hpp

#pragma once

#include <stdint.h>

#include <algorithm>
#include <atomic>
#include <filesystem>
#include <mutex>
#include <string>
#include <vector>

#include "mmtTlvDemuxer.h"
#include "remuxerHandler.h"

#include "IBonDriver2.h"
#include "config.hpp"


namespace BonDriver_LinuxDantto4k {

class BonDriver final : public IBonDriver2 {
public:
	explicit BonDriver(config::Config& config);
	~BonDriver();

	// cannot copy
	BonDriver(const BonDriver&) = delete;
	BonDriver& operator=(const BonDriver&) = delete;

	// IBonDriver
	const bool OpenTuner(void) override;
	void CloseTuner(void) override;

	const bool SetChannel(const uint8_t bCh) override;
	const float GetSignalLevel(void) override;

	const uint32_t WaitTsStream(const uint32_t dwTimeOut = 0) override;
	const uint32_t GetReadyCount(void) override;

	const bool GetTsStream(uint8_t *pDst, uint32_t *pdwSize, uint32_t *pdwRemain) override;
	const bool GetTsStream(uint8_t **ppDst, uint32_t *pdwSize, uint32_t *pdwRemain) override;

	void PurgeTsStream(void) override;

	void Release(void) override;

	// IBonDriver2
	const char16_t *GetTunerName(void) override;

	const bool IsTunerOpening(void) override;

	const char16_t *EnumTuningSpace(const uint32_t dwSpace) override;
	const char16_t *EnumChannelName(const uint32_t dwSpace, const uint32_t dwChannel) override;

	const bool SetChannel(const uint32_t dwSpace, const uint32_t dwChannel) override;

	const uint32_t GetCurSpace(void) override;
	const uint32_t GetCurChannel(void) override;

	static BonDriver * GetInstance();

private:
	std::mutex mtx_;
	std::filesystem::path current_path_;
	std::filesystem::path bondriver_path_;
	bool disable_adts_conversion_ = false;
	void* bondriver_module_ = nullptr;
	IBonDriver* bondriver_ = nullptr;
	IBonDriver2* bondriver2_ = nullptr;

	std::atomic_bool is_mmts_to_ts_enabled_ = true;
	std::vector<uint32_t> mmts_to_ts_channel_spaces_;

	MmtTlv::MmtTlvDemuxer mmtlv_demuxer_;
	RemuxerHandler remuxer_handler_{mmtlv_demuxer_};
	std::vector<uint8_t> input_buffer_;
	std::vector<uint8_t> output_buffer_;
	std::vector<uint8_t> remux_output_;

	static std::mutex instance_mtx_;
	static BonDriver *instance_;

	static void DestroyInstance();
};

} // namespace BonDriver_LinuxDantto4k
