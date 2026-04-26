// SPDX-License-Identifier: MIT
/*
 * ini file loader (config.hpp)
 *
 * Copyright (c) 2021 nns779
 *               2026 hendecarows
 */

#pragma once

#include <cstdint>
#include <algorithm>
#include <format>
#include <fstream>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

namespace util {

inline constexpr std::string_view WHITE_SPACE = " \n\r\t\f\v";
inline constexpr std::string_view SEPARATOR = ", \t";

std::string_view LTrim(std::string_view str) {
	// 先頭から見て、空白文字ではない文字の位置を探し見つけた位置を返す
	// 見つからない場合std::string::nposが返る
	auto pos = str.find_first_not_of(util::WHITE_SPACE);

	return (pos == std::string::npos) ? "" : str.substr(pos);
}

std::string_view RTrim(std::string_view str) {
	// 末尾から見て、空白文字ではない文字の位置を探し見つけた位置を返す
	// 見つからない場合std::string::nposが返る
	auto pos = str.find_last_not_of(util::WHITE_SPACE);
	
	// posは0始まりのインデックスなので、長さにするために+1が必要
	return (pos == std::string::npos) ? "" : str.substr(0, pos + 1);
}

std::string_view Trim(std::string_view str) {
	return RTrim(LTrim(str));
}

void Separate(std::string_view str, std::vector<std::string>& res) {
	auto ofs = std::string_view::size_type(0);
	const auto tail = str.length();

	while (ofs < tail) {
		// SEPARATORに含まれない文字の位置を探し見つけた位置を返す
		auto h = str.find_first_not_of(util::SEPARATOR, ofs);
		if (h == std::string_view::npos) {
			// 見つからない場合std::string::nposが返る
			break;
		}
		
		ofs = h;

		// SEPARATORに含まれる文字の位置を探し見つけた位置を返す
		auto t = str.find_first_of(util::SEPARATOR, ofs);
		if (t == std::string_view::npos) {
			// 見つからない場合std::string::nposが返る
			t = tail;
		}

		res.emplace_back(str.substr(ofs, t - ofs));

		ofs = t;
	}

	return;
}

} // namespace util


namespace config {

class Config final {
public:
	class Section final {
	public:
		Section() {}
		~Section() {}

		inline bool Exists(const std::string& key) const noexcept {
			return !!data_.count(key);
		}

		inline bool Set(const std::string& key, const std::string& value) {
			return data_.emplace(key, value).second;
		}

		inline const std::string& Get(const std::string& key) const {
			return data_.at(key);
		}

		inline std::string GetStr(const std::string& key, const std::string& default_value) const {
			auto it = data_.find(key);
			if (it != data_.end()) {
				return it->second;
			}
			return default_value;
		}

		inline bool GetBool(const std::string& key, bool default_value) const {
			try {
				auto str = Get(key);
				std::transform(str.begin(), str.end(), str.begin(), ::tolower);

				if (str == "true" || str == "1" || str == "yes" || str == "on") {
					return true;
				} else if (str == "false" || str == "0" || str == "no" || str == "off") {
					return false;
				}

				return default_value;
			} catch (...) {
				return default_value;
			}
		}

		inline int32_t GetInt(const std::string& key, int32_t default_value) const {
			try {
				return std::stoi(Get(key), nullptr, 0);
			} catch (...) {
				return default_value;
			}
		}

		inline uint32_t GetUInt(const std::string& key, uint32_t default_value) const {
			try {
				auto value = std::stoul(Get(key), nullptr, 0);
				return static_cast<uint32_t>(value);
			} catch (...) {
				return default_value;
			}
		}

		inline int32_t GetIntMinMax(const std::string& key, int32_t default_value, int32_t min_value, int32_t max_value) const {
			return std::clamp(GetInt(key, default_value), min_value, max_value);
		}

		inline uint32_t GetUIntMinMax(const std::string& key, uint32_t default_value, uint32_t min_value, uint32_t max_value) const {
			return std::clamp(GetUInt(key, default_value), min_value, max_value);
		}

		inline bool GetIntValues(const std::string& key, std::vector<int32_t>& values) const {
			try {
				auto str = Get(key);
				std::vector<std::string> res;
				util::Separate(str, res);
				for (const auto& s : res) {
					values.emplace_back(std::stoi(s));
				}
				return true;
			} catch (...) {
				return false;
			}
		}

