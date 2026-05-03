// IBonDriver2.h: IBonDriver2 クラスのインターフェイス
//
//////////////////////////////////////////////////////////////////////

#if !defined(_IBONDRIVER2_H_)
#define _IBONDRIVER2_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <cstdint>
#include "IBonDriver.h"


// 凡ドライバインタフェース2
class IBonDriver2 : public IBonDriver
{
public:
	virtual const char16_t *GetTunerName(void) = 0;

	virtual const bool IsTunerOpening(void) = 0;

	virtual const char16_t *EnumTuningSpace(const std::uint32_t dwSpace) = 0;
	virtual const char16_t *EnumChannelName(const std::uint32_t dwSpace, const std::uint32_t dwChannel) = 0;

	virtual const bool SetChannel(const std::uint32_t dwSpace, const std::uint32_t dwChannel) = 0;

	virtual const std::uint32_t GetCurSpace(void) = 0;
	virtual const std::uint32_t GetCurChannel(void) = 0;

// IBonDriver
	virtual void Release(void) = 0;
};

#endif // !defined(_IBONDRIVER2_H_)