		inline bool GetUIntValues(const std::string& key, std::vector<uint32_t>& values) const {
			try {
				auto str = Get(key);
				std::vector<std::string> res;
				util::Separate(str, res);
				for (const auto& s : res) {
					values.emplace_back(static_cast<uint32_t>(std::stoul(s)));
				}
				return true;
			} catch (...) {
				return false;
			}
		}

		inline std::vector<int32_t> GetIntValues(const std::string& key) const {
			std::vector<int32_t> values;
			std::vector<std::string> res;
			auto str = Get(key);
			util::Separate(str, res);
			for (const auto& s : res) {
				values.emplace_back(std::stoi(s));
			}

			return values;
		}

		inline std::vector<uint32_t> GetUIntValues(const std::string& key) const {
			std::vector<uint32_t> values;
			std::vector<std::string> res;
			auto str = Get(key);
			util::Separate(str, res);
			for (const auto& s : res) {
				values.emplace_back(static_cast<uint32_t>(std::stoul(s)));
			}

			return values;
		}

	private:
		std::unordered_map<std::string, std::string> data_;
	};

	Config() {}
	~Config() {}

	inline bool Load(const std::string& path) {
		std::ifstream ifs(path);
		if (!ifs.is_open()) {
			error_.append(std::format("failed to open {}", path));
			return false;
		}

		Config::Section *sct = nullptr;
		std::string line;
		size_t line_count = 0;

		while (std::getline(ifs, line)) {
			line_count++;
			auto sv = util::Trim(line);

			if (sv.empty()) {
				continue;
			}

			switch (sv.at(0)) {
			case ';': // 行コメント
			case '#':
				continue;

			case '[': // セクション [section]
			{
				// セクション名 [section] の終わり']'をチェック
				auto close_pos = sv.find_last_of(']');
				if (close_pos == std::string_view::npos || close_pos <= 1) {
					// ']' がない、または [] のように中身が空の場合をエラーとする
					error_.append(std::format("invalid section format at {}:{}", line_count, line));
					return false;
				}

				// セクション名 [section] を取り出す
				auto section = util::Trim(sv.substr(1, close_pos - 1));
				if (section.empty()) {
					// セクション名が空の場合をエラーとする
					error_.append(std::format("empty section name at {}:{}", line_count, line));
					return false;
				}

				// 既存セクションがあれば取得なければ新規作成
				sct = &sections_[std::string(section)];
				break;
			}

			default: // キーバリュー key=value
			{
				if (!sct) {
					// セクションが定義される前にキーが現れたらエラー
					error_.append(std::format("key found before any section at {}:{}", line_count, line));
					return false;
				}

				// キーとバリューの区切り'='を確認
				auto equal_pos = sv.find('=');
				if (equal_pos == std::string_view::npos) {
					// '=' が存在しない場合は不正な形式としてエラー
					error_.append(std::format("invalid key-value format missing '=' at {}:{}", line_count, line));
					return false;
				}

				std::string_view key, value;
				key = util::Trim(sv.substr(0, equal_pos));
				value = util::Trim(sv.substr(equal_pos + 1));

				if (key.empty()) {
					// キーが空の場合はエラー
					error_.append(std::format("empty key at {}:{}", line_count, line));
					return false;
				}

				// バリューのクォートを除去
				if (value.size() >= 2) {
					if ((value.front() == '"' && value.back() == '"') ||
						(value.front() == '\'' && value.back() == '\'')) {
						value = value.substr(1, value.size() - 2);
					}
				}

				sct->Set(std::string(key), std::string(value));
				break;
			}
			}
		}

		return true;
	}

	inline bool Exists(const std::string& section) const noexcept {
		return !!sections_.count(section);
	}

	inline const Section& Get(const std::string& section) const {
		return sections_.at(section);
	}

	inline bool Set(const std::string& section, const std::string& key, const std::string& value)
	{
		return sections_[section].Set(key, value);
	}

	inline bool HasError() const noexcept {
		return error_.empty() ? false : true;
	}

	inline const std::string& Error() const noexcept {
		return error_;
	}

private:
	std::string error_;
	std::unordered_map<std::string, Section> sections_;
};

} // namespace config